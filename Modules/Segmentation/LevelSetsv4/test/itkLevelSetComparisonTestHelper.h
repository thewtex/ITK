/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License; idx[1] = Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing; idx[1] = software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND; idx[1] = either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef __itkLevelSetComparisonTestHelper_h
#define __itkLevelSetComparisonTestHelper_h

#include "itkLevelSetImageBase.h"
#include "itkWhitakerSparseLevelSetBase.h"
#include "itkShiSparseLevelSetBase.h"
#include "itkMalcolmSparseLevelSetBase.h"

namespace itk
{
template< class TImage >
bool CompareDenseLevelSets( typename LevelSetImageBase< TImage >::Pointer iPhi1,
                            typename LevelSetImageBase< TImage >::Pointer iPhi2,
                            const typename TImage::PixelType& iTol )
{
  typedef ImageRegionConstIteratorWithIndex< TImage > IteratorType;

  typename TImage::ConstPointer image1 = iPhi1->GetImage();
  typename TImage::ConstPointer image2 = iPhi2->GetImage();

  IteratorType it1( image1, image1->GetLargestPossibleRegion() );
  it1.GoToBegin();

  IteratorType it2( image2, image2->GetLargestPossibleRegion() );
  it2.GoToBegin();

  while( !it1.IsAtEnd() && !it2.IsAtEnd() )
    {
    if( it1.GetIndex() != it2.GetIndex() )
      {
      return false;
      }

    typename TImage::PixelType val1 = iPhi1->Evaluate( it1->GetIndex() );
    typename TImage::PixelType val2 = iPhi2->Evaluate( it2->GetIndex() );

    if( vnl_math_abs( val1 - val2 ) > iTol )
      {
      std::cout << it1.GetIndex() << " ** ";
      std::cout << "val1: " << val1 <<  " != " << "val2: " << val2 << std::endl;
      return false;
      }
    ++it2;
    }

  return true;
}

template< typename TOutput, unsigned int VDimension >
bool CompareWhitakerLevelSets( typename WhitakerSparseLevelSetBase< TOutput, VDimension >::Pointer iPhi1,
                               typename WhitakerSparseLevelSetBase< TOutput, VDimension >::Pointer iPhi2,
                               const TOutput& iTol )
{
  typedef WhitakerSparseLevelSetBase< TOutput, VDimension > LevelSetType;
  typedef typename LevelSetType::Pointer                    LevelSetPointer;
  typedef typename LevelSetType::LayerIdType                LevelSetLayerIdType;
  typedef typename LevelSetType::LayerType                  LevelSetLayerType;
  typedef typename LevelSetType::LayerConstIterator         LevelSetLayerConstIterator;

  for( LevelSetLayerIdType id = LevelSetType::MinusTwoLayer();
        id < LevelSetType::PlusThreeLayer();
        ++id )
    {
    LevelSetLayerType layer1 = iPhi1->GetLayer( id );
    LevelSetLayerType layer2 = iPhi2->GetLayer( id );

    if( layer1.size() != layer2.size() )
      {
      std::cout << "Layer " << static_cast< int >( id )
                << " Have different sizes" << std::endl;
      return false;
      }

    LevelSetLayerConstIterator lIt1 = layer1.begin();
    LevelSetLayerConstIterator lIt2 = layer2.begin();

    while( lIt1 != layer1.end() && lIt2 != layer2.end() )
      {
      if( lIt1->first != lIt2->first )
        {
        return false;
        }
      else
        {
        if( vnl_math_abs( lIt1->second - lIt2->second ) > iTol )
          {
          return false;
          }
        }
      ++lIt1;
      ++lIt2;
      }
    }

  typedef typename LevelSetType::LabelMapType   LabelMapType;
  typedef typename LabelMapType::Pointer        LabelMapPointer;

  typedef typename LabelMapType::ConstIterator  LabelMapConstIterator;

  LabelMapPointer lm1 = iPhi1->GetLabelMap();
  LabelMapPointer lm2 = iPhi2->GetLabelMap();

  LabelMapConstIterator labelIt1( lm1 );
  LabelMapConstIterator labelIt2( lm2 );

  while( !labelIt1.IsAtEnd() && !labelIt2.IsAtEnd() )
    {
    if( labelIt1 != labelIt2 )
      {
      return false;
      }
    ++labelIt1;
    ++labelIt2;
    }

  return true;
}

template< unsigned int VDimension >
bool CompareShiLevelSets( typename ShiSparseLevelSetBase< VDimension >::Pointer iPhi1,
                          typename ShiSparseLevelSetBase< VDimension >::Pointer iPhi2,
                          const typename ShiSparseLevelSetBase< VDimension >::OutputType& iTol  = 0 )
{
  typedef ShiSparseLevelSetBase< VDimension >       LevelSetType;
  typedef typename LevelSetType::Pointer            LevelSetPointer;
  typedef typename LevelSetType::LayerIdType        LevelSetLayerIdType;
  typedef typename LevelSetType::LayerType          LevelSetLayerType;
  typedef typename LevelSetType::LayerConstIterator LevelSetLayerConstIterator;

  for( LevelSetLayerIdType id = LevelSetType::MinusThreeLayer();
        id < LevelSetType::PlusThreeLayer();
        id += 2 )
    {
    LevelSetLayerType layer1 = iPhi1->GetLayer( id );
    LevelSetLayerType layer2 = iPhi2->GetLayer( id );

    if( layer1.size() != layer2.size() )
      {
      return false;
      }

    LevelSetLayerConstIterator lIt1 = layer1.begin();
    LevelSetLayerConstIterator lIt2 = layer2.begin();

    while( lIt1 != layer1.end() && lIt2 != layer2.end() )
      {
      if( lIt1->first != lIt2->first )
        {
        return false;
        }
      else
        {
        if( vnl_math_abs( static_cast< int >( lIt1->second ) - static_cast< int >( lIt2->second ) ) > static_cast< int >( iTol ) )
          {
          return false;
          }
        }
      ++lIt1;
      ++lIt2;
      }
    }

  typedef typename LevelSetType::LabelMapType   LabelMapType;
  typedef typename LabelMapType::Pointer        LabelMapPointer;

  typedef typename LabelMapType::ConstIterator  LabelMapConstIterator;

  LabelMapPointer lm1 = iPhi1->GetLabelMap();
  LabelMapPointer lm2 = iPhi2->GetLabelMap();

  LabelMapConstIterator labelIt1( lm1 );
  LabelMapConstIterator labelIt2( lm2 );

  while( !labelIt1.IsAtEnd() && !labelIt2.IsAtEnd() )
    {
    if( labelIt1 != labelIt2 )
      {
      return false;
      }
    ++labelIt1;
    ++labelIt2;
    }

  return true;
}

template< unsigned int VDimension >
bool CompareMalcolmLevelSets( typename MalcolmSparseLevelSetBase< VDimension >::Pointer iPhi1,
                              typename MalcolmSparseLevelSetBase< VDimension >::Pointer iPhi2,
                              typename MalcolmSparseLevelSetBase< VDimension >::OutputType& iTol )
{
  typedef ShiSparseLevelSetBase< VDimension >       LevelSetType;
  typedef typename LevelSetType::Pointer            LevelSetPointer;
  typedef typename LevelSetType::LayerIdType        LevelSetLayerIdType;
  typedef typename LevelSetType::LayerType          LevelSetLayerType;
  typedef typename LevelSetType::LayerConstIterator LevelSetLayerConstIterator;

  for( LevelSetLayerIdType id = LevelSetType::MinusOneLayer();
        id < LevelSetType::PlusOneLayer();
        ++id )
    {
    LevelSetLayerType layer1 = iPhi1->GetLayer( id );
    LevelSetLayerType layer2 = iPhi2->GetLayer( id );

    if( layer1.size() != layer2.size() )
      {
      return false;
      }

    LevelSetLayerConstIterator lIt1 = layer1.begin();
    LevelSetLayerConstIterator lIt2 = layer2.begin();

    while( lIt1 != layer1.end() && lIt2 != layer2.end() )
      {
      if( lIt1->first != lIt2->first )
        {
        return false;
        }
      else
        {
        if( vnl_math_abs( lIt1->second - lIt2->second ) > iTol )
          {
          return false;
          }
        }
      ++lIt1;
      ++lIt2;
      }
    }

  typedef typename LevelSetType::LabelMapType   LabelMapType;
  typedef typename LabelMapType::Pointer        LabelMapPointer;

  typedef typename LabelMapType::ConstIterator  LabelMapConstIterator;

  LabelMapPointer lm1 = iPhi1->GetLabelMap();
  LabelMapPointer lm2 = iPhi2->GetLabelMap();

  LabelMapConstIterator labelIt1( lm1 );
  LabelMapConstIterator labelIt2( lm2 );

  while( !labelIt1.IsAtEnd() && !labelIt2.IsAtEnd() )
    {
    if( labelIt1 != labelIt2 )
      {
      return false;
      }
    ++labelIt1;
    ++labelIt2;
    }

  return true;
}
}

#endif
