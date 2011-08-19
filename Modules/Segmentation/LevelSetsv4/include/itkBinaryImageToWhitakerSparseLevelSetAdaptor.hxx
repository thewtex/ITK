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

#ifndef __itkBinaryImageToWhitakerSparseLevelSetAdaptor_hxx
#define __itkBinaryImageToWhitakerSparseLevelSetAdaptor_hxx

#include "itkBinaryImageToWhitakerSparseLevelSetAdaptor.h"


namespace itk
{
template< class TInputImage, typename TLevelSetValueType >
void BinaryImageToWhitakerSparseLevelSetAdaptor< TInputImage, TLevelSetValueType >
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

    FindPlusOneMinusOneLayer();

    PropagateToOutterLayers(-1, -2, -3 );
    PropagateToOutterLayers( 1,  2,  3 );

    m_LabelMap->Optimize();

    m_SparseLevelSet->SetLabelMap( m_LabelMap );

    // release the memory
    m_InternalImage = 0;
  }

template< class TInputImage, typename TLevelSetValueType >
void BinaryImageToWhitakerSparseLevelSetAdaptor< TInputImage, TLevelSetValueType >
::PropagateToOutterLayers( char LayerToBeScanned, char OutputLayer, char TestValue )
  {
    const LevelSetLayerType layerPlus1 = m_SparseLevelSet->GetLayer( LayerToBeScanned );

    LevelSetLayerType& layerPlus2 = m_SparseLevelSet->GetLayer( OutputLayer );
    const LevelSetOutputType plus2 = static_cast< LevelSetOutputType >( OutputLayer );

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


    // iterate on the layer to be scanned
    LevelSetLayerConstIterator nodeIt = layerPlus1.begin();
    LevelSetLayerConstIterator nodeEnd = layerPlus1.end();

    while( nodeIt != nodeEnd )
    {
      LevelSetInputType idx = nodeIt->first;
      neighIt.SetLocation( idx );

      for( typename NeighborhoodIteratorType::Iterator it = neighIt.Begin();
           !it.IsAtEnd();
           ++it )
        {
        // check if in the neighborhood there are values equal to TestValue
        if( it.Get() == TestValue )
          {
          LevelSetInputType tempIndex =
              neighIt.GetIndex( it.GetNeighborhoodOffset() );

          layerPlus2.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, plus2 ) );
          }
        }
      ++nodeIt;
    }

    LevelSetLabelObjectPointer ObjectPlus2 = LevelSetLabelObjectType::New();
    ObjectPlus2->SetLabel( int(OutputLayer) );

    nodeIt = layerPlus2.begin();
    nodeEnd = layerPlus2.end();

    while( nodeIt != nodeEnd )
      {
      if ( TestValue != m_LabelMap->GetBackgroundValue() )
        {
        m_LabelMap->GetLabelObject( TestValue )->RemoveIndex( nodeIt->first );
        }
      ObjectPlus2->AddIndex( nodeIt->first );
      m_InternalImage->SetPixel( nodeIt->first, OutputLayer );
      ++nodeIt;
      }

    ObjectPlus2->Optimize();
    m_LabelMap->AddLabelObject( ObjectPlus2 );
  }

template< class TInputImage, typename TLevelSetValueType >
void BinaryImageToWhitakerSparseLevelSetAdaptor< TInputImage, TLevelSetValueType >
::FindActiveLayer()
  {
    LevelSetLabelObjectPointer labelObject = m_LabelMap->GetLabelObject( -3 );

    const LevelSetOutputType zero = NumericTraits< LevelSetOutputType >::Zero;

    LevelSetLayerType& layer0 = m_SparseLevelSet->GetLayer( static_cast< char >( 0 ) );

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

      for( typename NeighborhoodIteratorType::Iterator it = neighIt.Begin();
           !it.IsAtEnd();
           ++it )
        {
        if( it.Get() == static_cast< char >( 3 ) )
          {
          layer0.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( idx, zero ) );
          break;
          }
        }

      ++lineIt;
      }

    if( !layer0.empty() )
      {
      LevelSetLabelObjectPointer ZeroSet = LevelSetLabelObjectType::New();
      ZeroSet->SetLabel( static_cast< char >( 0 ) );

      LevelSetLayerConstIterator nodeIt = layer0.begin();
      LevelSetLayerConstIterator nodeEnd = layer0.end();

      while( nodeIt != nodeEnd )
        {
        m_LabelMap->GetLabelObject( static_cast< char >( -3 ) )->RemoveIndex( nodeIt->first );
        ZeroSet->AddIndex( nodeIt->first );
        m_InternalImage->SetPixel( nodeIt->first, 0 );
        ++nodeIt;
        }

      ZeroSet->Optimize();
      m_LabelMap->AddLabelObject( ZeroSet );
      }
  }

template< class TInputImage, typename TLevelSetValueType >
void BinaryImageToWhitakerSparseLevelSetAdaptor< TInputImage, TLevelSetValueType >
::FindPlusOneMinusOneLayer()
  {
    const LevelSetOutputType minus1 = - NumericTraits< LevelSetOutputType >::One;
    const LevelSetOutputType plus1 = NumericTraits< LevelSetOutputType >::One;

    const LevelSetLayerType layer0 = m_SparseLevelSet->GetLayer( static_cast< char >( 0 ) );
    LevelSetLayerType& layerMinus1 = m_SparseLevelSet->GetLayer( static_cast< char >( -1 ) );
    LevelSetLayerType& layerPlus1 = m_SparseLevelSet->GetLayer( static_cast< char >( 1 ) );

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

    LevelSetLayerConstIterator nodeIt   = layer0.begin();
    LevelSetLayerConstIterator nodeEnd  = layer0.end();

    while( nodeIt != nodeEnd )
      {
      LevelSetInputType idx = nodeIt->first;
      neighIt.SetLocation( idx );

      for( typename NeighborhoodIteratorType::Iterator it = neighIt.Begin();
           !it.IsAtEnd();
           ++it )
        {
        if( it.Get() == static_cast< char >( -3 ) )
          {
          LevelSetInputType tempIndex =
              neighIt.GetIndex( it.GetNeighborhoodOffset() );

          layerMinus1.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, minus1 ) );
          }
        if( it.Get() == static_cast< char >( 3 ) )
          {
          LevelSetInputType tempIndex =
              neighIt.GetIndex( it.GetNeighborhoodOffset() );

          layerPlus1.insert(
                std::pair< LevelSetInputType, LevelSetOutputType >( tempIndex, plus1 ) );
          }
        }
      ++nodeIt;
      }

    LevelSetLabelObjectPointer ObjectMinus1 = LevelSetLabelObjectType::New();
    ObjectMinus1->SetLabel( static_cast< char >( -1 ) );

    nodeIt = layerMinus1.begin();
    nodeEnd = layerMinus1.end();

    while( nodeIt != nodeEnd )
      {
      m_LabelMap->GetLabelObject( static_cast< char >( -3 ) )->RemoveIndex( nodeIt->first );
      ObjectMinus1->AddIndex( nodeIt->first );
      m_InternalImage->SetPixel( nodeIt->first, static_cast< char >( -1 ) );
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
      ObjectPlus1->AddIndex( nodeIt->first );
      m_InternalImage->SetPixel( nodeIt->first, static_cast< char >( 1 ) );
      ++nodeIt;
      }

    ObjectPlus1->Optimize();
    m_LabelMap->AddLabelObject( ObjectPlus1 );
  }
}

#endif // __itkBinaryImageToWhitakerSparseLevelSetAdaptor_hxx
