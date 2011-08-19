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


#ifndef __itkLevelSetSparseEvolutionBase_hxx
#define __itkLevelSetSparseEvolutionBase_hxx

#include "itkLevelSetSparseEvolutionBase.h"

namespace itk
{
template< class TEquationContainer >
LevelSetSparseEvolutionBase<TEquationContainer>
::LevelSetSparseEvolutionBase() : m_StoppingCriterion( NULL ), m_InputImage( NULL ),
  m_EquationContainer( NULL ), m_LevelSetContainer( NULL ),
  m_DomainMapFilter( NULL ), m_Alpha( 0.9 ),
  m_Dt( 1. ), m_RMSChangeAccumulator( -1. ), m_UserDefinedDt( false )
{
}

template< class TEquationContainer >
LevelSetSparseEvolutionBase<TEquationContainer>
::~LevelSetSparseEvolutionBase()
{
  typename LevelSetContainerType::ConstIterator it = m_LevelSetContainer->Begin();
  while( it != m_LevelSetContainer->End() )
    {
    delete m_UpdateBuffer[ it->GetIdentifier() ];
    ++it;
    }
}

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
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

    // Get the LevelSetContainer from the EquationContainer
    m_LevelSetContainer = m_EquationContainer->GetEquation( 0 )->GetTerm( 0 )->GetLevelSetContainer();

    if( m_StoppingCriterion.IsNull() )
      {
      itkGenericExceptionMacro( << "m_StoppingCriterion is NULL" );
      }

    //Run iteration
    this->GenerateData();
    }

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
::SetTimeStep( const LevelSetOutputRealType& iDt )
    {
    if( iDt > NumericTraits< LevelSetOutputRealType >::epsilon() )
      {
      m_UserDefinedDt = true;
      m_Dt = iDt;
      this->Modified();
      }
    else
      {
      itkGenericExceptionMacro( <<"iDt should be > epsilon")
      }
    }


template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
::AllocateUpdateBuffer()
    {
    typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();
    while( it != m_LevelSetContainer->End() )
      {
      IdentifierType id = it->GetIdentifier();

      if( m_UpdateBuffer.find( id ) == m_UpdateBuffer.end() )
        {
        m_UpdateBuffer[ id ] = new LevelSetLayerType;
        }
      else
        {
        if( m_UpdateBuffer[ id ] )
          {
          m_UpdateBuffer[ id ]->clear();
          }
        else
          {
          m_UpdateBuffer[ id ] = new LevelSetLayerType;
          }
        }
      ++it;
      }
    }

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
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
void
LevelSetSparseEvolutionBase<TEquationContainer>
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
void
LevelSetSparseEvolutionBase<TEquationContainer>
::ComputeIteration()
  {
    std::cout << "Compute iteration" << std::endl;
    typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();

    while( it != m_LevelSetContainer->End() )
      {
      LevelSetPointer levelSet = it->GetLevelSet();
      LevelSetLayerIterator list_it = levelSet->GetLayer( 0 ).begin();
      LevelSetLayerIterator list_end = levelSet->GetLayer( 0 ).end();

      while( list_it != list_end )
        {
        LevelSetInputType idx = list_it->first;

        LevelSetOutputType temp_update = static_cast< LevelSetOutputType >(
          m_EquationContainer->GetEquation( it->GetIdentifier() )->Evaluate( idx ) );
        m_UpdateBuffer[ it->GetIdentifier() ]->insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( idx, temp_update ) );
//         std::cout << idx << ' ' << temp_update << std::endl;
        ++list_it;
        }
    ++it;
      }
  }

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
::ComputeDtForNextIteration()
    {
    std::cout << "ComputeDtForNextIteration" << std::endl;
    if( !m_UserDefinedDt )
      {
      if( ( m_Alpha > NumericTraits< LevelSetOutputRealType >::Zero ) &&
          ( m_Alpha < NumericTraits< LevelSetOutputRealType >::One ) )
        {
        LevelSetOutputRealType contribution = m_EquationContainer->ComputeCFLContribution();

        if( contribution > NumericTraits< LevelSetOutputRealType >::epsilon() )
          {
          m_Dt = m_Alpha / contribution;
          }
        else
          {
          if( contribution == NumericTraits< LevelSetOutputRealType >::max() )
            {
            itkGenericExceptionMacro( << "contribution is " << contribution );
            }
          else
            {
            itkGenericExceptionMacro( << "contribution is too low" );
            }
          }
        }
      else
        {
        itkGenericExceptionMacro( <<"m_Alpha should be in [0,1]" );
        }
      }

      std::cout << "Dt = " << m_Dt << std::endl;
//       m_Dt = 0.08;
    }

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
::UpdateLevelSets()
    {
      std::cout << "Update levelsets" << std::endl;

      typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();
      while( it != m_LevelSetContainer->End() )
      {
        LevelSetPointer levelSet = it->GetLevelSet();

        UpdateLevelSetFilterPointer update_levelset = UpdateLevelSetFilterType::New();
        update_levelset->SetInputLevelSet( levelSet );
        update_levelset->SetUpdate( * m_UpdateBuffer[it->GetIdentifier()] );
        update_levelset->SetEquationContainer( m_EquationContainer );
        update_levelset->SetDt( m_Dt );
        update_levelset->SetCurrentLevelSetId( it->GetIdentifier() );
        update_levelset->Update();

        levelSet->Graft( update_levelset->GetOutputLevelSet() );

        m_RMSChangeAccumulator = update_levelset->GetRMSChangeAccumulator();

        m_UpdateBuffer[it->GetIdentifier()]->clear();
        ++it;
      }
    }

template< class TEquationContainer >
void
LevelSetSparseEvolutionBase<TEquationContainer>
::UpdateEquations()
    {
    std::cout << "Update equations" << std::endl;
    m_EquationContainer->Update();
//     InitializeIteration();
    }
}
#endif // __itkLevelSetSparseEvolutionBase_hxx
