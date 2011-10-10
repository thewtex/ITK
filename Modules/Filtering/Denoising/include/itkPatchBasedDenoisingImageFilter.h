#ifndef __itkPatchBasedDenoisingImageFilter_h
#define __itkPatchBasedDenoisingImageFilter_h

#include "itkPatchBasedDenoisingBaseImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkNeighborhood.h"
#include "itkArray.h"
#include "itkVector.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkSample.h"

namespace itk
{

/** \class PatchBasedDenoisingImageFilter
 * \brief Implementation of a denoising filter that uses similar patches
 * to help remove noise from pixels.
 *
 * TODO insert more description here w/ help from Suyash
 * TODO provide some info on how to use the class
 * TODO provide references for relevant paper(s)
 *
 * \ingroup Filtering
 * \ingroup ITKDenoising
 * TODO provide appropriate see alsos (\sa)
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
  typedef ImageRegionIterator<OutputImageType>          OutputImageRegionIterator;
  typedef ImageRegionIterator<InputImageType>           InputImageRegionIterator;
  typedef ImageRegionConstIterator<InputImageType>      InputImageRegionConstIterator;

  /** Type definition for selecting the noise model */
  typedef enum { GAUSSIAN = 0, RICIAN = 1, POISSON = 2 } NoiseModelType;

  /** Type definition for the input and output pixel types.
      Output pixel type will be used in computations. */
  typedef typename Superclass::PixelType  PixelType;
  typedef typename Superclass::PixelValueType  PixelValueType;

  typedef typename NumericTraits< PixelType >::RealType RealType;
  typedef typename NumericTraits< PixelValueType >::RealType RealValueType;
  typedef Array<RealValueType> RealArrayType;
  typedef Array<unsigned short> ShortArrayType;

  // the boundary condition is not actually used anywhere (only in-bounds pixels are used)
  // so just pick one that compiles well
  typedef ZeroFluxNeumannBoundaryCondition<OutputImageType> BoundaryConditionType;
  typedef typename ::itk::Statistics::ImageToNeighborhoodSampleAdaptor<OutputImageType, BoundaryConditionType> ListAdaptorType;
  typedef typename ListAdaptorType::NeighborhoodRadiusType PatchRadiusType;
  typedef ListAdaptorType PatchSampleType;
  typedef ConstNeighborhoodIterator<InputImageType, BoundaryConditionType> InputImagePatchIterator;
  typedef Array<float> PatchWeightsType;

  /** Type definitions for delegate classes. */
  typedef itk::Statistics::RegionConstrainedSubsampler< PatchSampleType,
                                                        InputImageRegionType >  BaseSamplerType;
  typedef typename BaseSamplerType::Pointer                                     BaseSamplerPointer;
  typedef typename BaseSamplerType::InstanceIdentifier                          InstanceIdentifier;

  /** Maximum number of Newton-Raphson iterations for sigma update */
  itkStaticConstMacro(MaxSigmaUpdateIterations, unsigned int,
                      20);

  /** Set/Get the noise model type.
   * Defaults to GAUSSIAN.  Note, to activate the noise model,
   * you must also set the FidelityWeight to a nonzero value.
   */
  itkSetMacro(NoiseModel, NoiseModelType);
  itkGetConstMacro(NoiseModel, NoiseModelType);

  /** Set/Get the patch radius specified in physical coordinates
   * Right now, only allow isotropic patches in physical space
   * patches can be anisotropic in voxel space.
   */
  itkSetMacro(PatchRadius, unsigned int);
  itkGetConstMacro(PatchRadius, unsigned int);
  PatchRadiusType GetPatchRadiusInVoxels() const;
  PatchRadiusType GetPatchDiameterInVoxels() const;
  typename PatchRadiusType::SizeValueType GetPatchLengthInVoxels() const;

  /** Set/Get the patch weights
   * This function allows the user to set the weight matrix by providing
   * a 1-D array of weights.
   */
  void SetPatchWeights(const PatchWeightsType& weights);
  PatchWeightsType GetPatchWeights() const;

  /** Set/Get initial sigma estimate.  To keep the class
   * from improving this estimate, set DoSigmaEstimation to false
   */
  itkSetMacro(GaussianSigmaEntropy, RealType);
  itkGetConstMacro(GaussianSigmaEntropy, RealType);

  /** Set/Get flag indicating whether gaussian patch weights should be used.
   *  If this flag is true, the gaussian patch weights will override any
   *  weights provided via the SetPatchWeights method.
   */
  itkSetMacro(UseSmoothDiscPatchWeights, bool);
  itkBooleanMacro(UseSmoothDiscPatchWeights);
  itkGetConstMacro(UseSmoothDiscPatchWeights, bool);

  /** Set/Get the fraction of the image to use for sigma estimation.
   */
  itkSetClampMacro(FractionPixelsForSigmaUpdate, double, 0.0, 1.0);
  itkGetConstReferenceMacro(FractionPixelsForSigmaUpdate, double);

  /** Set/Get the sigma multiplication factor used to modify the estimated sigma
   */
  itkSetClampMacro(SigmaMultiplicationFactor, double, 0.000001, NumericTraits<double>::max());
  itkGetConstReferenceMacro(SigmaMultiplicationFactor, double);

  /** Set/Get flag indicating whether conditional derivatives should be computed
   */
  itkSetMacro(ComputeConditionalDerivatives, bool);
  itkBooleanMacro(ComputeConditionalDerivatives);
  itkGetConstMacro(ComputeConditionalDerivatives, bool);

  /** Set/Get the entropy weight term
   * TODO: put equation here in latex format
   */
  itkSetClampMacro(RegularizationWeight, double, 0.0f, NumericTraits<double>::max());
  itkGetConstMacro(RegularizationWeight, double);

  /** Set/Get the fidelity weight term
   * TODO: put equation here in latex format
   */
  itkSetClampMacro(FidelityWeight, double, 0.0f, NumericTraits<double>::max());
  itkGetConstMacro(FidelityWeight, double);

  /** Set/Get the class used for creating a subsample of patches. */
  itkSetObjectMacro(Sampler, BaseSamplerType);
  itkGetObjectMacro(Sampler, BaseSamplerType);

  itkSetMacro(DebugFilename, std::string);
  itkGetConstMacro(DebugFilename, std::string);

protected:
  PatchBasedDenoisingImageFilter()
  {
    std::cout << "In PatchBasedDenoisingImageFilter constructor..." << std::endl;
    m_PatchRadius     = 3;
    m_Sampler                = 0; // won't be valid until a sampler is provided
    m_NoiseModel = GAUSSIAN;
    m_SearchSpaceList = ListAdaptorType::New();
    m_UpdateBuffer = OutputImageType::New();
    m_DebugBuffer = OutputImageType::New();
  /** Desired accuracy of Newton-Raphson sigma update calculation */
    // original value
    m_SigmaUpdateAccuracy = 0.01f;
/*     m_SigmaUpdateAccuracy = 0.1f; */
  /** minimum probability, used to help avoid divide by zero */
    // original value
    m_MinProbability = NumericTraits<RealValueType>::min();
/*     m_MinProbability = 0.00001f; */
  /** minimum sigma, used to set the minimum update for the gaussian sigma entropy search */
    // original value
    m_MinSigma = NumericTraits<RealValueType>::min();
/*     m_MinSigma = 0.1f; */
    m_ComputeConditionalDerivatives = false;
    m_TotalNumberPixels = 0; // won't be valid until an image is provided
    m_FractionPixelsForSigmaUpdate = 0.25;
    m_SigmaUpdateDecimationFactor = 4;
    m_RegularizationWeight = 1.0;
    m_FidelityWeight = 0.0;
    m_SigmaMultiplicationFactor = 1.0;
  }

  ~PatchBasedDenoisingImageFilter() { };
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** Allocate memory for a temporary update container in the subclass*/
  virtual void AllocateUpdateBuffer();

  virtual void CopyInputToOutput();

  virtual void GenerateInputRequestedRegion();

  virtual void ComputeSigmaUpdate();

  virtual void ComputeImageUpdate();

  virtual void ApplyUpdate();

  virtual void Initialize() throw(DataObjectError);
  void InitializeSigmaEntropy(const Image<RGBPixel<PixelValueType> , ImageDimension>* imgPtr);
  void InitializeSigmaEntropy(const Image<RGBAPixel<PixelValueType> , ImageDimension>* imgPtr);
  void InitializeSigmaEntropy(const Image<Vector<PixelValueType,PixelType::Dimension> , ImageDimension>* imgPtr);
  void InitializeSigmaEntropy(const Image<FixedArray<PixelValueType,PixelType::Dimension> , ImageDimension>* imgPtr);
  void InitializeSigmaEntropy(const Image<Array<PixelValueType>, ImageDimension>* imgPtr);
  template <class TImage> void InitializeSigmaEntropy(const TImage* imgPtr);
  void ArrayInitializeSigmaEntropy(const InputImageType* imgPtr);
  virtual void InitializePatchWeights();
  virtual void InitializeIteration();

  virtual void PostProcessOutput();

  virtual void ThreadedComputeSigmaUpdate(const InputImageRegionType& regionToProcess,
                                          int threadId);

  virtual void ThreadedComputeImageUpdate(const InputImageRegionType& regionToProcess,
                                          int threadId);

  virtual void ThreadedApplyUpdate(const InputImageRegionType& regionToProcess,
                                   int itkNotUsed(threadId));

  virtual RealArrayType ResolveSigmaUpdate();

  virtual RealType ComputeJointEntropy(InstanceIdentifier id,
                                       typename ListAdaptorType::Pointer& inList,
                                       BaseSamplerPointer& sampler,
                                       RealType& debugVal);


  struct ThreadDataStruct
  {
#ifdef ITK_USE_SPROC
    char Pad1[128];
#endif
    RealArrayType           entropyFirstDerivative;
    RealArrayType           entropySecondDerivative;
    ShortArrayType          validDerivatives;
    BaseSamplerPointer sampler;
#ifdef ITK_USE_SPROC
    char Pad2[128];
#endif
  };

  /** The buffer that holds the updates for an iteration of the algorithm. */
  typename OutputImageType::Pointer m_UpdateBuffer;
  typename OutputImageType::Pointer m_DebugBuffer;

  std::string  m_DebugFilename;

  bool            m_UseSmoothDiscPatchWeights;
  double          m_RegularizationWeight;
  double          m_FidelityWeight;
  RealType        m_GaussianSigmaEntropy;
  RealArrayType   m_IntensityRescaleInvFactor;
  ShortArrayType  m_SigmaConverged;
  double          m_FractionPixelsForSigmaUpdate;
  unsigned int    m_SigmaUpdateDecimationFactor;
  double          m_SigmaMultiplicationFactor;
  bool            m_ComputeConditionalDerivatives;
  NoiseModelType  m_NoiseModel;

  PatchWeightsType m_PatchWeights;
  unsigned int m_ComponentsPerPixel;
  unsigned int m_TotalNumberPixels;
  unsigned int m_PatchRadius;
  std::vector<ThreadDataStruct> m_ThreadData;

  BaseSamplerPointer m_Sampler;
  typename ListAdaptorType::Pointer m_SearchSpaceList;

  double m_SigmaUpdateAccuracy;
  double m_MinProbability;
  double m_MinSigma;

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
                                     PatchBasedDenoisingImageFilter##y; } \
}

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkPatchBasedDenoisingImageFilter+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkPatchBasedDenoisingImageFilter.hxx"
#endif

#endif
