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
#ifndef __itkThinPlateSplineKernelTransformBase_txx
#define __itkThinPlateSplineKernelTransformBase_txx

#include "itkThinPlateSplineKernelTransformBase.h"
#include "itkThinPlateSplineRadialBasisFunctionR.h"
#include "itkThinPlateSplineRadialBasisFunctionR2LogR.h"

namespace itk
{

//Initialize new instance
template< class TScalarType, unsigned int NDimensions  >
ThinPlateSplineKernelTransformBase< TScalarType, NDimensions >::ThinPlateSplineKernelTransformBase()
{
  if( NDimensions == 2)
    {
    typedef ThinPlateSplineRadialBasisFunctionR2LogR< TScalarType > R2LogRType;
    typename R2LogRType::Pointer DefaultTwoDKernelFunction =  R2LogRType::New();
    m_Function = DefaultTwoDKernelFunction.GetPointer();
    }
  else
    {
    typedef ThinPlateSplineRadialBasisFunctionR< TScalarType > RType;
    typename RType::Pointer DefaultThreeDKernelFunction =  RType::New();
    m_Function = DefaultThreeDKernelFunction.GetPointer();
    }
}

//Compute the G matrix using the specified kernel function
template< class TScalarType, unsigned int NDimensions >
void
ThinPlateSplineKernelTransformBase< TScalarType, NDimensions >::ComputeG(const InputVectorType & x,
                                                                     GMatrixType & gmatrix) const
{
  //Throw an exception if no kernel functor has been set.
  if ( !m_Function )
  {
  itkExceptionMacro(<< "Kernel function not set");
  }

  const TScalarType r = m_Function->Evaluate( x.GetNorm() );
  gmatrix.fill(NumericTraits< TScalarType >::Zero);
  for ( unsigned int i = 0; i < NDimensions; i++ )
    {
    gmatrix[i][i] = r;
    }
}

//Computes the contribution of the current point warping to the deformation of other points.
template< class TScalarType, unsigned int NDimensions  >
void
ThinPlateSplineKernelTransformBase< TScalarType, NDimensions >::ComputeDeformationContribution(
  const InputPointType  & thisPoint,
  OutputPointType & result) const
{
  const unsigned long numberOfLandmarks = this->m_SourceLandmarks->GetNumberOfPoints();
  PointsConstIterator sp  = this->m_SourceLandmarks->GetPoints()->Begin();
  for ( unsigned int lnd = 0; lnd < numberOfLandmarks; lnd++ )
    {
    const InputVectorType position = thisPoint - sp->Value();

    const TScalarType r = m_Function->Evaluate( position.GetNorm() );
    for ( unsigned int odim = 0; odim < NDimensions; odim++ )
      {
      result[odim] += r * this->m_DMatrix(odim, lnd);
      }
    ++sp;
    }
}

} // namespace itk
#endif
