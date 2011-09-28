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
  this->m_RequiredData.insert( "BackwardGradient" );
  this->m_RequiredData.insert( "ForwardGradient" );
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
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::VectorType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::AdvectionSpeed( const LevelSetInputIndexType& iP ) const
{
  return this->m_AdvectionImage->GetPixel( iP );
}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP )
{
  VectorType advectionField = this->AdvectionSpeed( iP );
  LevelSetOutputRealType oValue = NumericTraits< LevelSetOutputRealType >::Zero;

  LevelSetGradientType backwardGradient = this->m_CurrentLevelSetPointer->EvaluateBackwardGradient( iP );
  LevelSetGradientType forwardGradient = this->m_CurrentLevelSetPointer->EvaluateForwardGradient( iP );

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    LevelSetOutputRealType component = advectionField[dim];

    if( component > NumericTraits< LevelSetOutputRealType >::Zero )
      {
      oValue += component * backwardGradient[dim];
      }
    else
      {
      oValue += component * forwardGradient[dim];
      }
    }

  return oValue;

}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationAdvectionTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP,
         const LevelSetDataType& iData )
{
  VectorType advectionField = this->AdvectionSpeed( iP );
  LevelSetOutputRealType oValue = NumericTraits< LevelSetOutputRealType >::Zero;

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    LevelSetOutputRealType component = advectionField[dim];

    if( component > NumericTraits< LevelSetOutputRealType >::Zero )
      {
      oValue += component * iData.BackwardGradient.m_Value[dim];
      }
    else
      {
      oValue += component * iData.ForwardGradient.m_Value[dim];
      }
    }

  return oValue;
}

}

#endif
