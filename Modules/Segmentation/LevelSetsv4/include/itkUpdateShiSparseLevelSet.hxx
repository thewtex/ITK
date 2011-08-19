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

#ifndef __itkUpdateShiSparseLevelSet_hxx
#define __itkUpdateShiSparseLevelSet_hxx

#include "itkUpdateShiSparseLevelSet.h"

namespace itk
{
template< unsigned int VDimension,
          class TEquationContainer >
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::UpdateShiSparseLevelSet() :
  m_RMSChangeAccumulator( NumericTraits< LevelSetOutputRealType >::Zero )
{
  this->m_OutputLevelSet = LevelSetType::New();
}

template< unsigned int VDimension,
          class TEquationContainer >
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::~UpdateShiSparseLevelSet()
{}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::Update()
{
  if( this->m_InputLevelSet.IsNull() )
    {
    itkGenericExceptionMacro( <<"m_InputLevelSet is NULL" );
    }

  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  this->m_OutputLevelSet->SetLayer( -1, this->m_InputLevelSet->GetLayer( -1 ) );
  this->m_OutputLevelSet->SetLayer(  1, this->m_InputLevelSet->GetLayer(  1 ) );

  this->m_OutputLevelSet->SetLabelMap( this->m_InputLevelSet->GetLabelMap() );

  typedef LabelMapToLabelImageFilter<LevelSetLabelMapType, LabelImageType> LabelMapToLabelImageFilterType;
  typename LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
  labelMapToLabelImageFilter->SetInput( this->m_InputLevelSet->GetLabelMap() );
  labelMapToLabelImageFilter->Update();

  this->m_InternalImage = labelMapToLabelImageFilter->GetOutput();
  this->m_InternalImage->DisconnectPipeline();

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

  // Step 2.1.1
  UpdateLout();

 // Step 2.1.2 - for each point x in L_out
  LevelSetLayerType & list_in = this->m_OutputLevelSet->GetLayer( -1 );

  LevelSetLayerIterator nodeIt = list_in.begin();
  LevelSetLayerIterator nodeEnd = list_in.end();

  while( nodeIt != nodeEnd )
    {
    LevelSetInputType currentIndex = nodeIt->first;

    neighIt.SetLocation( currentIndex );

    bool to_be_deleted = true;

    for( typename NeighborhoodIteratorType::Iterator
         i = neighIt.Begin();
         !i.IsAtEnd(); ++i )
      {
      LevelSetOutputType tempValue = i.Get();
      if ( tempValue > NumericTraits< LevelSetOutputType >::Zero )
        {
        to_be_deleted = false;
        break;
        }
      }
    if( to_be_deleted )
      {
//         std::cout << p.first << std::endl;
      LevelSetOutputType oldValue = -1;
      LevelSetOutputType newValue = -3;
      this->m_InternalImage->SetPixel( currentIndex, newValue );

      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      list_in.erase( tempIt );

      termContainer->UpdatePixel( currentIndex, oldValue, newValue );
      }
    else
      {
      ++nodeIt;
      }
    }

//     Step 2.1.3 - for each point x in L_in
  UpdateLin();

//     Step 2.1.4
  LevelSetLayerType & list_out = this->m_OutputLevelSet->GetLayer( 1 );

  nodeIt = list_out.begin();
  nodeEnd = list_out.end();

  while( nodeIt != nodeEnd )
    {
    LevelSetInputType currentIndex = nodeIt->first;

    neighIt.SetLocation( currentIndex );

    bool to_be_deleted = true;

    for( typename NeighborhoodIteratorType::Iterator
            i = neighIt.Begin();
        !i.IsAtEnd(); ++i )
      {
      LevelSetOutputType tempValue = i.Get();
      if ( tempValue < NumericTraits< LevelSetOutputType >::Zero )
        {
        to_be_deleted = false;
        break;
        }
      }
    if( to_be_deleted )
      {
      LevelSetOutputType oldValue = 1;
      LevelSetOutputType newValue = 3;
      this->m_InternalImage->SetPixel( currentIndex, newValue );

      LevelSetLayerIterator tempIt = nodeIt;
      ++nodeIt;
      list_out.erase( tempIt );

      termContainer->UpdatePixel( currentIndex, oldValue, newValue );
      }
    else
      {
      ++nodeIt;
      }
    }

  typedef LabelImageToLabelMapFilter< LabelImageType, LevelSetLabelMapType> LabelImageToLabelMapFilterType;
  typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
  labelImageToLabelMapFilter->SetInput( this->m_InternalImage );
  labelImageToLabelMapFilter->SetBackgroundValue( 3 );
  labelImageToLabelMapFilter->Update();

  LevelSetLabelMapPointer outputLabelMap = this->m_OutputLevelSet->GetLabelMap( );
  outputLabelMap->Graft( labelImageToLabelMapFilter->GetOutput() );
}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::UpdateLout()
{
  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  LevelSetLayerType & list_out  = this->m_OutputLevelSet->GetLayer(  1 );
  LevelSetLayerType & list_in   = this->m_OutputLevelSet->GetLayer( -1 );

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

  LevelSetLayerType insertListIn;
  LevelSetLayerType insertListOut;

  LevelSetLayerIterator nodeIt   = list_out.begin();
  LevelSetLayerIterator nodeEnd  = list_out.end();

  // for each point in Lz
  while( nodeIt != nodeEnd )
    {
    bool erased = false;
    const LevelSetInputType   currentIndex = nodeIt->first;
    const LevelSetOutputType  currentValue = nodeIt->second;

    // update the level set
    LevelSetOutputRealType update = termContainer->Evaluate( currentIndex );

    if( update < NumericTraits< LevelSetOutputRealType >::Zero )
      {
      if( Con( currentIndex, currentValue , update ) )
        {
        // CheckIn
        insertListIn.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIndex, -1 ) );

        LevelSetLayerIterator tempIt = nodeIt;
        ++nodeIt;
        list_out.erase( tempIt );
        erased = true;

        neighIt.SetLocation( currentIndex );

        for( typename NeighborhoodIteratorType::Iterator
            i = neighIt.Begin();
            !i.IsAtEnd(); ++i )
          {
          LevelSetOutputType tempValue = i.Get();

          if ( tempValue == 3 )
            {
            LevelSetInputType tempIndex =
                neighIt.GetIndex( i.GetNeighborhoodOffset() );

            insertListOut.insert(
                  std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, 1 ) );
            }
          }
        }
      }
    if( !erased )
      {
      ++nodeIt;
      }
    }

  nodeIt   = insertListOut.begin();
  nodeEnd  = insertListOut.end();

  // for each point in Lz
  while( nodeIt != nodeEnd )
    {
    list_out.insert( *nodeIt );

    this->m_InternalImage->SetPixel( nodeIt->first, 1 );
    termContainer->UpdatePixel( nodeIt->first, 3, 1 );

    ++nodeIt;
    }

  nodeIt = insertListIn.begin();
  nodeEnd = insertListIn.end();

  while( nodeIt != nodeEnd )
    {
    list_in.insert( *nodeIt );

    this->m_InternalImage->SetPixel( nodeIt->first, -1 );
    termContainer->UpdatePixel( nodeIt->first, 1, -1 );
    ++nodeIt;
    }
}

template< unsigned int VDimension,
          class TEquationContainer >
void
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::UpdateLin()
{
  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

  LevelSetLayerType & list_out  = this->m_OutputLevelSet->GetLayer(  1 );
  LevelSetLayerType & list_in   = this->m_OutputLevelSet->GetLayer( -1 );

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

  LevelSetLayerType insertListIn;
  LevelSetLayerType insertListOut;

  LevelSetLayerIterator nodeIt   = list_in.begin();
  LevelSetLayerIterator nodeEnd  = list_in.end();

  // for each point in Lz
  while( nodeIt != nodeEnd )
    {
    bool erased = false;
    const LevelSetInputType   currentIndex = nodeIt->first;
    const LevelSetOutputType  currentValue = nodeIt->second;

    // update for the current level set
    LevelSetOutputRealType update = termContainer->Evaluate( currentIndex );

    if( update > NumericTraits< LevelSetOutputRealType >::Zero )
      {
      if( Con( currentIndex, currentValue , update ) )
        {
        // CheckOut
        insertListOut.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( currentIndex, 1 ) );

        LevelSetLayerIterator tempIt = nodeIt;
        ++nodeIt;
        list_in.erase( tempIt );

        erased = true;

        neighIt.SetLocation( currentIndex );

        for( typename NeighborhoodIteratorType::Iterator
            i = neighIt.Begin();
            !i.IsAtEnd(); ++i )
          {
          LevelSetOutputType tempValue = i.Get();

          if ( tempValue == -3 )
            {
            LevelSetInputType tempIndex =
                neighIt.GetIndex( i.GetNeighborhoodOffset() );

            insertListIn.insert(
                  std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, -1 ) );
            }
          }
        }
      }
    if( !erased )
      {
      ++nodeIt;
      }
    }

  nodeIt   = insertListIn.begin();
  nodeEnd  = insertListIn.end();

  // for each point in insertListIn
  while( nodeIt != nodeEnd )
    {
    list_in.insert( *nodeIt );
    this->m_InternalImage->SetPixel( nodeIt->first, -1 );
    termContainer->UpdatePixel( nodeIt->first, -3, -1 );
    ++nodeIt;
    }

  nodeIt   = insertListOut.begin();
  nodeEnd  = insertListOut.end();

  // for each point in insertListOut
  while( nodeIt != nodeEnd )
    {
    list_out.insert( *nodeIt );
    this->m_InternalImage->SetPixel( nodeIt->first, 1 );
    termContainer->UpdatePixel( nodeIt->first, -1, 1 );
    ++nodeIt;
    }
}

template< unsigned int VDimension,
          class TEquationContainer >
bool
UpdateShiSparseLevelSet< VDimension, TEquationContainer >
::Con( const LevelSetInputType& iIdx,
            const LevelSetOutputType& iCurrentStatus,
            const LevelSetOutputRealType& iCurrentUpdate ) const
{
  TermContainerPointer termContainer = this->m_EquationContainer->GetEquation( this->m_CurrentLevelSetId );

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

  neighIt.SetLocation( iIdx );

  const LevelSetOutputType opposite_status = ( iCurrentStatus == 1 ) ? -1 : 1;

  for( typename NeighborhoodIteratorType::Iterator
            i = neighIt.Begin();
        !i.IsAtEnd(); ++i )
    {
    LevelSetOutputType tempValue = i.Get();

    if ( tempValue == opposite_status )
      {
      LevelSetInputType tempIdx =
          neighIt.GetIndex( i.GetNeighborhoodOffset() );

      LevelSetOutputRealType neighborUpdate = termContainer->Evaluate( tempIdx );

      if ( neighborUpdate * iCurrentUpdate > NumericTraits< LevelSetOutputType >::Zero )
        {
        return true;
        }
      }
    }
 return false;
}
}
#endif // __itkUpdateShiSparseLevelSet_hxx
