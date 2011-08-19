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

#ifndef __itkLevelSetEquationContainerBase_hxx
#define __itkLevelSetEquationContainerBase_hxx

#include "itkLevelSetEquationContainerBase.h"
#include "itkNumericTraits.h"

namespace itk
{
template< class TTermContainer >
LevelSetEquationContainerBase< TTermContainer >
::LevelSetEquationContainerBase() : m_Input( NULL ) {}

template< class TTermContainer >
LevelSetEquationContainerBase< TTermContainer >
::~LevelSetEquationContainerBase() {}

template< class TTermContainer >
typename LevelSetEquationContainerBase< TTermContainer >::LevelSetContainerType*
LevelSetEquationContainerBase< TTermContainer >
::GetLevelSetContainer()
{
  if( m_Container.empty() )
    {
    itkGenericExceptionMacro( << "m_Container is empty" );
    }

  MapContainerIterator it = m_Container.begin();

  return it->second->GetLevelSetContainer();
}

template< class TTermContainer >
void
LevelSetEquationContainerBase< TTermContainer >
::AddEquation( const LevelSetIdentifierType& iId,
               TermContainerPointer iEquation )
{
  if ( iEquation )
    {
    m_Container[iId] = iEquation;
    if( iEquation->GetInput() )
      {
      m_Input = iEquation->GetInput();
      }
    this->Modified();
    }
  else
    {
    itkGenericExceptionMacro( <<"Term supplied is null" );
    }
}

template< class TTermContainer >
typename LevelSetEquationContainerBase< TTermContainer >::TermContainerType*
LevelSetEquationContainerBase< TTermContainer >
::GetEquation( const LevelSetIdentifierType& iId )
{
  MapContainerIterator it = m_Container.find( iId );
  if( it == m_Container.end() )
    {
    itkGenericExceptionMacro( <<"this equation " << iId << " does not exist" );
    }
  return it->second;
}

template< class TTermContainer >
void
LevelSetEquationContainerBase< TTermContainer >
::Update()
{
  for( MapContainerIterator it = m_Container.begin();
    it != m_Container.end();
    ++it )
    {
    (it->second )->Update();
    }
}

template< class TTermContainer >
void
LevelSetEquationContainerBase< TTermContainer >
::UpdatePixel( const LevelSetInputIndexType& iP,
               const LevelSetOutputRealType & oldValue,
               const LevelSetOutputRealType & newValue )
{
  for( MapContainerIterator it = m_Container.begin();
    it != m_Container.end(); ++it )
    {
    (it->second )->UpdatePixel( iP, oldValue, newValue );
    }
  }

template< class TTermContainer >
void
LevelSetEquationContainerBase< TTermContainer >
::InitializeParameters()
{
  for( MapContainerIterator it = m_Container.begin();
    it != m_Container.end();
    ++it )
    {
    (it->second )->InitializeParameters();
    }
  }

template< class TTermContainer >
typename LevelSetEquationContainerBase< TTermContainer >::LevelSetOutputRealType
LevelSetEquationContainerBase< TTermContainer >
::ComputeCFLContribution() const
{
  LevelSetOutputRealType oValue = NumericTraits< LevelSetOutputRealType >::max();

  for( MapContainerConstIterator it = m_Container.begin();
       it != m_Container.end();
       ++it )
    {
    oValue = vnl_math_min( oValue, ( it->second )->ComputeCFLContribution() );
    }

  return oValue;
}

}

#endif // __itkLevelSetEquationContainerBase_hxx
