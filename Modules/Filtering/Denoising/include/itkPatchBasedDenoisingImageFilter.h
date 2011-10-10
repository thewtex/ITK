#ifndef __itkPatchBasedDenoisingImageFilter_h
#define __itkPatchBasedDenoisingImageFilter_h

#include "itkPatchBasedDenoisingBaseImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkVector.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkRegionConstrainedSubsampler.h"

namespace itk
{

/** \class PatchBasedDenoisingImageFilter
 * \brief Derived class implementing a specific patch-based denoising algorithm, as detailed below.
 *
 * This class is derived from the base class PatchBasedDenoisingBaseImageFilter; please refer to the
 * documentation of the base class first. This class implements a denoising filter that uses
 * iterative non-local, or semi-local, weighted averaging of image patches for image denoising. The
 * intensity at each pixel 'p' gets updated as a weighted average of intensities of a chosen subset
 * of pixels from the image.
 *
 * This class implements the denoising algorithm using a Gaussian kernel function for nonparametric
 * density estimation. The class implements a scheme to automatically estimated the kernel bandwidth
 * parameter (namely, sigma) using leave-one-out cross validation. It implements schemes for random
 * sampling of patches non-locally (from the entire image) as well as semi-locally (from the spatial
 * proximity of the pixel being denoised at the specific point in time). It implements a specific
 * scheme for defining patch weights (mask) as described in Awate and Whitaker 2005 IEEE CVPR and
 * 2006 IEEE TPAMI.
 *
 * \ingroup Filtering
 * \ingroup ITKDenoising
 * \sa PatchBasedDenoisingBaseImageFilter
 */

template <class TInputImage, class TOutputImage>
class ITK_EXPORT PatchBasedDenoisingImageFilter :
public PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
{
 public:
  /** Standard class typedefs. */
  typedef PatchBasedDenoisingImageFilter                                Self;
  typedef PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                                            Pointer;
  typedef SmartPointer<const Self>                                      ConstPointer;
  typedef typename Superclass::OutputImagePointer                       OutputImagePointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PatchBasedDenoisingImageFilter, ImageToImageFilter);

  /** Type definition for the input image. */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;

  /** Image dimension, assumed to be the same for input and output data*/
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Type definition for the input image region and size type. */
  typedef typename InputImageType::RegionType           InputImageRegionType;

  /** Type definition for the input image region iterator */
  typedef ImageRegionIterator<OutputImageType>          OutputImageRegionIteratorType;
  typedef ImageRegionConstIterator<InputImageType>      InputImageRegionConstIteratorType;

  /** Type definition for the input and output pixel types.
      Output pixel type will be used in computations. */
  typedef typename Superclass::PixelType      PixelType;
  typedef typename Superclass::PixelValueType PixelValueType;

  typedef typename NumericTraits< PixelType >::RealType RealType;
  typedef typename NumericTraits< PixelValueType >::RealType RealValueType;
  typedef Array<RealValueType>  RealArrayType;
  typedef Array<unsigned short> ShortArrayType;

  /** Type definition for patch weights type. */
  typedef typename Superclass::ListAdaptorType ListAdaptorType;
  typedef typename Superclass::PatchRadiusType PatchRadiusType;
  typedef typename Superclass::InputImagePatchIterator InputImagePatchIterator;
  typedef ListAdaptorType PatchSampleType;

  typedef typename Superclass::PatchWeightsType PatchWeightsType;

  /** Type definitions for delegate classes. */
  typedef itk::Statistics::RegionConstrainedSubsampler<
    PatchSampleType, InputImageRegionType >  BaseSamplerType;
  typedef typename BaseSamplerType::Pointer  BaseSamplerPointer;
  typedef typename BaseSamplerType::InstanceIdentifier InstanceIdentifier;



  /** Set/Get flag indicating whether smooth-disc patch weights should be used.
   *  If this flag is true, the smooth-disc patch weights will override any
   *  weights provided via the SetPatchWeights method.
   */
  itkSetMacro(UseSmoothDiscPatchWeights, bool);
  itkBooleanMacro(UseSmoothDiscPatchWeights);
  itkGetConstMacro(UseSmoothDiscPatchWeights, bool);



  /** Set/Get initial sigma estimate.
   * To prevent the class from automatically modifying this estimate,
   * set DoKernelBandwidthEstimation to false in the base class.
   */
  itkSetMacro(GaussianKernelSigma, RealType);
  itkGetConstMacro(GaussianKernelSigma, RealType);

  /** Set/Get the fraction of the image to use for sigma estimation.
   *  To reduce the computational burden for computing sigma,
   *  a small random fraction of the image pixels can be used.
   */
  itkSetClampMacro(FractionPixelsForSigmaUpdate, double, 0.01, 1.0);
  itkGetConstReferenceMacro(FractionPixelsForSigmaUpdate, double);

  /** Set/Get flag indicating whether conditional derivatives should be used estimating sigma. */
  itkSetMacro(ComputeConditionalDerivatives, bool);
  itkBooleanMacro(ComputeConditionalDerivatives);
  itkGetConstMacro(ComputeConditionalDerivatives, bool);

  /** Maximum number of Newton-Raphson iterations for sigma update. */
  itkStaticConstMacro(MaxSigmaUpdateIterations, unsigned int,
                      20);

  /** Set/Get the sigma multiplication factor used to modify the automatically-estimated sigma.
   *  At times, it may be desirable to modify the value of the automatically-estimated sigma.
   *  Typically, this number isn't very far from 1.
   *  Note: This is used only when DoKernelBandwidthEstimation is True/On.
   */
  itkSetClampMacro(SigmaMultiplicationFactor, double, 0.01, 100);
  itkGetConstReferenceMacro(SigmaMultiplicationFactor, double);



  /** Set/Get the class used for creating a subsample of patches. */
  itkSetObjectMacro(Sampler, BaseSamplerType);
  itkGetObjectMacro(Sampler, BaseSamplerType);

 protected:
  PatchBasedDenoisingImageFilter()
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
      // desired accuracy of Newton-Raphson sigma estimation
      m_GaussianKernelSigma             = 10.0;
      m_ComputeConditionalDerivatives   = false;
      m_FractionPixelsForSigmaUpdate    = 0.20;
      m_SigmaUpdateDecimationFactor     = static_cast<unsigned int>
        (vnl_math_rnd(1.0 / m_FractionPixelsForSigmaUpdate));
      m_SigmaUpdateConvergenceTolerance = 0.01;
      m_SigmaMultiplicationFactor       = 1.0;


      m_TotalNumberPixels  = 0; // won't be valid until an image is provided
      m_Sampler            = 0; // won't be valid until a sampler is provided
      m_ComponentsPerPixel = 0; // won't be valid until Initialize() gets called
      // m_IntensityRescaleInvFactor won't be allocated until Initialize() gets called
    }

  ~PatchBasedDenoisingImageFilter() { };
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** Allocate memory for a temporary update container in the subclass*/
  virtual void AllocateUpdateBuffer();

  virtual void CopyInputToOutput();

  virtual void GenerateInputRequestedRegion();


  virtual void Initialize() throw(DataObjectError);
  void InitializeKernelSigma(const Image<RGBPixel<PixelValueType>,
                             ImageDimension>* imgPtr);
  void InitializeKernelSigma(const Image<RGBAPixel<PixelValueType>,
                             ImageDimension>* imgPtr);
  void InitializeKernelSigma(const Image<Vector<PixelValueType,PixelType::Dimension>,
                             ImageDimension>* imgPtr);
  void InitializeKernelSigma(const Image<FixedArray<PixelValueType,PixelType::Dimension>,
                             ImageDimension>* imgPtr);
  void InitializeKernelSigma(const Image<Array<PixelValueType>,
                             ImageDimension>* imgPtr);
  template <class TImage> void InitializeKernelSigma(const TImage* imgPtr);
  void ArrayInitializeKernelSigma(const InputImageType* imgPtr);


  virtual void InitializePatchWeights();
  virtual void InitializePatchWeightsSmoothDisc();

  virtual void InitializeIteration();


  virtual void ComputeKernelBandwidthUpdate(); // derived from base class; define here
  virtual void ThreadedComputeSigmaUpdate(const InputImageRegionType& regionToProcess,
                                          const int threadId);
  virtual RealArrayType ResolveSigmaUpdate();


  virtual void ComputeImageUpdate();
  virtual void ThreadedComputeImageUpdate(const InputImageRegionType& regionToProcess,
                                          const int threadId);
  virtual RealType ComputeGradientJointEntropy(InstanceIdentifier id,
                                               typename ListAdaptorType::Pointer& inList,
                                               BaseSamplerPointer& sampler);


  virtual void ApplyUpdate();
  virtual void ThreadedApplyUpdate(const InputImageRegionType& regionToProcess,
                                   int itkNotUsed(threadId));


  virtual void PostProcessOutput();

  struct ThreadDataStruct
  {
    ShortArrayType     validDerivatives;
    RealArrayType      entropyFirstDerivative;
    RealArrayType      entropySecondDerivative;
    BaseSamplerPointer sampler;
  };
  std::vector<ThreadDataStruct> m_ThreadData;

  /** The buffer that holds the updates for an iteration of the algorithm. */
  typename OutputImageType::Pointer m_UpdateBuffer;

  unsigned int     m_ComponentsPerPixel;
  unsigned int     m_TotalNumberPixels;
  //
  bool             m_UseSmoothDiscPatchWeights;
  //
  RealType         m_GaussianKernelSigma;
  RealArrayType    m_IntensityRescaleInvFactor;
  double           m_FractionPixelsForSigmaUpdate;
  bool             m_ComputeConditionalDerivatives;
  double           m_MinSigma;
  double           m_MinProbability;
  unsigned int     m_SigmaUpdateDecimationFactor;
  double           m_SigmaUpdateConvergenceTolerance;
  ShortArrayType   m_SigmaConverged;
  double           m_SigmaMultiplicationFactor;
  //
  BaseSamplerPointer m_Sampler;
  typename ListAdaptorType::Pointer m_SearchSpaceList;

 private:
  PatchBasedDenoisingImageFilter(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** This callback method uses ImageSource::SplitRequestedRegion to acquire an
   * output region that it passes to ComputeSigma for processing. */
  static ITK_THREAD_RETURN_TYPE ComputeSigmaUpdateThreaderCallback( void *arg );

  /** This callback method uses ImageSource::SplitRequestedRegion to acquire a
   * region which it then passes to ComputeImageUpdate for processing. */
  static ITK_THREAD_RETURN_TYPE ComputeImageUpdateThreaderCallback( void *arg );

  /** This callback method uses ImageSource::SplitRequestedRegion to acquire a
   * region which it then passes to ThreadedApplyUpdate for processing. */
  static ITK_THREAD_RETURN_TYPE ApplyUpdateThreaderCallback( void *arg );

  struct ThreadFilterStruct
  {
    PatchBasedDenoisingImageFilter *Filter;
  };

 }; // end class PatchBasedDenoisingImageFilter

} // end namespace itk

// Define instantiation macro for this template
#define ITK_TEMPLATE_PatchBasedDenoisingImageFilter(_, EXPORT, x, y) namespace itk { \
    _(2(class EXPORT PatchBasedDenoisingImageFilter< ITK_TEMPLATE_2 x >)) | \
      namespace Templates { typedef PatchBasedDenoisingImageFilter< ITK_TEMPLATE_2 x > \
        PatchBasedDenoisingImageFilter##y; }                            \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkPatchBasedDenoisingImageFilter+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkPatchBasedDenoisingImageFilter.hxx"
#endif

#endif
