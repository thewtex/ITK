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
#ifndef __itkImageScanlineConstIterator_h
#define __itkImageScanlineConstIterator_h

#include "itkImageIterator.h"

namespace itk
{
/** \class ImageScanlineConstIterator
 * \brief A multi-dimensional iterator templated over image type that walks a
 * region of pixels.
 *
 * The itk::ImageScanlineConstIterator is optimized...
 * \code
 *
 *      it = ImageScanlineConstIterator()
 *      while ( !it.IsAtEnd() )
 *        {
 *        while ( !it.IsAtEndOfLine() )
 *          {
 *          *it += 100.0;
 *          ++it;
 *          }
 *        it.NextLine();
 *        }
 *
 * \endcode
 *
 * \ingroup ITKCommon
 *
 */
template< typename TImage >
class ITK_EXPORT ImageScanlineConstIterator:public ImageConstIterator< TImage >
{
public:
  /** Standard class typedef. */
  typedef ImageScanlineConstIterator   Self;
  typedef ImageConstIterator< TImage > Superclass;

  /** Dimension of the image that the iterator walks.  This constant is needed so
   * functions that are templated over image iterator type (as opposed to
   * being templated over pixel type and dimension) can have compile time
   * access to the dimension of the image that the iterator walks. */
  itkStaticConstMacro(ImageIteratorDimension, unsigned int,
                      Superclass::ImageIteratorDimension);

  /**
   * Index typedef support. While these were already typdef'ed in the superclass,
   * they need to be redone here for this subclass to compile properly with gcc.
   */
  /** Types inherited from the Superclass */
  typedef typename Superclass::IndexType             IndexType;
  typedef typename Superclass::SizeType              SizeType;
  typedef typename Superclass::OffsetType            OffsetType;
  typedef typename Superclass::RegionType            RegionType;
  typedef typename Superclass::ImageType             ImageType;
  typedef typename Superclass::PixelContainer        PixelContainer;
  typedef typename Superclass::PixelContainerPointer PixelContainerPointer;
  typedef typename Superclass::InternalPixelType     InternalPixelType;
  typedef typename Superclass::PixelType             PixelType;
  typedef typename Superclass::AccessorType          AccessorType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageScanlineConstIterator, ImageConstIterator);

  /** Default constructor. Needed since we provide a cast constructor. */
  ImageScanlineConstIterator():ImageConstIterator< TImage >()
  {
    m_SpanBeginOffset = 0;
    m_SpanEndOffset = 0;
  }

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. */
  ImageScanlineConstIterator(const ImageType *ptr,
                           const RegionType & region):
    ImageConstIterator< TImage >(ptr, region)
  {
    m_SpanBeginOffset = this->m_BeginOffset;
    m_SpanEndOffset   = this->m_BeginOffset + static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Constructor that can be used to cast from an ImageIterator to an
   * ImageScanlineConstIterator. Many routines return an ImageIterator, but for a
   * particular task, you may want an ImageScanlineConstIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageScanlineConstIterator. */
  ImageScanlineConstIterator(const ImageIterator< TImage > & it)
  {
    this->ImageConstIterator< TImage >::operator=(it);

    IndexType ind = this->GetIndex();
    m_SpanEndOffset = this->m_Offset + static_cast< OffsetValueType >( this->m_Region.GetSize()[0] )
                      - ( ind[0] - this->m_Region.GetIndex()[0] );
    m_SpanBeginOffset = m_SpanEndOffset
                        - static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Constructor that can be used to cast from an ImageConstIterator to an
   * ImageScanlineConstIterator. Many routines return an ImageIterator, but for a
   * particular task, you may want an ImageScanlineConstIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageScanlineConstIterator. */
  ImageScanlineConstIterator(const ImageConstIterator< TImage > & it)
  {
    this->ImageConstIterator< TImage >::operator=(it);

    IndexType ind = this->GetIndex();
    m_SpanEndOffset = this->m_Offset + static_cast< OffsetValueType >( this->m_Region.GetSize()[0] )
                      - ( ind[0] - this->m_Region.GetIndex()[0] );
    m_SpanBeginOffset = m_SpanEndOffset
                        - static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Move an iterator to the beginning of the region. "Begin" is
   * defined as the first pixel in the region. */
  void GoToBegin()
  {
    Superclass::GoToBegin();

    // reset the span offsets
    m_SpanBeginOffset = this->m_BeginOffset;
    m_SpanEndOffset   = this->m_BeginOffset + static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Move an iterator to the end of the region. "End" is defined as
   * one pixel past the last pixel of the region. */
  void GoToEnd()
  {
    Superclass::GoToEnd();

    // reset the span offsets
    m_SpanEndOffset = this->m_EndOffset;
    m_SpanBeginOffset = m_SpanEndOffset - static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Go to the beginning pixel of the current line.
   * \sa operator++ \sa operator-- \sa NextLine \sa IsAtEndOfLine */
  void GoToBeginOfLine(void)
  {
    this->Offset = m_SpanBeginOffset;
  }

  /** Go to the past end pixel of the current line.
   * \sa GoToBeginOfLine \sa operator++ \sa operator-- \sa NextLine \sa IsAtEndOfLine */
  void GoToEndOfLine(void)
  {
    this->Offset = m_SpanEndOffset;
  }

  /** Test if the index is at the end of line */
  inline bool IsAtEndOfLine(void)
  {
    return this->m_Offset >= m_SpanEndOffset;
  }


  /** Set the index. No bounds checking is performed. This is overridden
   * from the parent because we have an extra ivar.
   * \sa GetIndex */
  void SetIndex(const IndexType & ind)
  {
    Superclass::SetIndex(ind);
    m_SpanEndOffset = this->m_Offset + static_cast< OffsetValueType >( this->m_Region.GetSize()[0] )
                      - ( ind[0] - this->m_Region.GetIndex()[0] );
    m_SpanBeginOffset = m_SpanEndOffset - static_cast< OffsetValueType >( this->m_Region.GetSize()[0] );
  }

  /** Go to the next line.
   * \sa operator++  \sa operator-- \sa IsAtEndOfLine \sa PreviousLine \sa End */
  inline void NextLine(void)
  {
    this->Increment();
  };

  /** Increment (prefix) the fastest moving dimension of the iterator's index.
   * This operator will constrain the iterator within the region (i.e. the
   * iterator will automatically wrap from the end of the row of the region
   * to the beginning of the next row of the region) up until the iterator
   * tries to moves past the last pixel of the region.  Here, the iterator
   * will be set to be one pixel past the end of the region.
   * \sa operator++(int) */
  Self &
  operator++()
  {
    ++this->m_Offset;
    return *this;
  }


protected:
  OffsetValueType m_SpanBeginOffset; // one pixel before the beginning of the span
                                   // (row)
  OffsetValueType m_SpanEndOffset;   // one pixel past the end of the span (row)

private:
  void Increment(); // advance in a direction other than the fastest moving

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageScanlineConstIterator.hxx"
#endif

#endif
