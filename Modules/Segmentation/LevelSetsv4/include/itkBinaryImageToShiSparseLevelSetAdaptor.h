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

#ifndef __itkBinaryImageToShiSparseLevelSetAdaptor_h
#define __itkBinaryImageToShiSparseLevelSetAdaptor_h

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkShiSparseLevelSetBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include <list>
#include "itkObject.h"

namespace itk
{
/**
 *  \class BinaryImageToShiSparseLevelSetAdaptor
 *  \brief Base class converting a binary image to a Shi level set representation
 *
 *  \tparam TInput Input Image Type
 */
template< class TInputImage >
class BinaryImageToShiSparseLevelSetAdaptor : public Object
{
public:
  typedef BinaryImageToShiSparseLevelSetAdaptor   Self;
  typedef SmartPointer< Self >                    Pointer;
  typedef SmartPointer< const Self >              ConstPointer;
  typedef Object                                  Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( BinaryImageToShiSparseLevelSetAdaptor, Object );

  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef typename InputImageType::IndexType    InputImageIndexType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename NumericTraits< InputImagePixelType >::RealType
                                                InputPixelRealType;

  itkStaticConstMacro ( ImageDimension, unsigned int,
                       InputImageType::ImageDimension );

  typedef ShiSparseLevelSetBase< ImageDimension >      LevelSetType;
  typedef typename LevelSetType::Pointer               LevelSetPointer;
  typedef typename LevelSetType::InputType             LevelSetInputType;
  typedef typename LevelSetType::OutputType            LevelSetOutputType;

  typedef typename LevelSetType::LabelObjectType       LevelSetLabelObjectType;
  typedef typename LevelSetType::LabelObjectPointer    LevelSetLabelObjectPointer;
  typedef typename LevelSetType::LabelObjectLengthType LevelSetLabelObjectLengthType;
  typedef typename LevelSetType::LabelObjectLineType   LevelSetLabelObjectLineType;

  typedef typename LevelSetType::LabelMapType          LevelSetLabelMapType;
  typedef typename LevelSetType::LabelMapPointer       LevelSetLabelMapPointer;

  typedef typename LevelSetType::LayerType             LevelSetLayerType;
  typedef typename LevelSetType::LayerIterator         LevelSetLayerIterator;
  typedef typename LevelSetType::LayerConstIterator    LevelSetLayerConstIterator;

  /** this is the same as Procedure 1
   * Input is a binary image m_InputImage
   * Output is a label map with 1 as background layer and -1, 0 as foreground layers */
  void Initialize()
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

  // Set/Get the sparse levet set image
  itkSetObjectMacro( SparseLevelSet, LevelSetType );
  itkGetObjectMacro( SparseLevelSet, LevelSetType );

  // Set get the input image
  itkSetObjectMacro( InputImage, InputImageType );
  itkGetObjectMacro( InputImage, InputImageType );

protected:
  BinaryImageToShiSparseLevelSetAdaptor()
    {
    m_SparseLevelSet = LevelSetType::New();
    }
  ~BinaryImageToShiSparseLevelSetAdaptor() {}

  InputImagePointer       m_InputImage;
  LevelSetPointer         m_SparseLevelSet;
  LevelSetLabelMapPointer m_LabelMap;

  typedef Image< char, ImageDimension >         InternalImageType;
  typedef typename InternalImageType::Pointer   InternalImagePointer;

  InternalImagePointer m_InternalImage;

  typedef ImageRegionIteratorWithIndex< InputImageType >      InputIteratorType;
  typedef ImageRegionIteratorWithIndex< InternalImageType >   InternalIteratorType;

  typedef ShapedNeighborhoodIterator< InternalImageType > NeighborhoodIteratorType;

  /** Find the active layer separating the foreground and background regions */
  void FindActiveLayer()
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

private:
  BinaryImageToShiSparseLevelSetAdaptor( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkBinaryImageToShiSparseLevelSetAdaptor_h
