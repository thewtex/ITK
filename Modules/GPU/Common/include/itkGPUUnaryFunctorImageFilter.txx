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
  //std::cout << "ToDo: GPUUnaryFunctorImageFilter::GenerateOutputInformation()" << std::endl;
}


template< class TInputImage, class TOutputImage, class TFunction, class TParentImageFilter >
void
GPUUnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction, TParentImageFilter >
::GPUGenerateData()
{
  std::cout << "GPUGenerateData() from GPUUnaryFunctorImageFilter" <<std::endl;

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
  int argidx = (this->GetFunctor()).SetGPUKernelArguments(this->m_KernelManager, this->m_KernelHandle);
  // arguments set up
  this->m_KernelManager->SetKernelArgWithImage(this->m_KernelHandle, argidx++, inPtr->GetGPUDataManager());
  this->m_KernelManager->SetKernelArgWithImage(this->m_KernelHandle, argidx++, otPtr->GetGPUDataManager());
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
  {
    this->m_KernelManager->SetKernelArg(this->m_KernelHandle, argidx++, sizeof(int), &(imgSize[i]));
  }

  // launch kernel
  this->m_KernelManager->LaunchKernel( this->m_KernelHandle, (int)TInputImage::ImageDimension, globalSize, localSize );
}

/*
template< class TInputImage, class TOutputImage, class TFunction, class TParentImageFilter >
void
GPUUnaryFunctorImageFilter< TInputImage, TOutputImage, TFunction, TParentImageFilter >
::ThreadedGPUGenerateData(const OutputImageRegionType & outputRegionForThread,
                          int threadId)
{
  InputImagePointer  inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput(0);

  // Define the portion of the input to walk for this thread, using
  // the CallCopyOutputRegionToInputRegion method allows for the input
  // and output images to be different dimensions
  InputImageRegionType inputRegionForThread;

  this->CallCopyOutputRegionToInputRegion(inputRegionForThread, outputRegionForThread);

  //
  // ToDo : launch GPU kernel
  //

  // Define the iterators
  ImageRegionConstIterator< TInputImage > inputIt(inputPtr, inputRegionForThread);
  ImageRegionIterator< TOutputImage >     outputIt(outputPtr, outputRegionForThread);

  ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while ( !inputIt.IsAtEnd() )
    {
    outputIt.Set( m_Functor( inputIt.Get() ) );
    ++inputIt;
    ++outputIt;
    progress.CompletedPixel();  // potential exception thrown here
    }
}
*/

} // end of namespace itk

#endif
