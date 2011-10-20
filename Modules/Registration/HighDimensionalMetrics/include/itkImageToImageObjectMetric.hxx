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
#ifndef __itkImageToImageObjectMetric_hxx
#define __itkImageToImageObjectMetric_hxx

#include "itkImageToImageObjectMetric.h"
#include "itkPixelTraits.h"
#include "itkDisplacementFieldTransform.h"
#include "itkCompositeTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkIdentityTransform.h"
#include "itkCentralDifferenceImageFunction.h"

namespace itk
{

template<class TFixedImage,class TMovingImage,class TVirtualImage>
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ImageToImageObjectMetric()
{
  /* Instantiate the default dense threader, and set the callback and user-data
   * holder for threading */
  this->m_DenseValueAndDerivativeThreader = DenseValueAndDerivativeThreaderType::New();
  this->m_DenseValueAndDerivativeThreader->SetThreadedGenerateData(
    Self::DenseGetValueAndDerivativeThreadedCallback );
  this->m_DenseValueAndDerivativeThreader->SetHolder( this );

  /* Instantiate the default sampling threader, and set the callback and user-data
   * holder for threading */
  this->m_SampledValueAndDerivativeThreader =
    SampledValueAndDerivativeThreaderType::New();
  this->m_SampledValueAndDerivativeThreader->SetThreadedGenerateData(
    Self::SampledGetValueAndDerivativeThreadedCallback );
  this->m_SampledValueAndDerivativeThreader->SetHolder( this );

  this->m_ThreadingMemoryHasBeenInitialized = false;

  /* Both transforms default to an identity transform */
  typedef IdentityTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingImageDimension ) >
                                          MovingIdentityTransformType;
  typedef IdentityTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingImageDimension ) >
                                          FixedIdentityTransformType;
  this->m_FixedTransform = FixedIdentityTransformType::New();
  this->m_MovingTransform = MovingIdentityTransformType::New();

  /* Interpolators. Default to linear. */
  typedef LinearInterpolateImageFunction< FixedImageType,
                                          CoordinateRepresentationType >
                                                  FixedLinearInterpolatorType;
  typedef LinearInterpolateImageFunction< MovingImageType,
                                          CoordinateRepresentationType >
                                                  MovingLinearInterpolatorType;
  this->m_FixedInterpolator = FixedLinearInterpolatorType::New();
  this->m_MovingInterpolator = MovingLinearInterpolatorType::New();

  /* Setup default gradient filter. It gets initialized with default
   * parameters during Initialize. */
  this->m_DefaultFixedImageGradientFilter = DefaultFixedImageGradientFilter::New();
  this->m_DefaultMovingImageGradientFilter = DefaultMovingImageGradientFilter::New();
  this->m_FixedImageGradientFilter = this->m_DefaultFixedImageGradientFilter;
  this->m_MovingImageGradientFilter = this->m_DefaultMovingImageGradientFilter;

  /* Setup default gradient image function */
  typedef CentralDifferenceImageFunction<FixedImageType,
                                         CoordinateRepresentationType>
                                          FixedCentralDifferenceCalculatorType;
  typedef CentralDifferenceImageFunction<MovingImageType,
                                         CoordinateRepresentationType>
                                          MovingCentralDifferenceCalculatorType;
  typename FixedCentralDifferenceCalculatorType::Pointer
                  fixedCalculator = FixedCentralDifferenceCalculatorType::New();
  fixedCalculator->UseImageDirectionOn();
  this->m_FixedImageGradientCalculator = fixedCalculator;
  typename MovingCentralDifferenceCalculatorType::Pointer
                  movingCalculator = MovingCentralDifferenceCalculatorType::New();
  movingCalculator->UseImageDirectionOn();
  this->m_MovingImageGradientCalculator = movingCalculator;

  /* Setup default options assuming dense-sampling */
  this->m_DoFixedImagePreWarp = true;
  this->m_DoMovingImagePreWarp = true;
  this->m_UseFixedImageGradientFilter = true;
  this->m_UseMovingImageGradientFilter = true;
  this->m_UseFixedSampledPointSet = false;

  this->m_UserHasProvidedVirtualDomainImage = false;
  this->m_NumberOfThreadsHasBeenInitialized = false;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::~ImageToImageObjectMetric()
{
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::Initialize() throw ( itk::ExceptionObject )
{
  itkDebugMacro("Initialize entered");

  /* Verify things are connected */
  if ( !this->m_FixedImage )
    {
    itkExceptionMacro(<< "FixedImage is not present");
    }
  if ( !this->m_MovingImage )
    {
    itkExceptionMacro(<< "MovingImage is not present");
    }
  if ( !this->m_FixedTransform )
    {
    itkExceptionMacro(<< "FixedTransform is not present");
    }
  if ( !this->m_MovingTransform )
    {
    itkExceptionMacro(<< "MovingTransform is not present");
    }

  // If the image is provided by a source, update the source.
  if ( this->m_MovingImage->GetSource() )
    {
    this->m_MovingImage->GetSource()->Update();
    }

  // If the image is provided by a source, update the source.
  if ( this->m_FixedImage->GetSource() )
    {
    this->m_FixedImage->GetSource()->Update();
    }

  /* If a virtual image has not been set or created,
   * create one from fixed image settings */
  if( ! this->m_UserHasProvidedVirtualDomainImage )
    {
    /* Instantiate a virtual image, but do not call Allocate to allocate
     * the data, to save memory. We don't need data. We'll simply be iterating
     * over the image to get indecies and transform to points.
     * Note that it will be safer to have a dedicated VirtualImage class
     * that prevents accidental access of data. */
    /* Just copy information from fixed image */
    this->m_VirtualDomainImage = VirtualImageType::New();
    this->m_VirtualDomainImage->CopyInformation( this->m_FixedImage );
    /* CopyInformation does not copy buffered region */
    this->m_VirtualDomainImage->SetBufferedRegion(
      this->m_FixedImage->GetBufferedRegion() );
    this->m_VirtualDomainImage->SetRequestedRegion(
      this->m_FixedImage->GetRequestedRegion() );
    }

  /* Map the fixed samples into the virtual domain and store in
   * a searpate point set. */
  if( this->m_UseFixedSampledPointSet )
    {
    this->MapFixedSampledPointSetToVirtual();
    }

  /* Special checks for when the moving transform is dense/high-dimensional */
  if( this->m_MovingTransform->HasLocalSupport() )
    {
    /* Verify that virtual domain and displacement field are the same size
    * and in the same physical space. Handles CompositeTransform by checking
    * if first applied transform is DisplacementFieldTransform */
    this->VerifyDisplacementFieldSizeAndPhysicalSpace();

    /* Verify virtual image pixel type is scalar. Effects calc of offset
    in StoreDerivativeResult.
    NOTE:  Can this be checked at compile time? ConceptChecking has a
    HasPixelTraits class, but looks like it just verifies that type T
    has PixelTraits associated with it, and not a particular value. */
    if( PixelTraits< VirtualImagePixelType >::Dimension != 1 )
      {
      itkExceptionMacro("VirtualImagePixelType must be scalar for use "
                        "with high-dimensional transform. "
                        "Dimensionality is " <<
                        PixelTraits< VirtualImagePixelType >::Dimension );
      }
    }

  /*
   * Determine number of threads that will be used
   */

  /* Assign the virtual image region to the threader. Do this before
   * calling DetermineNumberOfThreadsToUse. */
  this->m_DenseValueAndDerivativeThreader->SetOverallObject(
                                            this->GetVirtualDomainRegion() );

  /* Assign the range to the sampling threader. */
  if( this->m_UseFixedSampledPointSet )
    {
    if( this->m_FixedSampledPointSet->GetNumberOfPoints() < 1 )
      {
      itkExceptionMacro("FixedSampledPointSet must have 1 or more points.");
      }
    SampledThreaderInputObjectType range;
    range[0] = 0;
    range[1] = this->m_FixedSampledPointSet->GetNumberOfPoints() - 1;
    this->m_SampledValueAndDerivativeThreader->SetOverallObject( range );
    }

  /* Determine how many threads will be used, given the set OverallObject.
   * The threader uses
   * its SplitRequestedObject method to split the image region over threads,
   * and it may decide to that using fewer than the number of available
   * threads is more effective. */
  ThreadIdType numThreads =
    this->m_DenseValueAndDerivativeThreader->DetermineNumberOfThreadsToUse();
  if( this->m_UseFixedSampledPointSet )
    {
    numThreads =
      this->m_SampledValueAndDerivativeThreader->DetermineNumberOfThreadsToUse();
    }
  this->SetNumberOfThreads( numThreads );
  this->m_NumberOfThreadsHasBeenInitialized = true;

  /* Clear this flag to force initialization of threading memory
   * in GetValueAndDerivativeThreadedExecute. */
  this->m_ThreadingMemoryHasBeenInitialized = false;

  /* Inititialize interpolators. */
  itkDebugMacro("Initialize Interpolators");
  this->m_FixedInterpolator->SetInputImage( this->m_FixedImage );
  this->m_MovingInterpolator->SetInputImage( this->m_MovingImage );

  /* Setup for image gradient calculations.
   * If pre-warping is enabled, the
   * calculator will be pointed to the warped image at time of warping. */
  if( ! this->m_UseFixedImageGradientFilter )
    {
    itkDebugMacro("Initialize FixedImageGradientCalculator");
    this->m_FixedImageGradientImage = NULL;
    this->m_FixedImageGradientCalculator->SetInputImage(this->m_FixedImage);
    }
  if( ! this->m_UseMovingImageGradientFilter )
    {
    itkDebugMacro("Initialize MovingImageGradientCalculator");
    this->m_MovingImageGradientImage = NULL;
    this->m_MovingImageGradientCalculator->SetInputImage(this->m_MovingImage);
    }

  /* Initialize resample image filters for pre-warping images if
   * option is set.
   * The proper number of threads is required. */
  if( this->m_DoFixedImagePreWarp )
    {
    this->m_FixedWarpResampleImageFilter = FixedWarpResampleImageFilterType::New();
    this->m_FixedWarpResampleImageFilter->SetOutputParametersFromImage(
                                                this->GetVirtualDomainImage() );
    this->m_FixedWarpResampleImageFilter->SetNumberOfThreads(
                                                    this->GetNumberOfThreads() );
    this->m_FixedWarpResampleImageFilter->SetTransform(
                                                    this->GetFixedTransform() );
    this->m_FixedWarpResampleImageFilter->SetInput( this->GetFixedImage() );

    /* Pre-warp the fixed image now so it's available below if
     * m_UseMovingImageGradientFilter is enabled.
     * Also, fixed images are currently never optimized, so we only
     * have to prewarp once, so do it here. */
    itkDebugMacro("Init: DoFixedImagePreWarp.");
    this->DoFixedImagePreWarp();
    }
  else
    {
    /* Free memory if allocated from a previous run */
    this->m_FixedWarpedImage = NULL;
    }

  if( this->m_DoMovingImagePreWarp )
    {
    this->m_MovingWarpResampleImageFilter =
                                      MovingWarpResampleImageFilterType::New();
    this->m_MovingWarpResampleImageFilter->SetOutputParametersFromImage(
                                                this->GetVirtualDomainImage() );
    this->m_MovingWarpResampleImageFilter->SetNumberOfThreads(
                                               this->GetNumberOfThreads() );
    this->m_MovingWarpResampleImageFilter->SetTransform(
                                               this->GetMovingTransform() );
    this->m_MovingWarpResampleImageFilter->SetInput( this->GetMovingImage() );

    /* Pre-warp the moving image, for use when a derived class needs it
     * before InitiateForIteration is called. */
    this->DoMovingImagePreWarp();
    }
  else
    {
    /* Free memory if allocated from a previous run */
    this->m_MovingWarpedImage = NULL;
    }

  /* Initialize default gradient image filters.
   * Do this after any pre-warping above. */
  itkDebugMacro("InitializeDefaultFixedImageGradientFilter");
  this->InitializeDefaultFixedImageGradientFilter();
  itkDebugMacro("InitializeDefaultMovingImageGradientFilter");
  this->InitializeDefaultMovingImageGradientFilter();

  /* If user set to use a pre-calculated fixed gradient image,
   * then we need to calculate the gradient image.
   * We only need to compute once since the fixed transform isn't
   * optimized.
   * Do this *after* setting up above for pre-warping. */
  if ( this->m_UseFixedImageGradientFilter )
    {
    itkDebugMacro("Initialize: ComputeFixedImageGradientFilterImage");
    this->ComputeFixedImageGradientFilterImage();
    }

  /* Compute gradient image for moving image. Needed now for
   * derived classes that use it before InitializeForIteration is called.
   * It's also computed at begin of every iteration. */
  if( this->m_UseMovingImageGradientFilter )
    {
    itkDebugMacro("Initialize: ComputeMovingImageGradientFilterImage");
    this->ComputeMovingImageGradientFilterImage();
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetValueAndDerivativeThreadedExecute( DerivativeType & derivativeReturn ) const
{
  //Initialize threading memory if this is the first time
  // in here since a call to Initialize, or if user has passed
  // in a different object for the results (why might they do that?).
  if( ! this->m_ThreadingMemoryHasBeenInitialized ||
      &derivativeReturn != this->m_DerivativeResult )
    {
    this->InitializeThreadingMemory( derivativeReturn );
    }

  //Initialization required for each iteration.
  this->InitializeForIteration();

  // Do the threaded evaluation. This will
  // call GetValueAndDerivativeThreadedCallback, which
  // iterates over virtual domain region and calls derived class'
  // GetValueAndDerivativeProcessPoint.
  if( this->m_UseFixedSampledPointSet )
    {
    this->m_SampledValueAndDerivativeThreader->StartThreadedExecution();
    }
  else
    {
    this->m_DenseValueAndDerivativeThreader->StartThreadedExecution();
    }

  // Determine the total number of points used during calculations.
  this->CollectNumberOfValidPoints();

  // To collect the results from each thread into final values
  // the derived class can call GetValueAndDerivativeThreadedPostProcess,
  // or do their own processing.
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::InitializeThreadingMemory( DerivativeType & derivativeReturn ) const
{
  /* Point our results object to the object provided by user. */
  this->m_DerivativeResult = &derivativeReturn;

  /* Per-thread results */
  this->m_MeasurePerThread.resize( this->GetNumberOfThreads() );
  this->m_NumberOfValidPointsPerThread.resize( this->GetNumberOfThreads() );
  this->m_DerivativesPerThread.resize( this->GetNumberOfThreads() );
  /* This one is intermediary, for getting per-point results. */
  this->m_LocalDerivativesPerThread.resize( this->GetNumberOfThreads() );
  /* Per-thread pre-allocated Jacobian objects for efficiency */
  this->m_MovingTransformJacobianPerThread.resize( this->GetNumberOfThreads() );

  /* This size always comes from the moving image */
  NumberOfParametersType globalDerivativeSize =
    this->m_MovingTransform->GetNumberOfParameters();

  itkDebugMacro("ImageToImageObjectMetric::Initialize: deriv size  "
                  << globalDerivativeSize << std::endl);
  /* NOTE: this does *not* get init'ed to 0 here. */
  if( this->m_DerivativeResult->GetSize() != globalDerivativeSize )
    {
    this->m_DerivativeResult->SetSize( globalDerivativeSize );
    }
  for (ThreadIdType i=0; i<this->GetNumberOfThreads(); i++)
    {
    /* Allocate intermediary per-thread storage used to get results from
     * derived classes */
    this->m_LocalDerivativesPerThread[i].SetSize(
                                          this->GetNumberOfLocalParameters() );
    this->m_MovingTransformJacobianPerThread[i].SetSize(
                                          this->VirtualImageDimension,
                                          this->GetNumberOfLocalParameters() );
    /* For transforms with local support, e.g. displacement field,
     * use a single derivative container that's updated by region
     * in multiple threads. */
    if ( this->m_MovingTransform->HasLocalSupport() )
      {
      itkDebugMacro(
        "ImageToImageObjectMetric::Initialize: tx HAS local support\n");
        /* Set each per-thread object to point to m_DerivativeResult */
        this->m_DerivativesPerThread[i].SetData(
                                      this->m_DerivativeResult->data_block(),
                                      this->m_DerivativeResult->Size(),
                                      false );
      }
    else
      {
      itkDebugMacro(
      "ImageToImageObjectMetric::Initialize: tx does NOT have local support\n");
      /* Global transforms get a separate derivatives container for each thread
       * that holds the result over a particular image region. */
        this->m_DerivativesPerThread[i].SetSize( globalDerivativeSize );
      }
    }
  /* This will be true until next call to Initialize */
  this->m_ThreadingMemoryHasBeenInitialized = true;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::InitializeForIteration() const
{
  /* Initialize some threading values that require per-iteration
   * initialization. */
  for (ThreadIdType i=0; i<this->GetNumberOfThreads(); i++)
    {
    this->m_NumberOfValidPointsPerThread[i] = 0;
    this->m_MeasurePerThread[i] = 0;
    if ( ! this->m_MovingTransform->HasLocalSupport() )
      {
      /* Be sure to init to 0 here, because the threader may not use
       * all the threads if the region is better split into fewer
       * subregions. */
      this->m_DerivativesPerThread[i].Fill( 0 );
      }
    }

  /* Clear derivative final result. This will
   * require an option to skip for use with multivariate metric. */
  this->m_DerivativeResult->Fill( 0 );

  /* Pre-warp the moving image if set to do so. Then we have
   * to recompute the image gradients if ImageFilter option is set.
   * Otherwise the moving image gradients only need be calculated
   * once, during initialize.
   * In contrast, the fixed image is not optimized so we only pre-warp
   * once, during Initialize. */
  if( this->m_DoMovingImagePreWarp )
    {
    this->DoMovingImagePreWarp();
    if( this->m_UseMovingImageGradientFilter )
      {
      this->ComputeMovingImageGradientFilterImage();
      }
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::CollectNumberOfValidPoints() const
{
  /* Count number of valid points.
   * Other post-processing can be done by calling
   * GetValueAndDerivativeThreadedPostProcess, or direclty
   * in the derived class. */
  this->m_NumberOfValidPoints = 0;
  for (ThreadIdType i=0; i<this->GetNumberOfThreads(); i++)
    {
    this->m_NumberOfValidPoints += this->m_NumberOfValidPointsPerThread[i];
    }
  itkDebugMacro( "ImageToImageObjectMetric: NumberOfValidPoints: "
                 << this->m_NumberOfValidPoints );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetValueAndDerivativeThreadedPostProcess( bool doAverage ) const
{
  /* For global transforms, sum the derivatives from each region. */
  if ( ! this->m_MovingTransform->HasLocalSupport() )
    {
    for (ThreadIdType i=0; i<this->GetNumberOfThreads(); i++)
      {
      *(this->m_DerivativeResult) += this->m_DerivativesPerThread[i];
      }
    }

  /* Accumulate the metric value from threads and store */
  this->m_Value = NumericTraits<InternalComputationValueType>::Zero;

  for(size_t i=0; i< this->m_MeasurePerThread.size(); i++)
    {
    this->m_Value += this->m_MeasurePerThread[i];
    }

  if( doAverage )
    {
    if ( ! this->m_MovingTransform->HasLocalSupport() )
      {
      *(this->m_DerivativeResult) /= this->m_NumberOfValidPoints;
      }
    this->m_Value /= this->m_NumberOfValidPoints;
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::DenseGetValueAndDerivativeThreadedCallback(
                const DenseThreaderInputObjectType & subRegion,
                ThreadIdType threadID,
                Self * self)
{
  SamplingIteratorHelper  iterator( self->m_VirtualDomainImage, subRegion );
  self->GetValueAndDerivativeProcessPointRange( iterator, threadID, self );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::SampledGetValueAndDerivativeThreadedCallback(
                const SampledThreaderInputObjectType & sampledRange,
                ThreadIdType threadID,
                Self * self)
{
  SamplingIteratorHelper iterator( self->m_VirtualDomainImage,
                                   self->m_VirtualSampledPointSet, sampledRange );
  self->GetValueAndDerivativeProcessPointRange( iterator, threadID, self );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetValueAndDerivativeProcessPointRange(
                SamplingIteratorHelper & samplingIterator,
                ThreadIdType threadID,
                Self * self)
{
  VirtualPointType            virtualPoint;
  VirtualIndexType            virtualIndex;
  FixedOutputPointType        mappedFixedPoint;
  FixedImagePixelType         mappedFixedPixelValue;
  FixedImageGradientType      mappedFixedImageGradient;
  MovingOutputPointType       mappedMovingPoint;
  MovingImagePixelType        mappedMovingPixelValue;
  MovingImageGradientType     mappedMovingImageGradient;
  bool                        pointIsValid = false;
  MeasureType                 metricValueResult;
  MeasureType                 metricValueSum = 0;

  /* Get pre-allocated local results object. This actually provides very
   * little benefit, since this only gets called once for each thread. However
   * if we get up to the hundres of threads, it might have an impact */
  DerivativeType & localDerivativeResult =
                                   self->m_LocalDerivativesPerThread[threadID];

  /* Iterate over the sub region */
  while( samplingIterator.GetNext( virtualIndex, virtualPoint ) )
  {
    /* Transform the point into fixed and moving spaces, and evaluate.
     * Different behavior with pre-warping enabled is handled transparently.
     * Do this in a try block to catch exceptions and print more useful info
     * then we otherwise get when exceptions are caught in MultiThreader. */
    try
      {
      self->TransformAndEvaluateFixedPoint( virtualIndex,
                                        virtualPoint,
                                        self->GetGradientSourceIncludesFixed(),
                                        mappedFixedPoint,
                                        mappedFixedPixelValue,
                                        mappedFixedImageGradient,
                                        pointIsValid );
      }
    catch( ExceptionObject & exc )
      {
      //NOTE: there must be a cleaner way to do this:
      std::string msg("Caught exception: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }

    if( !pointIsValid )
      {
      continue;
      }

    try
      {
      self->TransformAndEvaluateMovingPoint( virtualIndex,
                                      virtualPoint,
                                      self->GetGradientSourceIncludesMoving(),
                                      mappedMovingPoint,
                                      mappedMovingPixelValue,
                                      mappedMovingImageGradient,
                                      pointIsValid );
      }
    catch( ExceptionObject & exc )
      {
      std::string msg("Caught exception: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }

    if( !pointIsValid )
      {
      continue;
      }

    /* Call the user method in derived classes to do the specific
     * calculations for value and derivative. */
    try
      {
      pointIsValid = self->GetValueAndDerivativeProcessPoint(
                                     virtualPoint,
                                     mappedFixedPoint, mappedFixedPixelValue,
                                     mappedFixedImageGradient,
                                     mappedMovingPoint, mappedMovingPixelValue,
                                     mappedMovingImageGradient,
                                     metricValueResult, localDerivativeResult,
                                     threadID );
      }
    catch( ExceptionObject & exc )
      {
      //NOTE: there must be a cleaner way to do this:
      std::string msg("Exception in GetValueAndDerivativeProcessPoint:\n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }

    if( !pointIsValid )
      {
      continue;
      }

    /* Assign the results */
    self->m_NumberOfValidPointsPerThread[ threadID ]++;
    metricValueSum += metricValueResult;
    /* Store the result. The behavior depends on what type of
     * transform is being used. */
    self->StoreDerivativeResult( localDerivativeResult,
                                 virtualIndex, threadID );
  } //loop over region

  /* Store metric value result for this thread. */
  self->m_MeasurePerThread[threadID] = metricValueSum;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::StoreDerivativeResult(  DerivativeType & derivative,
                           const VirtualIndexType & virtualIndex,
                           ThreadIdType threadID )
{
  DerivativeType & derivativeResult = this->m_DerivativesPerThread[threadID];
  if ( ! this->m_MovingTransform->HasLocalSupport() )
    {
    derivativeResult += derivative;
    }
  else
    {
    // Dense transform, e.g. displacement field.
    // update derivative at some index
    // this requires the moving image displacement field to be
    // same size as virtual image, and that VirtualImage PixelType
    // is scalar.
    try
      {
      OffsetValueType offset =
        this->ComputeParameterOffsetFromVirtualDomainIndex( virtualIndex,
          this->m_MovingTransform->GetNumberOfLocalParameters() );
      for (NumberOfParametersType i=0;
            i < this->m_MovingTransform->GetNumberOfLocalParameters(); i++)
        {
        /* Be sure to *add* here and not assign. Required for proper behavior
         * with multi-variate metric. */
        derivativeResult[offset+i] += derivative[i];
        }
      }
    catch( ExceptionObject & exc )
      {
      std::string msg("Caught exception: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::TransformAndEvaluateFixedPoint(
                         const VirtualIndexType & index,
                         const VirtualPointType & virtualPoint,
                         const bool computeImageGradient,
                         FixedImagePointType & mappedFixedPoint,
                         FixedImagePixelType & mappedFixedPixelValue,
                         FixedImageGradientType & mappedFixedImageGradient,
                         bool & pointIsValid ) const
{
  pointIsValid = true;
  mappedFixedPixelValue = NumericTraits<FixedImagePixelType>::Zero;

  // map the point into fixed space
  mappedFixedPoint = this->m_FixedTransform->TransformPoint( virtualPoint );

  // check against the mask if one is assigned
  if ( this->m_FixedImageMask )
    {
    // Check if mapped point is within the support region of the fixed image
    // mask
    pointIsValid = this->m_FixedImageMask->IsInside( mappedFixedPoint );
    if( ! pointIsValid )
      {
      return;
      }
    }

  // Check if mapped point is inside image buffer
  pointIsValid = this->m_FixedInterpolator->IsInsideBuffer(mappedFixedPoint);
  if( ! pointIsValid )
    {
    return;
    }

  if( this->m_DoFixedImagePreWarp )
    {
    /* Get the pixel values at this index */
    mappedFixedPixelValue = this->m_FixedWarpedImage->GetPixel( index );
    if( computeImageGradient )
      {
      ComputeFixedImageGradientAtIndex( index, mappedFixedImageGradient );
      }
    }
  else
    {
    mappedFixedPixelValue = this->m_FixedInterpolator->Evaluate(mappedFixedPoint);
    if( computeImageGradient )
      {
      this->ComputeFixedImageGradientAtPoint( mappedFixedPoint,
                                       mappedFixedImageGradient );
      //Transform the gradient into the virtual domain. We compute gradient
      // in the fixed and moving domains and then transform to virtual.
      mappedFixedImageGradient =
        this->m_FixedTransform->TransformCovariantVector(
                                                      mappedFixedImageGradient,
                                                      mappedFixedPoint );
      }
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::TransformAndEvaluateMovingPoint(
                         const VirtualIndexType & index,
                         const VirtualPointType & virtualPoint,
                         const bool computeImageGradient,
                         MovingImagePointType & mappedMovingPoint,
                         MovingImagePixelType & mappedMovingPixelValue,
                         MovingImageGradientType & mappedMovingImageGradient,
                         bool & pointIsValid ) const
{
  pointIsValid = true;
  mappedMovingPixelValue = NumericTraits<MovingImagePixelType>::Zero;

  // map the point into moving space
  mappedMovingPoint = this->m_MovingTransform->TransformPoint( virtualPoint );

  // check against the mask if one is assigned
  if ( this->m_MovingImageMask )
    {
    // Check if mapped point is within the support region of the fixed image
    // mask
    pointIsValid = this->m_MovingImageMask->IsInside( mappedMovingPoint );
    if( ! pointIsValid )
      {
      return;
      }
    }

  // Check if mapped point is inside image buffer
  pointIsValid = this->m_MovingInterpolator->IsInsideBuffer(mappedMovingPoint);
  if( ! pointIsValid )
    {
    return;
    }

  if( this->m_DoMovingImagePreWarp )
    {
   /* Get the pixel values at this index */
    mappedMovingPixelValue = this->m_MovingWarpedImage->GetPixel( index );

    if( computeImageGradient )
      {
      ComputeMovingImageGradientAtIndex( index, mappedMovingImageGradient );
      }
    }
  else
    {
    mappedMovingPixelValue =
                      this->m_MovingInterpolator->Evaluate(mappedMovingPoint);
    if( computeImageGradient )
      {
      this->ComputeMovingImageGradientAtPoint( mappedMovingPoint,
                                        mappedMovingImageGradient );
      mappedMovingImageGradient =
        this->m_MovingTransform->TransformCovariantVector(
                                                     mappedMovingImageGradient,
                                                     mappedMovingPoint );
      }
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeFixedImageGradientAtPoint( const FixedImagePointType & mappedPoint,
                             FixedImageGradientType & gradient ) const
{
  if ( this->m_UseFixedImageGradientFilter )
    {
    ContinuousIndex< double, FixedImageDimension > tempIndex;
    this->m_FixedImage->TransformPhysicalPointToContinuousIndex(mappedPoint,
                                                           tempIndex);
    FixedImageIndexType mappedIndex;
    mappedIndex.CopyWithRound(tempIndex);
    gradient = this->m_FixedImageGradientImage->GetPixel(mappedIndex);
    }
  else
    {
    // if not using the gradient image
    gradient = this->m_FixedImageGradientCalculator->Evaluate(mappedPoint);
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeMovingImageGradientAtPoint(
                              const MovingImagePointType & mappedPoint,
                              MovingImageGradientType & gradient ) const
{
  if ( this->m_UseMovingImageGradientFilter )
    {
    ContinuousIndex< double, MovingImageDimension > tempIndex;
    this->m_MovingImage->TransformPhysicalPointToContinuousIndex(mappedPoint,
                                                           tempIndex);
    MovingImageIndexType mappedIndex;
    mappedIndex.CopyWithRound(tempIndex);
    gradient = this->m_MovingImageGradientImage->GetPixel(mappedIndex);
    }
  else
    {
    // if not using the gradient image
    gradient = this->m_MovingImageGradientCalculator->Evaluate(mappedPoint);
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeFixedImageGradientAtIndex(
                              const VirtualIndexType & index,
                              FixedImageGradientType & gradient ) const
{
  if ( this->m_UseFixedImageGradientFilter )
    {
    gradient = this->m_FixedImageGradientImage->GetPixel(index);
    }
  else
    {
    // if not using the gradient image
    gradient = this->m_FixedImageGradientCalculator->EvaluateAtIndex(index);
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeMovingImageGradientAtIndex(
                              const VirtualIndexType & index,
                              MovingImageGradientType & gradient ) const
{
  if ( this->m_UseMovingImageGradientFilter )
    {
    gradient = this->m_MovingImageGradientImage->GetPixel(index);
    }
  else
    {
    // if not using the gradient image
    gradient = this->m_MovingImageGradientCalculator->EvaluateAtIndex(index);
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::DoFixedImagePreWarp() const
{
  /* Call Modified to make sure the filter recalculates the output. We haven't
   * changed any settings, but we assume the transform parameters have changed,
   * e.g. while used during registration. */
  this->m_FixedWarpResampleImageFilter->Modified();
  this->m_FixedWarpResampleImageFilter->Update();
  this->m_FixedWarpedImage = this->m_FixedWarpResampleImageFilter->GetOutput();

  /* Point the interpolators and calculators to the warped images.
   * We should try to skip this for efficiency because setting of
   * SmartPointers is relatively slow. However, it only happens once
   * per iteration. It will be possible if
   * ResampleImageFilter always returns the same image pointer after
   * its first update, or if it can be set to allocate output during init. */
  /* No need to call Modified here on the calculators */
  if( ! this->m_UseFixedImageGradientFilter )
    {
    this->m_FixedImageGradientCalculator->SetInputImage( this->m_FixedWarpedImage );
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::DoMovingImagePreWarp() const
{
  /* Call Modified to make sure the filter recalculates the output. We haven't
   * changed any settings, but we assume the transform parameters have changed,
   * e.g. while used during registration. */
  this->m_MovingWarpResampleImageFilter->Modified();
  this->m_MovingWarpResampleImageFilter->Update();
  this->m_MovingWarpedImage = this->m_MovingWarpResampleImageFilter->GetOutput();

  /* Point the interpolator and calculator to the warped images. */
  /* No need to call Modified here on the calculators */
  if( ! this->m_UseMovingImageGradientFilter )
    {
    this->m_MovingImageGradientCalculator->SetInputImage( this->m_MovingWarpedImage );
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeFixedImageGradientFilterImage()
{
  FixedImageConstPointer  image;
  if( this->m_DoFixedImagePreWarp )
    {
    image = this->m_FixedWarpedImage;
    }
  else
    {
    image = this->m_FixedImage;
    }

  this->m_FixedImageGradientFilter->SetInput( image );
  this->m_FixedImageGradientFilter->Update();
  this->m_FixedImageGradientImage = this->m_FixedImageGradientFilter->GetOutput();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeMovingImageGradientFilterImage() const
{
  MovingImageConstPointer  image;
  if( this->m_DoMovingImagePreWarp )
    {
    image = this->m_MovingWarpedImage;
    }
  else
    {
    image = this->m_MovingImage;
    }

  this->m_MovingImageGradientFilter->SetInput( image );
  this->m_MovingImageGradientFilter->Update();
  this->m_MovingImageGradientImage = this->m_MovingImageGradientFilter->GetOutput();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::InitializeDefaultFixedImageGradientFilter()
{
  FixedImageConstPointer  image;
  if( this->m_DoFixedImagePreWarp )
    {
    image = this->m_FixedWarpedImage;
    }
  else
    {
    image = this->m_FixedImage;
    }

  const typename FixedImageType::SpacingType & spacing = image->GetSpacing();
  double maximumSpacing = 0.0;
  for ( ImageDimensionType i = 0; i < FixedImageDimension; i++ )
    {
    if ( spacing[i] > maximumSpacing )
      {
      maximumSpacing = spacing[i];
      }
    }
  this->m_DefaultFixedImageGradientFilter->SetSigma(maximumSpacing);
  this->m_DefaultFixedImageGradientFilter->SetNormalizeAcrossScale( true );
  this->m_DefaultFixedImageGradientFilter->SetNumberOfThreads(this->GetNumberOfThreads());
  this->m_DefaultFixedImageGradientFilter->SetUseImageDirection( true );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::InitializeDefaultMovingImageGradientFilter()
{
  MovingImageConstPointer  image;
  if( this->m_DoMovingImagePreWarp )
    {
    image = this->m_MovingWarpedImage;
    }
  else
    {
    image = this->m_MovingImage;
    }

  const typename MovingImageType::SpacingType & spacing = image->GetSpacing();
  double maximumSpacing = 0.0;
  for ( ImageDimensionType i = 0; i < MovingImageDimension; i++ )
    {
    if ( spacing[i] > maximumSpacing )
      {
      maximumSpacing = spacing[i];
      }
    }
  this->m_DefaultMovingImageGradientFilter->SetSigma(maximumSpacing);
  this->m_DefaultMovingImageGradientFilter->SetNormalizeAcrossScale(true);
  this->m_DefaultMovingImageGradientFilter->SetNumberOfThreads(this->GetNumberOfThreads());
  this->m_DefaultMovingImageGradientFilter->SetUseImageDirection(true);
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
bool
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetValueAndDerivativeProcessPoint(
        const VirtualPointType &          itkNotUsed(virtualPoint),
        const FixedImagePointType &       itkNotUsed(mappedFixedPoint),
        const FixedImagePixelType &       itkNotUsed(mappedFixedPixelValue),
        const FixedImageGradientType &    itkNotUsed(mappedFixedImageGradient),
        const MovingImagePointType &      itkNotUsed(mappedMovingPoint),
        const MovingImagePixelType &      itkNotUsed(mappedMovingPixelValue),
        const MovingImageGradientType &   itkNotUsed(mappedMovingImageGradient),
        MeasureType &                     itkNotUsed(metricValueReturn),
        DerivativeType &                  itkNotUsed(localDerivativeReturn),
        const ThreadIdType                itkNotUsed(threadID) ) const
{
  itkExceptionMacro("GetValueAndDerivativeProcessPoint called in base class. "
                    "Must be overridden by derived class.");
  return false;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::SetTransform( MovingTransformType* transform )
{
  SetMovingTransform( transform );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::MovingTransformType *
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetTransform()
{
  return GetMovingTransform();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::UpdateTransformParameters( DerivativeType & derivative,
                             ParametersValueType factor )
{
  /* Rely on transform::UpdateTransformParameters to verify proper
   * size of derivative */
  this->m_MovingTransform->UpdateTransformParameters( derivative, factor );
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::SetNumberOfThreads( ThreadIdType number )
{
  if( this->m_UseFixedSampledPointSet )
    {
    if( number != this->m_SampledValueAndDerivativeThreader->GetNumberOfThreads() )
      {
      this->m_SampledValueAndDerivativeThreader->SetNumberOfThreads( number );
      this->Modified();
      }
    }
  else
    {
    if( number != this->m_DenseValueAndDerivativeThreader->GetNumberOfThreads() )
      {
      this->m_DenseValueAndDerivativeThreader->SetNumberOfThreads( number );
      this->Modified();
      }
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
ThreadIdType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetNumberOfThreads( void ) const
{
  if( ! this->m_NumberOfThreadsHasBeenInitialized )
    {
    itkExceptionMacro("m_NumberOfThreadsHasBeenInitialized is false. "
                      "Initialize must be called to initialize the number "
                      "of threads first.");
    }
  if( this->m_UseFixedSampledPointSet )
    {
    return this->m_SampledValueAndDerivativeThreader->GetNumberOfThreads();
    }
  else
    {
    return this->m_DenseValueAndDerivativeThreader->GetNumberOfThreads();
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::CreateVirtualDomainImage( VirtualSpacingType & spacing,
                            VirtualOriginType & origin,
                            VirtualDirectionType & direction,
                            VirtualRegionType & region )
{
  this->m_VirtualDomainImage = VirtualImageType::New();
  this->m_VirtualDomainImage->SetSpacing( spacing );
  this->m_VirtualDomainImage->SetOrigin( origin );
  this->m_VirtualDomainImage->SetDirection( direction );
  this->m_VirtualDomainImage->SetRegions( region );
  this->m_VirtualDomainImage->Allocate();
  this->m_VirtualDomainImage->FillBuffer( 0 );
  this->m_UserHasProvidedVirtualDomainImage = true;
  this->Modified();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::SetVirtualDomainImage( VirtualImageType * virtualImage )
{
  itkDebugMacro("setting VirtualDomainImage to " << virtualImage);
  if ( this->m_VirtualDomainImage != virtualImage )
    {
    this->m_VirtualDomainImage = virtualImage;
    this->Modified();
    this->m_UserHasProvidedVirtualDomainImage = virtualImage != NULL;
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
OffsetValueType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::ComputeParameterOffsetFromVirtualDomainIndex( const VirtualIndexType & index, NumberOfParametersType numberOfLocalParameters ) const
{
  OffsetValueType offset =
    this->m_VirtualDomainImage->ComputeOffset(index) * numberOfLocalParameters;
  return offset;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::VirtualSpacingType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetVirtualDomainSpacing( void ) const
{
  if( this->m_VirtualDomainImage )
    {
    return this->m_VirtualDomainImage->GetSpacing();
    }
  else
    {
    itkExceptionMacro("m_VirtualDomainImage is undefined. Cannot "
                      " return spacing. ");
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::VirtualDirectionType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetVirtualDomainDirection( void ) const
{
  if( this->m_VirtualDomainImage )
    {
    return this->m_VirtualDomainImage->GetDirection();
    }
  else
    {
    itkExceptionMacro("m_VirtualDomainImage is undefined. Cannot "
                      " return direction. ");
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::VirtualOriginType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetVirtualDomainOrigin( void ) const
{
  if( this->m_VirtualDomainImage )
    {
    return this->m_VirtualDomainImage->GetOrigin();
    }
  else
    {
    itkExceptionMacro("m_VirtualDomainImage is undefined. Cannot "
                      " return origin. ");
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::VirtualRegionType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetVirtualDomainRegion( void ) const
{
  if( this->m_VirtualDomainImage )
    {
    return this->m_VirtualDomainImage->GetBufferedRegion();
    }
  else
    {
    itkExceptionMacro("m_VirtualDomainImage is undefined. Cannot "
                      " return region. ");
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::MapFixedSampledPointSetToVirtual()
{
  this->m_VirtualSampledPointSet = VirtualSampledPointSetType::New();
  this->m_VirtualSampledPointSet->Initialize();

  typedef typename FixedSampledPointSetType::PointsContainer PointsContainer;
  typename PointsContainer::ConstPointer
    points = this->m_FixedSampledPointSet->GetPoints();
  typename PointsContainer::ConstIterator It = points->Begin();

  typename FixedTransformType::InverseTransformBasePointer
    inverseTransform = this->m_FixedTransform->GetInverseTransform();
  if( inverseTransform.IsNull() )
    {
    itkExceptionMacro("Unable to get inverse transform for mapping sampled "
                      " point set.");
    }

  while( It != points->End() )
    {
    typename FixedSampledPointSetType::PointType
      point = inverseTransform->TransformPoint( It.Value() );
    this->m_VirtualSampledPointSet->SetPoint( It.Index(), point );
    ++It;
    }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::MeasureType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::GetValueResult() const
{
  return m_Value;
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
typename
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::NumberOfParametersType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::GetNumberOfParameters() const
{
  return this->m_MovingTransform->GetNumberOfParameters();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
const typename ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>::ParametersType &
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
::GetParameters() const
{
  return this->m_MovingTransform->GetParameters();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
typename
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >::NumberOfParametersType
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::GetNumberOfLocalParameters() const
{
  return this->m_MovingTransform->GetNumberOfLocalParameters();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
bool
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::HasLocalSupport() const
{
  return this->m_MovingTransform->HasLocalSupport();
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::VerifyDisplacementFieldSizeAndPhysicalSpace()
{

  // TODO: replace with a common external method to check this,
  // possibly something in Transform.

  /* Verify that virtual domain and displacement field are the same size
   * and in the same physical space.
   * Effects transformation, and calculation of offset in StoreDerivativeResult.
   * If it's a composite transform and the displacement field is the first
   * to be applied (i.e. the most recently added), then it has to be
   * of the same size, otherwise not.
   * Eventually we'll want a method in Transform something like a
   * GetInputDomainSize to check this cleanly. */
  typedef DisplacementFieldTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingImageDimension ) >
                                          MovingDisplacementFieldTransformType;
  typedef CompositeTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingImageDimension ) >
                                          MovingCompositeTransformType;
  MovingTransformType* transform;
  transform = this->m_MovingTransform.GetPointer();
  /* If it's a CompositeTransform, get the last transform (1st applied). */
  MovingCompositeTransformType* comptx =
               dynamic_cast< MovingCompositeTransformType * > ( transform );
  if( comptx != NULL )
    {
    transform = comptx->GetBackTransform().GetPointer();
    }
  /* Check that it's a DisplacementField type, or a derived type,
   * the only type we expect at this point. */
  MovingDisplacementFieldTransformType* deftx =
          dynamic_cast< MovingDisplacementFieldTransformType * >( transform );
  if( deftx == NULL )
    {
    itkExceptionMacro("Expected m_MovingTransform to be of type "
                      "DisplacementFieldTransform or derived." );
    }
  typedef typename MovingDisplacementFieldTransformType::DisplacementFieldType
                                                                    FieldType;
  typename FieldType::Pointer field = deftx->GetDisplacementField();
  typename FieldType::RegionType
    fieldRegion = field->GetBufferedRegion();
  VirtualRegionType virtualRegion =
                            this->m_VirtualDomainImage->GetBufferedRegion();
  if( virtualRegion.GetSize() != fieldRegion.GetSize() ||
      virtualRegion.GetIndex() != fieldRegion.GetIndex() )
    {
    itkExceptionMacro("Virtual domain and moving transform displacement field"
                      " must have the same size and index for "
                      " LargestPossibleRegion."
                      << std::endl << "Virtual size/index: "
                      << virtualRegion.GetSize() << " / "
                      << virtualRegion.GetIndex() << std::endl
                      << "Displacement field size/index: "
                      << fieldRegion.GetSize() << " / "
                      << fieldRegion.GetIndex() << std::endl );
    }

    /* check that the image occupy the same physical space, and that
     * each index is at the same physical location.
     * this code is from ImageToImageFilter */

    /* tolerance for origin and spacing depends on the size of pixel
     * tolerance for directions a fraction of the unit cube. */
    const double coordinateTol
      = 1.0e-6 * this->m_VirtualDomainImage->GetSpacing()[0];
    const double directionTol = 1.0e-6;

    if ( !this->m_VirtualDomainImage->GetOrigin().GetVnlVector().
               is_equal( field->GetOrigin().GetVnlVector(), coordinateTol ) ||
         !this->m_VirtualDomainImage->GetSpacing().GetVnlVector().
               is_equal( field->GetSpacing().GetVnlVector(), coordinateTol ) ||
         !this->m_VirtualDomainImage->GetDirection().GetVnlMatrix().as_ref().
               is_equal( field->GetDirection().GetVnlMatrix(), directionTol ) )
      {
      std::ostringstream originString, spacingString, directionString;
      originString << "m_VirtualDomainImage Origin: "
                   << this->m_VirtualDomainImage->GetOrigin()
                   << ", DisplacementField Origin: " << field->GetOrigin()
                   << std::endl;
      spacingString << "m_VirtualDomainImage Spacing: "
                    << this->m_VirtualDomainImage->GetSpacing()
                    << ", DisplacementField Spacing: "
                    << field->GetSpacing() << std::endl;
      directionString << "m_VirtualDomainImage Direction: "
                      << this->m_VirtualDomainImage->GetDirection()
                      << ", DisplacementField Direction: "
                      << field->GetDirection() << std::endl;
      itkExceptionMacro(<< "m_VirtualDomainImage and DisplacementField do not "
                        << "occupy the same physical space! You may be able to "
                        << "simply call displacementField->CopyInformation( "
                        << "m_VirtualDomainImage ) to align them. "
                        << std::endl
                        << originString.str() << spacingString.str()
                        << directionString.str() );
      }
}

template<class TFixedImage,class TMovingImage,class TVirtualImage>
class
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::SamplingIteratorHelper
{
public:
  typedef ImageRegionConstIteratorWithIndex<VirtualImageType> DenseIteratorType;

  bool GetNext( VirtualIndexType & virtualIndex, VirtualPointType & virtualPoint )
  {
    if( m_UseSampledPointSet )
      {
      if( m_NextSample > m_LastSample )
        {
        return false;
        }
      virtualPoint = m_SampledPointSet->GetPoint( m_NextSample );
      m_VirtualDomainImageLocal->TransformPhysicalPointToIndex(
                                            virtualPoint, virtualIndex);
      m_NextSample++;
      return true;
      }
    else
      {
      if( m_DenseIt.IsAtEnd() )
        {
        return false;
        }
      virtualIndex = m_DenseIt.GetIndex();
      m_VirtualDomainImageLocal->TransformIndexToPhysicalPoint(
                                            virtualIndex, virtualPoint);
      ++m_DenseIt;
      return true;
      }
  }

  /** Constructor for use with dense sampling */
  SamplingIteratorHelper( const VirtualImagePointer & virtualDomainImage,
                          const DenseThreaderInputObjectType & subRegion )
    {
    m_UseSampledPointSet = false;
    m_VirtualDomainImageLocal = virtualDomainImage;
    m_SubRegion = subRegion;
    m_DenseIt = DenseIteratorType( virtualDomainImage, subRegion );
    m_DenseIt.GoToBegin();
    }

  /** Constructor for use with sparse sampling with a point set */
  SamplingIteratorHelper( const VirtualImagePointer & virtualDomainImage,
                          const VirtualSampledPointSetPointer & pointSet,
                          const SampledThreaderInputObjectType & sampledRange )
    {
    m_UseSampledPointSet = true;
    m_VirtualDomainImageLocal = virtualDomainImage;
    m_SampledPointSet = pointSet;
    m_FirstSample = sampledRange[0];
    /* The range is inclusive */
    m_LastSample = sampledRange[1];
    m_NextSample = m_FirstSample;
    }

  /** Default constructor should not be used */
  SamplingIteratorHelper(){};

private:

  DenseIteratorType               m_DenseIt;
  DenseThreaderInputObjectType    m_SubRegion;

  VirtualImagePointer                   m_VirtualDomainImageLocal;
  VirtualSampledPointSetPointer         m_SampledPointSet;
  SampledThreaderInputObjectValueType   m_FirstSample;
  SampledThreaderInputObjectValueType   m_LastSample;
  SampledThreaderInputObjectValueType   m_NextSample;
  bool                                  m_UseSampledPointSet;
};

template<class TFixedImage,class TMovingImage,class TVirtualImage>
void
ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ImageToImageObjectMetric: " << std::endl
               << "GetNumberOfThreads: " << this->GetNumberOfThreads()
               << std::endl
               << "GetUseFixedImageGradientFilter: "
               << this->GetUseFixedImageGradientFilter()
               << std::endl
               << "GetUseMovingImageGradientFilter: "
               << this->GetUseMovingImageGradientFilter()
               << std::endl
               << "DoFixedImagePreWarp: " << this->GetDoFixedImagePreWarp()
               << std::endl
               << "DoMovingImagePreWarp: " << this->GetDoMovingImagePreWarp()
               << std::endl;

  if( this->m_NumberOfThreadsHasBeenInitialized )
    {
    os << indent << "GetNumberOfThreads: " << this->GetNumberOfThreads()
                 << std::endl;
    }
  else
    {
    os << indent << "Number of threads not yet initialized." << std::endl;
    }

  if( this->GetVirtualDomainImage() != NULL )
    {
    os << indent << "VirtualDomainImage: "
                 << this->GetVirtualDomainImage() << std::endl;
    }
  else
    {
    os << indent << "VirtualDomainImage is NULL." << std::endl;
    }
  if( this->GetFixedImage() != NULL )
    {
    os << indent << "FixedImage: " << this->GetFixedImage() << std::endl;
    }
  else
    {
    os << indent << "FixedImage is NULL." << std::endl;
    }
  if( this->GetMovingImage() != NULL )
    {
    os << indent << "MovingImage: " << this->GetMovingImage() << std::endl;
    }
  else
    {
    os << indent << "MovingImage is NULL." << std::endl;
    }
  if( this->GetFixedTransform() != NULL )
    {
    os << indent << "FixedTransform: " << this->GetFixedTransform() << std::endl;
    }
  else
    {
    os << indent << "FixedTransform is NULL." << std::endl;
    }
  if( this->GetMovingTransform() != NULL )
    {
    os << indent << "MovingTransform: " << this->GetMovingTransform()
       << std::endl;
    }
  else
    {
    os << indent << "MovingTransform is NULL." << std::endl;
    }
  if( this->GetFixedImageMask() != NULL )
    {
    os << indent << "FixedImageMask: " << this->GetFixedImageMask() << std::endl;
    }
  else
    {
    os << indent << "FixedImageMask is NULL." << std::endl;
    }
  if( this->GetMovingImageMask() != NULL )
    {
    os << indent << "MovingImageMask: " << this->GetMovingImageMask()
       << std::endl;
    }
  else
    {
    os << indent << "MovingImageMask is NULL." << std::endl;
    }
}

}//namespace itk

#endif
