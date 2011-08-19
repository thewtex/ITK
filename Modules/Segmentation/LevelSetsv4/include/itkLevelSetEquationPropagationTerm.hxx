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

#ifndef __itkLevelSetEquationPropagationTerm_hxx
#define __itkLevelSetEquationPropagationTerm_hxx

#include "itkLevelSetEquationPropagationTerm.h"

namespace itk
{
template< class TInput, class TLevelSetContainer >
typename LevelSetEquationPropagationTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationPropagationTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP )
  {
    // Construct upwind gradient values for use in the propagation speed term:
    //  $\beta G(\mathbf{x})\mid\nabla\phi\mid$
    // The following scheme for ``upwinding'' in the normal direction is taken
    // from Sethian, Ch. 6 as referenced above.

    LevelSetOutputRealType center_value =
      static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( iP ) );
    LevelSetInputIndexType pA, pB;
    LevelSetOutputRealType valueA, valueB;

    LevelSetOutputRealType ZERO = NumericTraits< LevelSetOutputRealType >::Zero;

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
    LevelSetOutputRealType propagation_gradient = ZERO;

    if ( propagation_gradient > NumericTraits< LevelSetOutputRealType >::Zero )
      {
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        propagation_gradient +=
            vnl_math_sqr( vnl_math_max( m_dx_backward[i], ZERO ) ) +
            vnl_math_sqr( vnl_math_min( m_dx_forward[i],  ZERO ) );
        }
      }
    else
      {
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        propagation_gradient +=
            vnl_math_sqr( vnl_math_min( m_dx_backward[i], ZERO ) ) +
            vnl_math_sqr( vnl_math_max( m_dx_forward[i],  ZERO) );
        }
      }
    propagation_gradient *= this->PropagationSpeed( iP );
//     std::cout << iP << ' ' << propagation_gradient << std::endl;
    return propagation_gradient;
  }

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationPropagationTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationPropagationTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP,
         const LevelSetDataType& iData )
  {
    LevelSetOutputRealType ZERO = NumericTraits< LevelSetOutputRealType >::Zero;
    LevelSetOutputRealType propagation_gradient = ZERO;

    if ( propagation_gradient > ZERO )
      {
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        propagation_gradient +=
            vnl_math_sqr( vnl_math_max( iData.BackwardGradient.m_Value[i], ZERO ) ) +
            vnl_math_sqr( vnl_math_min( iData.ForwardGradient.m_Value[i],  ZERO ) );
        }
      }
    else
      {
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        propagation_gradient +=
            vnl_math_sqr( vnl_math_min( iData.BackwardGradient.m_Value[i], ZERO ) ) +
            vnl_math_sqr( vnl_math_max( iData.ForwardGradient.m_Value[i],  ZERO) );
        }
      }
    propagation_gradient *= this->PropagationSpeed( iP );
//     std::cout << iP << ' ' << propagation_gradient << std::endl;
    return propagation_gradient;
  }
}

#endif
