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

#ifndef __itkLevelSetSegmentationImageBase_h
#define __itkLevelSetSegmentationImageBase_h

#include <map>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkImage.h"
#include "itkLevelSetDomainMapImageFilter.h"
#include "itkLevelSetContainer.h"
#include "itkImageRegionIteratorWithIndex.h"


namespace itk
{
/**
 *  \class LevelSetSegmentationImageBase
 *  \brief Image segmentation of level sets in container
 *
 * \tparam TIdentifier type of the identifier used to reference on level set function
 * \tparam TLevelSet type of level set function in the container.
 * \ingroup ITKLevelSetsv4
 */
template< class TLevelSetContainer >
class LevelSetSegmentationImageBase : public Object
{
public:
  typedef LevelSetSegmentationImageBase Self;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;
  typedef Object                        Superclass;

  /** Run-time type information */
  itkTypeMacro ( LevelSetSegmentationImageBase, Object );

  /** IdentifierType */
  typedef TLevelSetContainer                                     LevelSetContainerType;
  typedef typename LevelSetContainerType::Pointer                LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetIdentifierType LevelSetIdentifierType;
  typedef typename LevelSetContainerType::LevelSetContainerConstIteratorType
                                                                 LevelSetContainerConstIteratorType;
  typedef typename LevelSetContainerType::LevelSetContainerIteratorType
                                                                 LevelSetContainerIteratorType;

  /** typedefs related to the type of level set*/
  typedef typename LevelSetContainerType::LevelSetType LevelSetType;
  typedef typename LevelSetType::Pointer               LevelSetPointer;
  typedef typename LevelSetType::InputType             InputIndexType;
  typedef typename LevelSetType::OutputType            OutputType;
  typedef typename LevelSetType::OutputRealType        OutputRealType;
  typedef typename LevelSetType::PointType             PointType;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename LevelSetContainerType::IdListType          IdListType;
  typedef typename LevelSetContainerType::IdListImageType     IdListImageType;
  typedef typename LevelSetContainerType::IdListConstIterator IdListConstIterator;

  typedef typename LevelSetContainerType::DomainMapImageFilterType DomainMapImageFilterType;
  typedef typename DomainMapImageFilterType::Pointer               DomainMapImageFilterPointer;
  typedef typename DomainMapImageFilterType::LevelSetDomain        LevelSetDomainType;

  typedef std::map< LevelSetIdentifierType, LevelSetDomainType >   DomainContainerType;
  typedef typename DomainContainerType::iterator                   DomainIteratorType;
  typedef typename DomainContainerType::const_iterator             ConstDomainIteratorType;

  typedef Image<IdentifierType, Dimension >  SegmentImageType;
  typedef typename SegmentImageType::Pointer SegmentImagePointer;

  typedef ImageRegionIteratorWithIndex< SegmentImageType > SegmentImageIteratorType;

  /** Set/Get the level set container. */
  itkSetObjectMacro( LevelSetContainer, LevelSetContainerType );
  itkGetModifiableObjectMacro(LevelSetContainer, LevelSetContainerType );

  /* Get a segmentation image from the level-set container */
  SegmentImageType* GetSegmentationImage( const OutputType p );

  void AllocateSegmentationImageFromDomainMap();

  void FillSegmentationBasedOnDomainMap( const OutputType p );

  void FillSegmentationBasedOnLevelSetContainer( const OutputType p );

  virtual void AllocateSegmentationImageFromLevelSetDomain() = 0;

protected:
  /** \brief Default Constructor */
  LevelSetSegmentationImageBase();

  /** \brief Default Destructor */
  virtual ~LevelSetSegmentationImageBase();

  SegmentImagePointer       m_SegmentationImage;
  LevelSetContainerPointer  m_LevelSetContainer;

private:
  LevelSetSegmentationImageBase( const Self & ); // purposely not implemented
  void operator = ( const Self & ); // purposely not implemented

};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetSegmentationImageBase.hxx"
#endif

#endif // __itkLevelSetSegmentationImageBase_h
