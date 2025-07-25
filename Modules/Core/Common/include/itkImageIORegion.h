/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkImageIORegion_h
#define itkImageIORegion_h

#include <algorithm>
#include "itkIntTypes.h"
#include "itkObjectFactory.h"
#include "itkImageRegion.h"

namespace itk
{
/** \class ImageIORegion
 * \brief An ImageIORegion represents a structured region of data.
 *
 * ImageIORegion is an class that represents some structured portion or
 * piece of an Image. The ImageIORegion is represented with an index and
 * a size in each of the n-dimensions of the image. (The index is the
 * corner of the image, the size is the lengths of the image in each of
 * the topological directions.)  ImageIORegion is not templated over
 * dimension, but uses dynamic arrays instead.
 *
 * The first pixel of an image always have a Zero index. Therefore the
 * index values of ImageIORegion may not directly correspond to those
 * of ImageRegion. When translation between the two is performed one
 * much consider the largest possible region who has a non-zero
 * starting index for the image.
 *
 * \sa Region
 * \sa ImageRegion
 * \sa Index
 * \sa Size
 * \sa MeshRegion
 * \ingroup ITKCommon
 */

class ITKCommon_EXPORT ImageIORegion : public Region
{
public:
  /** Standard class type aliases. */
  using Self = ImageIORegion;
  using Superclass = Region;

  /** these types correspond to those of itk::Size, itk::Offset and itk::Index
   */
  using SizeValueType = itk::SizeValueType;
  using IndexValueType = itk::IndexValueType;
  using OffsetValueType = itk::OffsetValueType;

  /** Index type alias support An index is used to access pixel values. */
  using IndexType = std::vector<IndexValueType>;

  /** Size type alias support A size is used to define region bounds. */
  using SizeType = std::vector<SizeValueType>;

  /** Region type taken from the superclass */
  using RegionType = Superclass::RegionEnum;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ImageIORegion);

  /** Dimension of the image available at run time. */
  [[nodiscard]] unsigned int
  GetImageDimension() const;

  /** Dimension of the region to be written. This differs from the
   * the image dimension and is calculated at run-time by examining
   * the size of the image in each coordinate direction. */
  [[nodiscard]] unsigned int
  GetRegionDimension() const;

  /** Return the region type. Images are described with structured regions. */
  [[nodiscard]] RegionType
  GetRegionType() const override;

  /** Constructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the constructor is public. */
  ImageIORegion(unsigned int dimension);

  /** Constructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the constructor is public.  Default dimension is 2. */
  ImageIORegion() = default;

  /** Destructor. ImageIORegion is a lightweight object that is not reference
   * counted, so the destructor is public. */
  ~ImageIORegion() override;

  /** Copy constructor. ImageIORegion is a lightweight object that is not
   * reference counted, so the copy constructor is public. */
  ImageIORegion(const Self &) = default;

  /** Move-constructor. Fast and non-throwing. */
  ImageIORegion(Self &&) = default;

  /** Copy-assignment operator. ImageIORegion is a lightweight object that is not reference
   * counted, so operator= is public. */
  Self &
  operator=(const Self & region);

  /** Move-assignment operator. Fast and non-throwing. */
  Self &
  operator=(Self &&) = default;

  /** Set the index defining the corner of the region. */
  void
  SetIndex(const IndexType & index);

  /** Get index defining the corner of the region. */
  /** @ITKStartGrouping */
  [[nodiscard]] const IndexType &
  GetIndex() const;
  IndexType &
  GetModifiableIndex();
  /** @ITKEndGrouping */
  /** Set the size of the region. This plus the index determines the
   * rectangular shape, or extent, of the region. */
  void
  SetSize(const SizeType & size);

  /** Get the size of the region. */
  /** @ITKStartGrouping */
  [[nodiscard]] const SizeType &
  GetSize() const;
  SizeType &
  GetModifiableSize();
  /** @ITKEndGrouping */
  /** Convenience methods to get the size of the image in a particular
   * coordinate direction i. Do not try to access image sizes beyond the
   * the ImageDimension. */
  [[nodiscard]] SizeValueType
  GetSize(unsigned int i) const;

  [[nodiscard]] IndexValueType
  GetIndex(unsigned int i) const;

  void
  SetSize(const unsigned int i, SizeValueType size);

  void
  SetIndex(const unsigned int i, IndexValueType idx);

  /** Compare two regions. */
  bool
  operator==(const Self & region) const;

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  /** Test if an index is inside */
  [[nodiscard]] bool
  IsInside(const IndexType & index) const;

  /** Test if a region (the argument) is completely inside of this region */
  [[nodiscard]] bool
  IsInside(const Self & otherRegion) const;

  /** Get the number of pixels contained in this region. This just
   * multiplies the size components. */
  [[nodiscard]] SizeValueType
  GetNumberOfPixels() const;

protected:
  /** Methods invoked by Print() to print information about the object
   * including superclasses. Typically not called by the user (use Print()
   * instead) but used in the hierarchical print process to combine the
   * output of several classes.  */
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  unsigned int m_ImageDimension{ 2 };
  IndexType    m_Index{ IndexType(2) };
  SizeType     m_Size{ SizeType(2) };
};


// Declare operator<<
extern ITKCommon_EXPORT std::ostream &
                        operator<<(std::ostream & os, const ImageIORegion & region);


/** \class ImageIORegionAdaptor
 * \brief Helper class for converting ImageRegions into ImageIORegions and back.
 *
 * \ingroup ITKCommon
 */
template <unsigned int VDimension>
class ImageIORegionAdaptor
{
public:
  using ImageRegionType = ImageRegion<VDimension>;
  using ImageIORegionType = ImageIORegion;

  using ImageSizeType = typename ImageRegionType::SizeType;
  using ImageIndexType = typename ImageRegionType::IndexType;

  static void
  Convert(const ImageRegionType & inImageRegion,
          ImageIORegionType &     outIORegion,
          const ImageIndexType &  largestRegionIndex)
  {
    //
    // The ImageRegion and ImageIORegion objects may have different dimensions.
    // Here we only copy the common dimensions between the two. If the
    // ImageRegion
    // has more dimensions than the ImageIORegion, then the defaults of the
    // ImageRegion
    // will take care of the remaining codimension. If the ImageRegion has less
    // dimensions
    // than the ImageIORegion, then the remaining IO dimensions are simply
    // ignored.
    //
    const unsigned int ioDimension = outIORegion.GetImageDimension();
    const unsigned int imageDimension = VDimension;

    const unsigned int minDimension = std::min(ioDimension, imageDimension);

    const ImageSizeType &  size = inImageRegion.GetSize();
    const ImageIndexType & index = inImageRegion.GetIndex();

    for (unsigned int i = 0; i < minDimension; ++i)
    {
      outIORegion.SetSize(i, size[i]);
      outIORegion.SetIndex(i, index[i] - largestRegionIndex[i]);
    }

    //
    // Fill in the remaining codimension (if any) with default values
    //
    for (unsigned int k = minDimension; k < ioDimension; ++k)
    {
      outIORegion.SetSize(k, 1); // Note that default size in IO is 1 not 0
      outIORegion.SetIndex(k, 0);
    }
  }

  static void
  Convert(const ImageIORegionType & inIORegion,
          ImageRegionType &         outImageRegion,
          const ImageIndexType &    largestRegionIndex)
  {
    auto           size = MakeFilled<ImageSizeType>(1); // initialize with default values
    ImageIndexType index{};

    //
    // The ImageRegion and ImageIORegion objects may have different dimensions.
    // Here we only copy the common dimensions between the two. If the
    // ImageRegion
    // has more dimensions than the ImageIORegion, then the defaults of the
    // ImageRegion
    // will take care of the remaining codimension. If the ImageRegion has less
    // dimensions
    // than the ImageIORegion, then the remaining IO dimensions are simply
    // ignored.
    //
    const unsigned int ioDimension = inIORegion.GetImageDimension();
    const unsigned int imageDimension = VDimension;

    const unsigned int minDimension = std::min(ioDimension, imageDimension);

    for (unsigned int i = 0; i < minDimension; ++i)
    {
      size[i] = inIORegion.GetSize(i);
      index[i] = inIORegion.GetIndex(i) + largestRegionIndex[i];
    }

    outImageRegion.SetSize(size);
    outImageRegion.SetIndex(index);
  }
};
} // end namespace itk

#endif
