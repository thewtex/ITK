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
#ifndef __itkVnlComplexToComplexFFTImageFilter_hxx
#define __itkVnlComplexToComplexFFTImageFilter_hxx

#include "itkVnlComplexToComplexFFTImageFilter.h"
#include "itkProgressReporter.h"
#include "itkVnlFFTCommon.h"


namespace itk
{

template< typename TImage >
VnlComplexToComplexFFTImageFilter< TImage >
::VnlComplexToComplexFFTImageFilter()
{
}


template <typename TImage>
void
VnlComplexToComplexFFTImageFilter< TImage >
::GenerateData()
{
  //
  // Normalize the output if backward transform
  //
  if ( this->GetTransformDirection() == Superclass::INVERSE )
    {
    typedef ImageRegionIterator< OutputImageType >   IteratorType;
    SizeValueType totalOutputSize = this->GetOutput()->GetRequestedRegion().GetNumberOfPixels();
    IteratorType it(this->GetOutput(), outputRegionForThread);
    while( !it.IsAtEnd() )
      {
      PixelType val = it.Value();
      val /= totalOutputSize;
      it.Set(val);
      ++it;
      }
    }
}

} // end namespace itk

#endif // _itkVnlComplexToComplexFFTImageFilter_hxx
