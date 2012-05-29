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

#ifndef __itkLevelSetEquationTerm_hxx
#define __itkLevelSetEquationTerm_hxx

#include "itkLevelSetEquationTerm.h"

#include "itkNumericTraits.h"

namespace itk
{
// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::LevelSetEquationTerm()
{
  this->m_CurrentLevelSetId = LevelSetIdentifierType();

  this->m_Coefficient = NumericTraits< LevelSetOutputRealType >::One;
  this->m_CFLContribution = NumericTraits< LevelSetOutputRealType >::Zero;
  this->m_TermName = "";
}

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::~LevelSetEquationTerm()
{
}

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
const typename LevelSetEquationTerm< TInputImage, TLevelSetContainer >::RequiredDataType &
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::GetRequiredData() const
{
  return this->m_RequiredData;
}

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
void
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::SetLevelSetContainer( LevelSetContainerType* iContainer )
{
  if( iContainer )
    {
    this->m_LevelSetContainer = iContainer;
    this->m_Heaviside = iContainer->GetHeaviside();
    this->Modified();
    }
  else
    {
    itkGenericExceptionMacro( << "iContainer is NULL" );
    }
}

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
typename
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::LevelSetOutputRealType
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::Evaluate( const LevelSetInputIndexType& iP )
{
  if( vnl_math_abs( this->m_Coefficient ) > NumericTraits< LevelSetOutputRealType >::epsilon() )
    {
    return this->m_Coefficient * this->Value( iP );
    }
  else
    {
    return NumericTraits< LevelSetOutputRealType >::Zero;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
typename
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::LevelSetOutputRealType
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::Evaluate( const LevelSetInputIndexType& iP,
            const LevelSetDataType& iData )
{
  if( vnl_math_abs( this->m_Coefficient ) > NumericTraits< LevelSetOutputRealType >::epsilon() )
    {
    return this->m_Coefficient * this->Value( iP, iData );
    }
  else
    {
    return NumericTraits< LevelSetOutputRealType >::Zero;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInputImage, class TLevelSetContainer >
void
LevelSetEquationTerm< TInputImage, TLevelSetContainer >
::SetUp()
{
  this->m_CFLContribution = NumericTraits< LevelSetOutputRealType >::Zero;

  if( this->m_CurrentLevelSetPointer.IsNull() )
    {
    if( this->m_LevelSetContainer.IsNull() )
      {
      itkGenericExceptionMacro( <<"m_LevelSetContainer is NULL" );
      }
    this->m_CurrentLevelSetPointer = this->m_LevelSetContainer->GetLevelSet( this->m_CurrentLevelSetId );

    if( this->m_CurrentLevelSetPointer.IsNull() )
      {
      itkWarningMacro(
      << "m_CurrentLevelSetId does not exist in the level set container" );
      }
    }

  if( !this->m_Heaviside.IsNotNull() )
    {
    itkWarningMacro( << "m_Heaviside is NULL" );
    }
}
// ----------------------------------------------------------------------------

}

#endif // __itkLevelSetEquationTerm_hxx
