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

#ifndef __itkJointHistogramMutualInformationImageToImageObjectMetric_hxx
#define __itkJointHistogramMutualInformationImageToImageObjectMetric_hxx

#include "itkJointHistogramMutualInformationImageToImageObjectMetric.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageIterator.h"
#include "itkDiscreteGaussianImageFilter.h"

namespace itk
{

template <class TFixedImage,class TMovingImage,class TVirtualImage>
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::JointHistogramMutualInformationImageToImageObjectMetric()
{
  // Initialize histogram properties
  this->m_NumberOfHistogramBins = 20;
  this->m_FixedImageTrueMin     = NumericTraits< InternalComputationValueType >::Zero;
  this->m_FixedImageTrueMax     = NumericTraits< InternalComputationValueType >::Zero;
  this->m_MovingImageTrueMin    = NumericTraits< InternalComputationValueType >::Zero;
  this->m_MovingImageTrueMax    = NumericTraits< InternalComputationValueType >::Zero;
  this->m_FixedImageBinSize     = NumericTraits< InternalComputationValueType >::Zero;
  this->m_MovingImageBinSize    = NumericTraits< InternalComputationValueType >::Zero;
  this->m_Padding = 2;
  this->m_JointPDFSum = NumericTraits< InternalComputationValueType >::Zero;
  this->m_Log2 = vcl_log(2.0);
  this->m_VarianceForJointPDFSmoothing = 1.5;

  // We have our own GetValueAndDerivativeThreader's that we want
  // ImageToImageObjectMetric to use.
  this->m_DenseGetValueAndDerivativeThreader  = JointHistogramMutualInformationDenseGetValueAndDerivativeThreader::New();
  this->m_SparseGetValueAndDerivativeThreader = JointHistogramMutualInformationSparseGetValueAndDerivativeThreader::New();

  this->m_JointHistogramMutualInformationDenseComputeJointPDFThreader = JointHistogramMutualInformationDenseComputeJointPDFThreader::New();
  this->m_JointHistogramMutualInformationSparseComputeJointPDFThreader = JointHistogramMutualInformationSparseComputeJointPDFThreader::New();
  this->m_JointPDFInterpolator = JointPDFInterpolatorType::New();
}

template <class TFixedImage, class TMovingImage, class TVirtualImage>
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::~JointHistogramMutualInformationImageToImageObjectMetric()
{
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::Initialize() throw (itk::ExceptionObject)
{
  Superclass::Initialize();

  /** Get the fixed and moving image true max's and mins.
   *  Initialize them to the PixelType min and max. */
  this->m_FixedImageTrueMin =
                    vcl_numeric_limits<typename TFixedImage::PixelType>::max();
  this->m_FixedImageTrueMax =
                    vcl_numeric_limits<typename TFixedImage::PixelType>::min();
  this->m_MovingImageTrueMin =
                    vcl_numeric_limits<typename TMovingImage::PixelType>::max();
  this->m_MovingImageTrueMax =
                    vcl_numeric_limits<typename TMovingImage::PixelType>::min();

  /** Iterate through the fixed image and set the true
   *  max and min for the fixed image. */
  itk::ImageRegionConstIteratorWithIndex<TFixedImage>
                fi(this->m_FixedImage,this->m_FixedImage->GetRequestedRegion());

  /** \todo multi-thread me */
  while( !fi.IsAtEnd() )
    {
    typename TFixedImage::PointType fixedSpacePhysicalPoint;
    this->m_FixedImage->TransformIndexToPhysicalPoint(fi.GetIndex(), fixedSpacePhysicalPoint);
    if ( this->m_FixedImageMask.IsNull()  /* A null mask implies entire space is to be used.*/
         || this->m_FixedImageMask->IsInside(fixedSpacePhysicalPoint) )
       {
       const typename TFixedImage::PixelType currentValue = fi.Get();
       // update the Fixed Image true min accordingly
       if ( currentValue < this->m_FixedImageTrueMin )
         {
         this->m_FixedImageTrueMin = currentValue;
         }
       // update the Fixed Image true max accordingly
       if ( currentValue > this->m_FixedImageTrueMax )
         {
         this->m_FixedImageTrueMax = currentValue;
         }
       }
      ++fi;
    }
  /** Iterate through the moving image and set the true
   * max and min for the moving image. */
  itk::ImageRegionConstIteratorWithIndex<TMovingImage>
              mi(this->m_MovingImage,this->m_MovingImage->GetBufferedRegion());

  while( !mi.IsAtEnd() )
    {
    typename TMovingImage::PointType movingSpacePhysicalPoint;
    this->m_MovingImage->TransformIndexToPhysicalPoint
                                      (mi.GetIndex(), movingSpacePhysicalPoint);

    if ( this->m_MovingImageMask.IsNull() /* A null mask implies entire space is to be used.*/
         || this->m_MovingImageMask->IsInside(movingSpacePhysicalPoint) )
       {
       const typename TMovingImage::PixelType currentValue=mi.Get();
       // update the Moving Image true min accordingly
       if ( currentValue < this->m_MovingImageTrueMin )
         {
         this->m_MovingImageTrueMin = currentValue;
         }
       // update the Moving Image true max accordingly
       if ( currentValue > this->m_MovingImageTrueMax )
         {
         this->m_MovingImageTrueMax = currentValue;
         }
       }
      ++mi;
    }
  itkDebugMacro(" FixedImageMin: " << this->m_FixedImageTrueMin
                                   << " FixedImageMax: "
                                   << this->m_FixedImageTrueMax << std::endl);
  itkDebugMacro(" MovingImageMin: " << this->m_MovingImageTrueMin
                                    << " MovingImageMax: "
                                    << this->m_MovingImageTrueMax << std::endl);


  // Allocate memory for the joint PDF.
  this->m_JointPDF = JointPDFType::New();

  // Instantiate a region, index, size
  JointPDFRegionType jointPDFRegion;
  JointPDFIndexType  jointPDFIndex;
  JointPDFSizeType   jointPDFSize;

  // the jointPDF is of size NumberOfBins x NumberOfBins
  jointPDFSize.Fill(m_NumberOfHistogramBins);
  jointPDFIndex.Fill(0);
  jointPDFRegion.SetIndex(jointPDFIndex);
  jointPDFRegion.SetSize(jointPDFSize);

  // Set the regions and allocate
  this->m_JointPDF->SetRegions(jointPDFRegion);

  //By setting these values, the joint histogram physical locations will correspond to intensity values.
  JointPDFSpacingType spacing;
  spacing[0]=1/(InternalComputationValueType)(this->m_NumberOfHistogramBins-(InternalComputationValueType)this->m_Padding*2-1);
  spacing[1]=spacing[0];
  this->m_JointPDF->SetSpacing(spacing);
  this->m_JointPDFSpacing=this->m_JointPDF->GetSpacing();
  JointPDFPointType origin;
  origin[0]=this->m_JointPDFSpacing[0]*(InternalComputationValueType)this->m_Padding*(-1.0);
  origin[1]=origin[0];
  this->m_JointPDF->SetOrigin(origin);
  this->m_JointPDF->Allocate();
  this->m_JointPDFInterpolator->SetInputImage( this->m_JointPDF );

  // do the same thing for the marginal pdfs
  this->m_FixedImageMarginalPDF = MarginalPDFType::New();
  this->m_MovingImageMarginalPDF = MarginalPDFType::New();

  // Instantiate a region, index, size
  typedef typename MarginalPDFType::RegionType  MarginalPDFRegionType;
  typedef typename MarginalPDFType::SizeType    MarginalPDFSizeType;
  MarginalPDFRegionType marginalPDFRegion;
  MarginalPDFIndexType  marginalPDFIndex;
  MarginalPDFSizeType   marginalPDFSize;

  // the marginalPDF is of size NumberOfBins x NumberOfBins
  marginalPDFSize.Fill(m_NumberOfHistogramBins);
  marginalPDFIndex.Fill(0);
  marginalPDFRegion.SetIndex(marginalPDFIndex);
  marginalPDFRegion.SetSize(marginalPDFSize);

  // Set the regions and allocate
  this->m_FixedImageMarginalPDF->SetRegions(marginalPDFRegion);
  this->m_MovingImageMarginalPDF->SetRegions(marginalPDFRegion);

  //By setting these values, the marginal histogram physical locations will correspond to intensity values.
  typename MarginalPDFType::PointType fixedorigin;
  typename MarginalPDFType::PointType movingorigin;
  fixedorigin[0]=origin[0];
  movingorigin[0]=origin[1];
  this->m_FixedImageMarginalPDF->SetOrigin(fixedorigin);
  this->m_MovingImageMarginalPDF->SetOrigin(movingorigin);
  typename MarginalPDFType::SpacingType mspacing;
  mspacing[0]=spacing[0];
  this->m_FixedImageMarginalPDF->SetSpacing(mspacing);
  mspacing[0]=spacing[1];
  this->m_MovingImageMarginalPDF->SetSpacing(mspacing);
  this->m_FixedImageMarginalPDF->Allocate();
  this->m_MovingImageMarginalPDF->Allocate();
}


template <class TFixedImage, class TMovingImage, class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::InitializeForIteration() const
{
  Superclass::InitializeForIteration();

  /* Prepare histograms for use in GetValueAndDerivative */

  // Initialize the joint pdf and the fixed and moving image marginal pdfs
  PDFValueType pdfzero = NumericTraits< PDFValueType >::Zero;
  this->m_JointPDF->FillBuffer(pdfzero);
  this->m_FixedImageMarginalPDF->FillBuffer(pdfzero);
  this->m_MovingImageMarginalPDF->FillBuffer(pdfzero);

  /**
   * First, we compute the joint histogram
   */
  if( this->m_UseFixedSampledPointSet )
    {
    typename JointHistogramMutualInformationSparseComputeJointPDFThreader::DomainType sampledRange;
    sampledRange[0] = 0;
    sampledRange[1] = this->m_VirtualSampledPointSet->GetNumberOfPoints() - 1;
    this->m_JointHistogramMutualInformationSparseComputeJointPDFThreader->Execute( const_cast<Self *>(this), sampledRange );
    }
  else
    {
    this->m_JointHistogramMutualInformationDenseComputeJointPDFThreader->Execute( const_cast<Self *>(this), this->GetVirtualDomainRegion() );
    }

  // Optionally smooth the joint pdf
  if (this->m_VarianceForJointPDFSmoothing > NumericTraits< JointPDFValueType >::Zero )
    {
    typedef DiscreteGaussianImageFilter<JointPDFType,JointPDFType> DgType;
    typename DgType::Pointer dg = DgType::New();
    dg->SetInput(this->m_JointPDF);
    dg->SetVariance(this->m_VarianceForJointPDFSmoothing);
    dg->SetUseImageSpacingOff();
    dg->SetMaximumError(.01f);
    dg->Update();
    this->m_JointPDF = ( dg->GetOutput() );
    }

  // Compute moving image marginal PDF by summing over fixed image bins.
  typedef ImageLinearIteratorWithIndex<JointPDFType> JointPDFLinearIterator;
  JointPDFLinearIterator linearIter(m_JointPDF, m_JointPDF->GetBufferedRegion() );
  linearIter.SetDirection( 0 );
  linearIter.GoToBegin();
  unsigned int fixedIndex = 0;
  while( !linearIter.IsAtEnd() )
    {
    InternalComputationValueType sum = NumericTraits< InternalComputationValueType >::Zero;
    while( !linearIter.IsAtEndOfLine() )
      {
      sum += linearIter.Get();
      ++linearIter;
      }
    MarginalPDFIndexType mind;
    mind[0] = fixedIndex;
    m_FixedImageMarginalPDF->SetPixel(mind,static_cast<PDFValueType>(sum));
    linearIter.NextLine();
    ++fixedIndex;
    }

  linearIter.SetDirection( 1 );
  linearIter.GoToBegin();
  unsigned int movingIndex = 0;
  while( !linearIter.IsAtEnd() )
    {
    InternalComputationValueType sum = NumericTraits< InternalComputationValueType >::Zero;
    while( !linearIter.IsAtEndOfLine() )
      {
      sum += linearIter.Get();
      ++linearIter;
      }
    MarginalPDFIndexType mind;
    mind[0] = movingIndex;
    m_MovingImageMarginalPDF->SetPixel(mind,static_cast<PDFValueType>(sum));
    linearIter.NextLine();
    ++movingIndex;
    }

  // Calculate value
  this->m_Value = this->GetValue();
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
template <class TDomainPartitioner >
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationComputeJointPDFThreader<TDomainPartitioner>
::BeforeThreadedExecution()
{
  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  this->m_JointHistogramPerThread.resize( this->GetNumberOfThreadsUsed() );
  this->m_JointHistogramCountPerThread.resize( this->GetNumberOfThreadsUsed() );
  for( ThreadIdType i = 0; i < this->GetNumberOfThreadsUsed(); ++i )
    {
    if( this->m_JointHistogramPerThread[i].IsNull() )
      {
      this->m_JointHistogramPerThread[i] = JointHistogramType::New();
      }
    this->m_JointHistogramPerThread[i]->CopyInformation( self->m_JointPDF );
    this->m_JointHistogramPerThread[i]->Allocate();
    this->m_JointHistogramPerThread[i]->FillBuffer( NumericTraits< SizeValueType >::Zero );
    this->m_JointHistogramCountPerThread[i] = NumericTraits< SizeValueType >::Zero;
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
template <class TDomainPartitioner >
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationComputeJointPDFThreader<TDomainPartitioner>
::ProcessPoint( const VirtualIndexType & virtualIndex,
                const VirtualPointType & virtualPoint,
                const ThreadIdType threadId )
{
  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  typename EnclosingClassType::Superclass::FixedOutputPointType  mappedFixedPoint;
  typename EnclosingClassType::Superclass::FixedImagePixelType   fixedImageValue;
  FixedImageGradientType                                         fixedImageGradients;
  typename EnclosingClassType::Superclass::MovingOutputPointType mappedMovingPoint;
  typename EnclosingClassType::Superclass::MovingImagePixelType  movingImageValue;
  MovingImageGradientType                                        movingImageGradients;
  bool                                                           pointIsValid = false;

  try
    {
    pointIsValid = self->TransformAndEvaluateFixedPoint( virtualIndex,
                                          virtualPoint,
                                          false /*compute gradient*/,
                                          mappedFixedPoint,
                                          fixedImageValue,
                                          fixedImageGradients );
    if( pointIsValid )
      {
      pointIsValid = self->TransformAndEvaluateMovingPoint( virtualIndex,
                                            virtualPoint,
                                            false /*compute gradient*/,
                                            mappedMovingPoint,
                                            movingImageValue,
                                            movingImageGradients );
      }
    }
  catch( ExceptionObject & exc )
    {
    //NOTE: there must be a cleaner way to do this:
    std::string msg("Caught exception: \n");
    msg += exc.what();
    ExceptionObject err(__FILE__, __LINE__, msg);
    throw err;
    }

  /** Add the paired intensity points to the joint histogram */
  JointPDFPointType jointPDFpoint;
  self->ComputeJointPDFPoint( fixedImageValue, movingImageValue, jointPDFpoint );
  JointPDFIndexType jointPDFIndex;
  this->m_JointHistogramPerThread[threadId]->TransformPhysicalPointToIndex( jointPDFpoint, jointPDFIndex );
  this->m_JointHistogramPerThread[threadId]->GetPixel( jointPDFIndex )++;
  this->m_JointHistogramCountPerThread[threadId]++;
}


template <class TFixedImage,class TMovingImage,class TVirtualImage>
template <class TDomainPartitioner >
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationComputeJointPDFThreader<TDomainPartitioner>
::AfterThreadedExecution()
{
  const ThreadIdType numberOfThreadsUsed = this->GetNumberOfThreadsUsed();

  InternalComputationValueType totalHistogramCount = NumericTraits< InternalComputationValueType >::Zero;
  for( ThreadIdType i = 0; i < numberOfThreadsUsed; ++i )
    {
    totalHistogramCount += static_cast< InternalComputationValueType >( this->m_JointHistogramCountPerThread[i] );
    }

  itkAssertInDebugAndIgnoreInReleaseMacro( totalHistogramCount > NumericTraits< SizeValueType >::Zero );

  typedef ImageRegionIterator< JointPDFType > JointPDFIteratorType;
  JointPDFIteratorType jointPDFIt( this->m_EnclosingClass->m_JointPDF, this->m_EnclosingClass->m_JointPDF->GetBufferedRegion() );
  jointPDFIt.GoToBegin();
  typedef ImageRegionConstIterator< JointHistogramType > JointHistogramIteratorType;
  std::vector< JointHistogramIteratorType > jointHistogramPerThreadIts;
  for( ThreadIdType i = 0; i < numberOfThreadsUsed; ++i )
    {
    jointHistogramPerThreadIts.push_back( JointHistogramIteratorType( this->m_JointHistogramPerThread[i],
        this->m_JointHistogramPerThread[i]->GetBufferedRegion() ) );
    jointHistogramPerThreadIts[i].GoToBegin();
    }

  while( !jointPDFIt.IsAtEnd() )
    {
    for( ThreadIdType i = 0; i < numberOfThreadsUsed; ++i )
      {
      jointPDFIt.Value() += static_cast< JointPDFValueType >( jointHistogramPerThreadIts[i].Get() );
      ++jointHistogramPerThreadIts[i];
      }
    jointPDFIt.Value() /= totalHistogramCount;
    ++jointPDFIt;
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::JointHistogramMutualInformationDenseComputeJointPDFThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & imageSubRegion,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typedef ImageRegionConstIteratorWithIndex< VirtualImageType > IteratorType;
  IteratorType it( enclosingClass->m_VirtualDomainImage, imageSubRegion );
  for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
    enclosingClass->m_VirtualDomainImage->TransformIndexToPhysicalPoint( virtualIndex, virtualPoint );
    this->ProcessPoint( virtualIndex, virtualPoint, threadId );
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::JointHistogramMutualInformationSparseComputeJointPDFThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & indexSubRange,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typedef typename VirtualSampledPointSetType::MeshTraits::PointIdentifier ElementIdentifierType;
  const ElementIdentifierType begin = indexSubRange[0];
  const ElementIdentifierType end   = indexSubRange[1];
  for( ElementIdentifierType i = begin; i <= end; ++i )
    {
    virtualPoint = enclosingClass->m_VirtualSampledPointSet->GetPoint( i );
    enclosingClass->m_VirtualDomainImage->TransformPhysicalPointToIndex( virtualPoint, virtualIndex );
    this->ProcessPoint( virtualIndex, virtualPoint, threadId );
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
template <class TDomainPartitioner >
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationGetValueAndDerivativeThreader<TDomainPartitioner>
::BeforeThreadedExecution()
{
  Superclass::BeforeThreadedExecution();

  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  this->m_JointPDFInterpolatorPerThread.resize( this->GetNumberOfThreadsUsed() );
  this->m_FixedImageMarginalPDFInterpolatorPerThread.resize( this->GetNumberOfThreadsUsed() );
  this->m_MovingImageMarginalPDFInterpolatorPerThread.resize( this->GetNumberOfThreadsUsed() );

  for( ThreadIdType i = 0; i < this->GetNumberOfThreadsUsed(); ++i )
    {
    if( this->m_JointPDFInterpolatorPerThread[i].IsNull() )
      {
      this->m_JointPDFInterpolatorPerThread[i] = JointPDFInterpolatorType::New();
      }
    this->m_JointPDFInterpolatorPerThread[i]->SetInputImage( self->m_JointPDF );
    if( this->m_FixedImageMarginalPDFInterpolatorPerThread[i].IsNull() )
      {
      this->m_FixedImageMarginalPDFInterpolatorPerThread[i] = MarginalPDFInterpolatorType::New();
      }
    this->m_FixedImageMarginalPDFInterpolatorPerThread[i]->SetInputImage( self->m_FixedImageMarginalPDF );
    if( this->m_MovingImageMarginalPDFInterpolatorPerThread[i].IsNull() )
      {
      this->m_MovingImageMarginalPDFInterpolatorPerThread[i] = MarginalPDFInterpolatorType::New();
      }
    this->m_MovingImageMarginalPDFInterpolatorPerThread[i]->SetInputImage( self->m_MovingImageMarginalPDF );
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
template < class TDomainPartitioner >
bool
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationGetValueAndDerivativeThreader< TDomainPartitioner >
::ProcessPoint(
                    const VirtualPointType &,
                    const FixedImagePointType &,
                    const FixedImagePixelType &     fixedImageValue,
                    const FixedImageGradientType &,
                    const MovingImagePointType &    mappedMovingPoint,
                    const MovingImagePixelType &    movingImageValue,
                    const MovingImageGradientType & movingImageGradient,
                    MeasureType &,
                    DerivativeType &                localDerivativeReturn,
                    const ThreadIdType              threadID) const
{
  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  // check that the moving image sample is within the range of the true min
  // and max, hence being within the moving image mask
  if ( movingImageValue < self->m_MovingImageTrueMin )
    {
    return false;
    }
  else if ( movingImageValue > self->m_MovingImageTrueMax )
    {
    return false;
    }
  /** the scalingfactor is the MI specific scaling of the image gradient and jacobian terms */
  InternalComputationValueType scalingfactor = NumericTraits< InternalComputationValueType >::Zero; // for scaling the jacobian terms

  JointPDFPointType jointPDFpoint;
  bool pointok = self->ComputeJointPDFPoint( fixedImageValue, movingImageValue, jointPDFpoint );
  if ( !pointok )
    {
    return false;
    }
  InternalComputationValueType jointPDFValue = this->m_JointPDFInterpolatorPerThread[threadID]->Evaluate( jointPDFpoint );
  SizeValueType ind = 1;
  InternalComputationValueType dJPDF = this->ComputeJointPDFDerivative( jointPDFpoint, threadID , ind );
  typename MarginalPDFType::PointType mind;
  mind[0] = jointPDFpoint[ind];
  InternalComputationValueType movingImagePDFValue =
    this->m_MovingImageMarginalPDFInterpolatorPerThread[threadID]->Evaluate(mind);
  InternalComputationValueType dMmPDF =
    this->ComputeMovingImageMarginalPDFDerivative( mind , threadID );

  InternalComputationValueType term1 = NumericTraits< InternalComputationValueType >::Zero;
  InternalComputationValueType term2 = NumericTraits< InternalComputationValueType >::Zero;
  InternalComputationValueType eps = 1.e-16;
  if( jointPDFValue > eps &&  movingImagePDFValue > eps )
    {
    const InternalComputationValueType pRatio =
                            vcl_log(jointPDFValue)-vcl_log(movingImagePDFValue);
    term1 = dJPDF*pRatio;
    term2 = self->m_Log2 * dMmPDF * jointPDFValue / movingImagePDFValue;
    scalingfactor =  ( term2 - term1 );
    }  // end if-block to check non-zero bin contribution
  else
    {
    scalingfactor = NumericTraits< InternalComputationValueType >::Zero;
    }

  /* Use a pre-allocated jacobian object for efficiency */
  FixedTransformJacobianType & jacobian =
    const_cast< FixedTransformJacobianType &   >(this->m_MovingTransformJacobianPerThread[threadID]);

  /** For dense transforms, this returns identity */
  self->m_MovingTransform->ComputeJacobianWithRespectToParameters(
                                                            mappedMovingPoint,
                                                            jacobian);

  // this correction is necessary for consistent derivatives across N threads
  typedef typename DerivativeType::ValueType    DerivativeValueType;
  DerivativeValueType floatingpointcorrectionresolution = 10000.0;
  for ( NumberOfParametersType par = 0; par < self->GetNumberOfLocalParameters(); par++ )
    {
    InternalComputationValueType sum = NumericTraits< InternalComputationValueType >::Zero;
    for ( SizeValueType dim = 0; dim < MovingImageDimension; dim++ )
      {
      sum += scalingfactor * jacobian(dim, par) * movingImageGradient[dim];
      }
    localDerivativeReturn[par] = sum;
    intmax_t test = static_cast<intmax_t>
             ( localDerivativeReturn[par] * floatingpointcorrectionresolution );
    localDerivativeReturn[par] = static_cast<DerivativeValueType>
                                   ( test / floatingpointcorrectionresolution );
    }
  return true;
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::JointHistogramMutualInformationDenseGetValueAndDerivativeThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & imageSubRegion,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typename VirtualImageType::ConstPointer virtualImage = enclosingClass->GetVirtualDomainImage();
  typedef ImageRegionConstIteratorWithIndex< VirtualImageType > IteratorType;
  IteratorType it( virtualImage, imageSubRegion );
  for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
    virtualImage->TransformIndexToPhysicalPoint( virtualIndex, virtualPoint );
    this->ProcessVirtualPoint( virtualIndex, virtualPoint, threadId );
    }
}

template <class TFixedImage,class TMovingImage,class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::JointHistogramMutualInformationSparseGetValueAndDerivativeThreader
::ThreadedExecution( EnclosingClassType * enclosingClass,
                     const DomainType & indexSubRange,
                     const ThreadIdType threadId )
{
  VirtualPointType virtualPoint;
  VirtualIndexType virtualIndex;
  typename VirtualImageType::ConstPointer virtualImage                     = enclosingClass->GetVirtualDomainImage();
  typename VirtualSampledPointSetType::ConstPointer virtualSampledPointSet = enclosingClass->GetVirtualSampledPointSet();
  typedef typename VirtualSampledPointSetType::MeshTraits::PointIdentifier ElementIdentifierType;
  const ElementIdentifierType begin = indexSubRange[0];
  const ElementIdentifierType end   = indexSubRange[1];
  for( ElementIdentifierType i = begin; i <= end; ++i )
    {
    virtualPoint = virtualSampledPointSet->GetPoint( i );
    virtualImage->TransformPhysicalPointToIndex( virtualPoint, virtualIndex );
    this->ProcessVirtualPoint( virtualIndex, virtualPoint, threadId );
    }
}

template <class TFixedImage, class TMovingImage, class TVirtualImage>
typename JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>::MeasureType
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage,TMovingImage,TVirtualImage>
::GetValue() const
{
  /**
  1- The padding is 2 in this implementation.
  2- The MI energy is bounded in the range of [0  min(H(x),H(y))].
  3- The ComputeMutualInformation() iterator range should cover the entire PDF.
  4- The normalization is done based on NumberOfHistogramBins-1 instead of NumberOfHistogramBins. */
  InternalComputationValueType px,py,pxy;
  InternalComputationValueType total_mi = NumericTraits< InternalComputationValueType >::Zero;
  InternalComputationValueType local_mi;
  InternalComputationValueType eps =
                        NumericTraits<InternalComputationValueType>::epsilon();
  typename JointPDFType::IndexType index;
  for (SizeValueType ii = 0; ii<m_NumberOfHistogramBins; ii++)
    {
    MarginalPDFIndexType mind;
    mind[0] = ii;
    px = this->m_FixedImageMarginalPDF->GetPixel(mind);
    for (SizeValueType jj = 0; jj<m_NumberOfHistogramBins; jj++)
      {
      mind[0] = jj;
      py = this->m_MovingImageMarginalPDF->GetPixel(mind);
      InternalComputationValueType denom = px * py;
      index[0] = ii;
      index[1] = jj;
      pxy = m_JointPDF->GetPixel(index);
      local_mi = 0;
      if ( fabs(denom) > eps )
        {
        if (pxy / denom > eps )
          {
          //the classic mi calculation
          local_mi = pxy * vcl_log( pxy / denom );
          }
        }
      total_mi += local_mi;
      } // over jh bins 2
    } // over jh bins 1
  return ( -1.0 * total_mi / this->m_Log2  );
}

template <class TFixedImage, class TMovingImage, class TVirtualImage>
bool
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::ComputeJointPDFPoint( const FixedImagePixelType fixedImageValue,
                        const MovingImagePixelType movingImageValue,
                        JointPDFPointType& jointPDFpoint ) const
{
    InternalComputationValueType a =
        ( fixedImageValue - this->m_FixedImageTrueMin ) /
          ( this->m_FixedImageTrueMax - this->m_FixedImageTrueMin );
    InternalComputationValueType b =
        ( movingImageValue - this->m_MovingImageTrueMin ) /
           ( this->m_MovingImageTrueMax - this->m_MovingImageTrueMin );
    jointPDFpoint[0] = a;
    jointPDFpoint[1] = b;
    return this->m_JointPDFInterpolator->IsInsideBuffer( jointPDFpoint );
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
template < class TDomainPartitioner >
typename JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::InternalComputationValueType
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationGetValueAndDerivativeThreader< TDomainPartitioner >
::ComputeFixedImageMarginalPDFDerivative(
                                        const MarginalPDFPointType & margPDFpoint,
                                        const ThreadIdType threadID ) const
{
  InternalComputationValueType offset = 0.5*this->m_JointPDFSpacing[0];
  InternalComputationValueType eps = this->m_JointPDFSpacing[0];
  MarginalPDFPointType         leftpoint = margPDFpoint;
  leftpoint[0] -= offset;
  MarginalPDFPointType  rightpoint = margPDFpoint;
  rightpoint[0] += offset;
  if (leftpoint[0] < eps )
    {
    leftpoint[0] = eps;
    }
  if (rightpoint[0] < eps )
    {
    rightpoint[0] = eps;
    }
  if (leftpoint[0] > 1.0 )
    {
    leftpoint[0] = 1.0;
    }
  if (rightpoint[0] > 1.0 )
    {
    rightpoint[0] = 1.0;
    }
  InternalComputationValueType delta = rightpoint[0]-leftpoint[0];
  if ( delta > NumericTraits< InternalComputationValueType >::Zero )
    {
    InternalComputationValueType deriv = this->m_ThreaderFixedImageMarginalPDFInterpolator[threadID]->Evaluate(rightpoint) -
      this->m_ThreaderFixedImageMarginalPDFInterpolator[threadID]->Evaluate(leftpoint);
    return deriv/delta;
    }
  else
    {
    return NumericTraits< InternalComputationValueType >::Zero;
    }
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
template < class TDomainPartitioner >
typename JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::InternalComputationValueType
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationGetValueAndDerivativeThreader< TDomainPartitioner >
::ComputeMovingImageMarginalPDFDerivative(
                                        const MarginalPDFPointType & margPDFpoint,
                                        const ThreadIdType threadID ) const
{
  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  InternalComputationValueType offset = 0.5*self->m_JointPDFSpacing[0];
  InternalComputationValueType eps = self->m_JointPDFSpacing[0];
  MarginalPDFPointType  leftpoint = margPDFpoint;
  leftpoint[0] -= offset;
  MarginalPDFPointType  rightpoint = margPDFpoint;
  rightpoint[0] += offset;
  if( leftpoint[0] < eps )
    {
    leftpoint[0] = eps;
    }
  if( rightpoint[0] < eps )
    {
    rightpoint[0] = eps;
    }
  if( leftpoint[0] > 1.0 )
    {
    leftpoint[0] = 1.0;
    }
  if( rightpoint[0] > 1.0  )
    {
    rightpoint[0] = 1.0;
    }
  InternalComputationValueType delta = rightpoint[0] - leftpoint[0];
  if ( delta > NumericTraits< InternalComputationValueType >::Zero )
    {
    InternalComputationValueType deriv =
      this->m_MovingImageMarginalPDFInterpolatorPerThread[threadID]->Evaluate(rightpoint) -
      this->m_MovingImageMarginalPDFInterpolatorPerThread[threadID]->Evaluate(leftpoint);
    return deriv/delta;
    }
  else
    {
    return NumericTraits< InternalComputationValueType >::Zero;
    }
}

template <class TFixedImage, class TMovingImage, class TVirtualImage>
template < class TDomainPartitioner >
typename JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::InternalComputationValueType
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::JointHistogramMutualInformationGetValueAndDerivativeThreader< TDomainPartitioner >
::ComputeJointPDFDerivative( const JointPDFPointType & jointPDFpoint,
                             const ThreadIdType threadID,
                             const SizeValueType ind ) const
{
  typedef JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage> JointHistogramMIMetricType;
  JointHistogramMIMetricType * self = dynamic_cast< JointHistogramMIMetricType * >( this->m_EnclosingClass );

  InternalComputationValueType offset = 0.5*self->m_JointPDFSpacing[ind];
  InternalComputationValueType eps = self->m_JointPDFSpacing[ind];
  JointPDFPointType  leftpoint = jointPDFpoint;
  leftpoint[ind] -= offset;
  JointPDFPointType  rightpoint = jointPDFpoint;
  rightpoint[ind] += offset;

  if (leftpoint[ind] < eps )
    {
    leftpoint[ind] = eps;
    }

  if (rightpoint[ind] < eps )
    {
    rightpoint[ind] = eps;
    }

  if (leftpoint[ind] > 1.0 )
    {
    leftpoint[ind] = 1.0;
    }

  if (rightpoint[ind] > 1.0 )
    {
    rightpoint[ind] = 1.0;
    }

  InternalComputationValueType delta = rightpoint[ind] - leftpoint[ind];
  InternalComputationValueType deriv = NumericTraits< InternalComputationValueType >::Zero;
  if ( delta > NumericTraits< InternalComputationValueType >::Zero )
    {
    deriv = this->m_JointPDFInterpolatorPerThread[threadID]->Evaluate(rightpoint)-
          this->m_JointPDFInterpolatorPerThread[threadID]->Evaluate(leftpoint);
    return deriv/delta;
    }
  return deriv;
}

/** Print function */
template <class TFixedImage, class TMovingImage, class TVirtualImage>
void
JointHistogramMutualInformationImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::PrintSelf (std::ostream & os, Indent indent) const
{
  // Print the superclass
  Superclass::Print(os);

  os << indent << "NumberOfHistogramBins: ";
  os << this->m_NumberOfHistogramBins << std::endl;
  os << indent << "MovingImageTrueMin: ";
  os << this->m_MovingImageTrueMin << std::endl;
  os << indent << "MovingImageTrueMax: ";
  os << this->m_MovingImageTrueMax << std::endl;
  os << indent << "FixedImageBinSize: ";
  os << this->m_FixedImageBinSize << std::endl;
  os << indent << "MovingImageBinSize: ";
  os << this->m_MovingImageBinSize << std::endl;

  if( this->m_JointPDF.IsNotNull() )
    {
    os << indent << "JointPDF: ";
    os << this->m_JointPDF << std::endl;
    }
}

} // end namespace itk

#endif
