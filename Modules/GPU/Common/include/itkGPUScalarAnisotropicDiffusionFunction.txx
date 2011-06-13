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
#ifndef __itkGPUScalarAnisotropicDiffusionFunction_txx
#define __itkGPUScalarAnisotropicDiffusionFunction_txx

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkDerivativeOperator.h"
#include "itkGPUScalarAnisotropicDiffusionFunction.h"

namespace itk
{

template< class TImage >
GPUScalarAnisotropicDiffusionFunction< TImage >
::GPUScalarAnisotropicDiffusionFunction()
{
  m_AnisotropicDiffusionFunctionGPUBuffer = GPUDataManager::New();
  m_AnisotropicDiffusionFunctionGPUKernelManager = GPUKernelManager::New();

  // load GPU kernel
  std::ostringstream defines;

  if(ImageDimension > 3 || ImageDimension < 1)
  {
    itkExceptionMacro("GPUScalarAnisotropicDiffusionFunction supports 1/2/3D image.");
  }

  defines << "#define DIM_" << ImageDimension << "\n";
  defines << "#define BLOCK_SIZE " << BLOCK_SIZE[ImageDimension-1] << "\n";

  defines << "#define PIXELTYPE ";
  GetTypenameInString( typeid ( typename TImage::PixelType ), defines );

  std::string oclSrcPath = "./../OpenCL/GPUScalarAnisotropicDiffusionFunction.cl";

  std::cout << "Defines: " << defines.str() << "Source code path: " << oclSrcPath << std::endl;

  // load and build program
  this->m_AnisotropicDiffusionFunctionGPUKernelManager->LoadProgramFromFile( oclSrcPath.c_str(), defines.str().c_str() );

  // create kernel
  this->m_AverageGradientMagnitudeSquaredGPUKernelHandle = this->m_AnisotropicDiffusionFunctionGPUKernelManager->CreateKernel("AverageGradientMagnitudeSquared");
}

template< class TImage >
void
GPUScalarAnisotropicDiffusionFunction< TImage >
::CalculateAverageGradientMagnitudeSquared(TImage *ip)
{
  //itk::TimeProbe timer;
  //timer.Start();

  // GPU kernel to compute Average Squared Gradient Magnitude
  typedef typename itk::GPUTraits< TImage >::Type GPUImageType;
  typename GPUImageType::Pointer inPtr =  dynamic_cast< GPUImageType * >( ip );
  typename GPUImageType::SizeType outSize = inPtr->GetLargestPossibleRegion().GetSize();

  int imgSize[3];
  imgSize[0] = imgSize[1] = imgSize[2] = 1;
  float imgScale[3];
  imgScale[0] = imgScale[1] = imgScale[2] = 1.0f;
  int ImageDim = (int)TImage::ImageDimension;

  size_t localSize[3], globalSize[3];
  localSize[0] = localSize[1] = localSize[2] = BLOCK_SIZE[ImageDim-1];

  unsigned int numPixel = 1;
  unsigned int bufferSize = 1;
  for(int i=0; i<ImageDim; i++)
  {
    imgSize[i] = outSize[i];
    imgScale[i] = this->m_ScaleCoefficients[i];
    globalSize[i] = localSize[i]*(unsigned int)ceil((float)outSize[i]/(float)localSize[i]); // total # of threads
    bufferSize *= globalSize[i]/localSize[i];
    numPixel *= imgSize[i];
  }

  // Initialize & Allocate GPU Buffer
  if(bufferSize != m_AnisotropicDiffusionFunctionGPUBuffer->GetBufferSize())
  {
    m_AnisotropicDiffusionFunctionGPUBuffer->Initialize();
    m_AnisotropicDiffusionFunctionGPUBuffer->SetBufferSize( sizeof(float)*bufferSize );
    m_AnisotropicDiffusionFunctionGPUBuffer->Allocate();
  }


  typename GPUKernelManager::Pointer kernelManager = this->m_AnisotropicDiffusionFunctionGPUKernelManager;
  int kernelHandle = this->m_AverageGradientMagnitudeSquaredGPUKernelHandle;

  // Set arguments
  int argidx = 0;
  kernelManager->SetKernelArgWithImage(kernelHandle, argidx++, inPtr->GetGPUDataManager());
  kernelManager->SetKernelArgWithImage(kernelHandle, argidx++, m_AnisotropicDiffusionFunctionGPUBuffer);

  // Set filter scale parameter
  for(int i=0; i<ImageDim; i++)
  {
    kernelManager->SetKernelArg(kernelHandle, argidx++, sizeof(float), &(imgScale[i]));
  }

  // Set image size
  for(int i=0; i<ImageDim; i++)
  {
    kernelManager->SetKernelArg(kernelHandle, argidx++, sizeof(int), &(imgSize[i]));
  }

  // launch kernel
  kernelManager->LaunchKernel(kernelHandle, ImageDim, globalSize, localSize );

  // Read back intermediate sums from GPU and compute final value
  double sum = 0;
  float *intSum = new float[bufferSize];

  m_AnisotropicDiffusionFunctionGPUBuffer->SetCPUBufferPointer( intSum );
  m_AnisotropicDiffusionFunctionGPUBuffer->SetCPUDirtyFlag( true );   // CPU is dirty
  m_AnisotropicDiffusionFunctionGPUBuffer->SetGPUDirtyFlag( false );
  m_AnisotropicDiffusionFunctionGPUBuffer->MakeCPUBufferUpToDate();   // Copy GPU->CPU

  for(int i=0; i<bufferSize; i++)
  {
    sum += (double)intSum[i];
    //std::cout << "Partial sum : " << intSum[i] << ", Total sum : " << sum << std::endl;
  }

  this->SetAverageGradientMagnitudeSquared( (double)( sum / (double)numPixel ) );

  //std::cerr << "GPU took " << timer.GetMeanTime() << " seconds.\n";

  //std::cout << "GPU average squared gradient magnitude : " << (double)( sum / numPixel ) << std::endl;

  delete [] intSum;

  // -------------------

/*
  itk::TimeProbe cputimer;
  cputimer.Start();

  // CPU version
  typedef ConstNeighborhoodIterator< TImage >                           RNI_type;
  typedef ConstNeighborhoodIterator< TImage >                           SNI_type;
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< TImage > BFC_type;
  typedef typename NumericTraits<PixelType>::AccumulateType             AccumulateType;

  unsigned int                               i;
  ZeroFluxNeumannBoundaryCondition< TImage > bc;
  AccumulateType                             accumulator;
  PixelRealType                              val;
  SizeValueType                              counter;
  BFC_type                                   bfc;
  typename BFC_type::FaceListType faceList;
  typename RNI_type::RadiusType radius;
  typename BFC_type::FaceListType::iterator fit;

  RNI_type iterator_list[ImageDimension];
  SNI_type face_iterator_list[ImageDimension];
  DerivativeOperator< PixelType,
                      ImageDimension > operator_list[ImageDimension];

  SizeValueType Stride[ImageDimension];
  SizeValueType Center[ImageDimension];

  // Set up the derivative operators, one for each dimension
  for ( i = 0; i < ImageDimension; ++i )
    {
    operator_list[i].SetOrder(1);
    operator_list[i].SetDirection(i);
    operator_list[i].CreateDirectional();
    radius[i] = operator_list[i].GetRadius()[i];
    }

  // Get the various region "faces" that are on the data set boundary.
  faceList = bfc(ip, ip->GetRequestedRegion(), radius);
  fit      = faceList.begin();

  // Now do the actual processing
  accumulator = NumericTraits< AccumulateType >::Zero;
  counter     = NumericTraits< SizeValueType >::Zero;

  // First process the non-boundary region

  // Instead of maintaining a single N-d neighborhood of pointers,
  // we maintain a list of 1-d neighborhoods along each axial direction.
  // This is more efficient for higher dimensions.
  for ( i = 0; i < ImageDimension; ++i )
    {
    iterator_list[i] = RNI_type(operator_list[i].GetRadius(), ip, *fit);
    iterator_list[i].GoToBegin();
    Center[i] = iterator_list[i].Size() / 2;
    Stride[i] = iterator_list[i].GetStride(i);
    }
  while ( !iterator_list[0].IsAtEnd() )
    {
    counter++;
    for ( i = 0; i < ImageDimension; ++i )
      {
      val = iterator_list[i].GetPixel(Center[i] + Stride[i])
            - iterator_list[i].GetPixel(Center[i] - Stride[i]);
      PixelRealType tempval = val / -2.0f;
      val = tempval * this->m_ScaleCoefficients[i];
      accumulator += val * val;
      ++iterator_list[i];
      }
    }

  // Go on to the next region(s).  These are on the boundary faces.
  ++fit;
  while ( fit != faceList.end() )
    {
    for ( i = 0; i < ImageDimension; ++i )
      {
      face_iterator_list[i] = SNI_type(operator_list[i].GetRadius(), ip,
                                       *fit);
      face_iterator_list[i].OverrideBoundaryCondition(&bc);
      face_iterator_list[i].GoToBegin();
      Center[i] = face_iterator_list[i].Size() / 2;
      Stride[i] = face_iterator_list[i].GetStride(i);
      }

    while ( !face_iterator_list[0].IsAtEnd() )
      {
      counter++;
      for ( i = 0; i < ImageDimension; ++i )
        {
        val = face_iterator_list[i].GetPixel(Center[i] + Stride[i])
              - face_iterator_list[i].GetPixel(Center[i] - Stride[i]);
        PixelRealType tempval = val / -2.0f;
        val = tempval * this->m_ScaleCoefficients[i];
        accumulator += val * val;
        ++face_iterator_list[i];
        }
      }
    ++fit;
    }

  this->SetAverageGradientMagnitudeSquared( (double)( accumulator / counter ) );

  std::cerr << "CPU took " << cputimer.GetMeanTime() << " seconds.\n";
  std::cout << "CPU average squared gradient magnitude : " << (double)( accumulator / counter ) << std::endl;
*/
}
} // end namespace itk

#endif
