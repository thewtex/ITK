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
#ifndef __itkGaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor_hxx
#define __itkGaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor_hxx

#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor.h"

namespace itk
{

template<class TTransform>
GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor() :
  m_GaussianSmoothingVarianceForTheUpdateField( 1.75 ),
  m_GaussianSmoothingVarianceForTheTotalField( 0.5 )
{
}

template<class TTransform>
GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::~GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor()
{
}

template<class TTransform>
void
GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::AdaptTransformParameters()
{
  Superclass::AdaptTransformParameters();

  this->m_Transform->SetGaussianSmoothingVarianceForTheUpdateField(
    this->m_GaussianSmoothingVarianceForTheUpdateField );
  this->m_Transform->SetGaussianSmoothingVarianceForTheTotalField(
    this->m_GaussianSmoothingVarianceForTheTotalField );
}

template <class TTransform>
void
GaussianSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  os << indent << "Gaussian smoothing parameters: " << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheUpdateField: "
     << this->m_GaussianSmoothingVarianceForTheUpdateField
     << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheTotalField: "
     << this->m_GaussianSmoothingVarianceForTheTotalField
     << std::endl;
}

}  // namespace itk

#endif
