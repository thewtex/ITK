#ifndef __itkPatchBasedDenoisingBaseImageFilter_h
#define __itkPatchBasedDenoisingBaseImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkArray.h"
#include "itkSample.h"
#include "itkNumericTraits.h"
#include "itkRegionConstrainedSubsampler.h"
#include "itkConstantBoundaryCondition.h"
#include "itkImageToNeighborhoodSampleAdaptor.h"
#include "itkLoggerThreadWrapper.h"
#include "itkLogger.h"

namespace itk
{

/** \class PatchBasedDenoisingFilter
 * \brief Implementation of a denoising filter that uses similar patches
 * to help remove noise from pixels.
 *
 * TODO insert more description here w/ help from Suyash
 * TODO provide some info on how to use the class
 * TODO explain how this is the abstract base class implementing the basic algorithm
 * TODO see the itkFiniteDifferenceImageFilter documentation for example
 * TODO provide references for relevant paper(s)
 *
 * \ingroup Filtering
 * \ingroup ITKDenoising
 * TODO provide appropriate see alsos (\sa)
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT PatchBasedDenoisingBaseImageFilter :
  public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef PatchBasedDenoisingBaseImageFilter            Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(PatchBasedDenoisingBaseImageFilter, ImageToImageFilter);

  /** Input and output image types */
// TODO need to check that we can handle disparate pixel types for input / output data
  typedef TInputImage                        InputImageType;
  typedef TOutputImage                       OutputImageType;

  /** Image dimension, assumed to be the same for input and output data*/
  itkStaticConstMacro(ImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  /** Type definition for the input and output pixel types.
      Output pixel type will be used in computations. */
  typedef typename InputImageType::PixelType  InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;
  typedef OutputPixelType                     PixelType;
  typedef typename PixelType::ValueType       PixelValueType;

  typedef itk::LoggerThreadWrapper<itk::Logger> LoggerType;
  typedef typename LoggerType::Pointer          LoggerPointer;

  typedef enum { UNINITIALIZED = 0, INITIALIZED = 1 } FilterStateType;

  /** Set/Get the number of iterations to perform. */
  itkSetMacro(NumberOfIterations, unsigned int);
  itkGetConstReferenceMacro(NumberOfIterations, unsigned int);

  /** Set/Get the update frequency for sigma.
   *  The sigma will be updated every n iterations.
   */
  itkSetMacro(SigmaUpdateFrequency, unsigned int)
  itkGetConstMacro(SigmaUpdateFrequency, unsigned int);

  /** Set/Get flag indicating whether sigma estimation should be performed.
   * Defaults to true.
   */
  itkSetMacro(DoSigmaEstimation, bool);
  itkBooleanMacro(DoSigmaEstimation);
  itkGetConstMacro(DoSigmaEstimation, bool);

  /** Get the number of elapsed iterations of the filter. */
  itkGetConstReferenceMacro(ElapsedIterations, unsigned int);

  /** Set the state of the filter to INITIALIZED */
  virtual void SetStateToInitialized();

  /** Set the state of the filter to UNINITIALIZED */
  virtual void SetStateToUninitialized();

  /** Set/Get the state of the filter. */
#if !defined(CABLE_CONFIGURATION)
  itkSetMacro(State, FilterStateType);
  itkGetConstReferenceMacro(State, FilterStateType);
#endif

  /** Require the filter to be manually reinitialized (by calling
      SetStateToUninitialized() */
  itkSetMacro(ManualReinitialization, bool);
  itkGetConstReferenceMacro(ManualReinitialization, bool);
  itkBooleanMacro(ManualReinitialization);

  /** Set/Get the logger */
  itkSetObjectMacro(Logger, LoggerType);
  itkGetObjectMacro(Logger, LoggerType);

protected:
  PatchBasedDenoisingBaseImageFilter();
  ~PatchBasedDenoisingBaseImageFilter();
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** Allocate memory for a temporary update container in the subclass. */
  virtual void AllocateUpdateBuffer() = 0;

  virtual void ComputeSigmaUpdate() = 0;

  virtual void ComputeImageUpdate() = 0;

  virtual void ApplyUpdate() = 0;

  virtual void CopyInputToOutput() = 0;

  virtual void GenerateInputRequestedRegion();

  virtual void GenerateData();

  virtual bool Halt();

  virtual bool ThreadedHalt(void *itkNotUsed(threadInfo)) { return this->Halt(); }

  virtual void Initialize() { };

  virtual void InitializeIteration() { };

  itkSetMacro(ElapsedIterations, unsigned int);

  virtual void PreProcessInput() { };
  virtual void PostProcessOutput() { };

  unsigned int m_NumberOfIterations;
  unsigned int m_ElapsedIterations;  /** Indicates whether the filter automatically resets to UNINITIALIZED state
      after completing, or whether filter must be manually reset */
  unsigned int m_SigmaUpdateFrequency;
  bool         m_DoSigmaEstimation;

  bool m_ManualReinitialization;

  /** Logger for logging messages especially from multiple threads */
  LoggerPointer   m_Logger;

private:
  PatchBasedDenoisingBaseImageFilter(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** State that the filter is in, i.e. UNINITIALIZED or INITIALIZED */
  FilterStateType m_State;

}; // end class PatchBasedDenoisingBaseImageFilter

} // end namespace itk

// Define instantiation macro for this template

#define ITK_TEMPLATE_PatchBasedDenoisingBaseImageFilter(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT PatchBasedDenoisingBaseImageFilter< ITK_TEMPLATE_2 x >)) | \
namespace Templates { typedef PatchBasedDenoisingBaseImageFilter< ITK_TEMPLATE_2 x > \
                                     PatchBasedDenoisingBaseImageFilter##y; } \
}

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkPatchBasedDenoisingBaseImageFilter+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkPatchBasedDenoisingBaseImageFilter.hxx"
#endif

#endif
