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

#ifndef __itkBinaryImageToShiSparseLevelSetAdaptor_hxx
#define __itkBinaryImageToShiSparseLevelSetAdaptor_hxx

#include "itkBinaryImageToShiSparseLevelSetAdaptor.h"


namespace itk
{
template< class TInputImage >
void BinaryImageToShiSparseLevelSetAdaptor< TInputImage >
::Initialize()
  {
    m_LabelMap = LevelSetLabelMapType::New();
    m_LabelMap->SetBackgroundValue( static_cast< char >( 3 ) );
    m_LabelMap->CopyInformation( m_InputImage );

    m_InternalImage = InternalImageType::New();
    m_InternalImage->CopyInformation( m_InputImage );
    m_InternalImage->SetRegions( m_InputImage->GetBufferedRegion() );
    m_InternalImage->Allocate();
    m_InternalImage->FillBuffer( static_cast< char >( 3 ) );

    LevelSetLabelObjectPointer InnerPart = LevelSetLabelObjectType::New();
    InnerPart->SetLabel( static_cast< char >( -3 ) );

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
        labelIt.Set( static_cast< char >( -3 ) );
        }
      ++labelIt;
      ++iIt;
      }

    InnerPart->Optimize();
    m_LabelMap->AddLabelObject( InnerPart );

    FindActiveLayer();

    m_SparseLevelSet->SetLabelMap( m_LabelMap );
    m_InternalImage = 0;
  }


template< class TInputImage >
void BinaryImageToShiSparseLevelSetAdaptor< TInputImage >::
FindActiveLayer()
  {
    LevelSetLabelObjectPointer labelObject = m_LabelMap->GetLabelObject( -3 );

    LevelSetLayerType& layerMinus1 =
        m_SparseLevelSet->GetLayer( static_cast< char >( -1 ) );
    LevelSetLayerType& layerPlus1 =
        m_SparseLevelSet->GetLayer( static_cast< char >(  1 ) );

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

      bool boundary = false;

      for( typename NeighborhoodIteratorType::Iterator it = neighIt.Begin();
           !it.IsAtEnd();
           ++it )
        {
        if( it.Get() == static_cast< char >( 3 ) )
          {
          LevelSetInputType tempIndex =
              neighIt.GetIndex( it.GetNeighborhoodOffset() );

          layerPlus1.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, 1 ) );
          boundary = true;
          }
        }

      if( boundary )
        {
        layerMinus1.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( idx, -1 ) );
        }

      ++lineIt;
      }

    LevelSetLabelObjectPointer ObjectMinus1 = LevelSetLabelObjectType::New();
    ObjectMinus1->SetLabel( static_cast< char >( -1 ) );

    LevelSetLayerConstIterator nodeIt = layerMinus1.begin();
    LevelSetLayerConstIterator nodeEnd = layerMinus1.end();

    while( nodeIt != nodeEnd )
      {
      m_LabelMap->GetLabelObject( static_cast< char >( -3 ) )->RemoveIndex( nodeIt->first );
      ObjectMinus1->AddIndex( nodeIt->first );
      ++nodeIt;
      }

    ObjectMinus1->Optimize();
    m_LabelMap->AddLabelObject( ObjectMinus1 );

    LevelSetLabelObjectPointer ObjectPlus1 = LevelSetLabelObjectType::New();
    ObjectPlus1->SetLabel( static_cast< char >( 1 ) );

    nodeIt = layerPlus1.begin();
    nodeEnd = layerPlus1.end();

    while( nodeIt != nodeEnd )
      {
//      m_LabelMap->GetLabelObject( static_cast< char >( 3 ) )->RemoveIndex( nodeIt->first );
      ObjectPlus1->AddIndex( nodeIt->first );
      ++nodeIt;
      }

    ObjectPlus1->Optimize();
    m_LabelMap->AddLabelObject( ObjectPlus1 );
  }
}

#endif // __itkBinaryImageToShiSparseLevelSetAdaptor_h
