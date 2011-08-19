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


#ifndef __itkLevelSetEvolutionBase_hxx
#define __itkLevelSetEvolutionBase_hxx

#include "itkImage.h"
#include "itkLevelSetEvolutionBase.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include <list>
#include "itkObject.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"
#include "itkLevelSetEvolutionStoppingCriterionBase.h"

namespace itk
{
template< class TEquationContainer >
LevelSetEvolutionBase< TEquationContainer >
::LevelSetEvolutionBase() : m_StoppingCriterion( NULL ),
  m_EquationContainer( NULL ), m_LevelSetContainer( NULL ),
  m_UpdateBuffer( NULL ), m_DomainMapFilter( NULL ), m_Alpha( 0.9 ),
  m_Dt( 1. ), m_RMSChangeAccumulator( -1. ), m_UserDefinedDt( false )
{}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >::Update()
{
  m_DomainMapFilter = m_LevelSetContainer->GetDomainMapFilter();
  //Run iteration
  this->GenerateData();
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
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
LevelSetEvolutionBase< TEquationContainer >
::AllocateUpdateBuffer()
{
  this->m_UpdateBuffer = LevelSetContainerType::New();
  this->m_UpdateBuffer->CopyInformationAndAllocate( m_LevelSetContainer, true );
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
::GenerateData()
{
  // Get the LevelSetContainer from the EquationContainer
  m_LevelSetContainer = m_EquationContainer->GetEquation( 0 )->GetTerm( 0 )->GetLevelSetContainer();
  AllocateUpdateBuffer();

  InitializeIteration();

  typename StoppingCriterionType::IterationIdType iter = 0;
  m_StoppingCriterion->SetCurrentIteration( iter );
  m_StoppingCriterion->SetLevelSetContainer( m_LevelSetContainer );

  while( !m_StoppingCriterion->IsSatisfied() )
    {
    m_RMSChangeAccumulator = 0;

    // one iteration over all container
    // update each level set based on the different equations provided
    ComputeIteration();

    //       ComputeCFL();

    ComputeDtForNextIteration();

    UpdateLevelSets();
    Reinitialize();
    UpdateEquations();

    ++iter;

    m_StoppingCriterion->SetRMSChangeAccumulator( m_RMSChangeAccumulator );
    m_StoppingCriterion->SetCurrentIteration( iter );

    this->InvokeEvent( IterationEvent() );
    }
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
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
void
LevelSetEvolutionBase< TEquationContainer >
::ComputeIteration()
{
  InputImageConstPointer inputImage = m_EquationContainer->GetInput();

  DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
  DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

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
        LevelSetPointer levelSet = m_LevelSetContainer->GetLevelSet( *lIt - 1);

        LevelSetPointer levelSetUpdate = m_UpdateBuffer->GetLevelSet( *lIt - 1);

        LevelSetDataType characteristics;
        m_EquationContainer->GetEquation( *lIt - 1 )->ComputeRequiredData(
              it.GetIndex(), characteristics );

       LevelSetOutputRealType temp_update =
           m_EquationContainer->GetEquation( *lIt - 1 )->Evaluate( it.GetIndex(), characteristics );

        levelSetUpdate->GetImage()->SetPixel( it.GetIndex(), temp_update );
        }
      ++it;
      }
      ++map_it;
    }
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
::ComputeDtForNextIteration()
{
  // if the time step is not globally set
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
LevelSetEvolutionBase< TEquationContainer >
::UpdateLevelSets()
{
  typename LevelSetContainerType::Iterator it1 = m_LevelSetContainer->Begin();
  typename LevelSetContainerType::ConstIterator it2 = m_UpdateBuffer->Begin();

  LevelSetOutputRealType p;

  while( it1 != m_LevelSetContainer->End() )
    {
    LevelSetImagePointer image1 = it1->GetLevelSet()->GetImage();
    LevelSetImagePointer image2 = it2->GetLevelSet()->GetImage();

    LevelSetImageIteratorType It1( image1, image1->GetBufferedRegion() );
    LevelSetImageIteratorType It2( image2, image2->GetBufferedRegion() );
    It1.GoToBegin();
    It2.GoToBegin();

    while( !It1.IsAtEnd() )
      {
      p = m_Dt * It2.Get();
      It1.Set( It1.Get() + p );

      m_RMSChangeAccumulator += p*p;

      ++It1;
      ++It2;
      }

    ++it1;
    ++it2;
    }
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
::UpdateEquations()
{
  InitializeIteration();
//     m_EquationContainer->Update();
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
::Reinitialize()
{
  typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();

  while( it != m_LevelSetContainer->End() )
    {
    LevelSetImagePointer image = it->GetLevelSet()->GetImage();

    ThresholdFilterPointer thresh = ThresholdFilterType::New();
    thresh->SetLowerThreshold(
          NumericTraits< LevelSetOutputRealType >::NonpositiveMin() );
    thresh->SetUpperThreshold( 0 );
    thresh->SetInsideValue( 1 );
    thresh->SetOutsideValue( 0 );
    thresh->SetInput( image );
    thresh->Update();

    MaurerPointer maurer = MaurerType::New();
    maurer->SetInput( thresh->GetOutput() );
    maurer->SetSquaredDistance( false );
    maurer->SetUseImageSpacing( true );
    maurer->SetInsideIsPositive( false );
    maurer->Update();

    LevelSetImageIteratorType It1( image, image->GetBufferedRegion() );
    LevelSetImageIteratorType It2( maurer->GetOutput(), image->GetBufferedRegion() );
    It1.GoToBegin();
    It2.GoToBegin();
    while( !It1.IsAtEnd() )
      {
      It1.Set( It2.Get() );
      ++It1;
      ++It2;
      }
    ++it;
    }
}

}
#endif // __itkLevelSetEvolutionBase_hxx
