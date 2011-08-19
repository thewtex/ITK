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

#ifndef __itkUpdateMalcolmSparseLevelSet_hxx
#define __itkUpdateMalcolmSparseLevelSet_hxx

#include "itkUpdateMalcolmSparseLevelSet.h"


namespace itk
{
template< unsigned int VDimension, class TEquationContainer >
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::UpdateMalcolmSparseLevelSet() :
  m_RMSChangeAccumulator( NumericTraits< LevelSetOutputRealType >::Zero ),
  m_IsUsingUnPhasedPropagation( true )
{
  this->m_OutputLevelSet = LevelSetType::New();
}

template< unsigned int VDimension, class TEquationContainer >
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::~UpdateMalcolmSparseLevelSet()
{}

template< unsigned int VDimension, class TEquationContainer >
void
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::Update()
{
  if( this->m_InputLevelSet.IsNull() )
    {
    itkGenericExceptionMacro( <<"m_InputLevelSet is NULL" );
    }

  this->m_OutputLevelSet->SetLayer( 0, this->m_InputLevelSet->GetLayer( 0 ) );
  this->m_OutputLevelSet->SetLabelMap( this->m_InputLevelSet->GetLabelMap() );

  typedef LabelMapToLabelImageFilter<LevelSetLabelMapType, LabelImageType> LabelMapToLabelImageFilterType;
  typename LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
  labelMapToLabelImageFilter->SetInput( this->m_InputLevelSet->GetLabelMap() );
  labelMapToLabelImageFilter->Update();

  this->m_InternalImage = labelMapToLabelImageFilter->GetOutput();
  this->m_InternalImage->DisconnectPipeline();

  FillUpdateContainer();

  if( this->m_IsUsingUnPhasedPropagation )
    {
    EvolveWithUnPhasedPropagation();
    CompactLayersToSinglePixelThickness();
    }
  else
    {
    LevelSetLayerType& list_0 = this->m_OutputLevelSet->GetLayer( 0 );

    LevelSetLayerType list_pos;
    LevelSetLayerType update_pos;

    LevelSetLayerType list_neg;
    LevelSetLayerType update_neg;

    LevelSetLayerIterator nodeIt = list_0.begin();
    LevelSetLayerIterator nodeEnd = list_0.end();

    LevelSetLayerIterator upIt = this->m_Update.begin();

    while( nodeIt != nodeEnd )
      {
      assert( nodeIt->first == upIt->first );

      const LevelSetInputType currentIdx = nodeIt->first;
      const LevelSetOutputType update = upIt->second;

      if( update > 0 )
        {
        list_pos.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIdx, 0 ) );
        update_pos.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIdx, 1 ) );
        }
      else
        {
        list_neg.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIdx, 0 ) );
        update_neg.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIdx, -1 ) );
        }
      ++nodeIt;
      ++upIt;
      }

    // contraction
    EvolveWithPhasedPropagation( list_pos, update_pos, true );
    CompactLayersToSinglePixelThickness();

    // dilation
    EvolveWithPhasedPropagation( list_neg, update_neg, false );
    CompactLayersToSinglePixelThickness();
    }

  typedef LabelImageToLabelMapFilter< LabelImageType, LevelSetLabelMapType> LabelImageToLabelMapFilterType;
  typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
  labelImageToLabelMapFilter->SetInput( this->m_InternalImage );
  labelImageToLabelMapFilter->SetBackgroundValue( 1 );
  labelImageToLabelMapFilter->Update();

  LevelSetLabelMapPointer outputLabelMap = this->m_OutputLevelSet->GetLabelMap( );
  outputLabelMap->Graft( labelImageToLabelMapFilter->GetOutput() );
}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::FillUpdateContainer()
{
  LevelSetLayerType level0 = this->m_OutputLevelSet->GetLayer( 0 );

  LevelSetLayerIterator nodeIt = level0.begin();
  LevelSetLayerIterator nodeEnd = level0.end();

  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  while( nodeIt != nodeEnd )
    {
    const LevelSetInputType currentIndex = nodeIt->first;

    const LevelSetOutputRealType update = termContainer->Evaluate( currentIndex );

    LevelSetOutputType value = NumericTraits< LevelSetOutputType >::Zero;

    if( update > NumericTraits< LevelSetOutputRealType >::Zero )
      {
      value = NumericTraits< LevelSetOutputType >::One;
      }
    if( update < NumericTraits< LevelSetOutputRealType >::Zero )
      {
      value = - NumericTraits< LevelSetOutputType >::One;
      }

    this->m_Update.insert(
          std::pair< LevelSetInputType, LevelSetOutputType >( currentIndex, value ) );

    ++nodeIt;
    }
}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::EvolveWithUnPhasedPropagation()
{
  LevelSetOutputRealType oldValue;
  LevelSetOutputRealType newValue;
  LevelSetLayerType & level0 = this->m_OutputLevelSet->GetLayer( 0 );

  // neighborhood iterator
  ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

  typename NeighborhoodIteratorType::RadiusType radius;
  radius.Fill( 1 );

  NeighborhoodIteratorType neighIt( radius,
                                    this->m_InternalImage,
                                    this->m_InternalImage->GetLargestPossibleRegion() );

  neighIt.OverrideBoundaryCondition( &sp_nbc );

  typename NeighborhoodIteratorType::OffsetType sparse_offset;
  sparse_offset.Fill( 0 );

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    sparse_offset[dim] = -1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 0;
    }

  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  LevelSetLayerType InsertList;

  LevelSetLayerIterator nodeIt = level0.begin();
  LevelSetLayerIterator nodeEnd = level0.end();

  LevelSetLayerIterator upIt = this->m_Update.begin();

  while( nodeIt != nodeEnd )
    {
    const LevelSetInputType currentIdx = nodeIt->first;

    assert( currentIdx == upIt->first );

    const LevelSetOutputType update = upIt->second;

    if( update != NumericTraits< LevelSetOutputType >::Zero )
      {
      oldValue = 0;

      if( update > NumericTraits< LevelSetOutputType >::Zero )
        {
        newValue = 1;
        }
      else // if( update < NumericTraits< LevelSetOutputRealType >::Zero )
        {
        newValue = -1;
        }

      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      ++upIt;
      level0.erase( tempIt );

      this->m_InternalImage->SetPixel( currentIdx, newValue );
      termContainer->UpdatePixel( currentIdx, oldValue, newValue );

      neighIt.SetLocation( currentIdx );

      for( typename NeighborhoodIteratorType::Iterator
           i = neighIt.Begin();
           !i.IsAtEnd(); ++i )
        {
        LevelSetOutputType tempValue = i.Get();
        if( tempValue * newValue == -1 )
          {
          LevelSetInputType tempIndex =
              neighIt.GetIndex( i.GetNeighborhoodOffset() );

          InsertList.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, tempValue ) );
          }
        }
      }
    else
      {
      ++nodeIt;
      ++upIt;
      }
    }

  nodeIt = InsertList.begin();
  nodeEnd = InsertList.end();
  while( nodeIt != nodeEnd )
    {
    level0.insert(
          std::pair< LevelSetInputType, LevelSetOutputType >( nodeIt->first, 0 ) );

    this->m_InternalImage->SetPixel( nodeIt->first, 0 );
    termContainer->UpdatePixel( nodeIt->first, nodeIt->second, 0 );
    ++nodeIt;
    }
}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::EvolveWithPhasedPropagation( LevelSetLayerType& ioList,
                        LevelSetLayerType& ioUpdate,
                        const bool& iContraction )
  {
  assert( ioList.size() == ioUpdate.size() );

  ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

  typename NeighborhoodIteratorType::RadiusType radius;
  radius.Fill( 1 );

  NeighborhoodIteratorType neighIt( radius,
                                    this->m_InternalImage,
                                    this->m_InternalImage->GetLargestPossibleRegion() );

  neighIt.OverrideBoundaryCondition( &sp_nbc );

  typename NeighborhoodIteratorType::OffsetType sparse_offset;
  sparse_offset.Fill( 0 );

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    sparse_offset[dim] = -1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 0;
    }

  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  LevelSetLayerType InsertList;

  LevelSetLayerIterator nodeIt = ioList.begin();
  LevelSetLayerIterator nodeEnd = ioList.end();

  LevelSetLayerIterator upIt = ioUpdate.begin();

  while( nodeIt != nodeEnd )
    {
    assert( nodeIt->first == upIt->first );

    LevelSetOutputType oldValue = 0;
    LevelSetOutputType newValue;

    LevelSetOutputType update = upIt->second;
    LevelSetInputType currentIdx = nodeIt->first;

    if( update != NumericTraits< LevelSetOutputRealType >::Zero )
      {
      // only allow positive forces
      iContraction ? newValue = 1 : newValue = -1;
      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      ++upIt;
      ioList.erase( tempIt );
      this->m_OutputLevelSet->GetLayer( 0 ).erase( currentIdx );

      this->m_InternalImage->SetPixel( currentIdx, newValue );
      this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId )->UpdatePixel(
        currentIdx, oldValue , newValue );

      neighIt.SetLocation( currentIdx );

      for( typename NeighborhoodIteratorType::Iterator
          i = neighIt.Begin();
          !i.IsAtEnd(); ++i )
        {
        LevelSetOutputType tempValue = i.Get();

        if( tempValue * newValue == -1 )
          {
          LevelSetInputType tempIdx =
            neighIt.GetIndex( i.GetNeighborhoodOffset() );

          InsertList.insert(
            std::pair< LevelSetInputType, LevelSetOutputType >( tempIdx, tempValue ) );
            }
          }
        }
      else
        {
        ++nodeIt;
        ++upIt;
        }
      }

  nodeIt = InsertList.begin();
  nodeEnd = InsertList.end();

  while( nodeIt != nodeEnd )
    {
    this->m_OutputLevelSet->GetLayer( 0 ).insert(
          std::pair< LevelSetInputType, LevelSetOutputType >( nodeIt->first, 0 ) );

    termContainer->UpdatePixel( nodeIt->first, nodeIt->second, 0 );
    this->m_InternalImage->SetPixel( nodeIt->first, 0 );

    ++nodeIt;
    }
  }

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateMalcolmSparseLevelSet< VDimension, TEquationContainer >
::CompactLayersToSinglePixelThickness()
{
  LevelSetOutputRealType oldValue, newValue;
  LevelSetLayerType & list_0 = this->m_OutputLevelSet->GetLayer( 0 );

  ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

  typename NeighborhoodIteratorType::RadiusType radius;
  radius.Fill( 1 );

  NeighborhoodIteratorType neighIt( radius,
                                    this->m_InternalImage,
                                    this->m_InternalImage->GetLargestPossibleRegion() );

  neighIt.OverrideBoundaryCondition( &sp_nbc );

  typename NeighborhoodIteratorType::OffsetType sparse_offset;
  sparse_offset.Fill( 0 );

  for( unsigned int dim = 0; dim < ImageDimension; dim++ )
    {
    sparse_offset[dim] = -1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 1;
    neighIt.ActivateOffset( sparse_offset );
    sparse_offset[dim] = 0;
    }

  LevelSetLayerIterator nodeIt   = list_0.begin();
  LevelSetLayerIterator nodeEnd  = list_0.end();

  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  while( nodeIt != nodeEnd )
    {
    LevelSetInputType currentIdx = nodeIt->first;

    neighIt.SetLocation( currentIdx );

    bool positive = false;
    bool negative = false;

    oldValue = 0;

    for( typename NeighborhoodIteratorType::Iterator
        i = neighIt.Begin();
        !i.IsAtEnd(); ++i )
      {
      LevelSetOutputType tempValue = i.Get();
      if( tempValue == -1 )
        {
        negative = true;
        }
      if ( tempValue == 1 )
        {
        positive = true;
        }
      }

    if( negative && !positive )
      {
      newValue = -1;
      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      list_0.erase( tempIt );

      this->m_InternalImage->SetPixel( currentIdx, newValue );
      termContainer->UpdatePixel( currentIdx, oldValue , newValue );
      }
    else if( positive && !negative )
      {
      newValue = 1;
      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      list_0.erase( tempIt );

      this->m_InternalImage->SetPixel( currentIdx, newValue );

      termContainer->UpdatePixel( currentIdx, oldValue , newValue );
      }
    else if( !positive && !negative )
      {
      ++nodeIt;
      }
    else
      {
      ++nodeIt;
      }
    }
}

}
#endif // __itkUpdateMalcolmSparseLevelSet_hxx
