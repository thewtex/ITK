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

#ifndef __itkLevelSetEquationAdvectionTerm_hxx
#define __itkLevelSetEquationAdvectionTerm_hxx

#include "itkLevelSetEquationAdvectionTerm.h"

namespace itk
{
template< class TInput, class TLevelSetContainer >
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::LevelSetEquationAdvectionTerm()
{
  for( unsigned int i = 0; i < ImageDimension; i++ )
    {
    m_NeighborhoodScales[i] = 1.0;
    }
  this->m_TermName = "Advection term";
//  this->m_RequiredData.insert( "BackwardGradient" );
//  this->m_RequiredData.insert( "ForwardGradient" );
}

template< class TInput, class TLevelSetContainer >
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::~LevelSetEquationAdvectionTerm()
{
}


template< class TInput, class TLevelSetContainer >
void
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::InitializeParameters()
{
  this->SetUp();
}

template< class TInput, class TLevelSetContainer >
void
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Initialize( const LevelSetInputIndexType& )
{
}

template< class TInput, class TLevelSetContainer >
void
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Update()
{
}

template< class TInput, class TLevelSetContainer >
void
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::UpdatePixel( const LevelSetInputIndexType& itkNotUsed( iP ),
               const LevelSetOutputRealType& itkNotUsed( oldValue ),
               const LevelSetOutputRealType& itkNotUsed( newValue ) )
{
}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::AdvectionSpeed( const LevelSetInputIndexType& iP ) const
{
  return ( static_cast< LevelSetOutputRealType >( this->m_Input->GetPixel(iP) ) );
}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP )
{
  // Construct upwind gradient values for use in the advection speed term:
  //  $\beta G(\mathbf{x})\mid\nabla\phi\mid$
  // The following scheme for ``upwinding'' in the normal direction is taken
  // from Sethian, Ch. 6 as referenced above.

  //
  // TODO FIXME DO A SECOND REVIEW OF CODE STYLE
  //
  const LevelSetOutputRealType center_value =
    static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( iP ) );

  LevelSetInputIndexType pA;
  LevelSetInputIndexType pB;
  LevelSetOutputRealType valueA;
  LevelSetOutputRealType valueB;

  const LevelSetOutputRealType zero = NumericTraits< LevelSetOutputRealType >::Zero;

  /** Array of first derivatives */
  LevelSetOutputRealType m_dx_forward[itkGetStaticConstMacro(ImageDimension)];
  LevelSetOutputRealType m_dx_backward[itkGetStaticConstMacro(ImageDimension)];

  for ( unsigned int i = 0; i < ImageDimension; i++ )
    {
    pA = pB = iP;
    pA[i] += 1;
    pB[i] -= 1;

    valueA =
        static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pA ) );
    valueB =
        static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pB ) );

    m_dx_forward[i]  = ( valueA - center_value ) * m_NeighborhoodScales[i];
    m_dx_backward[i] = ( center_value - valueB ) * m_NeighborhoodScales[i];
    }

  /// \todo why this initialization ?
  LevelSetOutputRealType advection_gradient = zero;

  if ( advection_gradient > NumericTraits< LevelSetOutputRealType >::Zero )
    {
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      advection_gradient +=
        vnl_math_sqr( vnl_math_max( m_dx_backward[i], zero ) ) +
        vnl_math_sqr( vnl_math_min( m_dx_forward[i],  zero ) );
      }
    }
  else
    {
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      advection_gradient +=
        vnl_math_sqr( vnl_math_min( m_dx_backward[i], zero ) ) +
        vnl_math_sqr( vnl_math_max( m_dx_forward[i],  zero) );
      }
    }
  advection_gradient *= this->AdvectionSpeed( iP );

  return advection_gradient;
}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP,
         const LevelSetDataType& iData )
{
  //
  // TODO FIXME DO A SECOND REVIEW OF CODE STYLE
  //
  const LevelSetOutputRealType zero = NumericTraits< LevelSetOutputRealType >::Zero;
  LevelSetOutputRealType advection_gradient = zero;

  if ( advection_gradient > zero )
    {
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      advection_gradient +=
        vnl_math_sqr( vnl_math_max( iData.BackwardGradient.m_Value[i], zero ) ) +
        vnl_math_sqr( vnl_math_min( iData.ForwardGradient.m_Value[i],  zero ) );
      }
    }
  else
    {
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      advection_gradient +=
        vnl_math_sqr( vnl_math_min( iData.BackwardGradient.m_Value[i], zero ) ) +
        vnl_math_sqr( vnl_math_max( iData.ForwardGradient.m_Value[i],  zero) );
      }
    }
  advection_gradient *= this->AdvectionSpeed( iP );

  return advection_gradient;
}

}

#endif
