#ifndef __itkGPUUnaryFunctorImageFilter_txx
#define __itkGPUUnaryFunctorImageFilter_txx

#include "itkGPUUnaryFunctorImageFilter.h"

namespace itk
{

template< class TInputImage, class TOutputImage, class TFunction, class TParentImageFilter >
void
GPUUnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction, TParentImageFilter >
::GenerateOutputInformation()
{
  CPUSuperclass::GenerateOutputInformation();
}


template< class TInputImage, class TOutputImage, class TFunction, class TParentImageFilter >
void
GPUUnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction, TParentImageFilter >
::GPUGenerateData()
{
  // Applying functor using GPU kernel
  typedef typename itk::GPUTraits< TInputImage >::Type  GPUInputImage;
  typedef typename itk::GPUTraits< TOutputImage >::Type GPUOutputImage;

  typename GPUInputImage::Pointer  inPtr =  dynamic_cast< GPUInputImage * >( this->ProcessObject::GetInput(0) );
  typename GPUOutputImage::Pointer otPtr =  dynamic_cast< GPUOutputImage * >( this->ProcessObject::GetOutput(0) );

  typename GPUOutputImage::SizeType outSize = otPtr->GetLargestPossibleRegion().GetSize();

  int imgSize[3];
  imgSize[0] = imgSize[1] = imgSize[2] = 1;

  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
  {
     imgSize[i] = outSize[i];
  }

  size_t localSize[2], globalSize[2];
  localSize[0] = localSize[1] = 16;
  globalSize[0] = localSize[0]*(unsigned int)ceil((float)outSize[0]/(float)localSize[0]); // total # of threads
  globalSize[1] = localSize[1]*(unsigned int)ceil((float)outSize[1]/(float)localSize[1]);

  // arguments set up using Functor
  int argidx = (this->GetFunctor()).SetGPUKernelArguments(this->m_GPUKernelManager, m_UnaryFunctorImageFilterGPUKernelHandle);
  // arguments set up
  this->m_GPUKernelManager->SetKernelArgWithImage(m_UnaryFunctorImageFilterGPUKernelHandle, argidx++, inPtr->GetGPUDataManager());
  this->m_GPUKernelManager->SetKernelArgWithImage(m_UnaryFunctorImageFilterGPUKernelHandle, argidx++, otPtr->GetGPUDataManager());
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
  {
    this->m_GPUKernelManager->SetKernelArg(m_UnaryFunctorImageFilterGPUKernelHandle, argidx++, sizeof(int), &(imgSize[i]));
  }

  // launch kernel
  this->m_GPUKernelManager->LaunchKernel(m_UnaryFunctorImageFilterGPUKernelHandle, (int)TInputImage::ImageDimension, globalSize, localSize );
}

} // end of namespace itk

#endif
