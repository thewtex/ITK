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
#ifndef __itkUnaryFunctorWithIndexImageFilter_hxx
#define __itkUnaryFunctorWithIndexImageFilter_hxx

#include "itkUnaryFunctorWithIndexImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage, class TOutputImage, class TFunction  >
UnaryFunctorWithIndexImageFilter< TInputImage, TOutputImage, TFunction >
::UnaryFunctorWithIndexImageFilter()
{
}

template< class TInputImage, class TOutputImage, class TFunction  >
typename UnaryFunctorWithIndexImageFilter< TInputImage, TOutputImage, TFunction >::OutputImagePixelType
UnaryFunctorWithIndexImageFilter< TInputImage, TOutputImage, TFunction >
::InvokeFunctor(const OutputImageIndexType & index, const InputImagePixelType & value)
{
  return this->GetFunctor()( index, value );
}

} // end namespace itk

#endif
