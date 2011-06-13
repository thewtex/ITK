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
#ifndef __itkGPUPDEDeformableRegistrationFilter_txx
#define __itkGPUPDEDeformableRegistrationFilter_txx

#include "itkGPUPDEDeformableRegistrationFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkDataObject.h"

#include "itkGaussianOperator.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"

#include "vnl/vnl_math.h"

namespace itk
{
/**
 * Default constructor
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::GPUPDEDeformableRegistrationFilter()
{
  this->SetNumberOfRequiredInputs(2);

  this->SetNumberOfIterations(10);

  m_TempField = DeformationFieldType::New();

  /** Build GPU Program */

  std::ostringstream defines;

  if(TFixedImage::ImageDimension > 3 || TFixedImage::ImageDimension < 1)
  {
    itkExceptionMacro("GPUDenseFiniteDifferenceImageFilter supports 1/2/3D image.");
  }

  defines << "#define DIM_" << TDeformationField::ImageDimension << "\n";

  //PixelType is a Vector
  defines << "#define OUTPIXELTYPE ";
  GetTypenameInString( typeid ( typename TDeformationField::PixelType::ValueType ), defines );

  std::string oclSrcPath = "./../OpenCL/GPUPDEDeformableRegistrationFilter.cl";

  std::cout << "Defines: " << defines.str() << "Source code path: " << oclSrcPath << std::endl;

  // load and build program
  this->m_GPUKernelManager->LoadProgramFromFile( oclSrcPath.c_str(), defines.str().c_str() );

  // create kernel
  m_SmoothDeformationFieldGPUKernelHandle = this->m_GPUKernelManager->CreateKernel("SmoothDeformationField");

}

/*
 * Standard PrintSelf method.
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/*
 * Override the default implemenation for the case when the
 * initial deformation is not set.
 * If the initial deformation is not set, the output is
 * fill with zero vectors.
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::CopyInputToOutput()
{
  typename Superclass::InputImageType::ConstPointer inputPtr  = this->GetInput();

  if ( inputPtr )
    {
    this->Superclass::CopyInputToOutput();
    }
  else
    {
    typename Superclass::PixelType zeros;
    for ( unsigned int j = 0; j < ImageDimension; j++ )
      {
      zeros[j] = 0;
      }

    typedef typename itk::GPUTraits< TDeformationField >::Type       GPUOutputImage;
    typename GPUOutputImage::Pointer output = dynamic_cast<GPUOutputImage *>(this->GetOutput());

    ImageRegionIterator< OutputImageType > out( output, output->GetRequestedRegion() );

    while ( !out.IsAtEnd() )
      {
      out.Value() =  zeros;
      ++out;
      }

    //copy the deformation output to gpu
    output->GetGPUDataManager()->SetCPUBufferDirty();
    }
}

/*
 * Release memory of internal buffers
 */
/*
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::PostProcessOutput()
{
  this->Superclass::PostProcessOutput();
  m_TempField->Initialize();
}
*/
/*
 * Initialize flags
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::Initialize()
{
  this->Superclass::Initialize();
  m_StopRegistrationFlag = false;
  this->AllocateSmoothingBuffer();
}

/*
 * Smooth deformation using a separable Gaussian kernel
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::AllocateSmoothingBuffer()
{
  DeformationFieldPointer field = this->GetOutput();

  // copy field to TempField
  m_TempField->SetOrigin( field->GetOrigin() );
  m_TempField->SetSpacing( field->GetSpacing() );
  m_TempField->SetDirection( field->GetDirection() );
  m_TempField->SetLargestPossibleRegion(
    field->GetLargestPossibleRegion() );
  m_TempField->SetRequestedRegion(
    field->GetRequestedRegion() );
  m_TempField->SetBufferedRegion( field->GetBufferedRegion() );
  m_TempField->Allocate();
}


/*
 * Smooth deformation using a separable Gaussian kernel
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::GPUSmoothDeformationField()
{
  typedef typename DeformationFieldType::PixelType       VectorType;
  typedef typename VectorType::ValueType                 ScalarType;
  typedef GaussianOperator< ScalarType, ImageDimension > OperatorType;

  if (m_SmoothingKernel == NULL)
    {
    /*
    OperatorType *oper = new OperatorType;

    oper->SetDirection(0);
    double variance = vnl_math_sqr(m_StandardDeviations[j]);
    oper->SetVariance(variance);
    oper->SetMaximumError(m_MaximumError);
    oper->SetMaximumKernelWidth(m_MaximumKernelWidth);

    typename OperatorType::CoefficientVector coefficients;
    coefficients = oper->GenerateCoefficients(); //to be fixed
    //oper->CreateDirectional();
    */
    float coefficients[5] = {0.050882235450215044,
      0.21183832469709751, 0.47455887970537486,
      0.21183832469709751, 0.050882235450215044};

    m_SmoothingKernelSize = 5;
    m_SmoothingKernel     = new float[m_SmoothingKernelSize];

    for (int i=0; i<m_SmoothingKernelSize; i++)
      {
      m_SmoothingKernel[i] = (float) coefficients[i];
      }

    // convolution kernel for GPU
    m_GPUSmoothingKernel = GPUDataManager::New();
    m_GPUSmoothingKernel->SetBufferSize( sizeof(float)*m_SmoothingKernelSize );
    m_GPUSmoothingKernel->SetCPUBufferPointer( m_SmoothingKernel );
    m_GPUSmoothingKernel->Allocate();
    //m_GPUSmoothingKernel->SetTimeStamp( this->GetTimeStamp() );

    //delete oper;
    }
  // image arguments
  typedef typename itk::GPUTraits< TDeformationField >::Type   GPUBufferImage;
  typedef typename itk::GPUTraits< TDeformationField >::Type   GPUOutputImage;

  typename GPUBufferImage::Pointer bfPtr =  dynamic_cast< GPUBufferImage * >( m_TempField.GetPointer() );
  typename GPUOutputImage::Pointer otPtr =  dynamic_cast< GPUOutputImage * >( this->GetOutput() );//this->ProcessObject::GetOutput(0) );
  typename GPUOutputImage::SizeType outSize = otPtr->GetLargestPossibleRegion().GetSize();

  int imgSize[3];
  imgSize[0] = imgSize[1] = imgSize[2] = 1;

  int ImageDim = (int)TDeformationField::ImageDimension;

  for(int i=0; i<ImageDim; i++)
  {
    imgSize[i] = outSize[i];
  }

  size_t localSize[3], globalSize[3];
  localSize[0] = localSize[1] = localSize[2] = BLOCK_SIZE[ImageDim-1];
  for(int i=0; i<ImageDim; i++)
  {
    globalSize[i] = localSize[i]*(unsigned int)ceil((float)outSize[i]/(float)localSize[i]); // total # of threads
  }

  // arguments set up
  int argidx = 0;
  this->m_GPUKernelManager->SetKernelArgWithImage(m_SmoothDeformationFieldGPUKernelHandle, argidx++, otPtr->GetGPUDataManager());
  this->m_GPUKernelManager->SetKernelArgWithImage(m_SmoothDeformationFieldGPUKernelHandle, argidx++, bfPtr->GetGPUDataManager());

  this->m_GPUKernelManager->SetKernelArgWithImage(m_SmoothDeformationFieldGPUKernelHandle, argidx++, m_GPUSmoothingKernel);
  this->m_GPUKernelManager->SetKernelArg(m_SmoothDeformationFieldGPUKernelHandle, argidx++, sizeof(int), &(m_SmoothingKernelSize));

  for(int i=0; i<ImageDim; i++)
  {
    this->m_GPUKernelManager->SetKernelArg(m_SmoothDeformationFieldGPUKernelHandle, argidx++, sizeof(int), &(imgSize[i]));
  }

  // launch kernel
  this->m_GPUKernelManager->LaunchKernel(m_SmoothDeformationFieldGPUKernelHandle, (int)TDeformationField::ImageDimension, globalSize, localSize );

}

/*
 * Smooth deformation using a separable Gaussian kernel
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUPDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::SmoothDeformationField()
{
  this->GPUSmoothDeformationField();
}

} // end namespace itk

#endif
