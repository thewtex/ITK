#ifndef __itkPatchBasedDenoisingImageFilter_hxx
#define __itkPatchBasedDenoisingImageFilter_hxx

#include "itkPatchBasedDenoisingImageFilter.h"
#include "itkNthElementImageAdaptor.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileWriter.h"
#include "itkGaussianOperator.h"
#include "itkImageAlgorithm.h"
#include "itkVectorImageToImageAdaptor.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::PatchBasedDenoisingImageFilter()
{
  m_SearchSpaceList = ListAdaptorType::New();
  m_UpdateBuffer    = OutputImageType::New();

  // patch weights
  this->UseSmoothDiscPatchWeightsOn();

  // by default, turn on automatic kerel-sigma estimation
  this->DoKernelBandwidthEstimationOn();
  // minimum probability, used to avoid divide by zero
  m_MinProbability = NumericTraits<RealValueType>::min() * 100;
  // minimum sigma allowed, used to avoid divide by zero
  m_MinSigma       = NumericTraits<RealValueType>::min() * 100;

  m_ComputeConditionalDerivatives   = false;
  m_FractionPixelsForSigmaUpdate    = 0.20;
  m_SigmaUpdateDecimationFactor     = static_cast<unsigned int>
     (vnl_math_rnd(1.0 / m_FractionPixelsForSigmaUpdate));
  // desired accuracy of Newton-Raphson sigma estimation
  m_SigmaUpdateConvergenceTolerance = 0.01;
  m_SigmaMultiplicationFactor       = 1.0;

  m_NoiseSigmaIsSet = false;

  m_TotalNumberPixels  = 0; // won't be valid until an image is provided
  m_Sampler            = 0; // won't be valid until a sampler is provided
  m_ComponentsPerPixel = 0; // won't be valid until Initialize() gets called
  // m_IntensityRescaleInvFactor won't be allocated until Initialize() gets called
  // because we need the input image first.
}



template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::SetThreadData(int threadId, const ThreadDataStruct& data)
{
  if (threadId < static_cast<int>(m_ThreadData.size()))
  {
    m_ThreadData[threadId] = data;
  }
  else
  {
    itkExceptionMacro(<< "Invalid thread id " << threadId
                      << " or SetThreadData called before m_ThreadData (size="
                      << m_ThreadData.size()
                      << ") was initialized.");
  }
}

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::ThreadDataStruct
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::GetThreadData(int threadId)
{
  if (threadId < static_cast<int>(m_ThreadData.size()))
  {
    return m_ThreadData[threadId];
  }
  else
  {
    itkExceptionMacro(<< "Invalid thread id " << threadId
                      << " or GetThreadData called before m_ThreadData (size="
                      << m_ThreadData.size()
                      << ") was initialized.");
  }
  return ThreadDataStruct(); // keep the compiler happy
}


template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::SetNoiseSigma(const RealType& sigma)
{
  m_NoiseSigma = sigma;
  m_NoiseSigmaSquared = sigma * sigma;
  m_NoiseSigmaIsSet = true;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::CopyInputToOutput()
{
  const typename InputImageType::ConstPointer input  = this->GetInput();
  const typename OutputImageType::Pointer     output = this->GetOutput();

  if ( !input || !output )
  {
    itkExceptionMacro(<< "Input or Output image is NULL.");
  }

  InputImageRegionConstIteratorType inputIt(input,  input->GetRequestedRegion());
  OutputImageRegionIteratorType outputIt(output, output->GetRequestedRegion());
  for (inputIt.GoToBegin(), outputIt.GoToBegin();
       ! outputIt.IsAtEnd();
       ++inputIt, ++outputIt)
  {
    outputIt.Set(inputIt.Get());
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // Typical patch-based denoising algorithms would have access to the entire image for sampling
  // patches for entropy calculations and denoising.  But this may not be memory-friendly for
  // extremely large images.  If the user desires, they can limit the size of the requested region
  // and thus the size of the region available for calculations and patch selection.  But this needs
  // to be managed carefully. This class doesn't allow RequestRegion() yet.
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input
  const typename Superclass::InputImagePointer inputPtr
    = const_cast< InputImageType * >( this->GetInput() );

  if ( !inputPtr )
  {
    return;
  }

  // Try to set up a buffered region that will accommodate our
  // neighborhood operations.  This may not be possible and we
  // need to be careful not to request a region outside the largest
  // possible region, because the pipeline will give us whatever we
  // ask for.

  const PatchRadiusType voxelNeighborhoodSize = this->GetPatchRadiusInVoxels();

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename InputImageType::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();
  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( voxelNeighborhoodSize );
  {
    std::ostringstream msg;
    msg << "Padding inputRequestedRegion by " << voxelNeighborhoodSize << "\n"
        << "inputRequestedRegion: "           << inputRequestedRegion  << "\n"
        << "largestPossibleRegion: "          << inputPtr->GetLargestPossibleRegion() << "\n";
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
    e.SetDescription("Requested region is (at least partially) outside the largest possible region");
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
  const typename OutputImageType::Pointer output = this->GetOutput();

  m_UpdateBuffer->CopyInformation(output);
  m_UpdateBuffer->SetRequestedRegion(output->GetRequestedRegion());
  m_UpdateBuffer->SetBufferedRegion(output->GetBufferedRegion());
  m_UpdateBuffer->Allocate();
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::Initialize()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG, "In Initialize...\n");

  typename InputImageType::IndexType requiredIndex;
  requiredIndex.Fill(0);
  const typename InputImageType::RegionType largestRegion
    = this->GetInput()->GetLargestPossibleRegion();
  const PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  PatchRadiusType two;
  two.Fill(2);
  requiredIndex += two * radius;

  if ( !(largestRegion.IsInside(requiredIndex)) )
  {
    // The image needs to be at least as big as a single patch
    // throw an exception when this isn't the case
    itkExceptionMacro( << "Patch is larger than the entire image (in at least one dimension)."
                       << "\nImage region: " << largestRegion
                       << "\nPatch length (2*radius + 1): " << this->GetPatchDiameterInVoxels()
                       << "\nUse a smaller patch for this image.\n"; )
  }

  // Get the number of pixels in the input image.
  m_TotalNumberPixels = largestRegion.GetNumberOfPixels();

  // Get the number of components in the input image.
  typename InputImageType::IndexType firstIndex;
  firstIndex.Fill(0);
  m_ComponentsPerPixel = this->GetInput()->GetNumberOfComponentsPerPixel();

  // For automatic sigma estimation, select every 'k'th pixel.
  m_SigmaUpdateDecimationFactor
    = static_cast<unsigned int>
    (vnl_math_rnd(1.0 / m_FractionPixelsForSigmaUpdate));
  // For automatic sigma estimation, use at least 1% of pixels.
  m_SigmaUpdateDecimationFactor
    = vnl_math_min(m_SigmaUpdateDecimationFactor,
                   static_cast<unsigned int>
                   (vnl_math_rnd(m_TotalNumberPixels / 100.0)));
  // For automatic sigma estimation, can't use more than 100% of pixels.
  m_SigmaUpdateDecimationFactor
    = vnl_math_max(m_SigmaUpdateDecimationFactor, 1u);
  {
    std::ostringstream msg;
    msg << "m_FractionPixelsForSigmaUpdate: " << m_FractionPixelsForSigmaUpdate << ", "
        << "m_SigmaUpdateDecimationFactor: "  << m_SigmaUpdateDecimationFactor << "\n";
    this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
  }

  // initialize thread data struct
  const unsigned int numThreads = this->GetNumberOfThreads();
  for (unsigned int thread = 0; thread < numThreads; ++thread)
  {
    ThreadDataStruct newStruct;
    newStruct.entropyFirstDerivative.SetSize(m_ComponentsPerPixel);
    newStruct.entropySecondDerivative.SetSize(m_ComponentsPerPixel);
    newStruct.validDerivatives.SetSize(m_ComponentsPerPixel);
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      newStruct.validDerivatives[cc] = 0;
      newStruct.entropyFirstDerivative[cc] = 0;
      newStruct.entropySecondDerivative[cc] = 0;
    }
    newStruct.sampler = NULL;

    m_ThreadData.push_back(newStruct);
  }

  //
  NumericTraits<RealType>::SetLength(m_GaussianKernelSigma, m_ComponentsPerPixel);
  NumericTraits<RealType>::SetLength(m_IntensityRescaleInvFactor, m_ComponentsPerPixel);
  NumericTraits<PixelType>::SetLength(m_ImageMin, m_ComponentsPerPixel);
  NumericTraits<PixelType>::SetLength(m_ImageMax, m_ComponentsPerPixel);
  NumericTraits<PixelType>::SetLength(m_ZeroPixel, m_ComponentsPerPixel);
  m_ZeroPixel = NumericTraits<PixelType>::ZeroValue(m_ZeroPixel);
  this->ComputeMinMax(this->GetInput());

  this->EnforceConstraints();

  // for sigma calculation, keep the intensity range to 100 to avoid numerical issues.
  // compute the inverse factor to do multiplication later instead of division.
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    const PixelValueType max = GetComponent(m_ImageMax, cc);
    const PixelValueType min = GetComponent(m_ImageMin, cc);
    SetComponent(m_IntensityRescaleInvFactor, cc, 100.0 / (max - min));
  }

  if (this->GetDoKernelBandwidthEstimation())
  {
    // kernel sigma initialization for automatic sigma estimation
    this->InitializeKernelSigma();
  }
  else
  {
    // use the kernel sigma has already been set
    // either by default or explicitly by the user
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      // check that the set kernel sigma is valid
      if (GetComponent(m_GaussianKernelSigma,cc) <= m_MinSigma)
      {
        itkExceptionMacro (<< "Gaussian kernel sigma ("
                           << GetComponent(m_GaussianKernelSigma,cc)
                           << ") must be larger than "
                           << m_MinSigma);
      }
    }
  }
  if (!this->m_NoiseSigmaIsSet)
  {
    NumericTraits<RealType>::SetLength(m_NoiseSigma, m_ComponentsPerPixel);
    NumericTraits<RealType>::SetLength(m_NoiseSigmaSquared, m_ComponentsPerPixel);
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      // initialize to 5% of the intensity range
      RealValueType sigma = 5.0 / GetComponent(m_IntensityRescaleInvFactor,cc);
      SetComponent(m_NoiseSigma, cc, sigma);
      SetComponent(m_NoiseSigmaSquared, cc, sigma * sigma);
    }
  }

  if (m_Sampler.IsNull())
  {
    itkExceptionMacro
      (<< "Please supply a subsampling strategy that derives from itkRegionConstrainedSubsampler");
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::EnforceConstraints()
{

  // image cannot be constant
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    if (GetComponent(m_ImageMax, cc) <= GetComponent(m_ImageMin, cc))
    {
    itkExceptionMacro( << "Each image component must be nonconstant.  "
                       << "Component " << cc
                       << " has the constant value " << GetComponent(m_ImageMax, cc)
                       << ".\n"; );
    }
  }

  // for poisson or rician noise models, image must be >= 0
  if ( (this->GetNoiseModel() == Superclass::RICIAN) ||
       (this->GetNoiseModel() == Superclass::POISSON) )
  {
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      if (GetComponent(m_ImageMin, cc) < NumericTraits<PixelValueType>::Zero)
      {
        itkExceptionMacro( << "When using POISSON or RICIAN noise models, "
                           << "all components of all pixels in the image must "
                           << "be >= 0.  The smallest value for component "
                           << cc << " in the image is "
                           << GetComponent(m_ImageMin,cc) << ".\n"; );
      }
    }
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializePatchWeights()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG,
                        "InitializePatchWeights called ...\n");

  if (m_UseSmoothDiscPatchWeights)
  {
    // Redefine patch weights to make the patch more isotropic (less rectangular).
    this->InitializePatchWeightsSmoothDisc();
  }
  else
  {
    // Default patch weights of all unity.
    PatchWeightsType patchWeights;
    patchWeights.SetSize(this->GetPatchLengthInVoxels());
    patchWeights.Fill(1);
    this->SetPatchWeights(patchWeights);
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializePatchWeightsSmoothDisc()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG,
                        "InitializePatchWeightsSmoothDisc called ...\n");

  typedef itk::Image<float,ImageDimension> WeightsImageType;
  typedef float DistanceType;

  const unsigned int patchRadius = this->GetPatchRadius();

  // patch size in physical space
  const unsigned int physicalDiameter= 2 * patchRadius + 1;

  // allocate the patch weights (mask) as an image
  // this is in physical space
  typename WeightsImageType::SizeType physicalSize;
  physicalSize.Fill(physicalDiameter);
  typename WeightsImageType::RegionType physicalRegion(physicalSize);
  typename WeightsImageType::SpacingType physicalSpacing;
  physicalSpacing.Fill(1);
  typename WeightsImageType::Pointer physicalWeightsImage = WeightsImageType::New();
  physicalWeightsImage->SetRegions(physicalRegion);
  physicalWeightsImage->SetSpacing(physicalSpacing);
  physicalWeightsImage->Allocate();
  physicalWeightsImage->FillBuffer(1.0);

  const unsigned int physicalLength = physicalRegion.GetNumberOfPixels();
  const unsigned int centerPosition = (physicalLength - 1) / 2;

  ImageRegionIterator<WeightsImageType> pwIt(physicalWeightsImage, physicalRegion);
  unsigned int pos = 0;
  for ( pwIt.GoToBegin(), pos = 0; !pwIt.IsAtEnd(); ++pwIt, ++pos )
  {
    // compute distances of each pixel from center pixel
    Vector <DistanceType, ImageDimension> distanceVector;
    for (unsigned int d = 0; d < ImageDimension; d++)
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
          = static_cast<DistanceType> (pos            / vnl_math_rnd (pow(static_cast<double>(physicalDiameter),static_cast<int>(d))))
          - static_cast<DistanceType> (centerPosition / vnl_math_rnd (pow(static_cast<double>(physicalDiameter),static_cast<int>(d))));
      }
    }
    const float distanceFromCenter = distanceVector.GetNorm();

    // compute weights based on distance of pixel from center
    // details in Awate and Whitaker 2005 IEEE CVPR, 2006 IEEE TPAMI
    const unsigned int discRadius = patchRadius / 2;
    if (distanceFromCenter >= patchRadius + 1)
    {
      // weights for pixels outside disc of radius 2*discRadius+1
      // zero (minimum)
      pwIt.Set(0.0);
    }
    else if (distanceFromCenter <= discRadius)
    {
      // weights for pixels inside disc of radius discRadius
      // one (maximum)
      pwIt.Set(1.0);
    }
    else
    {
      // weights for pixels between the inner disc and the outer disc
      // weights between zero and one
      // determined by cubic-spline interpolation
      const unsigned int interval= (patchRadius + 1) - discRadius;
      const float weight
        = (-2.0 / pow (interval, 3.0)) * pow ((patchRadius + 1) - distanceFromCenter, 3.0f)
        + ( 3.0 / pow (interval, 2.0)) * pow ((patchRadius + 1) - distanceFromCenter, 2.0f);
      pwIt.Set(vnl_math_max (static_cast<float>(0), vnl_math_min (static_cast<float>(1), weight)));
    }
  }

  // take the patch weights in physical space and
  // resample to get patch weights in voxel space
  typedef itk::ResampleImageFilter<
    WeightsImageType, WeightsImageType > ResampleFilterType;
  typedef itk::IdentityTransform<
    double, ImageDimension > TransformType;
  typedef itk::LinearInterpolateImageFunction<
    WeightsImageType, double > InterpolatorType;

  typename ResampleFilterType::Pointer  resampler = ResampleFilterType::New();
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  typename TransformType::Pointer       transform = TransformType::New();
  transform->SetIdentity();
  //
  resampler->SetTransform( transform );
  resampler->SetInterpolator( interpolator );
  //
  resampler->SetInput( physicalWeightsImage );
  resampler->SetSize( this->GetPatchDiameterInVoxels() );
  resampler->SetDefaultPixelValue( 0 );
  resampler->SetOutputOrigin( physicalWeightsImage->GetOrigin() );
  resampler->SetOutputDirection( physicalWeightsImage->GetDirection() );
  resampler->SetOutputSpacing( this->GetInput()->GetSpacing() );
  resampler->Update();

  // patch weights (mask) in voxel space
  const typename WeightsImageType::Pointer voxelWeightsImage = resampler->GetOutput();
  ImageRegionConstIterator<WeightsImageType>
    vwIt (voxelWeightsImage, voxelWeightsImage->GetLargestPossibleRegion());

  // disc-smooth patch weights in voxel space
  PatchWeightsType patchWeights;
  patchWeights.SetSize(this->GetPatchLengthInVoxels());
  patchWeights.Fill(1);
  for ( vwIt.GoToBegin(), pos = 0; !vwIt.IsAtEnd(); ++vwIt, ++pos )
  {
    patchWeights[pos]
      = vnl_math_max ( static_cast<float>(0), // ensure weight >= 0
                       vnl_math_min ( static_cast<float>(1), // ensure weight <= 1
                                      vwIt.Get() ) );
    {
      std::ostringstream msg;
      msg << "patchWeights[ " << pos << " ]: " << patchWeights[pos] << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
  } // end for each element in the patch
  if (patchWeights[(this->GetPatchLengthInVoxels() - 1)/2] != 1.0)
  {
    itkExceptionMacro (<< "Center pixel's weight ("
                       << patchWeights[(this->GetPatchLengthInVoxels() - 1)/2]
                       << ") must be equal to 1.0 ");
  }

  //
  this->SetPatchWeights(patchWeights);
} // end InitializePatchWeights

template <class TInputImage, class TOutputImage>
template <typename T>
typename NumericTraits<T>::ValueType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedGetComponent(const T& pix, unsigned int itkNotUsed(idx), TrueType itkNotUsed(isPod))
{
  return pix;
}

template <class TInputImage, class TOutputImage>
template <typename T>
typename NumericTraits<T>::ValueType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedGetComponent(const T& pix, unsigned int idx, FalseType itkNotUsed(isPod))
{
  return pix[idx];
}

template <class TInputImage, class TOutputImage>
template <typename T>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedSetComponent(T& pix, unsigned int itkNotUsed(idx),
                         typename NumericTraits<T>::ValueType val, TrueType itkNotUsed(isPod))
{
  pix = val;
}

template <class TInputImage, class TOutputImage>
template <typename T>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedSetComponent(T& pix, unsigned int idx,
                         typename NumericTraits<T>::ValueType val, FalseType itkNotUsed(isPod))
{
  pix[idx] = val;
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedMinMax(const InputImageType* img, TrueType itkNotUsed(isPod))
{
  typedef MinimumMaximumImageFilter<InputImageType> MinMaxFilter;
  typename MinMaxFilter::Pointer minmax = MinMaxFilter::New();
  minmax->SetInput(img);
  minmax->Modified();
  minmax->Update();

  SetComponent(m_ImageMin, 0, minmax->GetMinimum());
  SetComponent(m_ImageMax, 0, minmax->GetMaximum());
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedMinMax(const InputImageType* img, FalseType itkNotUsed(isPod))
{
  typedef NthElementImageAdaptor<InputImageType, PixelValueType> AdaptorType;
  typedef MinimumMaximumImageFilter<AdaptorType>         MinMaxFilter;

  typename AdaptorType::Pointer adaptor = AdaptorType::New();
  adaptor->SetImage(const_cast<InputImageType*>(img));

  typename MinMaxFilter::Pointer minmax = MinMaxFilter::New();

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    adaptor->SelectNthElement(cc);
    minmax->SetInput(adaptor);
    minmax->Modified();
    minmax->Update();

    SetComponent(m_ImageMin, cc, minmax->GetMinimum());
    SetComponent(m_ImageMax, cc, minmax->GetMaximum());
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::DispatchedVectorMinMax(const InputImageType* img)
{
  typedef VectorImageToImageAdaptor< PixelValueType,
                                     ImageDimension > AdaptorType;
  typedef MinimumMaximumImageFilter< AdaptorType >    MinMaxFilter;

  typename AdaptorType::Pointer adaptor = AdaptorType::New();
  adaptor->SetImage(const_cast<InputImageType*>(img));

  typename MinMaxFilter::Pointer minmax = MinMaxFilter::New();

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    adaptor->SetExtractComponentIndex(cc);
    minmax->SetInput(adaptor);
    minmax->Modified();
    minmax->Update();

    SetComponent(m_ImageMin, cc, minmax->GetMinimum());
    SetComponent(m_ImageMax, cc, minmax->GetMaximum());
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeKernelSigma()
{
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    // initialize kernel sigma to 10% of the intensity range.
    // if the range is 100, the initial sigma will be 10.
    SetComponent(m_GaussianKernelSigma,cc,
                 10.0 * m_SigmaMultiplicationFactor / GetComponent(m_IntensityRescaleInvFactor,cc));
    {
      std::ostringstream msg;
      msg << "For component " << cc << ", "
          << "Image Intensity range: ["
          << GetComponent(m_ImageMin,cc) << ","
          << GetComponent(m_ImageMax,cc) << "], "
          << "IntensityRescaleInvFactor[" << cc << "]: "
          << GetComponent(m_IntensityRescaleInvFactor,cc) << " , "
          << "SigmaMultiplicationFactor: "
          << m_SigmaMultiplicationFactor << " , "
          << "GaussianKernelSigma initialized to: "
          << GetComponent(m_GaussianKernelSigma,cc) << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
  }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::InitializeIteration()
{
  this->m_Logger->Write(itk::LoggerBase::DEBUG, "Initializing Iteration now...\n");

  const PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  const typename OutputImageType::Pointer output = this->GetOutput();

  // Have sampler update any internal structures
  // across the entire image for each iteration
  m_SearchSpaceList->SetImage(output);
  m_SearchSpaceList->SetRadius(radius);
  m_Sampler->SetSample(m_SearchSpaceList);

  // re-initialize thread data struct, especially validity flags
  const unsigned int structSize = m_ThreadData.size();
  for (unsigned int thread = 0; thread < structSize; ++thread)
  {
    // reset entropy derivatives that are used to compute optimal sigma
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      m_ThreadData[thread].validDerivatives[cc] = 0;
      m_ThreadData[thread].entropyFirstDerivative[cc] = 0;
      m_ThreadData[thread].entropySecondDerivative[cc] = 0;
    }

    // provide a sampler to the thread
    m_ThreadData[thread].sampler = dynamic_cast<BaseSamplerType*>(m_Sampler->Clone().GetPointer());
    typename ListAdaptorType::Pointer searchList = ListAdaptorType::New();
    searchList->SetImage(output);
    searchList->SetRadius(radius);
    m_ThreadData[thread].sampler->SetSeed(thread);
    m_ThreadData[thread].sampler->SetSample(searchList);
    m_ThreadData[thread].sampler->SetSampleRegion(searchList->GetRegion());
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
  // through iterators which don't increment the
  // output timestamp
  this->GetOutput()->Modified();
}

template <class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ApplyUpdateThreaderCallback( void * arg )
{
  const unsigned int threadId    = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  const unsigned int threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  const ThreadFilterStruct * str
    = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  const unsigned int total = str->Filter->SplitRequestedRegion(threadId, threadCount,
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
  ImageAlgorithm::Copy(m_UpdateBuffer.GetPointer(), this->GetOutput(),
                       regionToProcess, regionToProcess);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeKernelBandwidthUpdate()
{
  // Set up for multithreaded processing.
  ThreadFilterStruct str;
  str.Filter = this;

  // calculate sigma update
  this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
  this->GetMultiThreader()->SetSingleMethod(this->ComputeSigmaUpdateThreaderCallback,
                                            &str);

  m_SigmaConverged.SetSize(m_ComponentsPerPixel);
  m_SigmaConverged.Fill(0);

  // back out the multiplication factor prior to optimizing, then put it back afterwards
  // rescale Gaussian kernel sigma assuming intensity range of 100, then undo afterwards
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    RealValueType scaledVal = GetComponent(m_GaussianKernelSigma, cc);
    scaledVal /= m_SigmaMultiplicationFactor;
    scaledVal *= GetComponent(m_IntensityRescaleInvFactor, cc);
    SetComponent(m_GaussianKernelSigma, cc, scaledVal);
  }

  RealArrayType sigmaUpdate;

  // perform Newton-Raphson optimization to find the optimal kernel sigma
  for (unsigned int i = 0; i < MaxSigmaUpdateIterations; ++i)
  {
    {
      std::ostringstream msg;
      msg << "Computing iteration " << i
          << " of kernel sigma update" << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }

    // multi-threaded computation of the first and second derivatives
    // of the entropy with respect to sigma
    this->GetMultiThreader()->SingleMethodExecute();

    // accumulate results from each thread and
    // appropriately update sigma after checks to prevent divergence or invalid sigma values
    sigmaUpdate = this->ResolveSigmaUpdate();

    {
      std::ostringstream msg;
      msg << "sigmaUpdate: " << sigmaUpdate
          << ", m_GaussianKernelSigma: " << m_GaussianKernelSigma
          << ", m_SigmaUpdateConvergenceTolerance: " << m_SigmaUpdateConvergenceTolerance
          << std::endl;
      this->m_Logger->Write(itk::LoggerBase::DEBUG, msg.str());
    }

    // check for convergence
    bool all_converged = true;
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      if (!m_SigmaConverged[cc])
      {
        if (vnl_math_abs(sigmaUpdate[cc]) <
            GetComponent(m_GaussianKernelSigma,cc) * m_SigmaUpdateConvergenceTolerance)
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
  } // end Newton-Raphson iterations

  // undo rescale of Gaussian kernel sigma
  // put the multiplication factor back in
  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
    RealValueType scaledVal = GetComponent(m_GaussianKernelSigma, cc);
    scaledVal /= GetComponent(m_IntensityRescaleInvFactor, cc);
    scaledVal *= m_SigmaMultiplicationFactor;
    SetComponent(m_GaussianKernelSigma, cc, scaledVal);
  }
}

template <class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeSigmaUpdateThreaderCallback( void * arg )
{
  const unsigned int threadId    = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  const unsigned int threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  const ThreadFilterStruct * str
    = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  const unsigned int total
    = str->Filter->SplitRequestedRegion(threadId, threadCount, splitRegion);

  if (threadId < total)
  {
    str->Filter->SetThreadData(threadId,
             str->Filter->ThreadedComputeSigmaUpdate(splitRegion, threadId,
                                       str->Filter->GetThreadData(threadId)));
  }

  return ITK_THREAD_RETURN_VALUE;
}

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::ThreadDataStruct
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ThreadedComputeSigmaUpdate(const InputImageRegionType &regionToProcess,
                             const int itkNotUsed(threadId),
                             ThreadDataStruct threadData)
{
  // create two image to list adaptors, one for the iteration over the region
  // the other for querying to find the patches
  // set the region of interest for the second to only include patches with at least as many
  //     in-bounds neighbors in the same areas of the patch as the query patch
  // initialize accumulators
  // for each element in the region
  //   set region of interest
  //   select a set of patches (random, gaussian, etc)
  //   for each of the patches
  //     compute the difference between the element's patch and the selected patch
  //     compute entropy derivatives using this difference
  //
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<OutputImageType>
    FaceCalculatorType;
  typedef typename FaceCalculatorType::FaceListType FaceListType;
  typedef typename ListAdaptorType::ConstIterator SampleIteratorType;

  const PatchRadiusType radius = this->GetPatchRadiusInVoxels();

  const typename OutputImageType::Pointer output = this->GetOutput();
  typename ListAdaptorType::Pointer inList = ListAdaptorType::New();
  inList->SetImage(output);
  inList->SetRadius(radius);

  BaseSamplerPointer sampler = threadData.sampler;

  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  // For the sigma estimation, we only want to use pixels from the first
  // region to avoid using boundary conditions (and it's faster).

  FaceCalculatorType faceCalculator;
  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt;

  const unsigned int lengthPatch = this->GetPatchLengthInVoxels();
  const unsigned int center = (lengthPatch - 1) / 2;
  RealArrayType jointEntropyFirstDerivative(m_ComponentsPerPixel);
  RealArrayType jointEntropySecondDerivative(m_ComponentsPerPixel);
  RealArrayType nbhdEntropyFirstDerivative(m_ComponentsPerPixel);
  RealArrayType nbhdEntropySecondDerivative(m_ComponentsPerPixel);
  jointEntropyFirstDerivative.Fill(0.0);
  jointEntropySecondDerivative.Fill(0.0);
  nbhdEntropyFirstDerivative.Fill(0.0);
  nbhdEntropySecondDerivative.Fill(0.0);

  // Only use pixels whose patch is entirely in bounds
  // for the sigma calculation
  fIt = faceList.begin();
  if (!(fIt->GetNumberOfPixels()))
  {
    // empty region, don't use.
    return threadData;
  }
  inList->SetRegion(*fIt);

  unsigned int sampleNum = 0;
  for (SampleIteratorType sampleIt = inList->Begin();
       sampleIt != inList->End();
       ++sampleIt, ++sampleNum)
  {
    if (sampleNum % m_SigmaUpdateDecimationFactor != 0)
    {
      // skip this sample
      continue;
    }
    InputImagePatchIterator currentPatch = sampleIt.GetMeasurementVector()[0];
    typename OutputImageType::IndexType  nIndex = currentPatch.GetIndex();
    InstanceIdentifier currentPatchId = inList->GetImage()->ComputeOffset(nIndex);

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
      rIndex[dim] = vnl_math_min(static_cast<long unsigned int>(nIndex[dim]),
                                 radius[dim]);
      rSize[dim]  = vnl_math_max(static_cast<long unsigned int>(nIndex[dim]),
                                 rSize[dim] - radius[dim] - 1) - rIndex[dim] + 1;
    }
    region.SetIndex(rIndex);
    region.SetSize(rSize);

    typename BaseSamplerType::SubsamplePointer selectedPatches =
      BaseSamplerType::SubsampleType::New();
    sampler->SetRegionConstraint(region);
    sampler->CanSelectQueryOff();
    sampler->Search(currentPatchId, selectedPatches);

    const unsigned int numPatches = selectedPatches->GetTotalFrequency();

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
          // save this off to avoid unnecessary repetitive accesses in calculations below
          const RealValueType sigmaKernel = GetComponent(m_GaussianKernelSigma,cc);

          // compute difference between selectedPatches[ii] and currentPatch

          // since we make sure that the search query can only take place in a certain image region
          // it is sufficient to rely on the fact that the current patch is in bounds

          selectedPatch.NeedToUseBoundaryConditionOff();
          // this partial loop unrolling works because the length of the patch is always odd
          // guaranteeing that center - 0 == lengthPatch - center+1 always
          RealValueType squaredNorm = 0.0;
          for (unsigned int jj = 0, kk = center+1; jj < center; ++jj, ++kk)
          {
            const PixelValueType selectedVal1 = GetComponent(selectedPatch.GetPixel(jj),cc);
            const PixelValueType selectedVal2 = GetComponent(selectedPatch.GetPixel(kk),cc);

            // rescale intensities, and differences, to a range of 100
            const RealValueType diff1
              = GetComponent(m_IntensityRescaleInvFactor,cc) * (GetComponent(currentPatchVec[jj],cc) - selectedVal1);
            const RealValueType diff2
              = GetComponent(m_IntensityRescaleInvFactor,cc) * (GetComponent(currentPatchVec[kk],cc) - selectedVal2);

            squaredNorm += diff1 * diff1;
            squaredNorm += diff2 * diff2;
          }
          const PixelValueType selectedVal = GetComponent(selectedPatch.GetPixel(center),cc);

          // rescale intensities, and differences, to a range of 100
          const RealValueType diff
            = GetComponent(m_IntensityRescaleInvFactor,cc) * (GetComponent(currentPatchVec[center],cc) - selectedVal);

          squaredNorm += diff * diff;
          centerPatchDifference = diff;

          const RealValueType distanceJointEntropy = vcl_sqrt(squaredNorm);

          const RealValueType gaussianJointEntropy
            = exp(- vnl_math_sqr(distanceJointEntropy / sigmaKernel) / 2.0);

          probJointEntropy[cc] += gaussianJointEntropy;

          const RealValueType factorJoint
            = squaredNorm / pow(sigmaKernel, 3.0) - (lengthPatch * 1 / sigmaKernel);

          probJointEntropyFirstDerivative[cc] += gaussianJointEntropy * factorJoint;
          probJointEntropySecondDerivative[cc]
            += gaussianJointEntropy
            * (vnl_math_sqr(factorJoint) +
               (lengthPatch * 1 / vnl_math_sqr(sigmaKernel)) -
               (3.0 * squaredNorm / pow(sigmaKernel, 4.0)));
          if (m_ComputeConditionalDerivatives)
          {
            const RealValueType distancePatchEntropySquared
              = squaredNorm - vnl_math_sqr(centerPatchDifference);
            const RealValueType distancePatchEntropy = vcl_sqrt(distancePatchEntropySquared);
            const RealValueType gaussianPatchEntropy
              = exp(- vnl_math_sqr(distancePatchEntropy / sigmaKernel) / 2.0);
            probPatchEntropy[cc] += gaussianPatchEntropy;
            const RealValueType factorPatch
              = distancePatchEntropySquared / pow(sigmaKernel, 3.0)
              - ((lengthPatch-1) / sigmaKernel);

            probPatchEntropyFirstDerivative[cc] += gaussianPatchEntropy * factorPatch;
            probPatchEntropySecondDerivative[cc]
              += gaussianPatchEntropy
              * (vnl_math_sqr(factorPatch) +
                 ((lengthPatch-1) / vnl_math_sqr(sigmaKernel)) -
                 (3.0*distancePatchEntropySquared / pow(sigmaKernel, 4.0)));
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

        itkAssertOrThrowMacro(probJointEntropy[cc] > 0.0, "probJointEntropy must be > 0.0");

        jointEntropyFirstDerivative[cc]
          -= probJointEntropyFirstDerivative[cc] / probJointEntropy[cc];
        jointEntropySecondDerivative[cc]
          -= probJointEntropySecondDerivative[cc] / probJointEntropy[cc]
          - vnl_math_sqr(probJointEntropyFirstDerivative[cc] / probJointEntropy[cc]);

        if (m_ComputeConditionalDerivatives)
        {
          probPatchEntropy[cc] /= numPatches;
          probPatchEntropy[cc] += m_MinProbability;
          probPatchEntropyFirstDerivative[cc] /= numPatches;
          probPatchEntropyFirstDerivative[cc] += m_MinProbability;
          probPatchEntropySecondDerivative[cc] /= numPatches;
          probPatchEntropySecondDerivative[cc] += m_MinProbability;

          itkAssertOrThrowMacro(probPatchEntropy[cc] > 0.0, "probPatchEntropy must be > 0.0");

          nbhdEntropyFirstDerivative[cc]
            -= probPatchEntropyFirstDerivative[cc] / probPatchEntropy[cc];
          nbhdEntropySecondDerivative[cc]
            -= probPatchEntropySecondDerivative[cc] / probPatchEntropy[cc]
            - vnl_math_sqr(probPatchEntropyFirstDerivative[cc] / probPatchEntropy[cc]);
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
        threadData.entropyFirstDerivative[cc]
          = jointEntropyFirstDerivative[cc]  - nbhdEntropyFirstDerivative[cc];
        threadData.entropySecondDerivative[cc]
          = jointEntropySecondDerivative[cc] - nbhdEntropySecondDerivative[cc];
      }
      else
      {
        threadData.entropyFirstDerivative[cc]  = jointEntropyFirstDerivative[cc];
        threadData.entropySecondDerivative[cc] = jointEntropySecondDerivative[cc];
      }
      threadData.validDerivatives[cc] = 1;
    }
    else
    {
      threadData.validDerivatives[cc] = 0;
    }
  } // end for each pixel component
  return threadData;
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


    // accumulate the first and second derivatives from all the threads
    const RealValueType kernelSigma = GetComponent(m_GaussianKernelSigma, cc);
    RealValueType firstDerivative = 0;
    RealValueType secondDerivative = 0;
    const unsigned int numThreads = m_ThreadData.size();
    for (unsigned int threadNum = 0; threadNum < numThreads; ++threadNum)
    {
      if (m_ThreadData[threadNum].validDerivatives[cc] > 0)
      {
        firstDerivative  += m_ThreadData[threadNum].entropyFirstDerivative[cc];
        secondDerivative += m_ThreadData[threadNum].entropySecondDerivative[cc];
      }
    }
    firstDerivative  /= static_cast<RealValueType>(m_TotalNumberPixels);
    secondDerivative /= static_cast<RealValueType>(m_TotalNumberPixels);
    {
      std::ostringstream msg;
      msg << "first deriv: " << firstDerivative
          << ", second deriv: " << secondDerivative
          << ", old sigma: " << kernelSigma
          << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }

    // if second derivative is zero or negative, compute update using gradient descent
    if ( (vnl_math_abs(secondDerivative) == NumericTraits<RealValueType>::Zero) ||
         (secondDerivative < 0) )
    {
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** Second derivative NOT POSITIVE \n");
      sigmaUpdate[cc] = -vnl_math_sgn(firstDerivative) * kernelSigma * 0.3;
    }
    else
    {
      // compute update using Newton-Raphson
      sigmaUpdate[cc] = -firstDerivative / secondDerivative;
    }
    {
      std::ostringstream msg;
      msg << "update: " << sigmaUpdate[cc] << std::endl;
      this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }


    // avoid very large updates to prevent instabilities in Newton-Raphson
    if (vnl_math_abs(sigmaUpdate[cc]) > kernelSigma * 0.3)
    {
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** Restricting large updates \n");
      sigmaUpdate[cc] = vnl_math_sgn(sigmaUpdate[cc]) * kernelSigma * 0.3;
      {
        std::ostringstream msg;
        msg << "update: " << sigmaUpdate[cc] << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }


    // keep the updated sigma from being too close to zero
    if (kernelSigma + sigmaUpdate[cc] < m_MinSigma)
    {
      this->m_Logger->Write(itk::LoggerBase::INFO,
                            "** TOO SMALL SIGMA: adjusting sigma\n");
      SetComponent(m_GaussianKernelSigma, cc, (kernelSigma + m_MinSigma) / 2.0);
      {
        std::ostringstream msg;
        msg << "new sigma: " << GetComponent(m_GaussianKernelSigma, cc) << std::endl;
        this->m_Logger->Write(itk::LoggerBase::INFO, msg.str());
      }
    }
    else
    {
      SetComponent(m_GaussianKernelSigma, cc, kernelSigma + sigmaUpdate[cc]);
      {
        std::ostringstream msg;
        msg << "new sigma: " << GetComponent(m_GaussianKernelSigma, cc) << std::endl;
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

  // compute smoothing updated for intensites at each pixel
  // based on gradient of the joint entropy
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
  const unsigned int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
  const unsigned int threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  const ThreadFilterStruct *str
    = (ThreadFilterStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  InputImageRegionType splitRegion;

  const unsigned int total
    = str->Filter->SplitRequestedRegion(threadId, threadCount, splitRegion);

  if (threadId < total)
  {
    str->Filter->SetThreadData(threadId,
             str->Filter->ThreadedComputeImageUpdate(splitRegion, threadId,
                                       str->Filter->GetThreadData(threadId)));
  }

  return ITK_THREAD_RETURN_VALUE;
}

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::ThreadDataStruct
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ThreadedComputeImageUpdate(const InputImageRegionType &regionToProcess,
                             const int threadId,
                             ThreadDataStruct threadData)
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
  typedef GaussianOperator<RealValueType, OutputImageType::ImageDimension> GaussianOperatorType;

  const PatchRadiusType radius = this->GetPatchRadiusInVoxels();

  typename OutputImageType::Pointer output = this->GetOutput();
  typename ListAdaptorType::Pointer inList = ListAdaptorType::New();
  inList->SetImage(output);
  inList->SetRadius(radius);

  BaseSamplerPointer sampler = threadData.sampler;

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

    // needed because the modified bessel functions are protected member functions of
    // GaussianOperator for some strange reason instead of being there own proper functions.
    GaussianOperatorType gOper;

    InputImageRegionConstIteratorType inputIt(this->GetInput(), *fIt);
    OutputImageRegionIteratorType     updateIt(m_UpdateBuffer, *fIt);
    OutputImageRegionIteratorType     outputIt(output, *fIt);

    updateIt.GoToBegin();
    outputIt.GoToBegin();
    inputIt.GoToBegin();

    for (SampleIteratorType sampleIt = inList->Begin(); sampleIt != inList->End(); ++sampleIt)
    {
      RealType result = outputIt.Get();

      const double smoothingWeight = this->GetSmoothingWeight();
      if (smoothingWeight > 0)
      {
        // get intensity update driven by patch-based denoiser
        const RealType gradientJointEntropy
          = this->ComputeGradientJointEntropy(sampleIt.GetInstanceIdentifier(), inList, sampler);

        for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
        {
          const RealValueType stepSizeSmoothing = 0.2;
          // we choose stepsize to 0.2 because empirical studies indicate that
          // 0.2 is a sufficiently small value that avoids instiabilities in most cases
          const RealValueType smoothVal = smoothingWeight * (stepSizeSmoothing * GetComponent(gradientJointEntropy, cc));
          SetComponent(result, cc, GetComponent(result, cc) + smoothVal);
        }
      } // end if smoothingWeight > 0

      const double fidelityWeight = this->GetFidelityWeight();
      if (fidelityWeight > 0)
      {
        const PixelType in  = inputIt.Get();
        const PixelType out = outputIt.Get();
        switch (this->GetNoiseModel())
        {
        case Superclass::GAUSSIAN:
        {
          for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
          {
            const RealValueType gradientFidelity = 2.0 * (GetComponent(in,cc) - GetComponent(out,cc));
            const RealValueType stepSizeFidelity = 0.5;
            const RealValueType noiseVal = fidelityWeight * ( stepSizeFidelity * gradientFidelity );
            SetComponent(result, cc,
                         GetComponent(result, cc) + noiseVal);
          }
          break;
        }
        case Superclass::RICIAN:
        {
          for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
          {
            const PixelValueType inVal = GetComponent(in, cc);
            const PixelValueType outVal = GetComponent(out, cc);
            const RealValueType sigmaSquared = GetComponent(m_NoiseSigmaSquared, cc);

            const RealValueType alpha = inVal * outVal / sigmaSquared;
            const RealValueType gradientFidelity
              = (inVal * (gOper.ModifiedBesselI1(alpha) / gOper.ModifiedBesselI0(alpha)) - outVal)
              / sigmaSquared;
            const RealValueType stepSizeFidelity = sigmaSquared;
            // update
            const RealValueType noiseVal = fidelityWeight * ( stepSizeFidelity * gradientFidelity );
            // ensure that the result is nonnegative
            SetComponent(result, cc,
                         vnl_math_max(GetComponent(result, cc) + noiseVal, static_cast<RealValueType>(0.0)));
          }
          break;
        }
        case Superclass::POISSON:
        {
          for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
          {
            const PixelValueType inVal = GetComponent(in, cc);
            const PixelValueType outVal = GetComponent(out, cc);

            const RealValueType gradientFidelity = (inVal - outVal) / (outVal + 0.00001);
            // prevent large unstable updates when out[cc] less than 1
            const RealValueType stepSizeFidelity
              = vnl_math_min(outVal, static_cast<PixelValueType>(0.99999)) + 0.00001;
            // update
            const RealValueType noiseVal = fidelityWeight * ( stepSizeFidelity * gradientFidelity );
            // ensure that the result is positive
            SetComponent(result, cc,
                         vnl_math_max(GetComponent(result, cc) + noiseVal, static_cast<RealValueType>(0.00001)));

          }
          break;
        }
        default:
        {
          itkExceptionMacro(<< "Unexpected noise model " << this->GetNoiseModel() << " specified.");
          break;
        }
        }
      } // end if fidelityWeight > 0

      // set update value, because we can't change the output until the other threads are
      // done using it.
      // updateBuffer contains the current update (referred to by updateIt)
      // this->GetInput() contains the original data (referred to by inputIt)
      // this->GetOutput() contains results from the previous iteration (referred to by outputIt)
      // at the end of this update, updateIt will hold the current iteration image
      // this->ApplyUpdate() will then copy the results to outputIt since we have to wait
      // until all threads have finished using outputIt to compute the update
      updateIt.Set(static_cast<PixelType>(result));

      ++updateIt;
      ++outputIt;
      ++inputIt;

      progress.CompletedPixel();
    } // end for each pixel in the sample
  } // end for each face in the face list
  return threadData;
} // end ThreadedComputeImageUpdate

template <class TInputImage, class TOutputImage>
typename PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>::RealType
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::ComputeGradientJointEntropy(InstanceIdentifier id,
                              typename ListAdaptorType::Pointer& inList,
                              BaseSamplerPointer& sampler)
{
  const InputImagePatchIterator currentPatch = inList->GetMeasurementVector(id)[0];
  const typename OutputImageType::IndexType nIndex = currentPatch.GetIndex();
  const InstanceIdentifier currentPatchId = inList->GetImage()->ComputeOffset(nIndex);

  const unsigned int lengthPatch = this->GetPatchLengthInVoxels();
  const unsigned int center = (lengthPatch - 1) / 2;

  const typename OutputImageType::Pointer output = this->GetOutput();
  typename OutputImageType::RegionType region = this->GetInput()->GetLargestPossibleRegion();
  typename OutputImageType::IndexType  rIndex;
  typename OutputImageType::SizeType   rSize = region.GetSize();
  const PatchRadiusType radius = this->GetPatchRadiusInVoxels();
  for (unsigned int dim = 0; dim < OutputImageType::ImageDimension; ++dim)
  {
    rIndex[dim] = vnl_math_min(static_cast<long unsigned int>(nIndex[dim]), radius[dim]);
    rSize[dim]  = vnl_math_max(static_cast<long unsigned int>(nIndex[dim]), rSize[dim] - radius[dim] - 1)
      - rIndex[dim] + 1;
  }
  region.SetIndex(rIndex);
  region.SetSize(rSize);

  typename BaseSamplerType::SubsamplePointer selectedPatches =
    BaseSamplerType::SubsampleType::New();

  sampler->SetRegionConstraint(region);
  sampler->CanSelectQueryOn();
  sampler->Search(currentPatchId, selectedPatches);

  const unsigned int numPatches = selectedPatches->GetTotalFrequency();

  RealArrayType centerPatchDifference(m_ComponentsPerPixel);
  centerPatchDifference.Fill(0.0);
  VariableLengthVector<PixelType> currentPatchVec(lengthPatch);
  VariableLengthVector<unsigned short> IsInBoundsVec(lengthPatch);
  // store the current patch prior to iterating over the selected patches
  // to avoid repeatedly calling GetPixel for this patch
  //
  // Determine which pixels are InBounds for the patch around the pixel being denoised.
  // Distances for this patch are computed only based on pixels that are InBounds.
  // As a result, the values of pixels outside the boundary is ignored, which effectively
  // ignores the boundary condition associated with the ListAdaptorType
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

  typename OutputImageType::IndexType lastSelectedIdx;
  typename OutputImageType::IndexType currSelectedIdx;
  InputImagePatchIterator selectedPatch;
  if (numPatches > 0)
  {
    selectedPatch = selectedPatches->Begin().GetMeasurementVector()[0];
    lastSelectedIdx = selectedPatch.GetIndex();
  }

  const PatchWeightsType patchWeights = this->GetPatchWeights();

  RealValueType sumOfGaussiansJointEntropy = 0.0;

  RealType gradientJointEntropy = m_ZeroPixel;

  for (typename BaseSamplerType::SubsampleConstIterator selectedIt = selectedPatches->Begin();
       selectedIt != selectedPatches->End();
       ++selectedIt)
  {
    currSelectedIdx = selectedIt.GetMeasurementVector()[0].GetIndex();
    selectedPatch += currSelectedIdx - lastSelectedIdx;
    lastSelectedIdx = currSelectedIdx;

    RealValueType squaredNorm = 0.0;
    RealValueType distanceJointEntropy = 0.0;

    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      RealValueType kernelSigma = GetComponent(m_GaussianKernelSigma, cc);
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
          const RealValueType weight1 = patchWeights[jj];
          const RealValueType weight2 = patchWeights[kk];
          // PixelValueType selectedVal1 = *(selectedPatch[jj]); // faster for scalar pixel types
          // PixelValueType selectedVal2 = *(selectedPatch[kk]);
          const PixelValueType selectedVal1 = GetComponent(selectedPatch.GetPixel(jj),cc);
          const PixelValueType selectedVal2 = GetComponent(selectedPatch.GetPixel(kk),cc);
          const RealValueType diff1 = weight1 * (selectedVal1 - GetComponent(currentPatchVec[jj],cc));
          const RealValueType diff2 = weight2 * (selectedVal2 - GetComponent(currentPatchVec[kk],cc));
          squaredNorm += diff1 * diff1;
          squaredNorm += diff2 * diff2;
        }

        // now compute the center value
        const RealValueType weight = patchWeights[center];
        const PixelValueType selectedVal = GetComponent(selectedPatch.GetPixel(center),cc);
        const RealValueType diff = weight * (selectedVal - GetComponent(currentPatchVec[center],cc));
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
            const RealValueType weight = patchWeights[jj];
            const PixelValueType selectedVal = GetComponent(selectedPatch.GetPixel(jj),cc);
            const RealValueType diff = weight * (selectedVal - GetComponent(currentPatchVec[jj],cc));
            squaredNorm += diff * diff;
          }
          if (IsInBoundsVec[kk])
          {
            const RealValueType weight = patchWeights[kk];
            const PixelValueType selectedVal = GetComponent(selectedPatch.GetPixel(kk),cc);
            const RealValueType diff = weight * (selectedVal - GetComponent(currentPatchVec[kk],cc));
            squaredNorm += diff * diff;
          }
        }

        // compute the center value (center is always in bounds)
        const RealValueType weight = patchWeights[center];
        // for vector
        const RealValueType diff
          = weight * (GetComponent(selectedPatch.GetPixel(center),cc) - GetComponent(currentPatchVec[center],cc));
        // faster for scalar
        // RealValueType diff = weight * (*(selectedPatch[center]) - currentPatchVec[center]);
        squaredNorm += diff * diff;
        centerPatchDifference[cc] = diff;
      } // end if entire patch is inbounds

      distanceJointEntropy += squaredNorm / vnl_math_sqr(kernelSigma);
    } // end for each pixel component

    const RealValueType gaussianJointEntropy = exp( -distanceJointEntropy / 2.0);
    sumOfGaussiansJointEntropy += gaussianJointEntropy;
    for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
    {
      SetComponent(gradientJointEntropy, cc,
                   GetComponent(gradientJointEntropy, cc) + centerPatchDifference[cc] * gaussianJointEntropy);
    }
  } // end for each selected patch

  for (unsigned int cc = 0; cc < m_ComponentsPerPixel; ++cc)
  {
      SetComponent(gradientJointEntropy, cc,
                   GetComponent(gradientJointEntropy, cc) / (sumOfGaussiansJointEntropy + m_MinProbability));
  }

  return gradientJointEntropy;
} // end ComputeGradientJointEntropy

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::PostProcessOutput()
{
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Total number of pixels: "
     << m_TotalNumberPixels << std::endl;
  os << indent << "ComponentsPerPixel: "
     << m_ComponentsPerPixel << std::endl;
  //
  os << indent << "PatchRadius (physical space): "
     << this->GetPatchRadius() << std::endl;
  os << indent << "PatchRadius (voxel space): "
     << this->GetPatchRadiusInVoxels() << std::endl;
  os << indent << "Use smooth disc patch weights: "
     << m_UseSmoothDiscPatchWeights << std::endl;
  //
  os << indent << "Smoothing weight: "
     << this->GetSmoothingWeight() << std::endl;
  os << indent << "Fidelity weight: "
     << this->GetFidelityWeight() << std::endl;
  //
  os << indent << "Gaussian kernel sigma: "
     << m_GaussianKernelSigma << std::endl;
  os << indent << "Compute conditional derivatives: "
     << m_ComputeConditionalDerivatives << std::endl;
  os << indent << "FractionPixelsForSigmaUpdate: "
     << m_FractionPixelsForSigmaUpdate << std::endl;
  os << indent << "SigmaUpdateDecimationFactor: "
     << m_SigmaUpdateDecimationFactor << std::endl;
  os << indent << "Min probability: "
     << m_MinProbability << std::endl;
  os << indent << "Min sigma: "
     << m_MinSigma       << std::endl;
  os << indent << "Sigma update convergence tolerance: "
     << m_SigmaUpdateConvergenceTolerance << std::endl;
  os << indent << "SigmaMultiplicationFactor: "
     << m_SigmaMultiplicationFactor << std::endl;

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
  os << std::endl;
}

}// end namespace itk

#endif
