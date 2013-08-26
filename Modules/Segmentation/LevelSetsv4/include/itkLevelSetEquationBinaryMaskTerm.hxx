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

#ifndef __itkLevelSetEquationBinaryMaskTerm_hxx
#define __itkLevelSetEquationBinaryMaskTerm_hxx

#include "itkLevelSetEquationBinaryMaskTerm.h"

namespace itk
{

template< class TInput, class TLevelSetContainer >
LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::LevelSetEquationBinaryMaskTerm()
{
  this->m_TermName = "Binary mask term";
  this->m_RequiredData.insert( "Value" );
}

template< class TInput, class TLevelSetContainer >
LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::~LevelSetEquationBinaryMaskTerm()
{}

template< class TInput, class TLevelSetContainer >
void LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::Update()
{}

template< class TInput, class TLevelSetContainer >
void LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::InitializeParameters()
{
  this->SetUp();
}


template< class TInput, class TLevelSetContainer >
void LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::Initialize( const LevelSetInputIndexType& itkNotUsed( iP ) )
{}


template< class TInput, class TLevelSetContainer >
void LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::UpdatePixel( const LevelSetInputIndexType& itkNotUsed( iP ),
               const LevelSetOutputRealType & itkNotUsed( oldValue ),
               const LevelSetOutputRealType & itkNotUsed( newValue ) )
{}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP )
{
  const InputPixelType pixel = this->m_Mask->GetPixel( iP );
  LevelSetOutputRealType oValue;
  if( pixel > 0 )
    {
    oValue = NumericTraits< LevelSetOutputRealType >::Zero;
    }
  else
    {
    oValue = NumericTraits<LevelSetOutputRealType>::max();
    }
  return oValue;
}

template< class TInput, class TLevelSetContainer >
typename LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >::LevelSetOutputRealType
LevelSetEquationBinaryMaskTerm< TInput, TLevelSetContainer >
::Value( const LevelSetInputIndexType& iP, const LevelSetDataType& itkNotUsed( iData ) )
{
  const InputPixelType pixel = this->m_Mask->GetPixel( iP );
  LevelSetOutputRealType oValue;
  if( pixel > 0 )
    {
    oValue = NumericTraits< LevelSetOutputRealType >::Zero;
    }
  else
    {
    oValue = NumericTraits<LevelSetOutputRealType>::max();
    }
  return oValue;
}

}
#endif
