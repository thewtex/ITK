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

#ifndef __itkMalcolmSparseLevelSetBase_h
#define __itkMalcolmSparseLevelSetBase_h

#include "itkImage.h"
#include "itkLevelSetSparseImageBase.h"

#include "itkLabelObject.h"
#include "itkLabelMap.h"

namespace itk
{
/**
 *  \class MalcolmSparseLevelSetBase
 *  \brief Derived class for the shi representation of level-set function
 *
 *  This representation is a "sparse" level-set function, where values could
 *  only be { -1, 0, +1 } and organized into 1 layer { 0 }.
 *
 *  \tparam VDimension Dimension of the input space
 *  \ingroup ITKLevelSetsv4
 */
template< unsigned int VDimension >
class MalcolmSparseLevelSetBase :
    public LevelSetSparseImageBase< int8_t, VDimension >
{
public:
  typedef MalcolmSparseLevelSetBase                     Self;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;
  typedef LevelSetSparseImageBase< int8_t, VDimension > Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MalcolmSparseLevelSetBase, LevelSetSparseImageBase);

  itkStaticConstMacro ( Dimension, unsigned int, VDimension );

  typedef typename Superclass::InputType        InputType;
  typedef typename Superclass::OutputType       OutputType;
  typedef typename Superclass::OutputRealType   OutputRealType;
  typedef typename Superclass::GradientType     GradientType;
  typedef typename Superclass::HessianType      HessianType;
  typedef typename Superclass::LevelSetDataType LevelSetDataType;

  typedef typename Superclass::LayerIdType            LayerIdType;
  typedef typename Superclass::LabelObjectType        LabelObjectType;
  typedef typename Superclass::LabelObjectPointer     LabelObjectPointer;
  typedef typename Superclass::LabelObjectLengthType  LabelObjectLengthType;
  typedef typename Superclass::LabelObjectLineType    LabelObjectLineType;

  typedef typename Superclass::LabelMapType     LabelMapType;
  typedef typename Superclass::LabelMapPointer  LabelMapPointer;
  typedef typename Superclass::RegionType       RegionType;

  typedef typename Superclass::LayerType          LayerType;
  typedef typename Superclass::LayerIterator      LayerIterator;
  typedef typename Superclass::LayerConstIterator LayerConstIterator;

  typedef typename Superclass::LayerMapType           LayerMapType;
  typedef typename Superclass::LayerMapIterator       LayerMapIterator;
  typedef typename Superclass::LayerMapConstIterator  LayerMapConstIterator;

  /** Returns the value of the level set function at a given location iP */
  virtual OutputType Evaluate( const InputType& iP ) const;

  /** Returns the Gradient of the level set function at a given location iP */
  virtual GradientType EvaluateGradient( const InputType& iP ) const;

  /** Returns the Hessian of the level set function at a given location iP */
  virtual HessianType EvaluateHessian( const InputType& iP ) const;

  /** Returns the Laplacian of the level set function at a given location iP */
  virtual OutputRealType EvaluateLaplacian( const InputType& iP ) const;

  /** Returns the MeanCurvature of the level set function at a given location iP */
  virtual OutputRealType EvaluateMeanCurvature( const InputType& iP ) const;

  virtual void Evaluate( const InputType& iP, LevelSetDataType& ioData ) const;
  virtual void EvaluateGradient( const InputType& iP, LevelSetDataType& ioData ) const;
  virtual void EvaluateHessian( const InputType& iP, LevelSetDataType& ioData ) const;
  virtual void EvaluateLaplacian( const InputType& iP, LevelSetDataType& ioData ) const;
  virtual void EvaluateMeanCurvature( const InputType& iP, LevelSetDataType& ioData ) const;

  /** Returns the gradient of the level set function at a given location iP
   * as part of the LevelSetDataType
   * \todo to be implemented */
  virtual void EvaluateForwardGradient( const InputType& iP, LevelSetDataType& ioData ) const;

  /** Returns the gradient of the level set function at a given location iP
   * as part of the LevelSetDataType
   * \todo to be implemented */
  virtual void EvaluateBackwardGradient( const InputType& iP, LevelSetDataType& ioData ) const;

  static inline LayerIdType MinusOneLayer() { return -1; }
  static inline LayerIdType ZeroLayer() { return 0; }
  static inline LayerIdType PlusOneLayer() { return 1; }

  /** Return the label object pointer with a given id */
  template< class TLabel >
  typename LabelObject< TLabel, Dimension >::Pointer
  GetAsLabelObject()
    {
    typedef LabelObject< TLabel, Dimension > OutputLabelObjectType;
    typename OutputLabelObjectType::Pointer object = OutputLabelObjectType::New();

    LabelObjectPointer labelObject = this->m_LabelMap->GetLabelObject( MinusOneLayer() );

    for( SizeValueType i = 0; i < labelObject->GetNumberOfLines(); i++ )
      {
      object->AddLine( labelObject->GetLine( i ) );
      }

    labelObject = this->m_LabelMap->GetLabelObject( 0 );

    for( SizeValueType i = 0; i < labelObject->GetNumberOfLines(); i++ )
      {
      object->AddLine( labelObject->GetLine( i ) );
      }
    object->Optimize();

    return object;
    }

protected:

  MalcolmSparseLevelSetBase();

  virtual ~MalcolmSparseLevelSetBase();

  /** Initialize the sparse field layers */
  virtual void InitializeLayers();

private:
  MalcolmSparseLevelSetBase( const Self& ); //purposely not implemented
  void operator = ( const Self& ); //purposely not implemented
};
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMalcolmSparseLevelSetBase.hxx"
#endif

#endif // __itkMalcolmSparseLevelSetBase_h
