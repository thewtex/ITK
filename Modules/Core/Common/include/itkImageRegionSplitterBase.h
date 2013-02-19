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
#ifndef __itkImageRegionSplitterBase_h
#define __itkImageRegionSplitterBase_h

#include "itkImageRegion.h"
#include "itkObjectFactory.h"

namespace itk
{
/** \class ImageRegionSplitterBase
 * \ingroup ITKCommon
 */

class ITKCommon_EXPORT ImageRegionSplitterBase
  :public Object
{
public:
  /** Standard class typedefs. */
  typedef ImageRegionSplitterBase    Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageRegionSplitter, Object);

    template <unsigned int VImageDimension>
    unsigned int GetNumberOfSplits(const ImageRegion<VImageDimension> & region,
                                   unsigned int requestedNumber) const
    {
      return this->GetNumberOfSplits( VImageDimension,
                                      region.GetIndex().m_Index,
                                      region.GetSize().m_Size );
    }

    template <unsigned int VImageDimension>
    unsigned int GetSplit(unsigned int i,
                          unsigned int numberOfPieces,
                          ImageRegion<VImageDimension> & region) const
    {
      typedef ImageRegion<VImageDimension> ImageRegionType;

      return this->GetSplit( VImageDimension,
                             i,
                             numberOfPieces,
// I am a bad person for using const_cast, I should look into how to
// modify the API...
                             const_cast<typename ImageRegionType::IndexType&>(region.GetIndex()).m_Index,
                             const_cast<typename ImageRegionType::SizeType&>(region.GetSize()).m_Size );
    }

protected:
    ImageRegionSplitterBase();


  virtual unsigned int GetNumberOfSplits(unsigned int dim,
                                         const IndexValueType regionIndex[],
                                         const SizeValueType regionSize[],
                                         unsigned int requestedNumber) const;
  virtual unsigned int GetSplit(unsigned int dim,
                                unsigned int i,
                                unsigned int numberOfPieces,
                                IndexValueType regionIndex[],
                                SizeValueType regionSize[]) const;

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  ImageRegionSplitterBase(const ImageRegionSplitterBase &); //purposely not implemented
  void operator=(const ImageRegionSplitterBase &);      //purposely not implemented
};
} // end namespace itk

#endif
