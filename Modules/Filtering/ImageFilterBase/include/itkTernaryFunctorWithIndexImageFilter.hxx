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
#ifndef __itkTernaryFunctorWithIndexImageFilter_hxx
#define __itkTernaryFunctorWithIndexImageFilter_hxx

#include "itkTernaryFunctorWithIndexImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage1, class TInputImage2,
          class TInputImage3, class TOutputImage, class TFunction  >
TernaryFunctorWithIndexImageFilter< TInputImage1, TInputImage2, TInputImage3, TOutputImage, TFunction >
::TernaryFunctorWithIndexImageFilter()
{
}

template< class TInputImage1, class TInputImage2,
          class TInputImage3, class TOutputImage, class TFunction  >
typename TernaryFunctorWithIndexImageFilter< TInputImage1, TInputImage2, TInputImage3, TOutputImage, TFunction >::OutputImagePixelType
TernaryFunctorWithIndexImageFilter< TInputImage1, TInputImage2, TInputImage3, TOutputImage, TFunction >
::InvokeFunctor(const OutputImageIndexType & index,
                const Input1ImagePixelType & value1,
                const Input2ImagePixelType & value2,
                const Input3ImagePixelType & value3)
{
  return this->GetFunctor()( index, value1, value2, value3 );
}
} // end namespace itk

#endif
