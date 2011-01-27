#ifndef __itkGPUMeanImageFilter_h
#define __itkGPUMeanImageFilter_h

#include "itkMeanImageFilter.h"
#include "itkGPUImageToImageFilter.h"

namespace itk
{

template< class TInputImage, class TOutputImage >
class ITK_EXPORT GPUMeanImageFilter: public GPUImageToImageFilter< TInputImage, TOutputImage, MeanImageFilter< TInputImage, TOutputImage > >
{
public:
  /** Standard class typedefs. */
  typedef GPUMeanImageFilter       Self;
  typedef GPUImageToImageFilter< TInputImage, TOutputImage, MeanImageFilter< TInputImage, TOutputImage > > Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GPUMeanImageFilter, Superclass);

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
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

protected:
  GPUMeanImageFilter();
  ~GPUMeanImageFilter();

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  virtual void GPUGenerateData();

private:
  GPUMeanImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented

  int m_KernelHandle;
};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_GPUMeanImageFilter(_, EXPORT, TypeX, TypeY)                  \
  namespace itk                                                                   \
  {                                                                               \
  _( 2 ( class EXPORT GPUMeanImageFilter< ITK_TEMPLATE_2 TypeX > ) )              \
  namespace Templates                                                             \
  {                                                                               \
  typedef GPUMeanImageFilter< ITK_TEMPLATE_2 TypeX > GPUMeanImageFilter##TypeY; \
  }                                                                               \
  }

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkGPUMeanImageFilter+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkGPUMeanImageFilter.txx"
#endif

#endif
