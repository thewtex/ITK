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
BinaryImageToShiSparseLevelSetAdaptor< TInputImage >
::BinaryImageToShiSparseLevelSetAdaptor() : m_InputImage( NULL )
  {
  this->m_SparseLevelSet = LevelSetType::New();
  }

template< class TInputImage >
void BinaryImageToShiSparseLevelSetAdaptor< TInputImage >
::Initialize()
{
  if( m_InputImage.IsNull() )
    {
    itkGenericExceptionMacro( << "m_InputImage is NULL" );
    }

  m_LabelMap = LevelSetLabelMapType::New();
  m_LabelMap->SetBackgroundValue( LevelSetType::PlusThreeLayer() );
  m_LabelMap->CopyInformation( m_InputImage );

  m_InternalImage = InternalImageType::New();
  m_InternalImage->CopyInformation( m_InputImage );
  m_InternalImage->SetRegions( m_InputImage->GetBufferedRegion() );
  m_InternalImage->Allocate();
  m_InternalImage->FillBuffer( LevelSetType::PlusThreeLayer() );

  LevelSetLabelObjectPointer innerPart = LevelSetLabelObjectType::New();
  innerPart->SetLabel( LevelSetType::MinusThreeLayer() );

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
      innerPart->AddIndex( iIt.GetIndex() );
      labelIt.Set( LevelSetType::MinusThreeLayer() );
      }
    ++labelIt;
    ++iIt;
    }

  innerPart->Optimize();
  m_LabelMap->AddLabelObject( innerPart );

  FindActiveLayer();

  m_SparseLevelSet->SetLabelMap( m_LabelMap );
  m_InternalImage = NULL;
}


template< class TInputImage >
void BinaryImageToShiSparseLevelSetAdaptor< TInputImage >::
FindActiveLayer()
{
  LevelSetLabelObjectPointer labelObject = m_LabelMap->GetLabelObject( LevelSetType::MinusThreeLayer() );

  LevelSetLayerType& layerMinus1 =
    m_SparseLevelSet->GetLayer( LevelSetType::MinusOneLayer() );
  LevelSetLayerType& layerPlus1 =
    m_SparseLevelSet->GetLayer( LevelSetType::PlusOneLayer() );

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
         !it.IsAtEnd(); ++it )
      {
      if( it.Get() == LevelSetType::PlusThreeLayer() )
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
  ObjectMinus1->SetLabel( LevelSetType::MinusOneLayer() );

  LevelSetLayerConstIterator nodeIt = layerMinus1.begin();
  LevelSetLayerConstIterator nodeEnd = layerMinus1.end();

  while( nodeIt != nodeEnd )
    {
    m_LabelMap->GetLabelObject( LevelSetType::MinusThreeLayer() )->RemoveIndex( nodeIt->first );
    ObjectMinus1->AddIndex( nodeIt->first );
    ++nodeIt;
    }

  ObjectMinus1->Optimize();
  m_LabelMap->AddLabelObject( ObjectMinus1 );

  LevelSetLabelObjectPointer ObjectPlus1 = LevelSetLabelObjectType::New();
  ObjectPlus1->SetLabel( LevelSetType::PlusOneLayer() );

  nodeIt = layerPlus1.begin();
  nodeEnd = layerPlus1.end();

  while( nodeIt != nodeEnd )
    {
    ObjectPlus1->AddIndex( nodeIt->first );
    ++nodeIt;
    }

  ObjectPlus1->Optimize();
  m_LabelMap->AddLabelObject( ObjectPlus1 );
}
}

#endif // __itkBinaryImageToShiSparseLevelSetAdaptor_h
