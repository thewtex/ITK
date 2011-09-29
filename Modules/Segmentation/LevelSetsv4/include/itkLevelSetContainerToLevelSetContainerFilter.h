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

#ifndef __itkLevelSetContainerToLevelSetContainerFilter_h
#define __itkLevelSetContainerToLevelSetContainerFilter_h

#include "itkLevelSetContainerSource.h"

#include "itkImageDuplicator.h"

#include "itkLevelSetDenseImageBase.h"
#include "itkDenseLevelSetContainer.h"

#include "itkWhitakerSparseLevelSetImage.h"
#include "itkSparseLevelSetContainer.h"

#include "itkShiSparseLevelSetImage.h"
#include "itkMalcolmSparseLevelSetImage.h"

namespace itk
{
/** \class LevelSetContainerToLevelSetContainerFilter
 * \brief
 *
 * \ingroup LevelSetContainerFilters
 *
 * \ingroup ITKLevelSetContainer
 */
template< class TLevelSetContainer >
class ITK_EXPORT LevelSetContainerToLevelSetContainerFilterBase :
    public LevelSetContainerSource< TLevelSetContainer >
{
public:
  /** Standard class typedefs. */
  typedef LevelSetContainerToLevelSetContainerFilterBase  Self;
  typedef LevelSetContainerSource< TLevelSetContainer >   Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetContainerToLevelSetContainerFilterBase,
               LevelSetContainerSource);

  /** Some convenient typedefs. */
  typedef TLevelSetContainer                      LevelSetContainerType;
  typedef typename LevelSetContainerType::Pointer LevelSetContainerPointer;

  typedef typename LevelSetContainerType::LevelSetType    LevelSetType;
  typedef typename LevelSetContainerType::LevelSetPointer LevelSetPointer;

  /** Set the LevelSetContainer input of this process object.  */
  using Superclass::SetInput;
  void SetInput(const LevelSetContainerType *input);

  /** Get the LevelSetContainer input of this process object.  */
  const LevelSetContainerType * GetInput(void) const;

  const LevelSetContainerType * GetInput(unsigned int idx) const;

protected:
  LevelSetContainerToLevelSetContainerFilterBase();
  ~LevelSetContainerToLevelSetContainerFilterBase() {}

private:
  LevelSetContainerToLevelSetContainerFilterBase(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented
};

template< class TLevelSetContainer >
class LevelSetContainerToLevelSetContainerFilter;

template< class TIdentifier, class TLevelSet >
class LevelSetContainerToLevelSetContainerFilter<
    DenseLevelSetContainer< TIdentifier, TLevelSet >
    > :
public LevelSetContainerToLevelSetContainerFilterBase<
    DenseLevelSetContainer< TIdentifier, TLevelSet > >
{
public:
  typedef DenseLevelSetContainer< TIdentifier, TLevelSet > LevelSetContainerType;

  /** Standard class typedefs. */
  typedef LevelSetContainerToLevelSetContainerFilter                      Self;
  typedef LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainerType >                                               Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetContainerToLevelSetContainerFilterBase,
               LevelSetContainerSource);

  /** Some convenient typedefs. */
  typedef typename LevelSetContainerType::Pointer                 LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetIdentifierType  LevelSetIdentifierType;

  typedef typename LevelSetContainerType::LevelSetType    LevelSetType;
  typedef typename LevelSetContainerType::LevelSetPointer LevelSetPointer;

  typedef typename LevelSetContainerType::ImageType     LevelSetImageType;
  typedef typename LevelSetContainerType::ImagePointer  LevelSetImagePointer;

protected:
  LevelSetContainerToLevelSetContainerFilter() {}
  ~LevelSetContainerToLevelSetContainerFilter() {}

  void CopyInputToOutput()
    {
    const LevelSetContainerType* input = this->GetInput();
    LevelSetContainerType* output = this->GetOutput();

    typename LevelSetContainerType::ConstIterator it = input->Begin();

    typedef ImageDuplicator< LevelSetImageType > ImageDuplicatorType;

    while( it != input->End )
      {
      LevelSetType* ls = it->GetLevelSet();
      const LevelSetImageType* lsImage = ls->GetImage();

      LevelSetIdentifierType id = it->GetIdentifier();

      LevelSetPointer outputLS = LevelSetType::New();

      typename ImageDuplicatorType::Pointer duplicator = ImageDuplicatorType::New();
      duplicator->SetInputImage( lsImage );
      duplicator->Update();

      outputLS->SetImage( duplicator->GetOutput() );

      output->AddLevelSet( id, outputLS, true );
      ++it;
      }
    }

private:
  LevelSetContainerToLevelSetContainerFilter( const Self& );
  void operator = ( const Self& );
};

template< class TIdentifier, typename TOutput, unsigned int VDimension >
class LevelSetContainerToLevelSetContainerFilter<
    SparseLevelSetContainer< TIdentifier, WhitakerSparseLevelSetImage< TOutput, VDimension > > >:
public LevelSetContainerToLevelSetContainerFilterBase<
    SparseLevelSetContainer< TIdentifier, WhitakerSparseLevelSetImage< TOutput, VDimension > > >
{
public:
  typedef WhitakerSparseLevelSetImage< TOutput, VDimension >    LevelSetType;
  typedef SparseLevelSetContainer< TIdentifier, LevelSetType >  LevelSetContainerType;

  /** Standard class typedefs. */
  typedef LevelSetContainerToLevelSetContainerFilter                      Self;
  typedef LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainerType >                                               Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetContainerToLevelSetContainerFilterBase,
               LevelSetContainerSource);

  /** Some convenient typedefs. */
  typedef typename LevelSetContainerType::Pointer                 LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetIdentifierType  LevelSetIdentifierType;

  typedef typename LevelSetContainerType::LevelSetPointer LevelSetPointer;

  typedef typename LevelSetType::LabelType    LevelSetLabelType;
  typedef typename LevelSetType::LabelMapType LevelSetLabelMapType;
  typedef typename LevelSetType::LayerMapType LevelSetLayerMapType;

protected:
  void CopyInputToOutput()
    {
    const LevelSetContainerType* input = this->GetInput();
    LevelSetContainerType* output = this->GetOutput();

    typename LevelSetContainerType::ConstIterator it = input->Begin();

    while( it != input->End )
      {
      LevelSetType* ls = it->GetLevelSet();

      LevelSetIdentifierType id = it->GetIdentifier();

      // copy the label map

      // get the layers

      LevelSetPointer outputLS = LevelSetType::New();

      output->AddLevelSet( id, outputLS, true );
      ++it;
      }
    }
};

template< unsigned int VDimension >
class LevelSetContainerToLevelSetContainerFilter< ShiSparseLevelSetImage< VDimension > > :
public LevelSetContainerToLevelSetContainerFilterBase< ShiSparseLevelSetImage< VDimension > >
{

};

template< unsigned int VDimension >
class LevelSetContainerToLevelSetContainerFilter< MalcolmSparseLevelSetImage< VDimension > > :
public LevelSetContainerToLevelSetContainerFilterBase< MalcolmSparseLevelSetImage< VDimension > >
{

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetContainerToLevelSetContainerFilter.hxx"
#endif

#endif
