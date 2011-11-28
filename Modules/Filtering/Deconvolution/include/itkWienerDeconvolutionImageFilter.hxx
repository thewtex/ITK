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

#ifndef __itkWienerDeconvolutionImageFilter_hxx
#define __itkWienerDeconvolutionImageFilter_hxx

#include "itkWienerDeconvolutionImageFilter.h"

#include "itkBinaryFunctorImageFilter.h"

namespace itk
{

template< class TInputImage, class TKernelImage, class TOutputImage, class TInternalPrecision >
WienerDeconvolutionImageFilter< TInputImage, TKernelImage, TOutputImage, TInternalPrecision >
::WienerDeconvolutionImageFilter()
{
  m_SignalToNoiseConstant = 100000.0;
}


template< class TInputImage, class TKernelImage, class TOutputImage, class TInternalPrecision >
void
WienerDeconvolutionImageFilter< TInputImage, TKernelImage, TOutputImage, TInternalPrecision >
::GenerateData()
{
  // Create a process accumulator for tracking the progress of this
  // minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter( this );

  typename InputImageType::Pointer localInput = InputImageType::New();
  localInput->Graft( this->GetInput() );

  const KernelImageType* kernelImage = this->GetKernelImage();

  InternalComplexImagePointerType input = NULL;
  InternalComplexImagePointerType kernel = NULL;

  this->PrepareInputs( localInput, kernelImage, input, kernel, progress, 0.7 );

  typedef Functor::WienerFunctor< InternalComplexType,
                         InternalComplexType,
                         InternalComplexType> FunctorType;
  typedef BinaryFunctorImageFilter< InternalComplexImageType,
                               InternalComplexImageType,
                               InternalComplexImageType, FunctorType > WienerFilterType;
  typename WienerFilterType::Pointer wienerFilter = WienerFilterType::New();
  wienerFilter->SetInput1( input );
  wienerFilter->SetInput2( kernel );
  wienerFilter->ReleaseDataFlagOn();
  wienerFilter->GetFunctor().SetNoiseToSignalConstant( 1.0 /  m_SignalToNoiseConstant );
  progress->RegisterInternalFilter( wienerFilter, 0.1 );

  // Free up the memory for the prepared inputs
  input = NULL;
  kernel = NULL;

  this->ProduceOutput( wienerFilter->GetOutput(), progress, 0.2 );
}

template< class TInputImage, class TOutputImage, class TKernelImage, class TInternalPrecision >
void
WienerDeconvolutionImageFilter< TInputImage, TOutputImage, TKernelImage, TInternalPrecision >
::PrintSelf(std::ostream &os, Indent indent) const
{
  this->Superclass::PrintSelf( os, indent );

  os << indent << "SignalToNoiseConstant: " << m_SignalToNoiseConstant << std::endl;
}

}  // end namespace itk
#endif
