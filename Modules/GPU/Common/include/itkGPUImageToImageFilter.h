#ifndef __itkGPUImageToImageFilter_h
#define __itkGPUImageToImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkGPUKernelManager.h"

namespace itk
{

/** \class GPUImageToImageFilter
 *
 * \brief class to abstract the behaviour of the GPU filters.
 *
 * GPUImageToImageFilter is the GPU version of ImageToImageFilter.
 * This class can accept both CPU and GPU image as input and output,
 * and apply filter accordingly. If GPU is available for use, then
 * GPUGenerateData() is called. Otherwise, GenerateData() in the
 * parent class (i.e., ImageToImageFilter) will be called.
 *
 * \ingroup ITKGPUCommon
 */
template< class TInputImage, class TOutputImage, class TParentImageFilter =
            ImageToImageFilter< TInputImage, TOutputImage > >
class ITK_EXPORT GPUImageToImageFilter : public TParentImageFilter
{
public:
  /** Standard class typedefs. */
  typedef GPUImageToImageFilter      Self;
  typedef TParentImageFilter         Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GPUImageToImageFilter, TParentImageFilter);

  /** Superclass typedefs. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType  OutputImagePixelType;

  /** Some convenient typedefs. */
  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  // macro to set if GPU is used
  itkSetMacro(GPUEnabled, bool);
  itkGetConstMacro(GPUEnabled, bool);
  itkBooleanMacro(GPUEnabled);

  void GenerateData();

  virtual void GraftOutput(DataObject *output);

protected:
  GPUImageToImageFilter();
  ~GPUImageToImageFilter();

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  virtual void GPUGenerateData() {
  }

  // GPU kernel manager
  typename GPUKernelManager::Pointer m_GPUKernelManager;

  // GPU kernel handle - kernel should be defined in specific filter (not in the
  // base class)
  //int m_KernelHandle;
private:
  GPUImageToImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);        //purposely not implemented

  bool m_GPUEnabled;
};

} // end namespace itk

#if ITK_TEMPLATE_TXX
#include "itkGPUImageToImageFilter.hxx"
#endif

#endif
