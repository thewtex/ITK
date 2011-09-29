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
#include "itkLevelSetContainer.h"

#include "itkWhitakerSparseLevelSetImage.h"
#include "itkLevelSetContainer.h"

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

template< class TIdentifier, class TImage >
class LevelSetContainerToLevelSetContainerFilter<
    LevelSetContainer< TIdentifier, LevelSetDenseImageBase< TImage > >
    > :
public LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainer< TIdentifier, LevelSetDenseImageBase< TImage > > >
{
public:
  typedef LevelSetDenseImageBase< TImage >                LevelSetType;
  typedef LevelSetContainer< TIdentifier, LevelSetType >  LevelSetContainerType;

  /** Standard class typedefs. */
  typedef LevelSetContainerToLevelSetContainerFilter                      Self;
  typedef LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainerType >                                               Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetContainerToLevelSetContainerFilter,
               LevelSetContainerToLevelSetContainerFilterBase);

  /** Some convenient typedefs. */
  typedef typename LevelSetContainerType::Pointer                 LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetIdentifierType  LevelSetIdentifierType;

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

    output->SetHeaviside( input->GetHeaviside() );
    output->SetDomainMapFilter( input->GetDomainMapFilter() );

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

template< class TIdentifier, class TLevelSet >
class LevelSetContainerToLevelSetContainerFilter<
    LevelSetContainer< TIdentifier, TLevelSet > >:
public LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainer< TIdentifier, TLevelSet > >
{
public:
  typedef LevelSetContainer< TIdentifier, TLevelSet > LevelSetContainerType;

  /** Standard class typedefs. */
  typedef LevelSetContainerToLevelSetContainerFilter                      Self;
  typedef LevelSetContainerToLevelSetContainerFilterBase<
    LevelSetContainerType >                                               Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LevelSetContainerToLevelSetContainerFilter,
               LevelSetContainerToLevelSetContainerFilterBase);

  /** Some convenient typedefs. */
  typedef typename LevelSetContainerType::Pointer                 LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetIdentifierType  LevelSetIdentifierType;

  typedef TLevelSet                                       LevelSetType;
  typedef typename LevelSetContainerType::LevelSetPointer LevelSetPointer;

  typedef typename LevelSetType::LabelMapType     LevelSetLabelMapType;
  typedef typename LevelSetType::LabelMapPointer  LevelSetLabelMapPointer;

  typedef typename LevelSetType::LayerMapType       LevelSetLayerMapType;
  typedef typename LevelSetType::LayerIdType        LevelSetLayerIdType;
  typedef typename LevelSetType::LayerIdListType    LevelSetLayerIdListType;
  typedef typename LevelSetType::LayerType          LevelSetLayerType;
  typedef typename LevelSetType::LayerConstIterator LevelSetLayerConstIterator;

protected:
  LevelSetContainerToLevelSetContainerFilter() {}
  ~LevelSetContainerToLevelSetContainerFilter() {}

  void CopyInputToOutput()
    {
    const LevelSetContainerType* input = this->GetInput();
    LevelSetContainerType* output = this->GetOutput();

    output->SetHeaviside( input->GetHeaviside() );
    output->SetDomainMapFilter( input->GetDomainMapFilter() );

    typename LevelSetContainerType::ConstIterator it = input->Begin();

    while( it != input->End )
      {
      LevelSetType* ls = it->GetLevelSet();

      LevelSetIdentifierType id = it->GetIdentifier();

      LevelSetPointer outputLS = LevelSetType::New();

      // ------------------
      // copy the label map
      LevelSetLabelMapType* labelmap = ls->GetLabelMap();

      LevelSetLabelMapPointer outputLabelMap = LevelSetLabelMapType::New();
      outputLabelMap->SetRegions( labelmap->GetLargestPossibleRegion() );
      outputLabelMap->SetBackgroundValue( labelmap->GetBackgroundValue() );

      typename LevelSetLabelMapType::ConstIterator labelmapIt( labelmap );
      labelmapIt.GoToBegin();

      while( !labelmapIt.IsAtEnd() )
        {
        outputLabelMap->AddLabelObject( labelmapIt->GetLabelObject() );
        ++labelmapIt;
        }

      outputLabelMap->Optimize();

      outputLS->SetLabelMap( outputLabelMap );

      // ------------------
      // get the layers

      LevelSetLayerIdListType layerIdList = ls->GetLabelList();

      typename LevelSetLayerIdListType::const_iterator layerIt = layerIdList.begin();

      while( layerIt != layerIdList.end() )
        {
        const LevelSetLayerType& layer = ls->GetLayer( *layerIt );

        if( !layer.empty() )
          {
          LevelSetLayerType& outputLayer = outputLS->GetLayer( *layerIt );

          outputLayer.insert( layer.begin(), layer.end() );
          outputLS->SetLayer( layerIt, outputLayer );
          }
        ++layerIt;
        }

      output->AddLevelSet( id, outputLS, true );
      ++it;
      }
    }

private:
  LevelSetContainerToLevelSetContainerFilter( const Self& );
  void operator = ( const Self& );
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetContainerToLevelSetContainerFilter.hxx"
#endif

#endif
