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


#ifndef __itkLevelSetMalcolmEvolutionBase_hxx
#define __itkLevelSetMalcolmEvolutionBase_hxx

#include "itkLevelSetMalcolmEvolutionBase.h"

namespace itk
{
template< class TEquationContainer >
void LevelSetMalcolmEvolutionBase< TEquationContainer >
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
    m_InputImage = m_EquationContainer->GetInput();

    if( m_InputImage.IsNull() )
      {
      itkGenericExceptionMacro( << "m_InputImage is NULL" );
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
void LevelSetMalcolmEvolutionBase< TEquationContainer >
::GenerateData()
    {
    AllocateUpdateBuffer();

    InitializeIteration();

    typename StoppingCriterionType::IterationIdType iter = 0;
    m_StoppingCriterion->SetCurrentIteration( iter );
    m_StoppingCriterion->SetLevelSetContainer( m_LevelSetContainer );

//    for( unsigned int iter = 0; iter < m_NumberOfIterations; iter++ )
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

      // DEBUGGING
//       typedef Image< char, ImageDimension >     LabelImageType;
//       typedef typename LabelImageType::Pointer  LabelImagePointer;
//       typedef LabelMapToLabelImageFilter<LevelSetLabelMapType, LabelImageType> LabelMapToLabelImageFilterType;
//       typedef ImageFileWriter< LabelImageType > WriterType;
//
//       typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();
//       while( it != m_LevelSetContainer->End() )
//         {
//         std::ostringstream filename;
//         filename << "/home/krm15/temp/" << iter << "_" <<  it->GetIdentifier() << ".mha";
//
//         LevelSetPointer levelSet = it->GetLevelSet();
//
//         typename LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
//         labelMapToLabelImageFilter->SetInput( levelSet->GetLabelMap() );
//         labelMapToLabelImageFilter->Update();
//
//         typename WriterType::Pointer writer = WriterType::New();
//         writer->SetInput( labelMapToLabelImageFilter->GetOutput() );
//         writer->SetFileName( filename.str().c_str() );
//         writer->Update();
//
//         ++it;
//         }

      ++iter;

      m_StoppingCriterion->SetRMSChangeAccumulator( m_RMSChangeAccumulator );
      m_StoppingCriterion->SetCurrentIteration( iter );
      this->InvokeEvent( IterationEvent() );
      }
    }

template< class TEquationContainer >
void LevelSetMalcolmEvolutionBase< TEquationContainer >
::InitializeIteration()
  {
    std::cout << "Initialize iteration" << std::endl;
    DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
    DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

    // Initialize parameters here
    m_EquationContainer->InitializeParameters();

    while( map_it != map_end )
    {
      // std::cout << map_it->second.m_Region << std::endl;
      InputImageIteratorType it( m_InputImage, map_it->second.m_Region );
      it.GoToBegin();

      while( !it.IsAtEnd() )
      {
        // std::cout << it.GetIndex() << std::endl;
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
    m_EquationContainer->Update();
  }

template< class TEquationContainer >
void LevelSetMalcolmEvolutionBase< TEquationContainer >
::UpdateLevelSets()
    {
    std::cout << "Update levelsets" << std::endl;

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
void LevelSetMalcolmEvolutionBase< TEquationContainer >
::UpdateEquations()
    {
    std::cout << "Update equations" << std::endl << std::endl;
    m_EquationContainer->Update();
//     this->InitializeIteration();
    }

}
#endif // __itkLevelSetMalcolmEvolutionBase_hxx
