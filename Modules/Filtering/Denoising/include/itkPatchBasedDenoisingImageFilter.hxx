#ifndef __itkPatchBasedDenoisingImageFilter_hxx
#define __itkPatchBasedDenoisingImageFilter_hxx
#include "itkPatchBasedDenoisingImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "vnl/vnl_math.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkNthElementImageAdaptor.h"
#include <fstream>

namespace itk {

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::PatchRadiusType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::GetPatchRadiusInVoxels() const
{
  PatchRadiusType radius;
  radius.Fill(m_PatchRadius);
  typename Self::Pointer thisPtr = const_cast< Self* >(this);
  typename InputImageType::Pointer  inputPtr  =
    const_cast< InputImageType* >( thisPtr->GetInput());
  typename InputImageType::SpacingType spacing = inputPtr->GetSpacing();
  for (unsigned int dim = 0; dim < ImageDimension; ++dim)
  {
    radius[dim] = radius[dim] / spacing[dim];
  }
  return radius;
}

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::PatchRadiusType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::GetPatchDiameterInVoxels() const
{
  PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  PatchRadiusType one;
  PatchRadiusType two;
  one.Fill(1);
  two.Fill(2);
  PatchRadiusType diameter = two * radius + one;
  return diameter;
}

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::PatchRadiusType::SizeValueType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::GetPatchLengthInVoxels() const
{
  PatchRadiusType diameter = this->GetPatchDiameterInVoxels();
  typename PatchRadiusType::SizeValueType length = 1;
  for (unsigned int dim=0; dim < ImageDimension; ++dim)
  {
    length *= diameter[dim];
  }
  return length;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::CopyInputToOutput()
{
  typename TInputImage::ConstPointer  input  = this->GetInput();
  typename TOutputImage::Pointer      output = this->GetOutput();

  if ( !input || !output )
  {
    itkExceptionMacro(<< "Either input and/or output is NULL.");
  }

  ImageRegionConstIterator<TInputImage>  in(input, output->GetRequestedRegion());
  ImageRegionIterator<TOutputImage> out(output, output->GetRequestedRegion());

  while( ! out.IsAtEnd() )
  {
    out.Set(in.Get());
    ++in;
    ++out;
  }
}

/**
 *
 */
template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // TODO Let user specify size of requested region
  // ideally the algorithm would have access to the entire image for
  // entropy calculations, patch selection, etc.  But this may not be
  // memory-friendly.  If the user desires, they should be able to limit
// the size of the requested region and thus the size of the region
// available for calculations and patch selection.  But this needs to
// be managed carefully.  (Sensible minimum region size limit, ensure
// everywhere in this class requested region is used instead of largest
// possible region etc.)
  // call the superclass' implementation of this method
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input
  typename Superclass::InputImagePointer  inputPtr  =
    const_cast< TInputImage * >( this->GetInput());

  if ( !inputPtr )
    {
    return;
    }

  PatchRadiusType voxelNeighborhoodSize = this->GetPatchRadiusInVoxels();

  // Try to set up a buffered region that will accommodate our
  // neighborhood operations.  This may not be possible and we
  // need to be careful not to request a region outside the largest
  // possible region, because the pipeline will give us whatever we
  // ask for.

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( voxelNeighborhoodSize );

  {
    std::ostringstream msg;
    msg << "Padding inputRequestedRegion by " << voxelNeighborhoodSize << "\n"
        << "inputRequestedRegion: " << inputRequestedRegion << "\n"
        << "largestPossibleRegion: " << inputPtr->GetLargestPossibleRegion()
        << "\n";
    this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
  }
  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
      {
        std::ostringstream msg;
        msg << "Cropped inputRequestedRegion to: " << inputRequestedRegion << "\n";
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );

    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
} // end GenerateInputRequestedRegion

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::AllocateUpdateBuffer()
{
  // The update buffer looks just like the output.
  typename TOutputImage::Pointer output = this->GetOutput();

  m_UpdateBuffer->SetOrigin(output->GetOrigin());
  m_UpdateBuffer->SetSpacing(output->GetSpacing());
  m_UpdateBuffer->SetDirection(output->GetDirection());
  m_UpdateBuffer->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
  m_UpdateBuffer->SetRequestedRegion(output->GetRequestedRegion());
  m_UpdateBuffer->SetBufferedRegion(output->GetBufferedRegion());
  m_UpdateBuffer->Allocate();
  if (this->GetDebug())
  {
    m_DebugBuffer->SetOrigin(output->GetOrigin());
    m_DebugBuffer->SetSpacing(output->GetSpacing());
    m_DebugBuffer->SetDirection(output->GetDirection());
    m_DebugBuffer->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
    m_DebugBuffer->SetRequestedRegion(output->GetRequestedRegion());
    m_DebugBuffer->SetBufferedRegion(output->GetBufferedRegion());
    m_DebugBuffer->Allocate();
    ImageRegionIterator<TOutputImage> debug(m_DebugBuffer, m_DebugBuffer->GetLargestPossibleRegion());
    debug = debug.Begin();
    while (!debug.IsAtEnd())
    {
      debug.Set(4000.0);
      ++debug;
    }
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::Initialize() throw(DataObjectError)
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG, "In Initialize...\n");
  typename TInputImage::IndexType requiredIndex;
  requiredIndex.Fill(0);
  typename TInputImage::RegionType largestRegion;
  largestRegion = this->GetInput()->GetLargestPossibleRegion();
  PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  PatchRadiusType two;
  two.Fill(2);
  requiredIndex += two * radius;
  if ( !(largestRegion.IsInside(requiredIndex)) )
  {
    // The image needs to be at least as big as a single patch
    // throw an exception when this isn't the case
    // build an exception
    DataObjectError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    PatchRadiusType one;
    one.Fill(1);
    std::ostringstream msg;
    msg << "Patch is larger than the entire image (in at least one dimension)."
        << "\nImage region: " << largestRegion
        << "\nPatch length (2*radius + 1): " << this->GetPatchDiameterInVoxels()
        << "\nUse a smaller patch for this image.\n";
    e.SetDescription(msg.str());
    typename Superclass::InputImagePointer  inputPtr  =
      const_cast< TInputImage * >( this->GetInput());
    e.SetDataObject(inputPtr);
    throw e;

  }

  m_TotalNumberPixels = largestRegion.GetNumberOfPixels();
  typename InputImageType::IndexType firstIndex;
  firstIndex.Fill(0);
  m_ComponentsPerPixel = this->GetInput()->GetPixel(firstIndex).Size();
  m_IntensityRescaleInvFactor.SetSize(m_ComponentsPerPixel);
  m_IntensityRescaleInvFactor.Fill(1.0);
  m_SigmaUpdateDecimationFactor = vcl_floor(1.0 / m_FractionPixelsForSigmaUpdate);
  m_SigmaUpdateDecimationFactor = vnl_math_min(m_SigmaUpdateDecimationFactor, m_TotalNumberPixels / 100);
  m_SigmaUpdateDecimationFactor = vnl_math_max(m_SigmaUpdateDecimationFactor, 1u);

  // initialize thread data struct
  unsigned int numThreads = this->GetNumberOfThreads();
  for (unsigned int thread = 0; thread < numThreads; ++thread)
  {
    ThreadDataStruct newStruct;
    newStruct.entropyFirstDerivative.SetSize(m_ComponentsPerPixel);
    newStruct.entropySecondDerivative.SetSize(m_ComponentsPerPixel);
    newStruct.validDerivatives.SetSize(m_ComponentsPerPixel);
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      newStruct.validDerivatives[cc] = 0;
    }
    newStruct.sampler = NULL;

    m_ThreadData.push_back(newStruct);
  }

  this->InitializeSigmaEntropy(this->GetInput());

  if (m_UseSmoothDiscPatchWeights)
  {
    this->InitializePatchWeights();
  }
  else
  {
  typename PatchRadiusType::SizeValueType expectedLength = this->GetPatchLengthInVoxels();
  itkAssertOrThrowMacro(m_PatchWeights.GetSize() == expectedLength,
                        "number of weights must equal the product of the patch diameter in voxels in each dimension");
  }
  if (m_Sampler.IsNull())
  {
    itkExceptionMacro(<< "Please supply a subsampling strategy that derives from itkRegionConstrainedSubsampler");
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const Image<RGBPixel<PixelValueType>, ImageDimension>* imgPtr)
{
  this->ArrayInitializeSigmaEntropy(imgPtr);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const Image<RGBAPixel<PixelValueType>, ImageDimension>* imgPtr)
{
  this->ArrayInitializeSigmaEntropy(imgPtr);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const Image<Vector<PixelValueType,PixelType::Dimension>, ImageDimension>* imgPtr)
{
  this->ArrayInitializeSigmaEntropy(imgPtr);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const Image<FixedArray<PixelValueType,PixelType::Dimension>, ImageDimension>* imgPtr)
{
  this->ArrayInitializeSigmaEntropy(imgPtr);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const Image<Array<PixelValueType>, ImageDimension>* imgPtr)
{
  this->ArrayInitializeSigmaEntropy(imgPtr);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ArrayInitializeSigmaEntropy(const InputImageType* imgPtr)
{
  typedef NthElementImageAdaptor<InputImageType, PixelValueType> AdaptorType;
  typename AdaptorType::Pointer adaptor = AdaptorType::New();
  typedef MinimumMaximumImageFilter<AdaptorType> MinMaxFilter;
  typename MinMaxFilter::Pointer minmax = MinMaxFilter::New();

  adaptor->SetImage(const_cast<InputImageType*>(imgPtr));

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    adaptor->SelectNthElement(cc);
    minmax->SetInput(adaptor);
    minmax->Modified();
    try {
      minmax->Update();
    }
    catch(ExceptionObject &err)
    {
      // Default of 10.0 comes from a typical image that
      // has a max of 100 and a minimum of 0
      m_GaussianSigmaEntropy[cc] = 10.0;
      itkWarningMacro("caught exception <" << err
                      << "> while trying to initialize sigma entropy component "
                      << cc << ".  Defaulting to a value of "
                      << m_GaussianSigmaEntropy[cc] << ".");
      continue;
    }
    // set sigma entropy to 10% of the intensity range
    PixelValueType minVal = minmax->GetMinimum();
    PixelValueType maxVal = minmax->GetMaximum();
    // for sigma calculation, we want the intensities to be in the range
    // [0,100].  Compute the inverse factor here so that we can
    // do multiplication later instead of division.
    // Because we are taking the difference of two intensities later
    // which will cancel out any addition of a constant, we do not need
    // to shift the intensities to align their minimum with 0.
    m_IntensityRescaleInvFactor[cc] = 100.0 / (maxVal - minVal);
    // initialize sigma entropy to 10% of the range.  Since the range
    // is now normalized, that will be a value of 10
    m_GaussianSigmaEntropy[cc] = 10.0;
    {
      std::ostringstream msg;
      msg << "For component " << cc << ", Image Intensity range: [" << minVal
          << "," << maxVal << "], GaussianSigmaEntropy intialized to: "
          << m_GaussianSigmaEntropy[cc] << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
    m_GaussianSigmaEntropy[cc] *= m_SigmaMultiplicationFactor;
    {
      std::ostringstream msg;
      msg << "GaussianSigmaEntropy after applying SigmaMultiplicationFactor: "
          << m_GaussianSigmaEntropy[cc] << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
  }
}

template <class TInputImage, class TOutputImage>
template <class TImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeSigmaEntropy(const TImage* imgPtr)
{
  typedef MinimumMaximumImageFilter<InputImageType> MinMaxFilter;
  typename MinMaxFilter::Pointer minmax = MinMaxFilter::New();
  minmax->SetInput(imgPtr);
  minmax->Modified();
  try {
    minmax->Update();
  }
  catch(ExceptionObject &err)
  {
    // Default of 10.0 comes from a typical image that
    // has a max of 100 and a minimum of 0
    m_GaussianSigmaEntropy[0] = 10.0;
    itkWarningMacro("caught exception <" << err
                    << "> while trying to initialize sigma entropy."
                    << "Defaulting to a value of "
                    << m_GaussianSigmaEntropy[0] << ".");
    return;
  }
  // set sigma entropy to 10% of the intensity range
  PixelType minVal = minmax->GetMinimum();
  PixelType maxVal = minmax->GetMaximum();
  // for sigma calculation, we want the intensities to be in the range
  // [0,100].  Compute the inverse factor here so that we can
  // do multiplication later instead of division
  // Because we are taking the difference of two intensities later
  // which will cancel out any addition of a constant, we do not need
  // to shift the intensities to align their minimum with 0.
  m_IntensityRescaleInvFactor[0] = 100.0 / (maxVal - minVal);
  // initialize sigma entropy to 10% of the range.  Since the range
  // is now normalized, that will be a value of 10
  m_GaussianSigmaEntropy[0] = 10.0;
 {
    std::ostringstream msg;
    msg << "Image Intensity range: [" << minVal
        << "," << maxVal << "], GaussianSigmaEntropy intialized to: "
        << m_GaussianSigmaEntropy[0] << std::endl;
    this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
  }
  m_GaussianSigmaEntropy[0] *= m_SigmaMultiplicationFactor;
  {
    std::ostringstream msg;
    msg << "GaussianSigmaEntropy after applying sigma multiplication factor: "
        << m_GaussianSigmaEntropy[0] << std::endl;
    this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializePatchWeights()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG,
                  "InitializePatchWeights called ...\n");
  const unsigned int physicalDiameter= 2 * (m_PatchRadius) + 1;

  typedef itk::Image<float,ImageDimension> WeightsImageType;
  typedef float DistanceType;

  typename WeightsImageType::Pointer physicalWeightsImage = WeightsImageType::New();
  typename WeightsImageType::SizeType physicalSize;
  physicalSize.Fill(physicalDiameter);
  typename WeightsImageType::RegionType physicalRegion(physicalSize);
  physicalWeightsImage->SetRegions(physicalRegion);
  physicalWeightsImage->Allocate();

  physicalWeightsImage->FillBuffer(0.0);

  ImageRegionIteratorWithIndex<WeightsImageType> pwIt(physicalWeightsImage,
                                                      physicalRegion);
  unsigned int pos;
  unsigned int physicalLength = physicalRegion.GetNumberOfPixels();
  const unsigned int centerPosition= (physicalLength - 1) / 2;
  for ( pwIt.GoToBegin(), pos = 0; !pwIt.IsAtEnd(); ++pwIt, ++pos )
  {
    Vector <DistanceType, ImageDimension> distanceVector;
    for (unsigned int d= 0; d < ImageDimension; d++)
    {
      if (d == 0)
      {
        distanceVector[d]
          = static_cast<DistanceType> (pos            % physicalDiameter)
          - static_cast<DistanceType> (centerPosition % physicalDiameter);
      }
      else
      {
        distanceVector[d]
          = static_cast<DistanceType> (pos            / vnl_math_rnd (pow(physicalDiameter,d)))
          - static_cast<DistanceType> (centerPosition / vnl_math_rnd (pow(physicalDiameter,d)));
      }
    }

    unsigned int discRadius= m_PatchRadius / 2;
    const float distanceFromCenter= distanceVector.GetNorm();

    if (distanceFromCenter >= m_PatchRadius + 1)
    {
      pwIt.Set(0.0);
    }
    else if (distanceFromCenter <= discRadius)
    {
      pwIt.Set(1.0);
    }
    else
    {
      const unsigned int interval= (m_PatchRadius + 1) - discRadius;
      pwIt.Set((-2.0 / pow (interval, 3.0)) * pow ((m_PatchRadius + 1) - distanceFromCenter, 3.0)
               + ( 3.0 / pow (interval, 2.0)) * pow ((m_PatchRadius + 1) - distanceFromCenter, 2.0));
    }
  }

  // now resample into voxel space
  unsigned int voxelLength = this->GetPatchLengthInVoxels();
  m_PatchWeights.SetSize(voxelLength);

  typedef itk::ResampleImageFilter<
                WeightsImageType, WeightsImageType >  ResampleFilterType;
  typedef itk::IdentityTransform< double, ImageDimension > TransformType;
  typedef itk::BSplineInterpolateImageFunction<
                          WeightsImageType, double >    InterpolatorType;

  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  typename TransformType::Pointer transform = TransformType::New();
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

  transform->SetIdentity();
  resampler->SetTransform( transform );
  resampler->SetInterpolator( interpolator );
  resampler->SetDefaultPixelValue( 0 );
  resampler->SetOutputSpacing( this->GetInput()->GetSpacing() );
  resampler->SetOutputOrigin( physicalWeightsImage->GetOrigin() );
  resampler->SetOutputDirection( physicalWeightsImage->GetDirection() );
  resampler->SetSize( this->GetPatchDiameterInVoxels() );
  resampler->SetInput( physicalWeightsImage );

  try
  {
    resampler->Update();
  }
  catch ( itk::ExceptionObject & excp )
  {
    std::cerr << "Error while resampling patch weights."
              << "  Cannot continue." << std::endl
              << excp << std::endl;
    throw excp;
  }

  typename WeightsImageType::Pointer voxelWeightsImage = WeightsImageType::New();
  voxelWeightsImage = resampler->GetOutput();
  ImageRegionIteratorWithIndex<WeightsImageType> vwIt(voxelWeightsImage,
                                                      voxelWeightsImage->GetLargestPossibleRegion());

  for ( vwIt.GoToBegin(), pos = 0; !vwIt.IsAtEnd(); ++vwIt, ++pos )
  {
    m_PatchWeights[pos] = vwIt.Get();
  } // end for each element in the patch

  itkAssertOrThrowMacro(m_PatchWeights[(this->GetPatchLengthInVoxels() - 1)/2] == 1.0, "Center pixel's weight must be == 1.0"); // weight for the center pixel
} // end InitializePatchWeights

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeIteration()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG, "Initializing Iteration now...\n");
  PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  typename OutputImageType::Pointer output = this->GetOutput();

  // Have sampler update any internal structures (ie kd trees)
  // across the entire image for each iteration
  m_SearchSpaceList->SetImage(output);
  m_SearchSpaceList->SetRadius(radius);
  m_Sampler->SetSample(m_SearchSpaceList);
  // TODO do we want a separate Update method for doing things like recalculating
  // the KdTree for a KdTree subsampler, or should it be recalculated automatically
  // when SetSample is called?
//   m_Sampler->Update();
  // re-initialize thread data struct, especially validity flags
  unsigned int structSize = m_ThreadData.size();
  for (unsigned int thread = 0; thread < structSize; ++thread)
  {
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      m_ThreadData[thread].validDerivatives[cc] = 0;
    }
    m_ThreadData[thread].sampler = dynamic_cast<BaseSamplerType*>(m_Sampler->Clone().GetPointer());

    typename ListAdaptorType::Pointer searchList = ListAdaptorType::New();
    searchList->SetImage(output);
    searchList->SetRadius(radius);
    m_ThreadData[thread].sampler->SetSeed(thread);
    m_ThreadData[thread].sampler->SetSample(searchList);
  }
}

template<class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ApplyUpdate()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG, "ApplyUpdate...\n");
  // Set up for multithreaded processing.
  ThreadFilterStruct str;
  str.Filter = this;
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ApplyUpdateThreaderCallback,
                                            &str);
  // Multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Explicitely call Modified on GetOutput here
  // since ThreadedApplyUpdate changes this buffer
  // through iterators which do not increment the
  // output timestamp
  this->GetOutput()->Modified();
}

template <class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ApplyUpdateThreaderCallback( void * arg )
{
  ThreadFilterStruct * str;
  int total, threadId, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if (threadId < total)
  {
    str->Filter->ThreadedApplyUpdate(splitRegion, threadId);
  }

  return ITK_THREAD_RETURN_VALUE;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ThreadedApplyUpdate(const InputImageRegionType &regionToProcess,
                      int itkNotUsed(threadId))
{
  ImageRegionIterator<OutputImageType>  u(m_UpdateBuffer,    regionToProcess);
  ImageRegionIterator<OutputImageType>  o(this->GetOutput(), regionToProcess);


  u = u.Begin();
  o = o.Begin();

  while ( !u.IsAtEnd() )
  {
    o.Set(u.Get());
    ++o;
    ++u;
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeSigmaUpdate()
{
  RealArrayType sigmaUpdate;

  // Set up for multithreaded processing.
  ThreadFilterStruct str;
  str.Filter = this;

  // calculate sigma update
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ComputeSigmaUpdateThreaderCallback,
                                            &str);

  m_SigmaConverged.SetSize(m_ComponentsPerPixel);
  m_SigmaConverged.Fill(0);
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    // back out the multiplication factor prior to optimizing, then put it back afterward
    m_GaussianSigmaEntropy[cc] /= m_SigmaMultiplicationFactor;
  }
  // Perform Newton-Raphson iterations to find the sigma entropy update
  for (unsigned int i=0; i < MaxSigmaUpdateIterations; ++i)
  {
    {
      std::ostringstream msg;
      msg << "Computing iteration " << i
          << " of Sigma entropy update" << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
    // Multithread the execution
    this->GetMultiThreader()->SingleMethodExecute();

    // Combine the results from each thread
    sigmaUpdate = this->ResolveSigmaUpdate();

    {
      std::ostringstream msg;
      msg << "sigmaUpdate: " << sigmaUpdate
          << ", m_GaussianSigmaEntropy: " << m_GaussianSigmaEntropy
          << ", m_SigmaUpdateAccuracy: " << m_SigmaUpdateAccuracy
          << std::endl;
      this->m_Logger->Write(itk::LoggerBase::DEBUG, msg.str());
    }

    bool all_converged = true;
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      if (!m_SigmaConverged[cc])
      {
        if (vnl_math_abs(sigmaUpdate[cc]) < m_GaussianSigmaEntropy[cc] * m_SigmaUpdateAccuracy)
        {
          m_SigmaConverged[cc] = 1;
        }
        else
        {
          all_converged = false;
        }
      }
    }
    if (all_converged)
    {
      break;
    }
  } // end newton-raphson iterations
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    // put the multiplication factor back in
    m_GaussianSigmaEntropy[cc] *= m_SigmaMultiplicationFactor;
  }
}

template <class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeSigmaUpdateThreaderCallback( void * arg )
{
  ThreadFilterStruct * str;
  int total, threadId, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if (threadId < total)
  {
    str->Filter->ThreadedComputeSigmaUpdate(splitRegion, threadId);
  }

  return ITK_THREAD_RETURN_VALUE;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ThreadedComputeSigmaUpdate(const InputImageRegionType &regionToProcess,
                             int threadId)
{
// create two image to list adaptors, one for the iteration over the region
// the other for querying to find the patches
// set the region of interest for the second to only include patches with at least as many in-bounds
//     neighbors in the same areas of the patch as the query patch
// initialize accumulators
// for each element in the region
//   set region of interest
//   select a set of patches (random, gaussian, etc)
//   for each of the patches
//     compute the difference between the element's patch and the selected patch
//     calculate entropy derivatives using this difference
//
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<OutputImageType>
    FaceCalculatorType;
  typedef typename FaceCalculatorType::FaceListType FaceListType;
  typedef typename ListAdaptorType::ConstIterator            SampleIteratorType;

  PatchRadiusType radius = this->GetPatchRadiusInVoxels();

  typename OutputImageType::Pointer output = this->GetOutput();
  typename ListAdaptorType::Pointer inList = ListAdaptorType::New();
  inList->SetImage(output);
  inList->SetRadius(radius);

  BaseSamplerPointer sampler = m_ThreadData[threadId].sampler;

  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  // For the sigma update calculation, we only want to use pixels from the first
  // region to avoid using boundary conditions (and it's faster).

  FaceCalculatorType faceCalculator;

  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt;

  unsigned int lengthPatch = this->GetPatchLengthInVoxels();
  unsigned int center;
  RealArrayType jointEntropyFirstDerivative(m_ComponentsPerPixel);
  RealArrayType jointEntropySecondDerivative(m_ComponentsPerPixel);
  RealArrayType nbhdEntropyFirstDerivative(m_ComponentsPerPixel);
  RealArrayType nbhdEntropySecondDerivative(m_ComponentsPerPixel);
  jointEntropyFirstDerivative.Fill(0.0);
  jointEntropySecondDerivative.Fill(0.0);
  nbhdEntropyFirstDerivative.Fill(0.0);
  nbhdEntropySecondDerivative.Fill(0.0);

  // Only use pixels whose patch is entirely in bounds for the
  // sigma calculation
  fIt = faceList.begin();

  if (!(fIt->GetNumberOfPixels()))
  {
    // empty region, don't use.
    return;
  }

  inList->SetRegion(*fIt);

  OutputImageRegionIterator debugIt;

  if (this->GetDebug())
  {
    debugIt = OutputImageRegionIterator(m_DebugBuffer, *fIt);
    debugIt.GoToBegin();
  }

  unsigned int sampleNum = 0;
  for (SampleIteratorType sampleIt = inList->Begin(); sampleIt != inList->End(); ++sampleIt, ++sampleNum)
  {
    if (sampleNum % m_SigmaUpdateDecimationFactor != 0)
    {
      // skip this sample
      continue;
    }
    InputImagePatchIterator currentPatch = sampleIt.GetMeasurementVector()[0];
    typename OutputImageType::IndexType  nIndex = currentPatch.GetIndex();
    InstanceIdentifier currentPatchId = inList->GetImage()->ComputeOffset(nIndex);

    center = (lengthPatch - 1) / 2;
    // select a set of patches from the full image, excluding points that have
    // neighbors outside the boundary at locations different than that of the current patch
    // For example, say we have a 7x10 image and current patch index == (0,1) with radius = 2.
    // Then the sampler should be constrained to only select other patches with indices
    // in the range (0:7-2-1,1:10-2-1) == (0:4,1:7)
    // Conversely if the current patch index == (5,8) with radius = 2,
    // the sampler should only select other patches with indices in the range (2:5,2:8)
    // That is, the range formula is min(index,radius):max(index,size-radius-1)

    typename OutputImageType::RegionType region = this->GetInput()->GetLargestPossibleRegion();
    typename OutputImageType::IndexType  rIndex;
    typename OutputImageType::SizeType   rSize = region.GetSize();
    for (unsigned int dim = 0; dim < OutputImageType::ImageDimension; ++dim)
    {
      rIndex[dim] = vnl_math_min(static_cast<long unsigned int>(nIndex[dim]), radius[dim]);
      rSize[dim] = vnl_math_max(static_cast<long unsigned int>(nIndex[dim]), rSize[dim] - radius[dim] - 1) - rIndex[dim] + 1;
    }
    region.SetIndex(rIndex);
    region.SetSize(rSize);

    typename BaseSamplerType::SubsamplePointer selectedPatches =
                                              BaseSamplerType::SubsampleType::New();
    sampler->SetRegionConstraint(region);
    sampler->CanSelectQueryOff();
    sampler->Search(currentPatchId, selectedPatches);

    unsigned int numPatches = selectedPatches->GetTotalFrequency();

    RealArrayType probJointEntropy(m_ComponentsPerPixel);
    RealArrayType probJointEntropyFirstDerivative(m_ComponentsPerPixel);
    RealArrayType probJointEntropySecondDerivative(m_ComponentsPerPixel);
    probJointEntropy.Fill(0.0);
    probJointEntropyFirstDerivative.Fill(0.0);
    probJointEntropySecondDerivative.Fill(0.0);

    RealArrayType probPatchEntropy(m_ComponentsPerPixel);
    RealArrayType probPatchEntropyFirstDerivative(m_ComponentsPerPixel);
    RealArrayType probPatchEntropySecondDerivative(m_ComponentsPerPixel);
    probPatchEntropy.Fill(0.0);
    probPatchEntropyFirstDerivative.Fill(0.0);
    probPatchEntropySecondDerivative.Fill(0.0);

    RealValueType centerPatchDifference;
    VariableLengthVector<PixelType> currentPatchVec(lengthPatch);
    // store the current patch prior to iterating over the selected patches
    // to avoid repeatedly calling GetPixel for this patch
    // because we know we are processing a region whose pixels are all in bounds, we don't
    // need to check this any further when dealing with the patches
    for (unsigned int jj = 0; jj < lengthPatch; ++jj)
    {
      currentPatchVec[jj] = currentPatch.GetPixel(jj);
      // the following is the equivalent to rescaling each pixel, but we want to avoid
      // repeated multiplication where possible
    }

    typename OutputImageType::IndexType lastSelectedIdx;
    typename OutputImageType::IndexType currSelectedIdx;
    InputImagePatchIterator selectedPatch;
    if (numPatches > 0)
    {
      selectedPatch = selectedPatches->Begin().GetMeasurementVector()[0];
      lastSelectedIdx = selectedPatch.GetIndex();
    }
    else
    {
      std::ostringstream msg;
      InputImagePatchIterator queryIt = sampler->GetSample()->GetMeasurementVector(currentPatchId)[0];
      msg << "unexpected index for current patch, search results are empty."
          << "\ncurrent patch id: " << currentPatchId
          << "\ncurrent patch index: " << nIndex
          << "\nindex calculated by searcher: "
          << queryIt.GetIndex(queryIt.GetCenterNeighborhoodIndex())
          << "\npatch accessed by searcher: ";
      queryIt.Print(msg);
      this->m_Logger->Write(itk::LoggerBase::WARNING, msg.str());
    }
    RealValueType squaredNorm;
    RealValueType sigmaEntropy;

    for (typename BaseSamplerType::SubsampleConstIterator selectedIt = selectedPatches->Begin();
         selectedIt != selectedPatches->End();
         ++selectedIt)
    {
      currSelectedIdx = selectedIt.GetMeasurementVector()[0].GetIndex();
      selectedPatch += currSelectedIdx - lastSelectedIdx;
      lastSelectedIdx = currSelectedIdx;
      for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
      {
        // only process if this component's sigma hasn't converged yet
        if (!m_SigmaConverged[cc])
        {
          squaredNorm = 0.0;
          // save this off to avoid unnecessary repetitive accessing in calculations below
          sigmaEntropy = m_GaussianSigmaEntropy[cc];

          // compute difference between selectedPatches[ii] and currentPatch

          // since we make sure that the search query can only take place in a certain image region
          // it is sufficient to rely on the fact that the current patch is in bounds

          selectedPatch.NeedToUseBoundaryConditionOff();
          // this partial loop unrolling works because the length of the patch is always odd
          // guaranteeing that center - 0 == lengthPatch - center+1 always
          for (unsigned int jj = 0, kk = center+1; jj < center; ++jj, ++kk)
          {
            // PixelType selectedVal = *(selectedPatch[jj]);
            // *(selectedPatch[jj]) is about 10% faster than selectedPatch.GetPixel(jj)
            // in scalar case but need to do selectedPatch.GetPixel(jj)[cc] in vector case
            PixelValueType selectedVal1 = selectedPatch.GetPixel(jj)[cc];
            PixelValueType selectedVal2 = selectedPatch.GetPixel(kk)[cc];
            RealValueType diff1 = m_IntensityRescaleInvFactor[cc] * (currentPatchVec[jj][cc] - selectedVal1);
            RealValueType diff2 = m_IntensityRescaleInvFactor[cc] * (currentPatchVec[kk][cc] - selectedVal2);
            squaredNorm += diff1 * diff1;
            squaredNorm += diff2 * diff2;
          }
          PixelValueType selectedVal = selectedPatch.GetPixel(center)[cc];
          RealValueType diff = m_IntensityRescaleInvFactor[cc] * (currentPatchVec[center][cc] - selectedVal);
          squaredNorm += diff * diff;
          centerPatchDifference = diff;

          RealValueType distanceJointEntropy;
          RealValueType distancePatchEntropySquared;
          RealValueType distancePatchEntropy;
          RealValueType gaussianJointEntropy;
          RealValueType gaussianPatchEntropy;
          RealValueType factorJoint;
          RealValueType factorPatch;

          distanceJointEntropy = vcl_sqrt(squaredNorm);
          gaussianJointEntropy = exp(- vnl_math_sqr(distanceJointEntropy / sigmaEntropy) / 2.0);

          probJointEntropy[cc] += gaussianJointEntropy;
          factorJoint = squaredNorm / pow(sigmaEntropy, 3.0) - (lengthPatch * 1 / sigmaEntropy);

          probJointEntropyFirstDerivative[cc] += gaussianJointEntropy * factorJoint;
          probJointEntropySecondDerivative[cc] += gaussianJointEntropy * (vnl_math_sqr(factorJoint) + (lengthPatch * 1 / vnl_math_sqr(sigmaEntropy)) - (3.0 * squaredNorm / pow(sigmaEntropy, 4.0)));

          if (m_ComputeConditionalDerivatives)
          {
            distancePatchEntropySquared = squaredNorm - vnl_math_sqr(centerPatchDifference);
            distancePatchEntropy = vcl_sqrt(distancePatchEntropySquared);
            gaussianPatchEntropy = exp(- vnl_math_sqr(distancePatchEntropy / sigmaEntropy) / 2.0);
            probPatchEntropy[cc] += gaussianPatchEntropy;
            factorPatch = distancePatchEntropySquared / pow(sigmaEntropy, 3.0) - ((lengthPatch-1) / sigmaEntropy);

            probPatchEntropyFirstDerivative[cc] += gaussianPatchEntropy * factorPatch;
            probPatchEntropySecondDerivative[cc] += gaussianPatchEntropy * (vnl_math_sqr(factorPatch) + ((lengthPatch-1) / vnl_math_sqr(sigmaEntropy)) - (3.0*distancePatchEntropySquared / pow(sigmaEntropy, 4.0)));
          }
        } // end if pixel component hasn't converged yet
      } // end for each pixel component
    } // end for each selected patch

    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      if (!m_SigmaConverged[cc])
      {
        probJointEntropy[cc] /= numPatches;
        probJointEntropy[cc] += m_MinProbability;
        probJointEntropyFirstDerivative[cc] /= numPatches;
        probJointEntropyFirstDerivative[cc] += m_MinProbability;
        probJointEntropySecondDerivative[cc] /= numPatches;
        probJointEntropySecondDerivative[cc] += m_MinProbability;

        {
          std::ostringstream msg;
          msg << "probJointEntropy[" << cc << "] = " << probJointEntropy[cc] << std::endl;
          msg << "probJointEntropy must be in (0.0, 1.0]" << std::endl;
          if (probJointEntropy[cc] <= 0.0 || probJointEntropy[cc] > 1.0)
          {
            this->m_Logger->Write(itk::LoggerBase::FATAL, msg.str());
          }
          itkAssertOrThrowMacro(probJointEntropy[cc] > 0.0, "probJointEntropy must be > 0.0");
        }

        jointEntropyFirstDerivative[cc] -= probJointEntropyFirstDerivative[cc] / probJointEntropy[cc];
        jointEntropySecondDerivative[cc] -= probJointEntropySecondDerivative[cc] / probJointEntropy[cc] - vnl_math_sqr(probJointEntropyFirstDerivative[cc] / probJointEntropy[cc]);
        if (m_ComputeConditionalDerivatives)
        {
          probPatchEntropy[cc] /= numPatches;
          probPatchEntropy[cc] += m_MinProbability;
          probPatchEntropyFirstDerivative[cc] /= numPatches;
          probPatchEntropyFirstDerivative[cc] += m_MinProbability;
          probPatchEntropySecondDerivative[cc] /= numPatches;
          probPatchEntropySecondDerivative[cc] += m_MinProbability;

          itkAssertOrThrowMacro(probPatchEntropy[cc] > 0.0, "probPatchEntropy must be > 0.0");

          nbhdEntropyFirstDerivative[cc] -= probPatchEntropyFirstDerivative[cc] / probPatchEntropy[cc];
          nbhdEntropySecondDerivative[cc] -= probPatchEntropySecondDerivative[cc] / probPatchEntropy[cc] - vnl_math_sqr(probPatchEntropyFirstDerivative[cc] / probPatchEntropy[cc]);
        }
      } // end if pixel component hasn't converged yet
    } // end for each pixel component
  } // end for each pixel in the sample

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    if (!m_SigmaConverged[cc])
    {
      if (m_ComputeConditionalDerivatives)
      {
        m_ThreadData[threadId].entropyFirstDerivative[cc] = jointEntropyFirstDerivative[cc] - nbhdEntropyFirstDerivative[cc];
        m_ThreadData[threadId].entropySecondDerivative[cc] = jointEntropySecondDerivative[cc] - nbhdEntropySecondDerivative[cc];
      }
      else
      {
        m_ThreadData[threadId].entropyFirstDerivative[cc] = jointEntropyFirstDerivative[cc];
        m_ThreadData[threadId].entropySecondDerivative[cc] = jointEntropySecondDerivative[cc];
      }
      m_ThreadData[threadId].validDerivatives[cc] = 1;
    }
    else
    {
      m_ThreadData[threadId].validDerivatives[cc] = 0;
    }
  } // end for each pixel component
} // end ThreadedComputeSigmaUpdate

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::RealArrayType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ResolveSigmaUpdate()
{
  RealArrayType sigmaUpdate(m_ComponentsPerPixel);
  sigmaUpdate.Fill(0);
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    if (m_SigmaConverged[cc])
    {
      // don't need to resolve anything, go on to the next component
      continue;
    }
    {
      std::ostringstream msg;
      msg << "Resolving sigma update for pixel component " << cc
          << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
    RealValueType firstDerivative = 0;
    RealValueType secondDerivative = 0;
    unsigned int numThreads = m_ThreadData.size();
    for (unsigned int threadNum = 0; threadNum < numThreads; ++threadNum)
    {
      if (m_ThreadData[threadNum].validDerivatives[cc] > 0)
      {
        firstDerivative += m_ThreadData[threadNum].entropyFirstDerivative[cc];
        secondDerivative += m_ThreadData[threadNum].entropySecondDerivative[cc];
      }
    }

    firstDerivative /= static_cast<RealValueType>(m_TotalNumberPixels);
    secondDerivative /= static_cast<RealValueType>(m_TotalNumberPixels);

    {
      std::ostringstream msg;
      msg << "first deriv: " << firstDerivative
          << ", second deriv: " << secondDerivative
          << ", old sigma: " << m_GaussianSigmaEntropy[cc]
          << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
    if (vnl_math_abs(secondDerivative) == NumericTraits<RealValueType>::Zero)
    {
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** Second derivative ZERO\n");
      sigmaUpdate[cc] = 1.0;
    }
    else
    {
      sigmaUpdate[cc] = -firstDerivative / secondDerivative;
    }

    {
      std::ostringstream msg;
      msg << "update: " << sigmaUpdate[cc] << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }

    if (secondDerivative < 0)
    {
      // handle negative second derivative
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** Second derivative negative\n");
      sigmaUpdate[cc] = -vnl_math_sgn(firstDerivative) * m_GaussianSigmaEntropy[cc] * 0.3;
      {
        std::ostringstream msg;
        msg << "update: " << sigmaUpdate[cc] << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }
    else if (vnl_math_abs(sigmaUpdate[cc]) > m_GaussianSigmaEntropy[cc] * 0.3)
    {
      // keep the update from being too large
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** Restricting large updates\n");
      sigmaUpdate[cc] = vnl_math_sgn(sigmaUpdate[cc]) * m_GaussianSigmaEntropy[cc] * 0.3;
      {
        std::ostringstream msg;
        msg << "update: " << sigmaUpdate[cc] << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }

    // keep the updated sigma from being too small
    if (m_GaussianSigmaEntropy[cc] + sigmaUpdate[cc] < m_MinSigma)
    {
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** TOO SMALL SIGMA: adjusting sigma\n");
      m_GaussianSigmaEntropy[cc] = (m_GaussianSigmaEntropy[cc] + m_MinSigma) / 2.0;
      {
        std::ostringstream msg;
        msg << "new sigma: " << m_GaussianSigmaEntropy[cc] << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }
    else
    {
      m_GaussianSigmaEntropy[cc] += sigmaUpdate[cc];
      {
        std::ostringstream msg;
        msg << "new sigma: " << m_GaussianSigmaEntropy[cc] << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }
} // end for each pixel component
  return sigmaUpdate;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeImageUpdate()
{
  // Set up for multithreaded processing.
  ThreadFilterStruct str;
  str.Filter = this;

  // calculate sigma update
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ComputeImageUpdateThreaderCallback,
                                            &str);

  // Multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

}

template <class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeImageUpdateThreaderCallback( void * arg )
{
  ThreadFilterStruct * str;
  int total, threadId, threadCount;

  threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  str = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if (threadId < total)
  {
    str->Filter->ThreadedComputeImageUpdate(splitRegion, threadId);
  }

  return ITK_THREAD_RETURN_VALUE;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ThreadedComputeImageUpdate(const InputImageRegionType &regionToProcess, int threadId)
{

// create two image to list adaptors, one for the iteration over the region
// the other for querying to find the patches
// set the region of interest for the second to only include patches with as many in-bounds
//     neighbors in the same areas of the query patch
// initialize accumulators
// for each element in the region
//   set region of interest
//   select a set of patches (random, gaussian, etc)
//   for each of the patches
//     compute the difference between the element's patch and the selected patch
//     calculate the gradient of the joint entropy using this difference
//
  typedef typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<OutputImageType>
    FaceCalculatorType;
  typedef typename FaceCalculatorType::FaceListType FaceListType;
  typedef typename ListAdaptorType::ConstIterator            SampleIteratorType;

  PatchRadiusType radius = this->GetPatchRadiusInVoxels();

  typename OutputImageType::Pointer output = this->GetOutput();
  typename ListAdaptorType::Pointer inList = ListAdaptorType::New();
  inList->SetImage(output);
  inList->SetRadius(radius);

  BaseSamplerPointer sampler = m_ThreadData[threadId].sampler;

  // TODO confirm progress approach
  ProgressReporter progress(this, threadId, regionToProcess.GetNumberOfPixels());

  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output

  FaceCalculatorType faceCalculator;

  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt;

  for (fIt = faceList.begin(); fIt != faceList.end(); ++fIt)
  {

    if (!(fIt->GetNumberOfPixels()))
    {
      // empty region, move on to next
      continue;
    }

    inList->SetRegion(*fIt);

    OutputImageRegionIterator updateIt(m_UpdateBuffer,  *fIt);
    OutputImageRegionIterator outputIt(output, *fIt);
    InputImageRegionConstIterator inputIt(this->GetInput(), *fIt);
    OutputImageRegionIterator debugIt;

    updateIt.GoToBegin();
    outputIt.GoToBegin();
    inputIt.GoToBegin();

    if (this->GetDebug())
    {
      debugIt = OutputImageRegionIterator(m_DebugBuffer, *fIt);
      debugIt.GoToBegin();
    }

    for (SampleIteratorType sampleIt = inList->Begin(); sampleIt != inList->End(); ++sampleIt)
    {
      RealType result = outputIt.Get();
      // TODO remove this debugVal
      RealType debugVal;
      if (m_RegularizationWeight > 0)
      {
        RealType gradientJointEntropy = this->ComputeJointEntropy(sampleIt.GetInstanceIdentifier(), inList, sampler, debugVal);
        if (this->GetDebug())
        {
          debugIt.Set(debugVal);
          ++debugIt;
        }
        for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
        {
          const RealValueType timeStep = 0.2;
          result[cc] += timeStep * m_RegularizationWeight * gradientJointEntropy[cc];
        }
      } // end if m_RegularizationWeight > 0
      if (m_FidelityWeight > 0)
      {
        PixelType in = inputIt.Get();
        PixelType out = outputIt.Get();
        switch (m_NoiseModel)
        {
          case GAUSSIAN:
          {
            for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
            {
              result[cc] += m_FidelityWeight * 2.0 * (in[cc] - out[cc]);
            }
            break;
          }
          case RICIAN:
          {
            // TODO implement
            itkExceptionMacro(<< "RICIAN noise model not currently implemented!");
          }
          case POISSON:
          {
            // TODO implement
            itkExceptionMacro(<< "RICIAN noise model not currently implemented!");
            break;
          }
          default:
          {
            // TODO implement
            itkExceptionMacro(<< "Unexpected noise model " << m_NoiseModel << " specified.");
            break;
          }
        }
      }
      // set update value, because we can't change the output until the other threads are
      // done using it.
      // updateBuffer contains the current update (referred to by updateIt)
      // this->GetInput() contains the original data (referred to by inputIt)
      // this->GetOutput() contains the results from the previous iteration (referred to by outputIt)
      // at the end of this update, updateIt will hold the current iteration image
      // this->ApplyUpdate() will then copy the results to outputIt since we have to wait
      // until all threads have finished using outputIt to compute the update
      // updateIt = outputIt + entropyWeight * gradientJointEntropy
      //                     + fidelityWeight * 2.0 * (inputIt - outputIt)

      updateIt.Set(static_cast<PixelType>(result));
      ++updateIt;
      ++outputIt;
      ++inputIt;
      progress.CompletedPixel();
    } // end for each pixel in the sample
  } // end for each face in the face list
} // end ThreadedComputeImageUpdate

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::RealType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeJointEntropy(InstanceIdentifier id,
                      typename ListAdaptorType::Pointer& inList,
                      BaseSamplerPointer& sampler,
                      RealType& debugVal)
{
  InputImagePatchIterator currentPatch = inList->GetMeasurementVector(id)[0];
  typename OutputImageType::IndexType  nIndex = currentPatch.GetIndex();
  InstanceIdentifier currentPatchId = inList->GetImage()->ComputeOffset(nIndex);
  unsigned int lengthPatch = this->GetPatchLengthInVoxels();
  const unsigned int center = (lengthPatch - 1) / 2;

  typename OutputImageType::Pointer output = this->GetOutput();
  typename OutputImageType::RegionType region = this->GetInput()->GetLargestPossibleRegion();
  typename OutputImageType::IndexType  rIndex;
  typename OutputImageType::SizeType   rSize = region.GetSize();
  PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  for (unsigned int dim = 0; dim < OutputImageType::ImageDimension; ++dim)
  {
    rIndex[dim] = vnl_math_min(static_cast<long unsigned int>(nIndex[dim]), radius[dim]);
    rSize[dim] = vnl_math_max(static_cast<long unsigned int>(nIndex[dim]), rSize[dim] - radius[dim] - 1) - rIndex[dim] + 1;
  }
  region.SetIndex(rIndex);
  region.SetSize(rSize);

  typename BaseSamplerType::SubsamplePointer selectedPatches =
    BaseSamplerType::SubsampleType::New();

  sampler->SetRegionConstraint(region);
  sampler->CanSelectQueryOn();
  sampler->Search(currentPatchId, selectedPatches);

  unsigned int numPatches = selectedPatches->GetTotalFrequency();

  RealValueType sumOfGaussiansJointEntropy = 0.0;

  RealType gradientJointEntropy;
  gradientJointEntropy.Fill(0.0);

  RealArrayType centerPatchDifference(m_ComponentsPerPixel);
  centerPatchDifference.Fill(0.0);
  VariableLengthVector<PixelType> currentPatchVec(lengthPatch);
  VariableLengthVector<unsigned short> IsInBoundsVec(lengthPatch);
  // store the current patch prior to iterating over the selected patches
  // to avoid repeatedly calling GetPixel for this patch
  // Don't worry about whether it's in bounds, just use the boundary condition as provided
  // by GetPixel()
  for (unsigned int jj = 0; jj < lengthPatch; ++jj)
  {
    bool IsInBounds;
    currentPatchVec[jj] = currentPatch.GetPixel(jj, IsInBounds);
    if (IsInBounds)
    {
      IsInBoundsVec[jj] = true;
    }
    else
    {
      IsInBoundsVec[jj] = false;
    }
  }
  RealValueType distanceJointEntropy;
  RealValueType gaussianJointEntropy = 0;

  typename OutputImageType::IndexType lastSelectedIdx;
  typename OutputImageType::IndexType currSelectedIdx;
  InputImagePatchIterator selectedPatch;
  if (numPatches > 0)
  {
    selectedPatch = selectedPatches->Begin().GetMeasurementVector()[0];
    lastSelectedIdx = selectedPatch.GetIndex();
  }
  RealValueType squaredNorm;

  for (typename BaseSamplerType::SubsampleConstIterator selectedIt = selectedPatches->Begin();
       selectedIt != selectedPatches->End();
       ++selectedIt)
  {
    currSelectedIdx = selectedIt.GetMeasurementVector()[0].GetIndex();
    selectedPatch += currSelectedIdx - lastSelectedIdx;
    lastSelectedIdx = currSelectedIdx;

    squaredNorm = 0.0;
    distanceJointEntropy = 0.0;

    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {

      // compute difference between selectedPatches[ii] and currentPatch
      if (currentPatch.InBounds())
      {
        // since we make sure that the search query can only take place in a certain image region
        // it is sufficient to rely on the fact that the current patch is in bounds
        selectedPatch.NeedToUseBoundaryConditionOff();

        // this partial loop unrolling works because the length of the patch is always odd
        // guaranteeing that center - 0 == lengthPatch - center+1 always
        for (unsigned int jj = 0, kk = center+1; jj < center; ++jj, ++kk)
        {
          RealValueType weight1 = m_PatchWeights[jj];
          RealValueType weight2 = m_PatchWeights[kk];
          // PixelValueType selectedVal1 = *(selectedPatch[jj]); // faster for scalar pixel types
          // PixelValueType selectedVal2 = *(selectedPatch[kk]);
          PixelValueType selectedVal1 = selectedPatch.GetPixel(jj)[cc];
          PixelValueType selectedVal2 = selectedPatch.GetPixel(kk)[cc];
          RealValueType diff1 = weight1 * (selectedVal1 - currentPatchVec[jj][cc]);
          RealValueType diff2 = weight2 * (selectedVal2 - currentPatchVec[kk][cc]);
          squaredNorm += diff1 * diff1;
          squaredNorm += diff2 * diff2;
        }

        // now compute the center value
        RealValueType weight = m_PatchWeights[center];
        PixelValueType selectedVal = selectedPatch.GetPixel(center)[cc];
        RealValueType diff = weight * (selectedVal - currentPatchVec[center][cc]);
        squaredNorm += diff * diff;
        centerPatchDifference[cc] = diff;

      }
      else
      {
        // since we make sure that the search query can only take place in a certain image region
        // the randomly selected patch will be at least as in bounds as the current patch
        selectedPatch.NeedToUseBoundaryConditionOff();
        for (unsigned int jj = 0, kk= center+1; jj < center; ++jj, ++kk)
        {
          if (IsInBoundsVec[jj])
          {
            RealValueType weight = m_PatchWeights[jj];
            PixelValueType selectedVal = selectedPatch.GetPixel(jj)[cc];
            RealValueType diff = weight * (selectedVal - currentPatchVec[jj][cc]);
            squaredNorm += diff * diff;
          }
          if (IsInBoundsVec[kk])
          {
            RealValueType weight = m_PatchWeights[kk];
            PixelValueType selectedVal = selectedPatch.GetPixel(kk)[cc];
            RealValueType diff = weight * (selectedVal - currentPatchVec[kk][cc]);
            squaredNorm += diff * diff;
          }
        }

        // compute the center value (center is always in bounds)
        RealValueType weight = m_PatchWeights[center];
        // for vector
        RealValueType diff = weight * (selectedPatch.GetPixel(center)[cc] - currentPatchVec[center][cc]);
        // faster for scalar
        // RealValueType diff = weight * (*(selectedPatch[center]) - currentPatchVec[center]);
        squaredNorm += diff * diff;
        centerPatchDifference[cc] = diff;
      } // end if entire patch is inbounds

      distanceJointEntropy += squaredNorm / vnl_math_sqr(m_GaussianSigmaEntropy[cc]);

    } // end for each pixel component
    // TODO figure out how to provide a debug JointEntropy image
    gaussianJointEntropy = exp( distanceJointEntropy / -2.0);
    sumOfGaussiansJointEntropy += gaussianJointEntropy;
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      gradientJointEntropy[cc] += centerPatchDifference[cc] * gaussianJointEntropy;
    }
  } // end for each selected patch

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    gradientJointEntropy[cc] /= (sumOfGaussiansJointEntropy + m_MinProbability);
    if (this->GetDebug())
    {
      debugVal[cc] = gradientJointEntropy[cc];
    }
  }

  return gradientJointEntropy;
} // end ComputeJointEntropy

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::PostProcessOutput()
{
  if (this->GetDebug() && (m_DebugFilename != ""))
  {
    // TODO eliminate debug buffer
    // write m_DebugBuffer to file
    typedef ImageFileWriter< OutputImageType >  WriterType;
    typename WriterType::Pointer writer = WriterType::New();

    writer->SetFileName( m_DebugFilename );

    writer->SetInput( m_DebugBuffer );
    try
    {
      writer->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
    }
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "PatchRadius (physical space): " << m_PatchRadius << std::endl;
  os << indent << "PatchRadius (voxel space): " << this->GetPatchRadiusInVoxels() << std::endl;
  os << indent << "Total number of pixels: " << m_TotalNumberPixels << std::endl;
  os << indent << "Use smooth disc patch weights: " << m_UseSmoothDiscPatchWeights
     << std::endl;
  os << indent << "Regularization weight: " << m_RegularizationWeight << std::endl;
  os << indent << "Fidelity weight: " << m_FidelityWeight << std::endl;
  os << indent << "Gaussian sigma entropy: " << m_GaussianSigmaEntropy << std::endl;
  os << indent << "Compute conditional derivatives: " << m_ComputeConditionalDerivatives << std::endl;
  os << indent << "Sigma update accuracy: " << m_SigmaUpdateAccuracy << std::endl;
  os << indent << "FractionPixelsForSigmaUpdate: " << m_FractionPixelsForSigmaUpdate << std::endl;
  os << indent << "SigmaUpdateDecimationFactor: " << m_SigmaUpdateDecimationFactor << std::endl;
  os << indent << "SigmaMultiplicationFactor: " << m_SigmaMultiplicationFactor << std::endl;
  os << indent << "IntensityRescaleInvFactor: " << m_IntensityRescaleInvFactor << std::endl;
  os << indent << "Min probability: " << m_MinProbability << std::endl;
  os << indent << "Min sigma: " << m_MinSigma << std::endl;

  if (m_Sampler)
  {
    os << indent << "Sampler: " << std::endl;
    m_Sampler->Print(os,indent.GetNextIndent());
  }
  else
  {
    os << indent << "Sampler: " << "(None)" << std::endl;
  }

  if (m_UpdateBuffer)
  {
    os << indent << "Update buffer:\n";
    m_UpdateBuffer->Print(os, indent.GetNextIndent());
    os << std::endl;
  }
  else
  {
    os << indent << "Update buffer is NULL" << std::endl;
  }
  if (m_DebugBuffer)
  {
    os << indent << "Debug buffer:\n";
    m_DebugBuffer->Print(os, indent.GetNextIndent());
    os << std::endl;
  }
  else
  {
    os << indent << "Debug buffer is NULL" << std::endl;
  }
  os << std::endl;
}

}// end namespace itk

#endif
