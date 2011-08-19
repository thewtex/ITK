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

#ifndef __itkBinaryImageToMalcolmSparseLevelSetAdaptor_hxx
#define __itkBinaryImageToMalcolmSparseLevelSetAdaptor_hxx

#include "itkBinaryImageToMalcolmSparseLevelSetAdaptor.h"

namespace itk
{
template< class TInputImage >
void BinaryImageToMalcolmSparseLevelSetAdaptor< TInputImage >
::Initialize()
  {
    m_LabelMap = LevelSetLabelMapType::New();
    m_LabelMap->SetBackgroundValue( static_cast< char >( 1 ) );
    m_LabelMap->CopyInformation( m_InputImage );

    m_InternalImage = InternalImageType::New();
    m_InternalImage->CopyInformation( m_InputImage );
    m_InternalImage->SetRegions( m_InputImage->GetBufferedRegion() );
    m_InternalImage->Allocate();
    m_InternalImage->FillBuffer( static_cast< char >( 1 ) );

    LevelSetLabelObjectPointer InnerPart = LevelSetLabelObjectType::New();
    InnerPart->SetLabel( static_cast< char >( -1 ) );

    // Precondition labelmap and phi
    InputIteratorType iIt( m_InputImage, m_InputImage->GetLargestPossibleRegion() );
    iIt.GoToBegin();

    InternalIteratorType labelIt( m_InternalImage,
                                  m_InternalImage->GetLargestPossibleRegion() );
    labelIt.GoToBegin();

    while( !iIt.IsAtEnd() )
      {
      if ( iIt.Get() != NumericTraits< InputImagePixelType >::Zero )
        {
        InnerPart->AddIndex( iIt.GetIndex() );
        labelIt.Set( static_cast< char >( -1 ) );
        }
      ++labelIt;
      ++iIt;
      }

    InnerPart->Optimize();
    m_LabelMap->AddLabelObject( InnerPart );

    FindActiveLayer();

    MinimalInterface();

    m_SparseLevelSet->SetLabelMap( m_LabelMap );
    m_InternalImage = 0;
  }

template< class TInputImage >
void BinaryImageToMalcolmSparseLevelSetAdaptor< TInputImage >
::FindActiveLayer()
  {
    LevelSetLabelObjectPointer labelObject = m_LabelMap->GetLabelObject( -1 );

    LevelSetLayerType & layer = m_SparseLevelSet->GetLayer( 0 );

    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill( 1 );

    ZeroFluxNeumannBoundaryCondition< InternalImageType > im_nbc;

    NeighborhoodIteratorType neighIt( radius,
                                      m_InternalImage,
                                      m_InternalImage->GetLargestPossibleRegion() );

    neighIt.OverrideBoundaryCondition( &im_nbc );

    typename NeighborhoodIteratorType::OffsetType neighOffset;
    neighOffset.Fill( 0 );

    for( unsigned int dim = 0; dim < ImageDimension; dim++ )
      {
      neighOffset[dim] = -1;
      neighIt.ActivateOffset( neighOffset );
      neighOffset[dim] = 1;
      neighIt.ActivateOffset( neighOffset );
      neighOffset[dim] = 0;
      }

    typename LevelSetLabelObjectType::ConstIndexIterator
        lineIt( labelObject );
    lineIt.GoToBegin();

    while( !lineIt.IsAtEnd() )
      {
      const LevelSetInputType & idx = lineIt.GetIndex();

      neighIt.SetLocation( idx );

      bool ZeroSet = false;

      for( typename NeighborhoodIteratorType::Iterator it = neighIt.Begin();
           !it.IsAtEnd();
           ++it )
        {
        if( it.Get() == static_cast< char >( 1 ) )
          {
          ZeroSet = true;
          }
        }

      if( ZeroSet )
        {
        layer.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( idx, 0 ) );
        m_InternalImage->SetPixel( idx, 0 );
        }

      ++lineIt;
      }

    LevelSetLabelObjectPointer ObjectZero = LevelSetLabelObjectType::New();
    ObjectZero->SetLabel( static_cast< char >( 0 ) );

    LevelSetLayerIterator nodeIt = layer.begin();
    LevelSetLayerIterator nodeEnd = layer.end();

    while( nodeIt != nodeEnd )
      {
      labelObject->RemoveIndex( nodeIt->first );
      ObjectZero->AddIndex( nodeIt->first );
      ++nodeIt;
      }

    ObjectZero->Optimize();
    m_LabelMap->AddLabelObject( ObjectZero );
  }

template< class TInputImage >
void BinaryImageToMalcolmSparseLevelSetAdaptor< TInputImage >
::MinimalInterface()
    {
    LevelSetOutputRealType oldValue, newValue;
    LevelSetLayerType & list_0 = m_SparseLevelSet->GetLayer( 0 );

    ZeroFluxNeumannBoundaryCondition< InternalImageType > sp_nbc;

    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill( 1 );

    NeighborhoodIteratorType neighIt( radius,
                                      m_InternalImage,
                                      m_InternalImage->GetLargestPossibleRegion() );

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
        char tempValue = i.Get();

        if( tempValue != NumericTraits< LevelSetOutputType >::Zero )
          {
          if( tempValue == -1 )
            {
            negative = true;
            if( positive )
              {
              break;
              }
            }
          else // ( tempValue == 1 )
            {
            positive = true;
            if( negative )
              {
              break;
              }
            }
          }
        }
      if( negative && !positive )
        {
        newValue = -1;
        LevelSetLayerIterator tempIt = nodeIt;
        ++nodeIt;
        list_0.erase( tempIt );

        m_LabelMap->GetLabelObject( 0 )->RemoveIndex( tempIt->first );
        m_LabelMap->GetLabelObject( -1 )->AddIndex( tempIt->first );
//        m_InternalImage->SetPixel( currentIdx, newValue );
        }
      else
        {
        if( positive && !negative )
          {
          newValue = 1;
          LevelSetLayerIterator tempIt = nodeIt;
          ++nodeIt;
          list_0.erase( tempIt );

          m_LabelMap->GetLabelObject( 0 )->RemoveIndex( tempIt->first );
//          m_LabelMap->GetLabelObject( 1 )->AddIndex( tempIt->first );
          }
        else
          {
          ++nodeIt;
          }
        }
      }
    }
}
#endif // __itkBinaryImageToMalcolmSparseLevelSetAdaptor_hxx
