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
#ifndef __itkImageFastLinearIterator_h
#define __itkImageFastLinearIterator_h

#include "itkImageFastLinearConstIterator.h"

namespace itk
{
/** \class ImageFastLinearIterator
 * \brief A multi-dimensional iterator templated over image type that walks a
 * region of pixels.
 *
 * The itk::ImageFastLinearIterator is optimized for iteration speed and is the
 * first choice for iterative, pixel-wise operations on an image.
 * ImageFastLinearIterator is the least specialized of the ITK image iterator
 * classes.  ImageFastLinearIterator is templated over the image type, and is
 * constrained to walk only within the specified region and along a line
 * parallel to one of the coordinate axes, "wrapping" to the next line as it
 * reaches the boundary of the image.  To walk the entire image, specify the
 * region to be \c image->GetRequestedRegion().
 *
 * Most of the functionality is inherited from the ImageFastLinearConstIterator.
 * The current class only adds write access to image pixels.
 *
 * \par MORE INFORMATION
 * For a complete description of the ITK Image Iterators and their API, please
 * see the Iterators chapter in the ITK Software Guide.  The ITK Software Guide
 * is available in print and as a free .pdf download from http://www.itk.org.
 *
 * example ImageFastLinearIterator.cxx
 * \ingroup ImageIterators
 *
 * \ingroup ITKCommon
 *
 * \wiki
 * \wikiexample{Iterators/ImageFastLinearIterator,Iterate over a region of an image (with write access)}
 * \endwiki
 */
template< typename TImage >
class ITK_EXPORT ImageFastLinearIterator:public ImageFastLinearConstIterator< TImage >
{
public:
  /** Standard class typedefs. */
  typedef ImageFastLinearIterator                Self;
  typedef ImageFastLinearConstIterator< TImage > Superclass;

  /** Types inherited from the Superclass */
  typedef typename Superclass::IndexType             IndexType;
  typedef typename Superclass::SizeType              SizeType;
  typedef typename Superclass::OffsetType            OffsetType;
  typedef typename Superclass::RegionType            RegionType;
  typedef typename Superclass::ImageType             ImageType;
  typedef typename Superclass::PixelContainer        PixelContainer;
  typedef typename Superclass::PixelContainerPointer PixelContainerPointer;\
  typedef typename Superclass::InternalPixelType     InternalPixelType;
  typedef typename Superclass::PixelType             PixelType;
  typedef typename Superclass::AccessorType          AccessorType;

  /** Default constructor. Needed since we provide a cast constructor. */
  ImageFastLinearIterator();

  /** Constructor establishes an iterator to walk a particular image and a
   * particular region of that image. */
  ImageFastLinearIterator(ImageType *ptr, const RegionType & region);

  /** Constructor that can be used to cast from an ImageIterator to an
   * ImageFastLinearIterator. Many routines return an ImageIterator but for a
   * particular task, you may want an ImageFastLinearIterator.  Rather than
   * provide overloaded APIs that return different types of Iterators, itk
   * returns ImageIterators and uses constructors to cast from an
   * ImageIterator to a ImageFastLinearIterator. */
  ImageFastLinearIterator(const ImageIterator< TImage > & it);

  /** Set the pixel value */
  void Set(const PixelType & value) const
  {
    this->m_PixelAccessorFunctor.Set(*( const_cast< InternalPixelType * >(
                                          this->m_Buffer + this->m_Offset ) ), value);
  }

  /** Return a reference to the pixel
   * This method will provide the fastest access to pixel
   * data, but it will NOT support ImageAdaptors. */
  PixelType & Value(void)
  { return *( const_cast< InternalPixelType * >( this->m_Buffer + this->m_Offset ) ); }

protected:
  /** the construction from a const iterator is declared protected
      in order to enforce const correctness. */
  ImageFastLinearIterator(const ImageFastLinearConstIterator< TImage > & it);
  Self & operator=(const ImageFastLinearConstIterator< TImage > & it);
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageFastLinearIterator.hxx"
#endif

#endif
