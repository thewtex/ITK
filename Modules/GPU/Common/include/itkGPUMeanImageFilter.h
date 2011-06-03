#ifndef __itkGPUMeanImageFilter_h
#define __itkGPUMeanImageFilter_h

//#include "pathToOpenCLSourceCode.h"
#include "itkMeanImageFilter.h"
#include "itkGPUImageToImageFilter.h"
#include "itkVersion.h"
#include "itkObjectFactoryBase.h"

namespace itk
{

/** \class GPUMeanImageFilter
 *
 * \brief GPU enabled implementation of the MeanImageFilter.
 *
 * FIXME   Won-Ki to write more documentation here...
 *
 */
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

  //int m_KernelHandle;
};

/** \class GPUMeanImageFilterFactory
 *
 * \brief Object Factory implemenatation for GPUMeanImageFilter
 */
class GPUMeanImageFilterFactory : public ObjectFactoryBase
{
public:
  typedef GPUMeanImageFilterFactory     Self;
  typedef ObjectFactoryBase             Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }
  const char* GetDescription() const { return "A Factory for GPUMeanImageFilter"; }

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GPUMeanImageFilterFactory, itk::ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    GPUMeanImageFilterFactory::Pointer factory = GPUMeanImageFilterFactory::New();
    ObjectFactoryBase::RegisterFactory(factory);
  }

private:
  GPUMeanImageFilterFactory(const Self&);    //purposely not implemented
  void operator=(const Self&); //purposely not implemented

#define OverrideMeanFilterTypeMacro(ipt,opt,dm)\
  {\
  typedef Image<ipt,dm> InputImageType;\
  typedef Image<opt,dm> OutputImageType;\
  this->RegisterOverride(\
  typeid(MeanImageFilter<InputImageType,OutputImageType>).name(),\
        typeid(GPUMeanImageFilter<InputImageType,OutputImageType>).name(),\
        "GPU Mean Image Filter Override",\
        true,\
        CreateObjectFunction<GPUMeanImageFilter<InputImageType,OutputImageType> >::New());\
  }


  GPUMeanImageFilterFactory()
    {
      //this->IfGPUISAvailable()
      //{
      OverrideMeanFilterTypeMacro(unsigned char, unsigned char, 1);
      OverrideMeanFilterTypeMacro(char, char, 1);
      OverrideMeanFilterTypeMacro(float,float,1);
      OverrideMeanFilterTypeMacro(int,int,1);
      OverrideMeanFilterTypeMacro(unsigned int,unsigned int,1);
      OverrideMeanFilterTypeMacro(double,double,1);

      OverrideMeanFilterTypeMacro(unsigned char, unsigned char, 2);
      OverrideMeanFilterTypeMacro(char, char, 2);
      OverrideMeanFilterTypeMacro(float,float,2);
      OverrideMeanFilterTypeMacro(int,int,2);
      OverrideMeanFilterTypeMacro(unsigned int,unsigned int,2);
      OverrideMeanFilterTypeMacro(double,double,2);

      OverrideMeanFilterTypeMacro(unsigned char, unsigned char, 3);
      OverrideMeanFilterTypeMacro(char, char, 3);
      OverrideMeanFilterTypeMacro(float,float,3);
      OverrideMeanFilterTypeMacro(int,int,3);
      OverrideMeanFilterTypeMacro(unsigned int,unsigned int,3);
      OverrideMeanFilterTypeMacro(double,double,3);
      //}
    }
};

} // end namespace itk


#if ITK_TEMPLATE_TXX
#include "itkGPUMeanImageFilter.txx"
#endif

#endif
