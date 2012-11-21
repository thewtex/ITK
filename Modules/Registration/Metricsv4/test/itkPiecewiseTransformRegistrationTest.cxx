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

#include <iostream>

#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkJointHistogramMutualInformationImageToImageMetricv4.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "itkCorrelationImageToImageMetricv4.h"
#include "itkTranslationTransform.h"
#include "itkGaussianImageSource.h"
#include "itkCyclicShiftImageFilter.h"
//#include "itkRegistrationParameterScalesFromPhysicalShift.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkPiecewiseTransform.h"
#include "itkTestingMacros.h"


/*
 * Test various v4 metrics with the PiecewiseTransform
 */

const unsigned int itkPiecewiseTransformRegistrationTestNDimensions = 2;
unsigned int itkPiecewiseTransformRegistrationTestImageSize = 100;

template
<class TScalar = double, unsigned int NDimensions = itkPiecewiseTransformRegistrationTestNDimensions>
class PiecewiseTransformRegistrationTestTransform : public itk::PiecewiseTransform<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef PiecewiseTransformRegistrationTestTransform    Self;
  typedef itk::PiecewiseTransform<TScalar, NDimensions>  Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( PiecewiseTransformRegistrationTestTransform, PiecewiseTransform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Sub transform type **/
  typedef typename Superclass::TransformType                TransformType;
  typedef typename Superclass::TransformTypePointer         TransformTypePointer;
  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer  InverseTransformBasePointer;
  typedef typename Superclass::NumberOfParametersType       NumberOfParametersType;
  typedef typename Superclass::TransformQueueType           TransformQueueType;
  typedef typename Superclass::WeightArrayType              WeightArrayType;
  typedef typename Superclass::OffsetArrayType              OffsetArrayType;
  typedef typename Superclass::InputPointType               InputPointType;

  virtual NumberOfParametersType GetNumberOfLocalParametersAtPoint(const InputPointType  & point) const
  {
    TransformQueueType transforms;
    WeightArrayType    m_Weights; //name like member vars b/c of kwstyle complaint - must be a bug
    OffsetArrayType    m_Offsets;
    this->GetTransformListAndNormalizedWeightsAtPoint( point, transforms, m_Weights, m_Offsets );
    NumberOfParametersType result = 0;
    for( itk::SizeValueType n=0; n < transforms.size(); n++ )
      {
      result += transforms[n]->GetNumberOfLocalParametersAtPoint( point );
      }
    return result;
  }

  virtual bool GetNumberOfLocalParametersAtPointIsConstant() const
  {
    return false;
  }

  virtual void Initialize()
  {
  if( this->GetNumberOfTransforms() != 1 ) //1 for debugging
    {
    itkExceptionMacro("Expected 2 transforms.");
    }
  }

protected:
  PiecewiseTransformRegistrationTestTransform(){};
  virtual ~PiecewiseTransformRegistrationTestTransform(){};

  virtual typename itk::LightObject::Pointer InternalClone() const
  {
    return NULL;
  }

  virtual void GetTransformListAndNormalizedWeightsAtPoint( const InputPointType & point, TransformQueueType & transforms, WeightArrayType & weights, OffsetArrayType & fullOffsets ) const
  {
    //Simple test method
    transforms.clear();
    weights.clear();
    fullOffsets.clear();

//TESTING
transforms.push_back( this->GetNthTransform(0) );
weights.SetSize(1);
weights[0] = 1.0;
fullOffsets.SetSize(1);
fullOffsets[0] = 0;
return;

    if( point[0] < itkPiecewiseTransformRegistrationTestImageSize / 3.0 )
      {
      transforms.push_back( this->GetNthTransform(0) );
      weights.SetSize(1);
      weights[0] = 1.0;
      fullOffsets.SetSize(1);
      fullOffsets[0] = 0;
      return;
      }

    if( point[0] < itkPiecewiseTransformRegistrationTestImageSize * 2.0 / 3.0 )
      {
      transforms.push_back( this->GetNthTransform(1) );
      weights.SetSize(1);
      weights[0] = 1.0;
      fullOffsets.SetSize(1);
      fullOffsets[0] = this->GetNthTransform(0)->GetNumberOfParameters();
      return;
      }

    //Otherwise, both are active
    transforms.push_back( this->GetNthTransform(0) );
    transforms.push_back( this->GetNthTransform(1) );
    weights.SetSize(2);
    //Unequal weights for better testing
    weights[0] = 1.0/3.0;
    weights[1] = 1.0 - weights[0];
    fullOffsets.SetSize(2);
    fullOffsets[0] = 0;
    fullOffsets[1] = this->GetNthTransform(0)->GetNumberOfParameters();
    return;
  }

private:
  PiecewiseTransformRegistrationTestTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );     // purposely not implemented

};

///////////////////////////////////////////////////////////////////

template<unsigned int Dimension, typename TImage, typename TMetric>
int PiecewiseTransformRegistrationTestRun( typename TMetric::Pointer metric, int numberOfIterations, typename TImage::PixelType maximumStepSize )
{
  typedef typename TImage::PixelType  PixelType;
  typedef PixelType                   CoordinateRepresentationType;

  // Create two simple images
  itk::SizeValueType ImageSize = itkPiecewiseTransformRegistrationTestImageSize;
  itk::OffsetValueType boundary = 6;
  if( Dimension == 3 )
    {
    itkPiecewiseTransformRegistrationTestImageSize = 60;
    boundary = 4;
    }

   // Declare Gaussian Sources
  typedef itk::GaussianImageSource< TImage >        GaussianImageSourceType;
  typedef typename GaussianImageSourceType::Pointer GaussianImageSourcePointer;

  typename TImage::SizeType size;
  size.Fill( ImageSize );

  typename TImage::SpacingType spacing;
  spacing.Fill( itk::NumericTraits<CoordinateRepresentationType>::One );

  typename TImage::PointType origin;
  origin.Fill( itk::NumericTraits<CoordinateRepresentationType>::Zero );

  typename TImage::DirectionType direction;
  direction.Fill( itk::NumericTraits<CoordinateRepresentationType>::One );

  typename GaussianImageSourceType::Pointer  fixedImageSource = GaussianImageSourceType::New();

  fixedImageSource->SetSize(    size    );
  fixedImageSource->SetOrigin(  origin  );
  fixedImageSource->SetSpacing( spacing );
  fixedImageSource->SetNormalized( false );
  fixedImageSource->SetScale( 1.0f );
  fixedImageSource->Update();
  typename TImage::Pointer  fixedImage  = fixedImageSource->GetOutput();

  // zero-out the boundary
  itk::ImageRegionIteratorWithIndex<TImage> it( fixedImage, fixedImage->GetLargestPossibleRegion() );
  for( it.GoToBegin(); ! it.IsAtEnd(); ++it )
    {
    for( itk::SizeValueType n=0; n < Dimension; n++ )
      {
      if( it.GetIndex()[n] < boundary || (static_cast<itk::OffsetValueType>(size[n]) - it.GetIndex()[n]) <= boundary )
        {
        it.Set( itk::NumericTraits<PixelType>::Zero );
        break;
        }
      }
    }

  // shift the fixed image to get the moving image
  typedef itk::CyclicShiftImageFilter<TImage, TImage> CyclicShiftFilterType;
  typename CyclicShiftFilterType::Pointer shiftFilter = CyclicShiftFilterType::New();
  typename CyclicShiftFilterType::OffsetType imageShift;
  typename CyclicShiftFilterType::OffsetValueType maxImageShift = boundary-1;
  imageShift.Fill( maxImageShift );
  imageShift[0] = maxImageShift / 2;
  shiftFilter->SetInput( fixedImage );
  shiftFilter->SetShift( imageShift );
  shiftFilter->Update();
  typename TImage::Pointer movingImage = shiftFilter->GetOutput();

  // create a piecwise transform will two translation transforms
  typedef PiecewiseTransformRegistrationTestTransform<double, Dimension> PiecewiseTransformType;
  typedef itk::TranslationTransform<double, Dimension>                   TranslationTransformType;
  typename PiecewiseTransformType::Pointer   piecewiseTransform = PiecewiseTransformType::New();
  typename TranslationTransformType::Pointer translationTransform = TranslationTransformType::New();
  translationTransform->SetIdentity();
  piecewiseTransform->AddTransform( translationTransform );
//  translationTransform = TranslationTransformType::New();
//  translationTransform->SetIdentity();
//  piecewiseTransform->AddTransform( translationTransform );

  // setup metric
  //
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
//  metric->SetMovingTransform( piecewiseTransform );
  metric->SetMovingTransform( translationTransform );
  metric->SetUseMovingImageGradientFilter( false );
  metric->SetUseFixedImageGradientFilter( false );

  std::cout << "Dense sampling." << std::endl;
  metric->SetUseFixedSampledPointSet( false );

  // initialize
  piecewiseTransform->Initialize();
  metric->Initialize();

  // calculate initial metric value
  typename TMetric::MeasureType initialValue = metric->GetValue();

  // scales estimator - not yet supported by PiecewiseTransform
  //typedef itk::RegistrationParameterScalesFromPhysicalShift< TMetric > RegistrationParameterScalesFromPhysicalShiftType;
  //typename RegistrationParameterScalesFromPhysicalShiftType::Pointer shiftScaleEstimator = RegistrationParameterScalesFromPhysicalShiftType::New();
  //shiftScaleEstimator->SetMetric(metric);

  //
  // optimizer
  //
  typedef itk::GradientDescentOptimizerv4  OptimizerType;
  typename OptimizerType::Pointer  optimizer = OptimizerType::New();
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
  //optimizer->SetScalesEstimator( shiftScaleEstimator );
  if( maximumStepSize > 0 )
    {
    optimizer->SetMaximumStepSizeInPhysicalUnits( maximumStepSize );
    }
  optimizer->StartOptimization();

  std::cout << "image size: " << size;
  std::cout << ", # of iterations: " << optimizer->GetNumberOfIterations() << ", max step size: " << optimizer->GetMaximumStepSizeInPhysicalUnits() << std::endl;
  std::cout << "optimizer scales: " << optimizer->GetScales() << std::endl;
  std::cout << "imageShift: " << imageShift << std::endl;
  std::cout << "Transform final parameters: " << piecewiseTransform->GetParameters() << std::endl;

  // final metric value
  typename TMetric::MeasureType finalValue = metric->GetValue();
  std::cout << "metric value: initial: " << initialValue << ", final: " << finalValue << std::endl;

  // test that the final position is close to the truth
  double tolerance = static_cast<double>(0.11);
  for( itk::SizeValueType n=0; n < Dimension; n++ )
    {
    if( vcl_fabs( 1.0 - ( static_cast<double>(imageShift[n]) / piecewiseTransform->GetParameters()[n] ) ) > tolerance ||
        vcl_fabs( 1.0 - ( static_cast<double>(imageShift[n]) / piecewiseTransform->GetParameters()[n+Dimension] ) ) > tolerance )
      {
      std::cerr << "XXX Failed. Final transform parameters are not within tolerance of image shift. XXX" << std::endl;
      return EXIT_FAILURE;
      }
    }
  // test that metric value is minimized
  if( finalValue >= initialValue )
    {
    std::cerr << "XXX Failed. Final metric value is not less than initial value. XXX" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////
int itkPiecewiseTransformRegistrationTest(int argc, char *argv[])
{
  const unsigned int Dimension = 2;
  typedef itk::Image< double, Dimension > ImageType;

  // options
  // we have two options for iterations and step size to accomodate
  // the different behavior of metrics
  int numberOfIterations1 = 50;
  ImageType::PixelType maximumStepSize1 = 1.0;
  int numberOfIterations2 = 120;
  ImageType::PixelType maximumStepSize2 = 0.1;

  if( argc > 1 )
    {
    numberOfIterations1 = atoi( argv[1] );
    }
  if( argc > 2 )
    {
    maximumStepSize1 = atof( argv[2] );
    }
  if( argc > 3 )
    {
    numberOfIterations2 = atoi( argv[3] );
    }
  if( argc > 4 )
    {
    maximumStepSize2 = atof( argv[4] );
    }

  std::cout << std::endl << "******************* Dimension: " << Dimension << std::endl;

  bool passed = true;

  // MeanSquares
  {
  typedef itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** MeanSquares metric: " << std::endl;
  if( PiecewiseTransformRegistrationTestRun<Dimension, ImageType, MetricType>( metric, numberOfIterations1, maximumStepSize1 ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // ANTS Neighborhood Correlation
  {
  typedef itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** ANTSNeighborhoodCorrelation metric: " << std::endl;
  if( PiecewiseTransformRegistrationTestRun<Dimension, ImageType, MetricType>( metric, numberOfIterations1, maximumStepSize1 ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Correlation
  {
  typedef itk::CorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** Correlation metric: " << std::endl;
  if( PiecewiseTransformRegistrationTestRun<Dimension, ImageType, MetricType>( metric, numberOfIterations1, maximumStepSize1 ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Joint Histogram
  {
  typedef itk::JointHistogramMutualInformationImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** JointHistogramMutualInformation metric: " << std::endl;
  if( PiecewiseTransformRegistrationTestRun<Dimension, ImageType, MetricType>( metric, numberOfIterations1, maximumStepSize1 ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Mattes - currently unsupported, expect exception
  {
  typedef itk::MattesMutualInformationImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** MattesMutualInformation metric: expect exception" << std::endl;
  TRY_EXPECT_EXCEPTION( (PiecewiseTransformRegistrationTestRun<Dimension, ImageType, MetricType>( metric, numberOfIterations2, maximumStepSize2 ) ) )
  }

  if( passed )
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }
}
