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
#ifndef itkImageMaskSpatialObject_h
#define itkImageMaskSpatialObject_h

#include "itkNumericTraits.h"
#include "itkImageSpatialObject.h"
#include "itkImageSliceConstIteratorWithIndex.h"

namespace itk
{
/**
 * \class ImageMaskSpatialObject
 * \brief Implementation of an image mask as spatial object.
 *
 * This class derives from the ImageSpatialObject and overloads the
 * IsInsideInObjectSpace()
 * method.  One of the common uses of this class is to serve as Mask for the
 * Image Registration Metrics.
 *
 * \note The bounding box of an image mask is defined in such a way that
 * any point whose nearest pixel has a non-zero value (or matches the
 * specified mask value) is inside the bounding box. When all the pixels of
 * an image are zero, the bounding box of the image mask is empty, and
 * its bounds are all zero.
 *
 * \sa ImageSpatialObject SpatialObject CompositeSpatialObject
 * \ingroup ITKSpatialObjects
 */

template <unsigned int TDimension = 3, typename TPixel = unsigned char>
class ITK_TEMPLATE_EXPORT ImageMaskSpatialObject : public ImageSpatialObject<TDimension, TPixel>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ImageMaskSpatialObject);

  using Self = ImageMaskSpatialObject<TDimension, TPixel>;
  using Superclass = ImageSpatialObject<TDimension, TPixel>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  using typename Superclass::ScalarType;
  using typename Superclass::PixelType;
  using typename Superclass::ImageType;
  using typename Superclass::ImagePointer;
  using typename Superclass::IndexType;
  using typename Superclass::RegionType;
  using typename Superclass::SizeType;
  using typename Superclass::TransformType;
  using typename Superclass::PointType;
  using typename Superclass::BoundingBoxType;
  using typename Superclass::InterpolatorType;

  using SliceIteratorType = itk::ImageSliceConstIteratorWithIndex<ImageType>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ImageMaskSpatialObject);

  /** Test whether a point is inside the object.
   *
   * A point is inside the image mask when the value of its nearest pixel is
   * non-zero or matches the Mask_Value value.
   *
   * For computational speed purposes, it is faster if the method does not  check the name of the class and the
   * current depth.
   */
  bool
  IsInsideInObjectSpace(const PointType & point) const override;

  /** Specify the value in the mask image that defines the object.
   * You must also call SetUseMaskValue(true) to enable the use
   * of the mask value, otherwise the value is ignored and any
   * non-zero value in the mask is used to define the object. */
  /** @ITKStartGrouping */
  itkSetMacro(MaskValue, PixelType);
  itkGetConstReferenceMacro(MaskValue, PixelType);
  /** @ITKEndGrouping */
  itkBooleanMacro(UseMaskValue);
  itkSetMacro(UseMaskValue, bool);
  itkGetConstReferenceMacro(UseMaskValue, bool);

  /* Avoid hiding the overload that supports depth and name arguments */
  using Superclass::IsInsideInObjectSpace;

  /** Computes the bounding box of the image mask, in the index space of the image.
   * The bounding box is returned as an image region. Each call to this function
   * will recompute the region.
   * This function is useful in cases, where you may have a mask image
   * resulting from say a segmentation and you want to get the smallest box
   * region that encapsulates the mask image.
   *
   * \note This function is introduced with ITK 5.0, replacing
   * `GetAxisAlignedBoundingBoxRegion()`.
   */
  RegionType
  ComputeMyBoundingBoxInIndexSpace() const;

#if !defined(ITK_LEGACY_REMOVE)
  /** Compute axis aligned bounding box from the image mask.
   * \note With ITK 5.0, this function is superseded by `ComputeMyBoundingBoxInIndexSpace()`
   */
  itkLegacyMacro(RegionType GetAxisAlignedBoundingBoxRegion() const;)

#endif

protected:
  /** Get the boundaries of a specific object.  This function needs to
   *  be called every time one of the object's components is
   *  changed. */
  void
  ComputeMyBoundingBox() override;

  ImageMaskSpatialObject();
  ~ImageMaskSpatialObject() override = default;

  [[nodiscard]] typename LightObject::Pointer
  InternalClone() const override;

private:
  bool      m_UseMaskValue{ false };
  PixelType m_MaskValue{ NumericTraits<PixelType>::OneValue() };
};
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkImageMaskSpatialObject.hxx"
#endif

#endif // itkImageMaskSpatialObject_h
