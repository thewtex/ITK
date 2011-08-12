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
::LevelSetEvolutionBase() : m_StoppingCriterion( NULL ), m_InputImage( NULL ),
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
  std::cout << "Generate data" << std::endl;
  m_InputImage = m_EquationContainer->GetInput();

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

      // DEBUGGING
/*      typedef Image< unsigned char, ImageDimension > WriterImageType;
      typedef BinaryThresholdImageFilter< LevelSetImageType, WriterImageType >  FilterType;
      typedef ImageFileWriter< WriterImageType > WriterType;
      typedef typename WriterType::Pointer       WriterPointer;

      typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();
      while( it != m_LevelSetContainer->End() )
      {
        std::ostringstream filename;
        filename << "/home/krm15/temp/" << iter << "_" <<  it->GetIdentifier() << ".png";

        LevelSetPointer levelSet = it->GetLevelSet();

        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput( levelSet->GetImage() );
        filter->SetOutsideValue( 0 );
        filter->SetInsideValue(  255 );
        filter->SetLowerThreshold( NumericTraits<typename LevelSetImageType::PixelType>::NonpositiveMin() );
        filter->SetUpperThreshold( 0 );
        filter->Update();

        WriterPointer writer2 = WriterType::New();
        writer2->SetInput( filter->GetOutput() );
        writer2->SetFileName( filename.str().c_str() );
        writer2->Update();
        ++it;
      }
*/

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
  DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
  DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

  std::cout << "Initialize iteration" << std::endl;

  // Initialize parameters here
  m_EquationContainer->InitializeParameters();

  while( map_it != map_end )
    {
    //       std::cout << map_it->second.m_Region << std::endl;

    InputImageIteratorType it( m_InputImage, map_it->second.m_Region );
    it.GoToBegin();

    while( !it.IsAtEnd() )
      {
      //         std::cout << it.GetIndex() << std::endl;
      IdListType lout = map_it->second.m_List;

      if( lout.empty() )
        {
        itkGenericExceptionMacro( <<"No level set exists at voxel" );
        }

      for( IdListIterator lIt = lout.begin(); lIt != lout.end(); ++lIt )
        {
//           std::cout << *lIt -1 << " ";
        m_EquationContainer->GetEquation( *lIt - 1 )->Initialize( it.GetIndex() );
        }
//         std::cout << std::endl;
        ++it;
      }
    ++map_it;
    }
  m_EquationContainer->Update();
}

template< class TEquationContainer >
void
LevelSetEvolutionBase< TEquationContainer >
::ComputeIteration()
{
  DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
  DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

  std::cout << "Begin iteration" << std::endl;

  while( map_it != map_end )
    {
//       std::cout << map_it->second.m_Region << std::endl;

    InputImageIteratorType it( m_InputImage, map_it->second.m_Region );
    it.GoToBegin();

    while( !it.IsAtEnd() )
      {
//         std::cout << it.GetIndex() << std::endl;
      IdListType lout = map_it->second.m_List;

      if( lout.empty() )
        {
        itkGenericExceptionMacro( <<"No level set exists at voxel" );
        }

      for( IdListIterator lIt = lout.begin(); lIt != lout.end(); ++lIt )
        {
//           std::cout << *lIt << " ";
        LevelSetPointer levelSet = m_LevelSetContainer->GetLevelSet( *lIt - 1);
//           std::cout << levelSet->Evaluate( it.GetIndex() ) << ' ';

        LevelSetPointer levelSetUpdate = m_UpdateBuffer->GetLevelSet( *lIt - 1);

        LevelSetOutputRealType temp_update =
            m_EquationContainer->GetEquation( *lIt - 1 )->Evaluate( it.GetIndex() );

        levelSetUpdate->GetImage()->SetPixel( it.GetIndex(), temp_update );
        }
//         std::cout << std::endl;
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
