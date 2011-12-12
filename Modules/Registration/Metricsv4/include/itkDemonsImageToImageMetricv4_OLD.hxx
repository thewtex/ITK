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
#ifndef __itkDemonsImageToImageMetricv4_OLD_hxx
#define __itkDemonsImageToImageMetricv4_OLD_hxx

#include "itkDemonsImageToImageMetricv4_OLD.h"

namespace itk
{

template < class TFixedImage, class TMovingImage, class TVirtualImage >
DemonsImageToImageMetricv4_OLD<TFixedImage,TMovingImage,TVirtualImage>
::DemonsImageToImageMetricv4_OLD()
{
  // We have our own GetValueAndDerivativeThreader's that we want
  // ImageToImageMetricv4_OLD to use.
  this->m_DenseGetValueAndDerivativeThreader  = DemonsDenseGetValueAndDerivativeThreaderType::New();
  this->m_SparseGetValueAndDerivativeThreader = DemonsSparseGetValueAndDerivativeThreaderType::New();
}

template < class TFixedImage, class TMovingImage, class TVirtualImage >
DemonsImageToImageMetricv4_OLD<TFixedImage,TMovingImage,TVirtualImage>
::~DemonsImageToImageMetricv4_OLD()
{
}

template < class TFixedImage, class TMovingImage, class TVirtualImage  >
void
DemonsImageToImageMetricv4_OLD<TFixedImage,TMovingImage,TVirtualImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk


#endif
