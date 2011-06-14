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
#ifndef __itkGPUDemonsRegistrationFilter_txx
#define __itkGPUDemonsRegistrationFilter_txx
#include "itkGPUDemonsRegistrationFilter.h"

namespace itk
{
/**
 * Default constructor
 */
template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
GPUDemonsRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::GPUDemonsRegistrationFilter()
{
  typename GPUDemonsRegistrationFunctionType::Pointer drfp;
  drfp = GPUDemonsRegistrationFunctionType::New();

  this->SetDifferenceFunction( static_cast< FiniteDifferenceFunctionType * >(
                                 drfp.GetPointer() ) );

  m_UseMovingImageGradient = false;
}

template< class TFixedImage, class TMovingImage, class TDeformationField, class TParentImageFilter >
void
GPUDemonsRegistrationFilter< TFixedImage, TMovingImage, TDeformationField, TParentImageFilter >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk

#endif
