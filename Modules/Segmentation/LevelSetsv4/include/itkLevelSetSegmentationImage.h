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

#ifndef __itkLevelSetSegmentationImage_h
#define __itkLevelSetSegmentationImage_h

#include "itkLevelSetContainer.h"
#include "itkWhitakerSparseLevelSetImage.h"
#include "itkShiSparseLevelSetImage.h"
#include "itkMalcolmSparseLevelSetImage.h"
#include "itkLevelSetSegmentationImageBase.h"


namespace itk
{
/**
 *  \class LevelSetSegmentationImage
 *  \brief Image segmentation of level sets in container
 *
 * \tparam TIdentifier type of the identifier used to reference on level set function
 * \tparam TLevelSet type of level set function in the container.
 * \ingroup ITKLevelSetsv4
 */

template< class TIdentifier >
class LevelSetSegmentationImage
{};

template< class TIdentifier, class TImage >
class LevelSetSegmentationImage< LevelSetContainer< TIdentifier, LevelSetDenseImage< TImage > > > :
        public LevelSetSegmentationImageBase< LevelSetContainer< TIdentifier, LevelSetDenseImage< TImage > > >
{
public:
  typedef LevelSetDenseImage< TImage >                           LevelSetType;
  typedef LevelSetContainer< TIdentifier, LevelSetType >         LevelSetContainerType;
  typedef LevelSetSegmentationImage                              Self;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;
  typedef LevelSetSegmentationImageBase< LevelSetContainerType > Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetSegmentationImage, LevelSetSegmentationImageBase );

  /** typedefs related to the type of level set*/
  typedef typename LevelSetType::Pointer          LevelSetPointer;
  typedef typename LevelSetType::InputType        InputIndexType;
  typedef typename LevelSetType::OutputType       OutputType;
  typedef typename LevelSetType::OutputRealType   OutputRealType;
  typedef typename LevelSetType::PointType        PointType;

  typedef TIdentifier LevelSetIdentifierType;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::SegmentImageType    SegmentImageType;
  typedef typename Superclass::SegmentImagePointer SegmentImagePointer;

  virtual void AllocateSegmentationImageFromLevelSetDomain();

protected:
  /** \brief Default Constructor */
  LevelSetSegmentationImage(){}

  /** \brief Default Destructor */
  ~LevelSetSegmentationImage(){}

private:
  LevelSetSegmentationImage( const Self & ); // purposely not implemented
  void operator = ( const Self & ); // purposely not implemented

};

template< class TIdentifier, class TOutput, unsigned int VDimension >
class LevelSetSegmentationImage< LevelSetContainer< TIdentifier,
        WhitakerSparseLevelSetImage< TOutput, VDimension > > > :
        public LevelSetSegmentationImageBase< LevelSetContainer< TIdentifier,
        WhitakerSparseLevelSetImage< TOutput, VDimension > > >
{
public:
  typedef WhitakerSparseLevelSetImage< TOutput, VDimension >     LevelSetType;
  typedef LevelSetContainer< TIdentifier, LevelSetType >         LevelSetContainerType;
  typedef LevelSetSegmentationImage                              Self;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;
  typedef LevelSetSegmentationImageBase< LevelSetContainerType > Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetSegmentationImage, LevelSetSegmentationImageBase );

  /** typedefs related to the type of level set*/
  typedef typename LevelSetType::Pointer  LevelSetPointer;
  typedef TIdentifier                     LevelSetIdentifierType;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::SegmentImageType    SegmentImageType;
  typedef typename Superclass::SegmentImagePointer SegmentImagePointer;

  virtual void AllocateSegmentationImageFromLevelSetDomain();

protected:
  LevelSetSegmentationImage(){}
  ~LevelSetSegmentationImage(){}

private:
  LevelSetSegmentationImage( const Self & ); // purposely not implemented
  void operator = ( const Self & ); // purposely not implemented

};


template< class TIdentifier, unsigned int VDimension >
class LevelSetSegmentationImage< LevelSetContainer< TIdentifier,
        ShiSparseLevelSetImage< VDimension > > > :
        public LevelSetSegmentationImageBase< LevelSetContainer< TIdentifier,
        ShiSparseLevelSetImage< VDimension > > >
{
public:
  typedef ShiSparseLevelSetImage< VDimension >                   LevelSetType;
  typedef LevelSetContainer< TIdentifier, LevelSetType >         LevelSetContainerType;
  typedef LevelSetSegmentationImage                              Self;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;
  typedef LevelSetSegmentationImageBase< LevelSetContainerType > Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetSegmentationImage, LevelSetSegmentationImageBase );

  /** typedefs related to the type of level set*/
  typedef typename LevelSetType::Pointer  LevelSetPointer;
  typedef TIdentifier                     LevelSetIdentifierType;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::SegmentImageType    SegmentImageType;
  typedef typename Superclass::SegmentImagePointer SegmentImagePointer;

  virtual void AllocateSegmentationImageFromLevelSetDomain();

protected:
  LevelSetSegmentationImage(){}
  ~LevelSetSegmentationImage(){}

private:
  LevelSetSegmentationImage( const Self & ); // purposely not implemented
  void operator = ( const Self & ); // purposely not implemented

};


template< class TIdentifier, unsigned int VDimension >
class LevelSetSegmentationImage< LevelSetContainer< TIdentifier,
        MalcolmSparseLevelSetImage< VDimension > > > :
        public LevelSetSegmentationImageBase< LevelSetContainer< TIdentifier,
        MalcolmSparseLevelSetImage< VDimension > > >
{
public:
  typedef MalcolmSparseLevelSetImage< VDimension >               LevelSetType;
  typedef LevelSetContainer< TIdentifier, LevelSetType >         LevelSetContainerType;
  typedef LevelSetSegmentationImage                              Self;
  typedef SmartPointer< Self >                                   Pointer;
  typedef SmartPointer< const Self >                             ConstPointer;
  typedef LevelSetSegmentationImageBase< LevelSetContainerType > Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro ( LevelSetSegmentationImage, LevelSetSegmentationImageBase );

  /** typedefs related to the type of level set*/
  typedef typename LevelSetType::Pointer  LevelSetPointer;
  typedef TIdentifier                     LevelSetIdentifierType;

  itkStaticConstMacro ( Dimension, unsigned int, LevelSetType::Dimension );

  typedef typename Superclass::SegmentImageType    SegmentImageType;
  typedef typename Superclass::SegmentImagePointer SegmentImagePointer;

  virtual void AllocateSegmentationImageFromLevelSetDomain();

protected:
  LevelSetSegmentationImage(){}
  ~LevelSetSegmentationImage(){}

private:
  LevelSetSegmentationImage( const Self & ); // purposely not implemented
  void operator = ( const Self & ); // purposely not implemented

};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetSegmentationImage.hxx"
#endif

#endif // __itkLevelSetSegmentationImage_h
