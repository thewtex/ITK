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

#ifndef __itkBinaryImageToMalcolmSparseLevelSetAdaptor_h
#define __itkBinaryImageToMalcolmSparseLevelSetAdaptor_h

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkMalcolmSparseLevelSetBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include <list>
#include "itkObject.h"

namespace itk
{
/**
 *  \class BinaryImageToMalcolmSparseLevelSetAdaptor
 *  \brief Base class converting a binary image to a Malcolm level set representation
 *
 *  \tparam TInput Input Image Type
 */
template< class TInputImage >
class BinaryImageToMalcolmSparseLevelSetAdaptor : public Object
{
public:
  typedef BinaryImageToMalcolmSparseLevelSetAdaptor   Self;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;
  typedef Object                                      Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( BinaryImageToMalcolmSparseLevelSetAdaptor, Object );

  typedef TInputImage                           InputImageType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef typename InputImageType::IndexType    InputImageIndexType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename NumericTraits< InputImagePixelType >::RealType
                                                InputPixelRealType;

  itkStaticConstMacro ( ImageDimension, unsigned int,
                       InputImageType::ImageDimension );

  typedef MalcolmSparseLevelSetBase< ImageDimension >   LevelSetType;
  typedef typename LevelSetType::Pointer                LevelSetPointer;
  typedef typename LevelSetType::InputType              LevelSetInputType;
  typedef typename LevelSetType::OutputType             LevelSetOutputType;
  typedef typename LevelSetType::OutputRealType         LevelSetOutputRealType;

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

  // Set/Get the sparse levet set image
  itkSetObjectMacro( SparseLevelSet, LevelSetType );
  itkGetObjectMacro( SparseLevelSet, LevelSetType );

  // Set get the input image
  itkSetObjectMacro( InputImage, InputImageType );
  itkGetObjectMacro( InputImage, InputImageType );

protected:
  BinaryImageToMalcolmSparseLevelSetAdaptor()
    {
    m_SparseLevelSet = LevelSetType::New();
    }
  ~BinaryImageToMalcolmSparseLevelSetAdaptor() {}

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

  /** Ensure that the 0 level set layer is only of single pixel thickness */
  void MinimalInterface()
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

private:
  BinaryImageToMalcolmSparseLevelSetAdaptor( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkBinaryImageToMalcolmSparseLevelSetAdaptor_h
