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

#include "itkImageIORegion.h"

namespace itk
{

class ImageIORegion;

/** \class ImageRegionSplitterBase
 * \brief Divide an image region into several pieces.
 *
 * ImageRegionSplitterBase divides an ImageRegion into smaller regions.
 * ImageRegionSplitterBase is used by the ImageSource,
 * StreamingImageFilter, streaming ImageIO classes to divide a region
 * into a series of smaller subregions.
 *
 * This object has two basic methods: GetNumberOfSplits() and
 * GetSplit().
 *
 * GetNumberOfSplits() is used to determine how may subregions a given
 * region can be divided.  You call GetNumberOfSplits with an argument
 * that is the number of subregions you want.  If the image region can
 * support that number of subregions, that number is returned.
 * Otherwise, the maximum number of splits less then or equal to the
 * argumen  be returned.  For example, if a region splitter class only divides
 * a region into horizontal slabs, then the maximum number of splits
 * will be the number of rows in the region.
 *
 * GetSplit() returns the ith of N subregions (as an ImageRegion object).
 *
 * This ImageRegionSplitterBase class divides a region along the
 * outermost dimension. If the outermost dimension has size 1 (i.e. a
 * volume with a single slice), the ImageRegionSplitter will divide
 * the region along the next outermost dimension. If that dimension
 * has size 1, the process continues with the next outermost
 * dimension.
 *
 * \sa ImageRegionSplitterDirection
 *
 * \ingroup ITKSystemObjects
 * \ingroup DataProcessing
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
  itkTypeMacro(ImageRegionSplitterBase, Object);

  /** How many pieces can the specified region be split? A given region
   * cannot always be divided into the requested number of pieces. For
   * instance, if the numberOfPieces exceeds the number of pixels along
   * a certain dimensions, then some splits will not be possible. This
   * method returns a number less than or equal to the requested number
   * of pieces. */
  template <unsigned int VImageDimension>
    unsigned int GetNumberOfSplits(const ImageRegion<VImageDimension> & region,
                                   unsigned int requestedNumber) const
  {
    return this->GetNumberOfSplits( VImageDimension,
                                    region.GetIndex().m_Index,
                                    region.GetSize().m_Size,
                                    requestedNumber);
  }
  inline unsigned int GetNumberOfSplits(const ImageIORegion &region,
                                        unsigned int requestedNumber) const
  {
    return this->GetNumberOfSplits( region.GetImageDimension(),
                                    &region.GetIndex()[0],
                                    &region.GetSize()[0],
                                    requestedNumber);
  }

  /** Get a region definition that represents the ith piece a specified region.
   * The "numberOfPieces" must be equal to what
   * GetNumberOfSplits() returns. */
  // The return value is the maximum number of splits, available, if
  // "i" is greater than or equal to the return value the value of the
  // region is undefined.
  template <unsigned int VImageDimension>
    unsigned int GetSplit( unsigned int i,
                           unsigned int numberOfPieces,
                           ImageRegion<VImageDimension> & region ) const
  {
    typedef ImageRegion<VImageDimension> ImageRegionType;

    return this->GetSplit( VImageDimension,
                           i,
                           numberOfPieces,
                           region.GetModifiableIndex().m_Index,
                           region.GetModifiableSize().m_Size );
  }
  unsigned int GetSplit( unsigned int i,
                         unsigned int numberOfPieces,
                         ImageIORegion & region ) const
  {
    return this->GetSplit( region.GetImageDimension(),
                           i,
                           numberOfPieces,
                           &region.GetModifiableIndex()[0],
                           &region.GetModifiableSize()[0] );
  }

protected:
  ImageRegionSplitterBase();


  virtual unsigned int GetNumberOfSplits( unsigned int dim,
                                          const IndexValueType regionIndex[],
                                          const SizeValueType regionSize[],
                                          unsigned int requestedNumber ) const;

  virtual unsigned int GetSplit( unsigned int dim,
                                 unsigned int i,
                                 unsigned int numberOfPieces,
                                 IndexValueType regionIndex[],
                                 SizeValueType regionSize[] ) const;

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  ImageRegionSplitterBase(const ImageRegionSplitterBase &); //purposely not implemented
  void operator=(const ImageRegionSplitterBase &);      //purposely not implemented
};
} // end namespace itk

#endif
