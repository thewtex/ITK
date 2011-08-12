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
#include "itkLevelSetImageBase.h"

#include "itkLabelObject.h"
#include "itkLabelMap.h"

namespace itk
{
/**
 *  \class MalcolmSparseLevelSetBase
 *  \brief Derived class for the shi representation of level-set function
 *
 *  \tparam VDimension Dimension of the input space
 *  \ingroup ITKLevelSetsv4
 */
template< unsigned int VDimension >
class MalcolmSparseLevelSetBase :
    public LevelSetBase< Index< VDimension >,
                         VDimension,
                         char,
                         ImageBase< VDimension > >
//    public LevelSetImageBase< Image< char, VDimension > >
{
public:
  typedef Index< VDimension >                     IndexType;
  typedef char                                    OutputType;
  typedef ImageBase< VDimension >                 ImageBaseType;

  typedef MalcolmSparseLevelSetBase               Self;
  typedef SmartPointer< Self >                    Pointer;
  typedef SmartPointer< const Self >              ConstPointer;
  typedef LevelSetBase< IndexType, VDimension,
                        OutputType, ImageBaseType >
                                                  Superclass;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MalcolmSparseLevelSetBase, LevelSetBase);

  itkStaticConstMacro ( Dimension, unsigned int,
                        VDimension );

  typedef typename Superclass::InputType      InputType;
  typedef typename Superclass::OutputRealType OutputRealType;
  typedef typename Superclass::GradientType   GradientType;
  typedef typename Superclass::HessianType    HessianType;

  typedef LabelObject< char, VDimension >       LabelObjectType;
  typedef typename LabelObjectType::Pointer     LabelObjectPointer;
  typedef typename LabelObjectType::LengthType  LabelObjectLengthType;
  typedef typename LabelObjectType::LineType    LabelObjectLineType;

  typedef LabelMap< LabelObjectType >         LabelMapType;
  typedef typename LabelMapType::Pointer      LabelMapPointer;

  typedef std::map< IndexType, OutputType,
                    Functor::IndexLexicographicCompare< VDimension > >
                                                  LayerType;
  typedef typename LayerType::iterator            LayerIterator;
  typedef typename LayerType::const_iterator      LayerConstIterator;

  typedef std::map< char, LayerType >             LayerMapType;
  typedef typename LayerMapType::iterator         LayerMapIterator;
  typedef typename LayerMapType::const_iterator   LayerMapConstIterator;

  /** Returns the layer affiliation of a given location iP */
  virtual char Status( const InputType& iP ) const;

  /** Returns the value of the level set function at a given location iP */
  virtual OutputType Evaluate( const InputType& iP ) const;

  /** Returns the gradient of the level set function at a given location iP
   * \todo to be implemented */
  virtual GradientType EvaluateGradient( const InputType& iP ) const;

  /** Returns the Hessian of the level set function at a given location iP
   * \todo to be implemented */
  virtual HessianType EvaluateHessian( const InputType& iP ) const;

  /** Initialize the label map point and the sparse-field layers */
  virtual void Initialize();

  /** Copy level set information from data object */
  virtual void CopyInformation( const DataObject* data );

  /** Graft data object as level set object */
  virtual void Graft( const DataObject* data );

  /** Return the const pointer to a layer map with given id  */
  const LayerType& GetLayer( char iVal ) const;

  /** Return the pointer to a layer map with given id  */
  LayerType& GetLayer( char iVal );

  /** Set a layer map with id to the given layer pointer */
  void SetLayer( char iVal, const LayerType& iLayer );

  /** Return the label object pointer with a given id */
  template< class TLabel >
  LabelObject< TLabel, Dimension >* GetAsLabelObject()
    {
    typedef LabelObject< TLabel, Dimension > OutputLabelObjectType;
    typename OutputLabelObjectType::Pointer object =
        OutputLabelObjectType::New();

    LabelObjectPointer labelObject = m_LabelMap->GetLabelObject( -1 );

    for( SizeValueType i = 0; i < labelObject->GetNumberOfLines(); i++ )
      {
      object->AddLine( labelObject->GetLine( i ) );
      }

    labelObject = m_LabelMap->GetLabelObject( 0 );

    for( SizeValueType i = 0; i < labelObject->GetNumberOfLines(); i++ )
      {
      object->AddLine( labelObject->GetLine( i ) );
      }
    object->Optimize();

    return object.GetPointer();
    }

  /** Set/Get the label map for computing the sparse representation */
  virtual void SetLabelMap( LabelMapType* iLabelMap );
  itkGetObjectMacro( LabelMap, LabelMapType );

protected:

  MalcolmSparseLevelSetBase();

  virtual ~MalcolmSparseLevelSetBase()    {}

  /** Initialize the sparse field layers */
  void InitializeLayers();

  LayerMapType     m_Layers;
  LabelMapPointer  m_LabelMap;

  typedef GradientType ScalingType;
  ScalingType m_NeighborhoodScales;

private:
  MalcolmSparseLevelSetBase( const Self& );
  void operator = ( const Self& );
};
}
#include "itkMalcolmSparseLevelSetBase.hxx"
#endif // __itkMalcolmSparseLevelSetBase_h
