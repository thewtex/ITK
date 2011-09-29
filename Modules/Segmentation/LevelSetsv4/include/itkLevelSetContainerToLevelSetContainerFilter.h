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

#include "itkLevelSetDenseImageBase.h"
#include "itkWhitakerSparseLevelSetImage.h"
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
  void SetInput(const InputLevelSetContainerType *input);

  /** Get the LevelSetContainer input of this process object.  */
  const InputLevelSetContainerType * GetInput(void) const;

  const InputLevelSetContainerType * GetInput(unsigned int idx) const;

protected:
  LevelSetContainerToLevelSetContainerFilter();
  ~LevelSetContainerToLevelSetContainerFilter() {}

private:
  LevelSetContainerToLevelSetContainerFilter(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented
};

template< class TLevelSetContainer >
class LevelSetContainerToLevelSetContainerFilter
{};

template< class TImage >
class LevelSetContainerToLevelSetContainerFilter< LevelSetDenseImageBase< TImage > :
public LevelSetContainerToLevelSetContainerFilter< LevelSetDenseImageBase< TImage > >
{

};

template< typename TOutput, unsigned int VDimension >
class LevelSetContainerToLevelSetContainerFilter< WhitakerSparseLevelSetImage< TOutput, VDimension > > :
public LevelSetContainerToLevelSetContainerFilterBase< WhitakerSparseLevelSetImage< TOutput, VDimension > >
{

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
