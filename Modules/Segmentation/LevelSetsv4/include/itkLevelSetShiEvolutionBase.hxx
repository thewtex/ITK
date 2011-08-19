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


#ifndef __itkLevelSetShiEvolutionBase_hxx
#define __itkLevelSetShiEvolutionBase_hxx

#include "itkLevelSetShiEvolutionBase.h"

namespace itk
{
template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::Update()
{
  if( m_EquationContainer.IsNull() )
    {
    itkGenericExceptionMacro( << "m_EquationContainer is NULL" );
    }

  if( !m_EquationContainer->GetEquation( 0 ) )
    {
    itkGenericExceptionMacro( << "m_EquationContainer->GetEquation( 0 ) is NULL" );
    }

  m_DomainMapFilter = m_LevelSetContainer->GetDomainMapFilter();

  // Get the image to be segmented
  InputImageConstPointer inputImage = m_EquationContainer->GetInput();

  if( inputImage.IsNull() )
    {
    itkGenericExceptionMacro( << "Input Image is NULL" );
    }

  TermContainerPointer Equation0 = m_EquationContainer->GetEquation( 0 );
  TermPointer term0 = Equation0->GetTerm( 0 );

  // Get the LevelSetContainer from the EquationContainer
  m_LevelSetContainer = term0->GetLevelSetContainer();

  if( term0.IsNull() )
    {
    itkGenericExceptionMacro( << "m_EquationContainer->GetEquation( 0 ) is NULL" );
    }

  if( !term0->GetLevelSetContainer() )
    {
    itkGenericExceptionMacro( << "m_LevelSetContainer is NULL" );
    }

  //Run iteration
  this->GenerateData();
}

template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::GenerateData()
{
  AllocateUpdateBuffer();

  InitializeIteration();

  typename StoppingCriterionType::IterationIdType iter = 0;
  m_StoppingCriterion->SetCurrentIteration( iter );
  m_StoppingCriterion->SetLevelSetContainer( m_LevelSetContainer );

  while( !m_StoppingCriterion->IsSatisfied() )
    {
    std::cout <<"Iteration " <<iter << std::endl;
    m_RMSChangeAccumulator = NumericTraits< LevelSetOutputRealType >::Zero;

    // one iteration over all container
    // update each level set based on the different equations provided
    ComputeIteration();

    ComputeDtForNextIteration();

    UpdateLevelSets();
    UpdateEquations();

    ++iter;

    m_StoppingCriterion->SetRMSChangeAccumulator( m_RMSChangeAccumulator );
    m_StoppingCriterion->SetCurrentIteration( iter );
    this->InvokeEvent( IterationEvent() );
    }
}

template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::InitializeIteration()
{
  // Get the image to be segmented
  InputImageConstPointer inputImage = m_EquationContainer->GetInput();

  DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
  DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

  // Initialize parameters here
  m_EquationContainer->InitializeParameters();

  while( map_it != map_end )
    {
    InputImageConstIteratorType it( inputImage, map_it->second.m_Region );
    it.GoToBegin();

    while( !it.IsAtEnd() )
      {
      IdListType lout = map_it->second.m_List;

      if( lout.empty() )
        {
        itkGenericExceptionMacro( <<"No level set exists at voxel" );
        }

      for( IdListIterator lIt = lout.begin(); lIt != lout.end(); ++lIt )
        {
        m_EquationContainer->GetEquation( *lIt - 1 )->Initialize( it.GetIndex() );
        }
      ++it;
      }
    ++map_it;
    }
  m_EquationContainer->UpdateInternalEquationTerms();
}

template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::ComputeIteration()
{
}

template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::ComputeDtForNextIteration()
{
}


template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::UpdateLevelSets()
{
  typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();

  while( it != m_LevelSetContainer->End() )
    {
    LevelSetPointer levelSet = it->GetLevelSet();

    UpdateLevelSetFilterPointer update_levelset = UpdateLevelSetFilterType::New();
    update_levelset->SetInputLevelSet( levelSet );
    update_levelset->SetCurrentLevelSetId( it->GetIdentifier() );
    update_levelset->SetEquationContainer( m_EquationContainer );
    update_levelset->Update();

    levelSet->Graft( update_levelset->GetOutputLevelSet() );

    m_RMSChangeAccumulator = update_levelset->GetRMSChangeAccumulator();

    ++it;
    }
}

template< class TEquationContainer >
void LevelSetShiEvolutionBase< TEquationContainer >
::UpdateEquations()
{
  m_EquationContainer->UpdateInternalEquationTerms();
}

}
#endif // __itkLevelSetShiEvolutionBase_h
