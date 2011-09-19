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
#ifndef __itkBSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor_hxx
#define __itkBSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor_hxx

#include "itkBSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor.h"

namespace itk
{

template<class TTransform>
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor()
{
  this->m_SplineOrder = 3;
  this->m_NumberOfControlPointsForTheUpdateField.Fill( 4 );
  this->m_NumberOfControlPointsForTheTotalField.Fill( 0 );
  this->m_EnforceStationaryBoundary = true;
}

template<class TTransform>
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::~BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor()
{
}

/**
 * set mesh size for update field
 */
template<class TTransform>
void
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::SetMeshSizeForTheUpdateField( const ArrayType &meshSize )
{
  ArrayType numberOfControlPoints;
  for( unsigned int d = 0; d < SpaceDimension; d++ )
    {
    numberOfControlPoints[d] = meshSize[d] + this->m_SplineOrder;
    }
  this->SetNumberOfControlPointsForTheUpdateField( numberOfControlPoints );
}

/**
 * set mesh size for total field
 */
template<class TTransform>
void
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::SetMeshSizeForTheTotalField( const ArrayType &meshSize )
{
  ArrayType numberOfControlPoints;
  for( unsigned int d = 0; d < SpaceDimension; d++ )
    {
    numberOfControlPoints[d] = meshSize[d] + this->m_SplineOrder;
    }
  this->SetNumberOfControlPointsForTheTotalField( numberOfControlPoints );
}

template<class TTransform>
void
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::AdaptTransformParameters()
{
  Superclass::AdaptTransformParameters();

  this->m_Transform->SetSplineOrder( this->m_SplineOrder );

  this->m_Transform->SetNumberOfControlPointsForTheUpdateField(
    this->m_NumberOfControlPointsForTheUpdateField );
  this->m_Transform->SetNumberOfControlPointsForTheTotalField(
    this->m_NumberOfControlPointsForTheTotalField );

  this->m_Transform->SetEnforceStationaryBoundary( this->m_EnforceStationaryBoundary );
}

template <class TTransform>
void
BSplineSmoothingOnUpdateDisplacementFieldTransformParametersAdaptor<TTransform>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  if( this->m_EnforceStationaryBoundary )
    {
    os << indent << "Enforce stationary boundary" << std::endl;
    }
  else
    {
    os << indent << "Does not enforce stationary boundary" << std::endl;
    }
  os << indent << "B-spline parameters: " << std::endl;
  os << indent << "  spline order = " << this->m_SplineOrder << std::endl;
  os << indent << "  number of control points for the update field = "
    << this->m_NumberOfControlPointsForTheUpdateField << std::endl;
  os << indent << "  number of control points for the total field = "
    << this->m_NumberOfControlPointsForTheTotalField << std::endl;
}

}  // namespace itk

#endif
