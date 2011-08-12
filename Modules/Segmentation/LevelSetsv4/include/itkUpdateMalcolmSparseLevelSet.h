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

#ifndef __itkUpdateMalcolmSparseLevelSet_h
#define __itkUpdateMalcolmSparseLevelSet_h

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkMalcolmSparseLevelSetBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include <list>
#include "itkObject.h"

namespace itk
{
/**
 *  \class UpdateMalcolmSparseLevelSet
 *  \brief Base class for updating the Malcolm representation of level-set function
 *
 *  \tparam VDimension Dimension of the input space
 *  \tparam TEquationContainer Container of the system of levelset equations
 *  \ingroup ITKLevelSetsv4
 */
template< unsigned int VDimension,
          class TEquationContainer >
class UpdateMalcolmSparseLevelSet : public Object
{
public:
  typedef UpdateMalcolmSparseLevelSet   Self;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;
  typedef Object                        Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( UpdateMalcolmSparseLevelSet, Object );

  itkStaticConstMacro( ImageDimension, unsigned int, VDimension );

  typedef MalcolmSparseLevelSetBase< ImageDimension >  LevelSetType;
  typedef typename LevelSetType::Pointer               LevelSetPointer;
  typedef typename LevelSetType::InputType             LevelSetInputType;
  typedef typename LevelSetType::OutputType            LevelSetOutputType;

  typedef typename LevelSetType::LabelMapType          LevelSetLabelMapType;
  typedef typename LevelSetType::LabelMapPointer       LevelSetLabelMapPointer;

  typedef typename LevelSetType::LabelObjectType       LevelSetLabelObjectType;
  typedef typename LevelSetType::LabelObjectPointer    LevelSetLabelObjectPointer;
  typedef typename LevelSetType::LabelObjectLengthType LevelSetLabelObjectLengthType;
  typedef typename LevelSetType::LabelObjectLineType   LevelSetLabelObjectLineType;

  typedef typename LevelSetType::LayerType             LevelSetLayerType;
  typedef typename LevelSetType::LayerIterator         LevelSetLayerIterator;
  typedef typename LevelSetType::LayerConstIterator    LevelSetLayerConstIterator;
  typedef typename LevelSetType::OutputRealType        LevelSetOutputRealType;

  typedef typename LevelSetType::LayerMapType           LevelSetLayerMapType;
  typedef typename LevelSetType::LayerMapIterator       LevelSetLayerMapIterator;
  typedef typename LevelSetType::LayerMapConstIterator  LevelSetLayerMapConstIterator;

  typedef TEquationContainer                      EquationContainerType;
  typedef typename EquationContainerType::Pointer EquationContainerPointer;

  itkGetObjectMacro( OutputLevelSet, LevelSetType );

  /** Update function for initializing and computing the output level set */
  void Update()
  {
    if( m_InputLevelSet.IsNull() )
      {
      itkGenericExceptionMacro( <<"m_InputLevelSet is NULL" );
      }
//    if( m_Update->empty() )
//      {
//      itkGenericExceptionMacro( <<"m_Update is empty" );
//      }

    m_OutputLevelSet->SetLayer( 0, m_InputLevelSet->GetLayer( 0 ) );
    m_OutputLevelSet->SetLabelMap( m_InputLevelSet->GetLabelMap() );

    typedef LabelMapToLabelImageFilter<LevelSetLabelMapType, LabelImageType> LabelMapToLabelImageFilterType;
    typename LabelMapToLabelImageFilterType::Pointer labelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
    labelMapToLabelImageFilter->SetInput( m_InputLevelSet->GetLabelMap() );
    labelMapToLabelImageFilter->Update();

    m_InternalImage = labelMapToLabelImageFilter->GetOutput();
    m_InternalImage->DisconnectPipeline();

    FillUpdateContainer();

    if( m_UnPhased )
      {
      UnPhasedPropagation();
      MinimalInterface();
      }
    else
      {
      LevelSetLayerType& list_0 = m_OutputLevelSet->GetLayer( 0 );

      LevelSetLayerType list_pos;
      LevelSetLayerType update_pos;

      LevelSetLayerType list_neg;
      LevelSetLayerType update_neg;

      LevelSetLayerIterator nodeIt = list_0.begin();
      LevelSetLayerIterator nodeEnd = list_0.end();

      LevelSetLayerIterator upIt = m_Update.begin();

      while( nodeIt != nodeEnd )
        {
        assert( nodeIt->first == upIt->first );

        LevelSetInputType currentIdx = nodeIt->first;
        LevelSetOutputType update = upIt->second;

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
      PhasedPropagation( list_pos, update_pos, true );
      MinimalInterface();

      // dilation
      PhasedPropagation( list_neg, update_neg, false );
      MinimalInterface();
      }

    typedef LabelImageToLabelMapFilter< LabelImageType, LevelSetLabelMapType> LabelImageToLabelMapFilterType;
    typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
    labelImageToLabelMapFilter->SetInput( m_InternalImage );
    labelImageToLabelMapFilter->SetBackgroundValue( 1 );
    labelImageToLabelMapFilter->Update();

    m_OutputLevelSet->GetLabelMap( )->Graft( labelImageToLabelMapFilter->GetOutput() );
  }

  /** Set/Get the sparse levet set image */
  itkSetObjectMacro( InputLevelSet, LevelSetType );
  itkGetObjectMacro( InputLevelSet, LevelSetType );

  /** Set/Get the RMS change for the update */
  itkGetMacro( RMSChangeAccumulator, LevelSetOutputRealType );

  /** Set/Get the Equation container for computing the update */
  itkSetObjectMacro( EquationContainer, EquationContainerType );
  itkGetObjectMacro( EquationContainer, EquationContainerType );

  /** Set/Get the current level set id */
  itkSetMacro( CurrentLevelSetId, IdentifierType );
  itkGetMacro( CurrentLevelSetId, IdentifierType );

protected:
  UpdateMalcolmSparseLevelSet() :
    m_RMSChangeAccumulator( NumericTraits< LevelSetOutputRealType >::Zero ),
    m_UnPhased( false )
    {
    m_OutputLevelSet = LevelSetType::New();
    }

  ~UpdateMalcolmSparseLevelSet() {}

  // input
  LevelSetPointer   m_InputLevelSet;

  // output
  LevelSetPointer   m_OutputLevelSet;

  LevelSetLayerType m_Update;

  IdentifierType           m_CurrentLevelSetId;
  LevelSetOutputRealType   m_RMSChangeAccumulator;
  EquationContainerPointer m_EquationContainer;

  typedef Image< char, ImageDimension >     LabelImageType;
  typedef typename LabelImageType::Pointer  LabelImagePointer;

  LabelImagePointer m_InternalImage;

  typedef ShapedNeighborhoodIterator< LabelImageType > NeighborhoodIteratorType;

  bool m_UnPhased;

  /** Compute the updates for all points in the 0 layer and store in UpdateContainer */
  void FillUpdateContainer()
    {
    LevelSetLayerType Level0 = m_OutputLevelSet->GetLayer( 0 );

    LevelSetLayerIterator nodeIt = Level0.begin();
    LevelSetLayerIterator nodeEnd = Level0.end();

    while( nodeIt != nodeEnd )
      {
      LevelSetInputType currentIndex = nodeIt->first;

      LevelSetOutputRealType update =
          m_EquationContainer->GetEquation( m_CurrentLevelSetId )->Evaluate( currentIndex );

      LevelSetOutputType value = NumericTraits< LevelSetOutputType >::Zero;

      if( update > NumericTraits< LevelSetOutputRealType >::Zero )
        {
        value = NumericTraits< LevelSetOutputType >::One;
        }
      if( update < NumericTraits< LevelSetOutputRealType >::Zero )
        {
        value = - NumericTraits< LevelSetOutputType >::One;
        }

      m_Update.insert(
            std::pair< LevelSetInputType, LevelSetOutputType >( currentIndex, value ) );

      ++nodeIt;
      }
    }

  /** Update the zero layer for all points with values stored in UpdateContainer
   *  Move points to -1 or +1 layers */
  void UnPhasedPropagation()
    {
    LevelSetOutputRealType oldValue, newValue;
    LevelSetLayerType & Level0 = m_OutputLevelSet->GetLayer( 0 );

    // neighborhood iterator
    ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

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

    LevelSetLayerType InsertList;

    LevelSetLayerIterator nodeIt = Level0.begin();
    LevelSetLayerIterator nodeEnd = Level0.end();

    LevelSetLayerIterator upIt = m_Update.begin();

    while( nodeIt != nodeEnd )
      {
      LevelSetInputType currentIdx = nodeIt->first;
      LevelSetInputType upIdx = upIt->first;

      assert( currentIdx == upIdx );

      LevelSetOutputType update = upIt->second;

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
        Level0.erase( tempIt );

        m_InternalImage->SetPixel( currentIdx, newValue );
//         m_EquationContainer->GetEquation( m_CurrentLevelSetId )->UpdatePixel(
//               currentIdx, oldValue, newValue );

        neighIt.SetLocation( currentIdx );

        for( typename NeighborhoodIteratorType::Iterator
             i = neighIt.Begin();
             !i.IsAtEnd(); ++i )
          {
          char tempValue = i.Get();
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
      Level0.insert(
            std::pair< LevelSetInputType, LevelSetOutputType >( nodeIt->first, 0 ) );

//       m_EquationContainer->UpdatePixel( nodeIt->first, nodeIt->second, 0 );

      m_InternalImage->SetPixel( nodeIt->first, 0 );

      ++nodeIt;
      }
    }

  /** Update separately the zero layer for points with positive/negative update values
   *  Move points to -1 or +1 layers */
  void PhasedPropagation( LevelSetLayerType& ioList,
                          LevelSetLayerType& ioUpdate,
                          const bool& iContraction )
    {
    assert( ioList.size() == ioUpdate.size() );

    ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

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

    LevelSetLayerType InsertList;

    LevelSetLayerIterator nodeIt = ioList.begin();
    LevelSetLayerIterator nodeEnd = ioList.end();

    LevelSetLayerIterator upIt = ioUpdate.begin();

    while( nodeIt != nodeEnd )
      {
      assert( nodeIt->first == upIt->first );

      LevelSetOutputType oldValue;
      LevelSetOutputType newValue;

      LevelSetOutputType update = upIt->second;
      LevelSetInputType currentIdx = nodeIt->first;

      bool to_be_updated = false;

      if( update != NumericTraits< LevelSetOutputRealType >::Zero )
        {
        // only allow positive forces
        iContraction ? newValue = 1 : newValue = -1;
        LevelSetLayerIterator tempIt = nodeIt;
        ++nodeIt;
        ++upIt;
        ioList.erase( tempIt );
        m_OutputLevelSet->GetLayer( 0 ).erase( currentIdx );

        m_InternalImage->SetPixel( currentIdx, newValue );
//           m_EquationContainer->GetEquation( m_CurrentLevelSetId )->UpdatePixel(
//                 currentIdx, oldValue , newValue );

        neighIt.SetLocation( currentIdx );

        for( typename NeighborhoodIteratorType::Iterator
            i = neighIt.Begin();
            !i.IsAtEnd(); ++i )
          {
          char tempValue = i.Get();

          if( tempValue * newValue == -1 )
            {
            LevelSetInputType tempIdx =
              neighIt.GetIndex( i.GetNeighborhoodOffset() );

            InsertList.insert(
              std::pair< LevelSetInputType, LevelSetOutputType >( tempIdx, 0 ) );

//              m_EquationContainer->GetEquation( m_CurrentLevelSetId )->UpdatePixel(
//                    tempIdx, oldValue , 0 );
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
      m_OutputLevelSet->GetLayer( 0 ).insert(
            std::pair< LevelSetInputType, LevelSetOutputType >( nodeIt->first, 0 ) );

//       m_EquationContainer->UpdatePixel( nodeIt->first, nodeIt->second, 0 );

      m_InternalImage->SetPixel( nodeIt->first, 0 );

      ++nodeIt;
      }
    }

  /** Make sure the layers are of single pixel thickness only */
  void MinimalInterface()
    {
    LevelSetOutputRealType oldValue, newValue;
    LevelSetLayerType & list_0 = m_OutputLevelSet->GetLayer( 0 );

    ZeroFluxNeumannBoundaryCondition< LabelImageType > sp_nbc;

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

        m_InternalImage->SetPixel( currentIdx, newValue );
//         m_EquationContainer->GetEquation( m_CurrentLevelSetId )->UpdatePixel(
//               currentIdx, oldValue , newValue );
        }
      else if( positive && !negative )
        {
        newValue = 1;
        LevelSetLayerIterator tempIt = nodeIt;
        ++nodeIt;
        list_0.erase( tempIt );

        m_InternalImage->SetPixel( currentIdx, newValue );
  //         m_EquationContainer->GetEquation( m_CurrentLevelSetId )->UpdatePixel(
  //               currentIdx, oldValue , newValue );
        }
      else if( !positive && !negative )
        {
        std::cout << "Neither positive nor negative" << std::endl;
        ++nodeIt;
        }
      else
        {
        ++nodeIt;
        }
      }
    }

private:
  UpdateMalcolmSparseLevelSet( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkUpdateMalcolmSparseLevelSet_h
