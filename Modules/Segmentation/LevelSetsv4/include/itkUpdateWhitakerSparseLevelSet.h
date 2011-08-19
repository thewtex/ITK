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

#ifndef __itkUpdateWhitakerSparseLevelSet_h
#define __itkUpdateWhitakerSparseLevelSet_h

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkWhitakerSparseLevelSetBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include <list>
#include "itkObject.h"


#include "itkImageFileWriter.h"

namespace itk
{
/**
 *  \class UpdateWhitakerSparseLevelSet
 *  \brief Base class for updating the level-set function
 *
 *  \tparam VDimension Dimension of the input space
 *  \tparam TLevelSetValueType Output type (float or double) of the levelset function
 *  \tparam TEquationContainer Container of the system of levelset equations
 *  \ingroup ITKLevelSetsv4
 */
template< unsigned int VDimension,
          typename TLevelSetValueType,
          class TEquationContainer >
class UpdateWhitakerSparseLevelSet : public Object
{
public:
  typedef UpdateWhitakerSparseLevelSet  Self;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;
  typedef Object                        Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( UpdateWhitakerSparseLevelSet, Object );

  itkStaticConstMacro( ImageDimension, unsigned int, VDimension );

  typedef TLevelSetValueType  LevelSetOutputType;

  typedef WhitakerSparseLevelSetBase< LevelSetOutputType, ImageDimension >
                                                       LevelSetType;
  typedef typename LevelSetType::Pointer               LevelSetPointer;
  typedef typename LevelSetType::InputType             LevelSetInputType;

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

  typedef Image< char, ImageDimension >     LabelImageType;
  typedef typename LabelImageType::Pointer  LabelImagePointer;

  typedef LabelMapToLabelImageFilter<LevelSetLabelMapType, LabelImageType>  LabelMapToLabelImageFilterType;
  typedef LabelImageToLabelMapFilter< LabelImageType, LevelSetLabelMapType> LabelImageToLabelMapFilterType;

  itkGetObjectMacro( OutputLevelSet, LevelSetType );

  /** Update function for initializing and computing the output level set */
  void Update();

  /** Set/Get the sparse levet set image */
  itkSetObjectMacro( InputLevelSet, LevelSetType );
  itkGetObjectMacro( InputLevelSet, LevelSetType );

  /** Set/Get the Dt for the update */
  itkSetMacro( Dt, LevelSetOutputType );
  itkGetMacro( Dt, LevelSetOutputType );

  /** Set/Get the RMS change for the update */
  itkGetMacro( RMSChangeAccumulator, LevelSetOutputType );

  /** Set/Get the Equation container for computing the update */
  itkSetObjectMacro( EquationContainer, EquationContainerType );
  itkGetObjectMacro( EquationContainer, EquationContainerType );

  /** Set/Get the current level set id */
  itkSetMacro( CurrentLevelSetId, IdentifierType );
  itkGetMacro( CurrentLevelSetId, IdentifierType );

  /** Set the update map for all points in the zero layer */
  void SetUpdate( const LevelSetLayerType& iUpdate )
    {
    m_Update = iUpdate;
    }

protected:
  UpdateWhitakerSparseLevelSet() : m_Dt( NumericTraits< LevelSetOutputType >::One ),
    m_RMSChangeAccumulator( NumericTraits< LevelSetOutputType >::Zero ),
    m_MinStatus( -3 ),  m_MaxStatus( 3 )
    {
    m_TempLevelSet = LevelSetType::New();
    m_OutputLevelSet = LevelSetType::New();
    }
  ~UpdateWhitakerSparseLevelSet() {}

  LevelSetOutputType m_Dt;
  LevelSetOutputType m_RMSChangeAccumulator;
  IdentifierType     m_CurrentLevelSetId;

  EquationContainerPointer m_EquationContainer;

  LevelSetLayerType  m_Update;
  LevelSetPointer    m_InputLevelSet;
  LevelSetPointer    m_OutputLevelSet;

  LevelSetPointer   m_TempLevelSet;
  LevelSetLayerType m_TempPhi;

  char              m_MinStatus;
  char              m_MaxStatus;

  LabelImagePointer m_InternalImage;

  typedef ShapedNeighborhoodIterator< LabelImageType > NeighborhoodIteratorType;

  /** Update zero level set layer by moving relevant points to layers -1 or 1 */
  void UpdateZeroLevelSet();

  /** Update -1 level set layer by moving relevant points to layers -2 or 0 */
  void UpdateLminus1();

  /** Update +1 level set layer by moving relevant points to layers 0 or 2 */
  void UpdateLplus1();

  /** Update zero level set layer by moving relevant points to layers -3 or -1 */
  void UpdateLminus2();

  /** Update +2 level set layer by moving relevant points to layers 1 or 3 */
  void UpdateLplus2();

  /** Move identified points into 0 level set layer */
  void MovePointIntoZeroLevelSet();

  /** Move identified points into -1 level set layer */
  void MovePointFromMinus1();

  /** Move identified points into +1 level set layer */
  void MovePointFromPlus1();

  /** Move identified points into -2 level set layer */
  void MovePointFromMinus2();

  /** Move identified points into +2 level set layer */
  void MovePointFromPlus2();

private:
  UpdateWhitakerSparseLevelSet( const Self& );
  void operator = ( const Self& );
};
}

#include "itkUpdateWhitakerSparseLevelSet.hxx"
#endif // __itkUpdateWhitakerSparseLevelSet_h
