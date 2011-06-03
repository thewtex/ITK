/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkGPUDenseFiniteDifferenceImageFilter_txx
#define __itkGPUDenseFiniteDifferenceImageFilter_txx

#include "itkGPUDenseFiniteDifferenceImageFilter.h"

#include <list>
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkNeighborhoodAlgorithm.h"

namespace itk
{
template< class TInputImage, class TOutputImage, class TParentImageFilter >
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::GPUDenseFiniteDifferenceImageFilter()
{
  m_UpdateBuffer = UpdateBufferType::New();

  /**
   * FiniteDifferenceImageFilter requires two GPU kernels
   * 1. CalculateChange
   * 2. ApplyUpdate
   * Kernel for 1 is defined in the specific DifferenceFunction.
   * Kernel for 2 can be used for entire subclass of GPUDense..,
   * it is defined and created here.
   */

  std::ostringstream defines;

  if(TInputImage::ImageDimension > 3)
  {
    itkExceptionMacro("GPUDenseFiniteDifferenceImageFilter supports 1/2/3D image.");
  }

  defines << "#define DIM_" << TInputImage::ImageDimension << "\n";

  defines << "#define BUFPIXELTYPE ";
  GetTypenameInString( typeid ( typename UpdateBufferType::PixelType ), defines );

  defines << "#define OUTPIXELTYPE ";
  GetTypenameInString( typeid ( typename TOutputImage::PixelType ), defines );

  std::string oclSrcPath = "./../OpenCL/GPUDenseFiniteDifferenceImageFilter.cl";

  std::cout << "Defines: " << defines.str() << "Source code path: " << oclSrcPath << std::endl;

  // load and build program
  this->m_KernelManager->LoadProgramFromFile( oclSrcPath.c_str(), defines.str().c_str() );

  // create kernel
  this->m_KernelHandle = this->m_KernelManager->CreateKernel("ApplyUpdate");
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
void
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::CopyInputToOutput()
{
  CPUSuperclass::CopyInputToOutput();
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
void
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::AllocateUpdateBuffer()
{
  CPUSuperclass::AllocateUpdateBuffer();
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
void
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::GPUApplyUpdate(TimeStepType dt)
{
  // GPU version of ApplyUpdate - single threaded
  // Apply entire update buffer to output image
  typedef typename itk::GPUTraits< UpdateBufferType >::Type   GPUBufferImage;
  typedef typename itk::GPUTraits< TOutputImage >::Type       GPUOutputImage;

  typename GPUBufferImage::Pointer bfPtr =  dynamic_cast< GPUBufferImage * >( m_UpdateBuffer.GetPointer() );
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

  // arguments set up
  int argidx = 0;
  this->m_KernelManager->SetKernelArgWithImage(this->m_KernelHandle, argidx++, bfPtr->GetGPUDataManager());
  this->m_KernelManager->SetKernelArgWithImage(this->m_KernelHandle, argidx++, otPtr->GetGPUDataManager());
  this->m_KernelManager->SetKernelArg(this->m_KernelHandle, argidx++, sizeof(float), &(dt));
  for(int i=0; i<(int)TInputImage::ImageDimension; i++)
  {
    this->m_KernelManager->SetKernelArg(this->m_KernelHandle, argidx++, sizeof(int), &(imgSize[i]));
  }

  // launch kernel
  this->m_KernelManager->LaunchKernel( this->m_KernelHandle, (int)TInputImage::ImageDimension, globalSize, localSize );

  // Explicitely call Modified on GetOutput here. Do we need this?
  //this->GetOutput()->Modified();
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
typename
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >::TimeStepType
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::GPUCalculateChange()
{
  //
  // WIP: GPU version of CalculateChange
  //
  // OutputImage -> UpdateBuffer
  //

//-----------

  //typedef typename OutputImageType::RegionType                    RegionType;
  typedef typename OutputImageType::SizeType                      SizeType;
  //typedef typename OutputImageType::IndexType                     IndexType;
  //typedef typename FiniteDifferenceFunctionType::NeighborhoodType NeighborhoodIteratorType;

  //typedef ImageRegionIterator< UpdateBufferType > UpdateIteratorType;

  typename OutputImageType::Pointer output = this->GetOutput();

  TimeStepType timeStep;
  void *       globalData;

  // Get the FiniteDifferenceFunction to use in calculations.
  //const typename FiniteDifferenceFunctionType::Pointer df = this->GetDifferenceFunction();
  //const typename GPUFiniteDifferenceFunction< OutputImageType >::Pointer df = ( GPUFiniteDifferenceFunction< OutputImageType > * )( &(this->GetDifferenceFunction()) );

  GPUFiniteDifferenceFunction< OutputImageType > *df = dynamic_cast< GPUFiniteDifferenceFunction< OutputImageType > * >( this->GetDifferenceFunction().GetPointer() );

  const SizeType radius = df->GetRadius();

  globalData = df->GetGlobalDataPointer();
  df->GPUComputeUpdate( output, m_UpdateBuffer, globalData );

  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  //typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< OutputImageType >
  //FaceCalculatorType;

  //typedef typename FaceCalculatorType::FaceListType FaceListType;

  //FaceCalculatorType faceCalculator;

  //FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  //typename FaceListType::iterator fIt = faceList.begin();

  // Ask the function object for a pointer to a data structure it
  // will use to manage any global values it needs.  We'll pass this
  // back to the function object at each calculation and then
  // again so that the function object can use it to determine a
  // time step for this iteration.
  //globalData = df->GetGlobalDataPointer();

  // Process the non-boundary region.
  //NeighborhoodIteratorType nD(radius, output, *fIt);
  //UpdateIteratorType       nU(m_UpdateBuffer,  *fIt);
  //nD.GoToBegin();
  //while ( !nD.IsAtEnd() )
  //  {
  //  nU.Value() = df->ComputeUpdate(nD, globalData);
  //  ++nD;
  //  ++nU;
  //  }

  //// Process each of the boundary faces.

  //NeighborhoodIteratorType bD;
  //UpdateIteratorType       bU;
  //for ( ++fIt; fIt != faceList.end(); ++fIt )
  //  {
  //  bD = NeighborhoodIteratorType(radius, output, *fIt);
  //  bU = UpdateIteratorType  (m_UpdateBuffer, *fIt);

  //  bD.GoToBegin();
  //  bU.GoToBegin();
  //  while ( !bD.IsAtEnd() )
  //    {
  //    bU.Value() = df->ComputeUpdate(bD, globalData);
  //    ++bD;
  //    ++bU;
  //    }
  //  }

  //// Ask the finite difference function to compute the time step for
  //// this iteration.  We give it the global data pointer to use, then
  //// ask it to free the global data memory.
  timeStep = df->ComputeGlobalTimeStep(globalData);
  df->ReleaseGlobalDataPointer(globalData);

  return timeStep;


  /*
  int          threadCount;
  TimeStepType dt;

  // Set up for multithreaded processing.
  DenseFDThreadStruct str;

  str.Filter = this;
  str.TimeStep = NumericTraits< TimeStepType >::Zero;  // Not used during the
  // calculate change step.
  this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
  this->GetMultiThreader()->SetSingleMethod(this->CalculateChangeThreaderCallback,
                                            &str);

  // Initialize the list of time step values that will be generated by the
  // various threads.  There is one distinct slot for each possible thread,
  // so this data structure is thread-safe.
  threadCount = this->GetMultiThreader()->GetNumberOfThreads();
  str.TimeStepList = new TimeStepType[threadCount];
  str.ValidTimeStepList = new bool[threadCount];
  for ( int i = 0; i < threadCount; ++i )
    {
    str.ValidTimeStepList[i] = false;
    }

  // Multithread the execution
  this->GetMultiThreader()->SingleMethodExecute();

  // Resolve the single value time step to return
  dt = this->ResolveTimeStep(str.TimeStepList, str.ValidTimeStepList, threadCount);
  delete[] str.TimeStepList;
  delete[] str.ValidTimeStepList;

  // Explicitely call Modified on m_UpdateBuffer here
  // since ThreadedCalculateChange changes this buffer
  // through iterators which do not increment the
  // update buffer timestamp
  this->m_UpdateBuffer->Modified();

  return dt;
  */
}

/*
template< class TInputImage, class TOutputImage, class TParentImageFilter >
ITK_THREAD_RETURN_TYPE
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::ApplyUpdateThreaderCallback(void *arg)
{
  DenseFDThreadStruct *str;
  int                  total, threadId, threadCount;

  threadId = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->ThreadID;
  threadCount = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->NumberOfThreads;

  str = (DenseFDThreadStruct *)( ( (MultiThreader::ThreadInfoStruct *)( arg ) )->UserData );

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  ThreadRegionType splitRegion;
  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if ( threadId < total )
    {
    str->Filter->ThreadedApplyUpdate(str->TimeStep, splitRegion, threadId);
    }

  return ITK_THREAD_RETURN_VALUE;
}



template< class TInputImage, class TOutputImage, class TParentImageFilter >
ITK_THREAD_RETURN_TYPE
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::CalculateChangeThreaderCallback(void *arg)
{
  DenseFDThreadStruct *str;
  int                  total, threadId, threadCount;

  threadId = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->ThreadID;
  threadCount = ( (MultiThreader::ThreadInfoStruct *)( arg ) )->NumberOfThreads;

  str = (DenseFDThreadStruct *)( ( (MultiThreader::ThreadInfoStruct *)( arg ) )->UserData );

  // Execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  // Using the SplitRequestedRegion method from itk::ImageSource.
  ThreadRegionType splitRegion;

  total = str->Filter->SplitRequestedRegion(threadId, threadCount,
                                            splitRegion);

  if ( threadId < total )
    {
    str->TimeStepList[threadId] =
      str->Filter->ThreadedCalculateChange(splitRegion, threadId);
    str->ValidTimeStepList[threadId] = true;
    }

  return ITK_THREAD_RETURN_VALUE;
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
void
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::ThreadedApplyUpdate(TimeStepType dt, const ThreadRegionType & regionToProcess,
                      int)
{
  ImageRegionIterator< UpdateBufferType > u(m_UpdateBuffer,    regionToProcess);
  ImageRegionIterator< OutputImageType >  o(this->GetOutput(), regionToProcess);

  u = u.Begin();
  o = o.Begin();

  while ( !u.IsAtEnd() )
    {
    o.Value() += static_cast< PixelType >( u.Value() * dt );  // no adaptor
                                                              // support here
    ++o;
    ++u;
    }
}

template< class TInputImage, class TOutputImage, class TParentImageFilter >
typename
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >::TimeStepType
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::ThreadedCalculateChange(const ThreadRegionType & regionToProcess, int)
{
  typedef typename OutputImageType::RegionType                    RegionType;
  typedef typename OutputImageType::SizeType                      SizeType;
  typedef typename OutputImageType::IndexType                     IndexType;
  typedef typename FiniteDifferenceFunctionType::NeighborhoodType NeighborhoodIteratorType;

  typedef ImageRegionIterator< UpdateBufferType > UpdateIteratorType;

  typename OutputImageType::Pointer output = this->GetOutput();

  TimeStepType timeStep;
  void *       globalData;

  // Get the FiniteDifferenceFunction to use in calculations.
  const typename FiniteDifferenceFunctionType::Pointer df =
    this->GetDifferenceFunction();
  const SizeType radius = df->GetRadius();

  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< OutputImageType >
  FaceCalculatorType;

  typedef typename FaceCalculatorType::FaceListType FaceListType;

  FaceCalculatorType faceCalculator;

  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt = faceList.begin();

  // Ask the function object for a pointer to a data structure it
  // will use to manage any global values it needs.  We'll pass this
  // back to the function object at each calculation and then
  // again so that the function object can use it to determine a
  // time step for this iteration.
  globalData = df->GetGlobalDataPointer();

  // Process the non-boundary region.
  NeighborhoodIteratorType nD(radius, output, *fIt);
  UpdateIteratorType       nU(m_UpdateBuffer,  *fIt);
  nD.GoToBegin();
  while ( !nD.IsAtEnd() )
    {
    nU.Value() = df->ComputeUpdate(nD, globalData);
    ++nD;
    ++nU;
    }

  // Process each of the boundary faces.

  NeighborhoodIteratorType bD;
  UpdateIteratorType       bU;
  for ( ++fIt; fIt != faceList.end(); ++fIt )
    {
    bD = NeighborhoodIteratorType(radius, output, *fIt);
    bU = UpdateIteratorType  (m_UpdateBuffer, *fIt);

    bD.GoToBegin();
    bU.GoToBegin();
    while ( !bD.IsAtEnd() )
      {
      bU.Value() = df->ComputeUpdate(bD, globalData);
      ++bD;
      ++bU;
      }
    }

  // Ask the finite difference function to compute the time step for
  // this iteration.  We give it the global data pointer to use, then
  // ask it to free the global data memory.
  timeStep = df->ComputeGlobalTimeStep(globalData);
  df->ReleaseGlobalDataPointer(globalData);

  return timeStep;
}
*/

template< class TInputImage, class TOutputImage, class TParentImageFilter >
void
GPUDenseFiniteDifferenceImageFilter< TInputImage, TOutputImage, TParentImageFilter >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk

#endif
