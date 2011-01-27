#ifndef __itkGPUMeanImageFilter_txx
#define __itkGPUMeanImageFilter_txx

#include "itkGPUMeanImageFilter.h"

//
// OpenCL source file path
//
char* GPUMeanImageFilterSourcePath = "../../../ITK/Code/GPU/GPUMeanImageFilter.cl";

namespace itk
{

template< class TInputImage, class TOutputImage >
GPUMeanImageFilter< TInputImage, TOutputImage >::GPUMeanImageFilter()
{
  char buf[100];

  // preamble strings
  if(TInputImage::ImageDimension == 1)
    sprintf(buf, "#define PIXELTYPE %s\n#define DIM_ONE\n", typeid( TInputImage::PixelType ).name());
  else if(TInputImage::ImageDimension == 2)
    sprintf(buf, "#define PIXELTYPE %s\n#define DIM_TWO\n", typeid( TInputImage::PixelType ).name());
  else if(TInputImage::ImageDimension == 3)
    sprintf(buf, "#define PIXELTYPE %s\n#define DIM_THREE\n", typeid( TInputImage::PixelType ).name());
  else
    itkExceptionMacro("GPUMeanImageFilter supports 1/2/3D image.");

  // load and build program
  m_KernelManager->LoadProgramFromFile(GPUMeanImageFilterSourcePath, buf);

  // create kernel
  m_KernelHandle = m_KernelManager->CreateKernel("MeanFilter");
}

template< class TInputImage, class TOutputImage >
GPUMeanImageFilter< TInputImage, TOutputImage >::~GPUMeanImageFilter()
{}

template< class TInputImage, class TOutputImage >
void
GPUMeanImageFilter< TInputImage, TOutputImage >::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  //os << indent << "GPU: " << ( m_GPUEnabled ? "Enabled" : "Disabled" ) << std::endl;
}


template< class TInputImage, class TOutputImage >
void
GPUMeanImageFilter< TInputImage, TOutputImage >::GPUGenerateData()
{
  TInputImage::Pointer  inPtr =  dynamic_cast< TInputImage * >( this->ProcessObject::GetInput(0) );
  TOutputImage::Pointer otPtr =  dynamic_cast< TOutputImage * >( this->ProcessObject::GetOutput(0) );
  TOutputImage::SizeType outSize = otPtr->GetLargestPossibleRegion().GetSize();

  int radius[3], imgSize[3];
  radius[0] = radius[1] = radius[2] = 0;
  imgSize[0] = imgSize[1] = imgSize[2] = 1;
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
  {
    radius[i]  = (this->GetRadius())[i];
    imgSize[i] = outSize[i];
  }

  size_t localSize[2], globalSize[2];
  localSize[0] = localSize[1] = 16;
  globalSize[0] = localSize[0]*(unsigned int)ceil((float)outSize[0]/(float)localSize[0]); // total # of threads
  globalSize[1] = localSize[1]*(unsigned int)ceil((float)outSize[1]/(float)localSize[1]);

  // arguments set up
  int argidx = 0;
  m_KernelManager->SetKernelArgWithImage(m_KernelHandle, argidx++, inPtr->GetGPUDataManager());
  m_KernelManager->SetKernelArgWithImage(m_KernelHandle, argidx++, otPtr->GetGPUDataManager());
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
    m_KernelManager->SetKernelArg(m_KernelHandle, argidx++, sizeof(int), &(radius[i]));
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
    m_KernelManager->SetKernelArg(m_KernelHandle, argidx++, sizeof(int), &(imgSize[i]));

  // launch kernel
  m_KernelManager->LaunchKernel(m_KernelHandle, (int)TInputImage::ImageDimension, globalSize, localSize);
}



} // end namespace itk

#endif
