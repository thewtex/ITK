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

#ifndef __itkLevelSetImageBase_hxx
#define __itkLevelSetImageBase_hxx

#include "itkLevelSetImageBase.h"

namespace itk
{
// ----------------------------------------------------------------------------
template< class TImage >
LevelSetImageBase< TImage >
::LevelSetImageBase() : Superclass(), m_Image( NULL )
{}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
typename LevelSetImageBase< TImage >::OutputType
LevelSetImageBase< TImage >::Evaluate( const InputType& iP ) const
{
  return m_Image->GetPixel( iP );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
typename LevelSetImageBase< TImage >::GradientType
LevelSetImageBase< TImage >::EvaluateGradient( const InputType& iP ) const
{
  OutputRealType center_value =
      static_cast< OutputRealType >( this->Evaluate( iP ) );

  InputType pA, pB;
  pA = pB = iP;

  GradientType dx_forward;
  GradientType dx_backward;

  for( unsigned int dim = 0; dim < Dimension; dim++ )
    {
    pA[dim] += 1;
    pB[dim] -= 1;

    OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
    OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );

    dx_forward[dim] = ( valueA - center_value ); // * m_NeighborhoodScales[dim];
    dx_backward[dim] = ( center_value - valueB ); // * m_NeighborhoodScales[dim];

    pA[dim] = pB[dim] = iP[dim];
    }

  return dx_forward;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
typename LevelSetImageBase< TImage >::HessianType
LevelSetImageBase< TImage >
::EvaluateHessian( const InputType& iP ) const
{
  HessianType oHessian;

  OutputRealType center_value =
      static_cast< OutputRealType >( this->Evaluate( iP ) );

  InputType pA, pB, pAa, pBa, pCa, pDa;
  pA = pB = iP;

  for( unsigned int dim1 = 0; dim1 < Dimension; dim1++ )
    {
    pA[dim1] += 1;
    pB[dim1] -= 1;

    OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
    OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );

    oHessian[dim1][dim1] = ( valueA + valueB - 2.0 * center_value );
      // * vnl_math_sqr(m_NeighborhoodScales[i]);

    pAa = pB;
    pBa = pB;

    pCa = pA;
    pDa = pA;

    for( unsigned int dim2 = dim1 + 1; dim2 < Dimension; dim2++ )
      {
      pAa[dim2] -= 1;
      pBa[dim2] += 1;

      pCa[dim2] -= 1;
      pDa[dim2] += 1;

      OutputRealType valueAa = static_cast< OutputRealType >( this->Evaluate( pAa ) );
      OutputRealType valueBa = static_cast< OutputRealType >( this->Evaluate( pBa ) );
      OutputRealType valueCa = static_cast< OutputRealType >( this->Evaluate( pCa ) );
      OutputRealType valueDa = static_cast< OutputRealType >( this->Evaluate( pDa ) );

      oHessian[dim1][dim2] = oHessian[dim2][dim1] =
          0.25 * ( valueAa - valueBa - valueCa + valueDa );
          //  * m_NeighborhoodScales[i] * m_NeighborhoodScales[j];

      pAa[dim2] = pB[dim2];
      pBa[dim2] = pB[dim2];

      pCa[dim2] = pA[dim2];
      pDa[dim2] = pA[dim2];
      }
    pA[dim1] = iP[dim1];
    pB[dim1] = iP[dim1];
    }

  return oHessian;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::Evaluate( const InputType& iP, LevelSetDataType& ioData ) const
{
  // if it has not already been computed before
  if( !ioData.Value.first )
    {
    ioData.Value.first = true;
    ioData.Value.second = m_Image->GetPixel( iP );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::EvaluateGradient( const InputType& iP, LevelSetDataType& ioData ) const
{
  // if it has not already been computed before
  if( !ioData.Gradient.first )
    {
    ioData.Gradient.second = true;
    // compute the gradient

    ///\todo implement the computation of the gradient
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::EvaluateHessian( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.Hessian.first )
    {
    ioData.Hessian.first = true;

    // compute the hessian
    ///\todo implement the computation of the hessian
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::Initialize()
{
  Superclass::Initialize();

  m_Image = NULL;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::CopyInformation(const DataObject *data)
{
  Superclass::CopyInformation( data );

  const Self *LevelSet = NULL;

  try
    {
    LevelSet = dynamic_cast< const Self * >( data );
    }
  catch ( ... )
    {
    // LevelSet could not be cast back down
    itkExceptionMacro( << "itk::LevelSetImageBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetImageBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TImage >
void
LevelSetImageBase< TImage >
::Graft( const DataObject* data )
{
  Superclass::Graft( data );
  const Self *LevelSet = NULL;

  try
    {
    LevelSet = dynamic_cast< const Self* >( data );
    }
  catch( ... )
    {
    // image could not be cast back down
    itkExceptionMacro( << "itk::LevelSetImageBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                         << typeid( Self * ).name() );
    }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetImageBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  this->m_Image = LevelSet->m_Image;
}
// ----------------------------------------------------------------------------

}
#endif // __itkLevelSetImageBase_hxx
