/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkImageRegistrationMethodv4_hxx
#define __itkImageRegistrationMethodv4_hxx

#include "itkImageRegistrationMethodv4.h"

#include "itkDiscreteGaussianImageFilter.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageToImageMetricv4.h"
#include "itkIterationReporter.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkRegistrationParameterScalesFromPhysicalShift.h"
#include "itkShrinkImageFilter.h"

namespace itk
{
/**
 * Constructor
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::ImageRegistrationMethodv4()
{
  this->SetNumberOfRequiredOutputs( 1 );

  this->m_CurrentLevel = 0;
  this->m_CurrentIteration = 0;
  this->m_CurrentMetricValue = 0.0;
  this->m_CurrentConvergenceValue = 0.0;
  this->m_IsConverged = false;

  this->m_CompositeTransform = CompositeTransformType::New();

  typedef IdentityTransform<RealType, ImageDimension> IdentityTransformType;

  typename IdentityTransformType::Pointer defaultFixedInitialTransform = IdentityTransformType::New();
  this->m_FixedInitialTransform = defaultFixedInitialTransform;

  typename IdentityTransformType::Pointer defaultMovingInitialTransform = IdentityTransformType::New();
  this->m_MovingInitialTransform = defaultMovingInitialTransform;

  // By default we set up a 3-level image registration.

  m_NumberOfLevels = 0;
  this->SetNumberOfLevels( 3 );

  this->m_OutputTransform = OutputTransformType::New();

  DecoratedOutputTransformPointer transformDecorator = DecoratedOutputTransformType::New().GetPointer();
  transformDecorator->Set( this->m_OutputTransform );
  this->ProcessObject::SetNthOutput( 0, transformDecorator );

  this->m_ShrinkFactorsPerLevel.SetSize( this->m_NumberOfLevels );
  this->m_ShrinkFactorsPerLevel[0] = 4;
  this->m_ShrinkFactorsPerLevel[1] = 2;
  this->m_ShrinkFactorsPerLevel[2] = 1;

  this->m_SmoothingSigmasPerLevel.SetSize( this->m_NumberOfLevels );
  this->m_SmoothingSigmasPerLevel[0] = 2;
  this->m_SmoothingSigmasPerLevel[1] = 1;
  this->m_SmoothingSigmasPerLevel[2] = 0;

  this->m_SmoothingSigmasAreSpecifiedInPhysicalUnits = true;

  typedef MattesMutualInformationImageToImageMetricv4<FixedImageType, MovingImageType> DefaultMetricType;
  typename DefaultMetricType::Pointer mutualInformationMetric = DefaultMetricType::New();
  mutualInformationMetric->SetNumberOfHistogramBins( 20 );
  mutualInformationMetric->SetUseMovingImageGradientFilter( false );
  mutualInformationMetric->SetUseFixedImageGradientFilter( false );
  mutualInformationMetric->SetUseFixedSampledPointSet( false );
  this->m_Metric = mutualInformationMetric;

  this->m_MetricSamplingStrategy = NONE;
  this->m_MetricSamplingPercentagePerLevel.SetSize( this->m_NumberOfLevels );
  this->m_MetricSamplingPercentagePerLevel.Fill( 1.0 );

  typedef RegistrationParameterScalesFromPhysicalShift<DefaultMetricType> DefaultScalesEstimatorType;
  typename DefaultScalesEstimatorType::Pointer scalesEstimator = DefaultScalesEstimatorType::New();
  scalesEstimator->SetMetric( mutualInformationMetric );
  scalesEstimator->SetTransformForward( true );

  typedef GradientDescentOptimizerv4 DefaultOptimizerType;
  typename DefaultOptimizerType::Pointer optimizer = DefaultOptimizerType::New();
  optimizer->SetLearningRate( 1.0 );
  optimizer->SetNumberOfIterations( 1000 );
  optimizer->SetScalesEstimator( scalesEstimator );
  this->m_Optimizer = optimizer;
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::~ImageRegistrationMethodv4()
{
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetFixedImage( SizeValueType index, const FixedImageType *image )
{
  itkDebugMacro( "setting fixed image input " << index << " to " << image );
  if( image != static_cast<FixedImageType *>( this->ProcessObject::GetInput( 2 * index ) ) )
    {
    this->ProcessObject::SetNthInput( 2 * index, const_cast<FixedImageType *>( image ) );
    this->Modified();
    }
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
const typename ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>::FixedImageType *
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::GetFixedImage( SizeValueType index ) const
{
  itkDebugMacro( "returning fixed image input " << index << " of "
                                    << static_cast<const FixedImageType *>( this->ProcessObject::GetInput( 2 * index ) ) );
  return static_cast<const FixedImageType *>( this->ProcessObject::GetInput( 2 * index ) );
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetMovingImage( SizeValueType index, const MovingImageType *image )
{
  itkDebugMacro( "setting moving image input " << index << " to " << image );
  if( image != static_cast<MovingImageType *>( this->ProcessObject::GetInput( 2 * index + 1 ) ) )
    {
    this->ProcessObject::SetNthInput( 2 * index + 1, const_cast<MovingImageType *>( image ) );
    this->Modified();
    }
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
const typename ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>::MovingImageType *
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::GetMovingImage( SizeValueType index ) const
{
  itkDebugMacro( "returning moving image input " << index << " of "
                                    << static_cast<const MovingImageType *>( this->ProcessObject::GetInput( 2 * index + 1 ) ) );
  return static_cast<const MovingImageType *>( this->ProcessObject::GetInput( 2 * index + 1 ) );
}

/*
 * Initialize by setting the interconnects between components.
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::InitializeRegistrationAtEachLevel( const SizeValueType level )
{
  // Sanity checks

  if( this->GetNumberOfInputs() % 2 != 0 )
    {
    itkExceptionMacro( "The number of fixed and moving images is not equal." );
    }

  SizeValueType numberOfImagePairs = static_cast<unsigned int>( 0.5 * this->GetNumberOfInputs() );

  if( numberOfImagePairs == 0 )
    {
    itkExceptionMacro( "There are no input images." );
    }

  if( numberOfImagePairs > 1 )
    {
    // If more than one image pair is set, we assume that the multi-metric
    // is being used.  We check to see if the number of image pairs is equal
    // to the number of metrics in the multi-metric.

    typename MultiMetricType::Pointer multiMetric = dynamic_cast<MultiMetricType *>( this->m_Metric.GetPointer() );
    if( multiMetric )
      {
      SizeValueType numberOfMetrics = multiMetric->GetNumberOfMetrics();
      if( numberOfMetrics != numberOfImagePairs )
        {
        itkExceptionMacro( "Mismatch between number of image pairs and the number of metrics." );
        }
      }
    else
      {
      itkExceptionMacro( "There is more than one image pair.  Need to use a MultiMetricType." );
      }
    }

  if ( !this->m_Optimizer )
    {
    itkExceptionMacro( "The optimizer is not present." );
    }
  if ( !this->m_Metric )
    {
    itkExceptionMacro( "The image metric is not present." );
    }

  this->m_CurrentIteration = 0;
  this->m_CurrentMetricValue = 0.0;
  this->m_CurrentConvergenceValue = 0.0;
  this->m_IsConverged = false;

  this->InvokeEvent( InitializeEvent() );

  // For each level, we adapt the current transform.  For many transforms, e.g.
  // affine, the base transform adaptor does not do anything.  However, in the
  // case of other transforms, e.g. the b-spline and displacement field transforms
  // the fixed parameters are changed to reflect an increase in transform resolution.
  // This could involve increasing the mesh size of the B-spline transform or
  // increase the resolution of the displacement field.

  if( this->m_TransformParametersAdaptorsPerLevel[level] )
    {
    this->m_TransformParametersAdaptorsPerLevel[level]->SetTransform( this->m_OutputTransform );
    this->m_TransformParametersAdaptorsPerLevel[level]->AdaptTransformParameters();
    }

  // Set-up the composite transform at initialization
  if( level == 0 )
    {
    this->m_CompositeTransform->ClearTransformQueue();

    // If the moving initial transform is a composite transform, unroll
    // it into m_CompositeTransform.  This is a temporary fix to accommodate
    // the lack of support for calculating the jacobian in the composite
    // transform.

    this->m_CompositeTransform->AddTransform( this->m_MovingInitialTransform );
    this->m_CompositeTransform->AddTransform( this->m_OutputTransform );
    this->m_CompositeTransform->FlattenTransformQueue();
    }
  this->m_CompositeTransform->SetOnlyMostRecentTransformToOptimizeOn();

  // At each resolution and for each image pair, we can
  //   1. subsample the reference domain (typically the fixed image) and/or
  //   2. smooth the fixed and moving images.

  typedef ShrinkImageFilter<FixedImageType, FixedImageType> ShrinkFilterType;
  typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
  shrinkFilter->SetShrinkFactors( this->m_ShrinkFactorsPerLevel[level] );
  shrinkFilter->SetInput( this->GetFixedImage( 0 ) );
  shrinkFilter->Update();

  this->m_Metric->SetVirtualDomainFromImage( shrinkFilter->GetOutput() );
  this->m_Metric->SetFixedTransform( this->m_FixedInitialTransform );
  this->m_Metric->SetMovingTransform( this->m_CompositeTransform );

  for( unsigned int n = 0; n < numberOfImagePairs; n++ )
    {
    typedef DiscreteGaussianImageFilter<FixedImageType, FixedImageType> FixedImageSmoothingFilterType;
    typename FixedImageSmoothingFilterType::Pointer fixedImageSmoothingFilter = FixedImageSmoothingFilterType::New();
    if( this->m_SmoothingSigmasAreSpecifiedInPhysicalUnits == true )
      {
      fixedImageSmoothingFilter->SetUseImageSpacingOn();
      }
    else
      {
      fixedImageSmoothingFilter->SetUseImageSpacingOff();
      }
    fixedImageSmoothingFilter->SetVariance( vnl_math_sqr( this->m_SmoothingSigmasPerLevel[level] ) );
    fixedImageSmoothingFilter->SetMaximumError( 0.01 );
    fixedImageSmoothingFilter->SetInput( this->GetFixedImage( n ) );

    this->m_FixedSmoothImages.push_back( fixedImageSmoothingFilter->GetOutput() );
    this->m_FixedSmoothImages[n]->Update();
    this->m_FixedSmoothImages[n]->DisconnectPipeline();

    typedef DiscreteGaussianImageFilter<MovingImageType, MovingImageType> MovingImageSmoothingFilterType;
    typename MovingImageSmoothingFilterType::Pointer movingImageSmoothingFilter = MovingImageSmoothingFilterType::New();
    if( this->m_SmoothingSigmasAreSpecifiedInPhysicalUnits == true )
      {
      movingImageSmoothingFilter->SetUseImageSpacingOn();
      }
    else
      {
      movingImageSmoothingFilter->SetUseImageSpacingOff();
      }
    movingImageSmoothingFilter->SetVariance( vnl_math_sqr( this->m_SmoothingSigmasPerLevel[level] ) );
    movingImageSmoothingFilter->SetMaximumError( 0.01 );
    movingImageSmoothingFilter->SetInput( this->GetMovingImage( n ) );

    this->m_MovingSmoothImages.push_back( movingImageSmoothingFilter->GetOutput() );
    this->m_MovingSmoothImages[n]->Update();
    this->m_MovingSmoothImages[n]->DisconnectPipeline();

    // Update the image metric

    typename MultiMetricType::Pointer multiMetric = dynamic_cast<MultiMetricType *>( this->m_Metric.GetPointer() );
    if( multiMetric )
      {
      dynamic_cast<MetricType *>( multiMetric->GetMetricQueue()[n].GetPointer() )->SetFixedImage( this->m_FixedSmoothImages[n] );
      dynamic_cast<MetricType *>( multiMetric->GetMetricQueue()[n].GetPointer() )->SetMovingImage( this->m_MovingSmoothImages[n] );
      }
    else
      {
      this->m_Metric->SetFixedImage( this->m_FixedSmoothImages[n] );
      this->m_Metric->SetMovingImage( this->m_MovingSmoothImages[n] );
      }
    }

  if( this->m_MetricSamplingStrategy != NONE )
    {
    this->SetMetricSamplePoints();
    }

  // Update the optimizer

  this->m_Optimizer->SetMetric( this->m_Metric );

  if( ( this->m_Optimizer->GetScales() ).Size() != this->m_OutputTransform->GetNumberOfLocalParameters() )
    {
    typedef typename OptimizerType::ScalesType ScalesType;
    ScalesType scales;
    scales.SetSize( this->m_OutputTransform->GetNumberOfLocalParameters() );
    scales.Fill( NumericTraits<typename ScalesType::ValueType>::OneValue() );
    this->m_Optimizer->SetScales( scales );
    }
}

/*
 * Start the registration
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::GenerateData()
{
  for( this->m_CurrentLevel = 0; this->m_CurrentLevel < this->m_NumberOfLevels; this->m_CurrentLevel++ )
    {
    this->InitializeRegistrationAtEachLevel( this->m_CurrentLevel );

    this->m_Metric->Initialize();

    this->m_Optimizer->StartOptimization();
    }
}

/**
 * Set the moving transform adaptors per stage
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetTransformParametersAdaptorsPerLevel( TransformParametersAdaptorsContainerType & adaptors )
{
  if( this->m_NumberOfLevels != adaptors.size() )
    {
    itkExceptionMacro( "The number of levels does not equal the number array size." );
    }
  else
    {
    itkDebugMacro( "Setting the transform parameter adaptors." );
    this->m_TransformParametersAdaptorsPerLevel = adaptors;
    this->Modified();
    }
}

/**
 * Get the moving transform adaptors per stage
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
const typename  ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>::TransformParametersAdaptorsContainerType &
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::GetTransformParametersAdaptorsPerLevel() const
{
  return this->m_TransformParametersAdaptorsPerLevel;
}

/**
 * Set the number of levels
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetNumberOfLevels( const SizeValueType numberOfLevels )
{
  if( this->m_NumberOfLevels != numberOfLevels )
    {
    this->m_NumberOfLevels = numberOfLevels;

    // Set default transform adaptors which don't do anything to the input transform
    // Similarly, fill in some default values for the shrink factors, smoothing sigmas,
    // and learning rates.

    this->m_TransformParametersAdaptorsPerLevel.clear();
    for( SizeValueType level = 0; level < this->m_NumberOfLevels; level++ )
      {
      typename TransformParametersAdaptorType::Pointer transformParametersAdaptor = TransformParametersAdaptorType::New();
      this->m_TransformParametersAdaptorsPerLevel.push_back( transformParametersAdaptor.GetPointer() );
      }

    this->m_ShrinkFactorsPerLevel.SetSize( this->m_NumberOfLevels );
    this->m_ShrinkFactorsPerLevel.Fill( 1 );

    this->m_SmoothingSigmasPerLevel.SetSize( this->m_NumberOfLevels );
    this->m_SmoothingSigmasPerLevel.Fill( 1.0 );

    this->m_MetricSamplingPercentagePerLevel.SetSize( this->m_NumberOfLevels );
    this->m_MetricSamplingPercentagePerLevel.Fill( 1.0 );

    this->Modified();
    }
}

/**
 * Get the metric samples
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetMetricSamplePoints()
{
  SizeValueType numberOfLocalMetrics = 1;

  typename MultiMetricType::Pointer multiMetric = dynamic_cast<MultiMetricType *>( this->m_Metric.GetPointer() );
  if( multiMetric )
    {
    numberOfLocalMetrics = multiMetric->GetNumberOfMetrics();
    }

  for( unsigned int n = 0; n < numberOfLocalMetrics; n++ )
    {
    typename MetricSamplePointSetType::Pointer samplePointSet = MetricSamplePointSetType::New();
    samplePointSet->Initialize();

    typedef typename MetricSamplePointSetType::PointType SamplePointType;

    typedef typename MetricType::VirtualImageType         VirtualDomainImageType;
    typedef typename VirtualDomainImageType::RegionType   VirtualDomainRegionType;
    const VirtualDomainImageType * virtualImage;
    if( numberOfLocalMetrics == 1 )
      {
      virtualImage = this->m_Metric->GetVirtualImage();
      }
    else
      {
      virtualImage = dynamic_cast<MetricType *>( multiMetric->GetMetricQueue()[n].GetPointer() )->GetVirtualImage();
      }
    const VirtualDomainRegionType & virtualDomainRegion = virtualImage->GetRequestedRegion();
    const typename VirtualDomainImageType::SpacingType oneThirdVirtualSpacing = virtualImage->GetSpacing() / 3.0;

    typedef typename Statistics::MersenneTwisterRandomVariateGenerator RandomizerType;
    typename RandomizerType::Pointer randomizer = RandomizerType::New();
    randomizer->SetSeed( 1234 );

    unsigned long index = 0;

    switch( this->m_MetricSamplingStrategy )
      {
      case REGULAR:
        {
        const unsigned long sampleCount = static_cast<unsigned long>( vcl_ceil( 1.0 / this->m_MetricSamplingPercentagePerLevel[this->m_CurrentLevel] ) );
        unsigned long count = sampleCount; //Start at sampleCount to keep behavior backwards identical, using first element.
        ImageRegionConstIteratorWithIndex<VirtualDomainImageType> It( virtualImage, virtualDomainRegion );
        for( It.GoToBegin(); !It.IsAtEnd(); ++It )
          {
          if( count == sampleCount )
            {
            count=0; //Reset counter
            SamplePointType point;
            virtualImage->TransformIndexToPhysicalPoint( It.GetIndex(), point );

            // randomly perturb the point within a voxel (approximately)
            for( unsigned int d = 0; d < ImageDimension; d++ )
              {
              point[d] += randomizer->GetNormalVariate() * oneThirdVirtualSpacing[d];
              }
            samplePointSet->SetPoint( index, point );
            ++index;
            }
          ++count;
          }
        break;
        }
      case RANDOM:
        {
        const unsigned long totalVirtualDomainVoxels = virtualDomainRegion.GetNumberOfPixels();
        const unsigned long sampleCount = static_cast<unsigned long>( static_cast<float>( totalVirtualDomainVoxels ) * this->m_MetricSamplingPercentagePerLevel[this->m_CurrentLevel] );
        ImageRandomConstIteratorWithIndex<VirtualDomainImageType> ItR( virtualImage, virtualDomainRegion );
        ItR.SetNumberOfSamples( sampleCount );
        for( ItR.GoToBegin(); !ItR.IsAtEnd(); ++ItR )
          {
          SamplePointType point;
          virtualImage->TransformIndexToPhysicalPoint( ItR.GetIndex(), point );

          // randomly perturb the point within a voxel (approximately)
          for ( unsigned int d = 0; d < ImageDimension; d++ )
            {
            point[d] += randomizer->GetNormalVariate() * oneThirdVirtualSpacing[d];
            }
          samplePointSet->SetPoint( index, point );
          ++index;
          }
        break;
        }
      default:
        {
        itkExceptionMacro( "Invalid sampling strategy requested." );
        }
      }

    if( numberOfLocalMetrics == 1 )
      {
      this->m_Metric->SetFixedSampledPointSet( samplePointSet );
      this->m_Metric->SetUseFixedSampledPointSet( true );
      }
    else
      {
      dynamic_cast<MetricType *>( multiMetric->GetMetricQueue()[n].GetPointer() )->SetFixedSampledPointSet( samplePointSet );
      dynamic_cast<MetricType *>( multiMetric->GetMetricQueue()[n].GetPointer() )->SetUseFixedSampledPointSet( true );
      }
    }
}

/*
 * PrintSelf
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << "Number of levels = " << this->m_NumberOfLevels << std::endl;

  os << indent << "Shrink factors: " << this->m_ShrinkFactorsPerLevel << std::endl;
  os << indent << "Smoothing sigmas: " << this->m_SmoothingSigmasPerLevel << std::endl;

  if( this->m_SmoothingSigmasAreSpecifiedInPhysicalUnits == true )
    {
    os << indent << indent << "Smoothing sigmas are specified in physical units." << std::endl;
    }
  else
    {
    os << indent << indent << "Smoothing sigmas are specified in voxel units." << std::endl;
    }

  os << indent << "Metric sampling strategy: " << this->m_MetricSamplingStrategy << std::endl;

  os << indent << "Metric sampling percentage: ";
  for( unsigned int i = 0; i < this->m_NumberOfLevels; i++ )
    {
    os << this->m_MetricSamplingPercentagePerLevel[i] << " ";
    }
  os << std::endl;
}

/*
 *  Get output transform
 */
template<typename TFixedImage, typename TMovingImage, typename TTransform>
const typename ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>::DecoratedOutputTransformType *
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::GetOutput() const
{
  return static_cast<const DecoratedOutputTransformType *>( this->ProcessObject::GetOutput( 0 ) );
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
DataObject::Pointer
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::MakeOutput( DataObjectPointerArraySizeType output )
{
  switch ( output )
    {
    case 0:
      return DecoratedOutputTransformType::New().GetPointer();
      break;
    default:
      itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
    }
}

template<typename TFixedImage, typename TMovingImage, typename TTransform>
void
ImageRegistrationMethodv4<TFixedImage, TMovingImage, TTransform>
::SetMetricSamplingPercentage( const RealType samplingPercentage )
{
  MetricSamplingPercentageArrayType samplingPercentagePerLevel;
  samplingPercentagePerLevel.SetSize( this->m_NumberOfLevels );
  samplingPercentagePerLevel.Fill( samplingPercentage );
  this->SetMetricSamplingPercentagePerLevel( samplingPercentagePerLevel );
}

} // end namespace itk
#endif
