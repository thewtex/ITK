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
#ifndef __itkGPUFiniteDifferenceFunction_txx
#define __itkGPUFiniteDifferenceFunction_txx

#include "itkGPUFiniteDifferenceFunction.h"

namespace itk
{
template< class TImageType >
GPUFiniteDifferenceFunction< TImageType >::
GPUFiniteDifferenceFunction()
{
  // initialize variables
  m_Radius.Fill(0);
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    m_ScaleCoefficients[i] = 1.0;
    }
}


template< class TImageType >
void
GPUFiniteDifferenceFunction< TImageType >::SetRadius(const RadiusType & r)
{
  m_Radius = r;
}

template< class TImageType >
const typename GPUFiniteDifferenceFunction< TImageType >::RadiusType &
GPUFiniteDifferenceFunction< TImageType >::GetRadius() const
{
  return m_Radius;
}

template< class TImageType >
void
GPUFiniteDifferenceFunction< TImageType >::
SetScaleCoefficients(PixelRealType vals[ImageDimension])
{
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    m_ScaleCoefficients[i] = vals[i];
    }
}

template< class TImageType >
const typename GPUFiniteDifferenceFunction< TImageType >::NeighborhoodScalesType
GPUFiniteDifferenceFunction< TImageType >::ComputeNeighborhoodScales() const
{
  NeighborhoodScalesType neighborhoodScales;

  neighborhoodScales.Fill(0.0);
  typedef typename NeighborhoodScalesType::ComponentType NeighborhoodScaleType;
  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    if ( this->m_Radius[i] > 0 )
      {
      neighborhoodScales[i] = this->m_ScaleCoefficients[i] / this->m_Radius[i];
      }
    }
  return neighborhoodScales;
}


template< class TImageType >
void
GPUFiniteDifferenceFunction< TImageType >::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Radius: " << m_Radius << std::endl;
  os << indent << "ScaleCoefficients: " << m_ScaleCoefficients;
}


} // end namespace itk

#endif
