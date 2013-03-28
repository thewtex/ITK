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
#ifndef __itkMeanSquaresImageToImageMetricComputationTypeTemplatev4_hxx
#define __itkMeanSquaresImageToImageMetricComputationTypeTemplatev4_hxx

#include "itkMeanSquaresImageToImageMetricComputationTypeTemplatev4.h"

namespace itk
{

template <class InternalComputationValueType, class TFixedImage, class TMovingImage, class TVirtualImage, class TMetricTraits>
MeanSquaresImageToImageMetricComputationTypeTemplatev4<InternalComputationValueType, TFixedImage,TMovingImage,TVirtualImage,TMetricTraits>
::MeanSquaresImageToImageMetricComputationTypeTemplatev4()
{
    // We have our own GetValueAndDerivativeThreader's that we want
    // ImageToImageMetricv4 to use.
  this->m_DenseGetValueAndDerivativeThreader  = MeanSquaresDenseGetValueAndDerivativeThreaderType::New();
  this->m_SparseGetValueAndDerivativeThreader = MeanSquaresSparseGetValueAndDerivativeThreaderType::New();
}

template <class InternalComputationValueType, class TFixedImage, class TMovingImage, class TVirtualImage, class TMetricTraits >
MeanSquaresImageToImageMetricComputationTypeTemplatev4<InternalComputationValueType, TFixedImage,TMovingImage,TVirtualImage,TMetricTraits>
::~MeanSquaresImageToImageMetricComputationTypeTemplatev4()
{
}

template <class InternalComputationValueType, class TFixedImage, class TMovingImage, class TVirtualImage, class TMetricTraits  >
void
MeanSquaresImageToImageMetricComputationTypeTemplatev4<InternalComputationValueType, TFixedImage,TMovingImage,TVirtualImage,TMetricTraits>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk


#endif
