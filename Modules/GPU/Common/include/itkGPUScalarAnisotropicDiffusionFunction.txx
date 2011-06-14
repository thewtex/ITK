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
::GPUCalculateAverageGradientMagnitudeSquared(TImage *ip)
{
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
  float *intermSum = new float[bufferSize];

  m_AnisotropicDiffusionFunctionGPUBuffer->SetCPUBufferPointer( intermSum );
  m_AnisotropicDiffusionFunctionGPUBuffer->SetCPUDirtyFlag( true );   // CPU is dirty
  m_AnisotropicDiffusionFunctionGPUBuffer->SetGPUDirtyFlag( false );
  m_AnisotropicDiffusionFunctionGPUBuffer->MakeCPUBufferUpToDate();   // Copy GPU->CPU

  for(int i=0; i<bufferSize; i++)
  {
    sum += (double)intermSum[i];
  }

  this->SetAverageGradientMagnitudeSquared( (double)( sum / (double)numPixel ) );

  delete [] intermSum;
}
} // end namespace itk

#endif
