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
#ifndef itkMRASlabIdentifier_h
#define itkMRASlabIdentifier_h

#include "itkObject.h"
#include "itkImage.h"
#include <vector>

namespace itk
{
/**
 * \class MRASlabIdentifier
 * \brief Identifies slabs in MR images comparing minimum intensity averages.
 *
 * This class is templated over the type of image.
 * In many cases, a 3D MR image is constructed by merging smaller 3D
 * blocks (slabs) which were acquired with different settings such as magnetic
 * settings and patient positions. Therefore, stripe like patterns with slabs
 * can be present in the resulting image. Such artifacts are called "slab
 * boundary" artifacts or "venetian blind" artifacts.
 *
 * Due to the slab boundary artifacts in an image, even same tissue class'
 * intensity values might vary significantly along the borders of slabs.
 * Such rough value changes are not appropriate for some image processing
 * methods. For example, MRIBiasFieldCorrectionFilter assumes a smooth bias
 * field. However, with the slab boundary artifacts, the bias field estimation
 * scheme that MRIBiasFieldCorrectionFilter uses might not adapt well.
 * The MRIBiasFieldCorrectionFilter creates regions for slabs using the
 * MRASlabIdentifier and then applies its bias correction scheme to each slab.
 *
 * For this identifier, a slice means 2D image data which is extracted from
 * the input image along one of three axes \f$(x, y, z)\f$. Users can specify
 * the slicing axis using the SetSlicingDirection(int dimension) member, where
 * the \p dimension variable follows the convention \f${X, Y, Z} : {0, 1, 2}\f$.
 *
 * The identification scheme used works according to the following steps:
 *   -# Users should specify how many pixels per slice the identifier
 *      will sample.
 *   -# For each slice, the identifier searches the specified number of pixels
 *      of which intensity values are greater than 0 and less than those
 *      of the other pixels in the slice.
 *   -# The identifier calculates the average for each slice and the overall
 *      average using the search results.
 *   -# For each slice, it subtracts the overall average from the slice average.
 *      If the sign of the subtraction result changes, then it assumes that a
 *      slab ends and another slab begins.
 *
 * \ingroup ITKBiasCorrection
 */
template <typename TInputImage>
class ITK_TEMPLATE_EXPORT MRASlabIdentifier : public Object
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(MRASlabIdentifier);

  /** Standard class type aliases. */
  using Self = MRASlabIdentifier;
  using Superclass = Object;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MRASlabIdentifier);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type definition for the input image. */
  using ImageType = TInputImage;

  /** Pointer type for the image. */
  using ImagePointer = typename TInputImage::Pointer;

  /** Const Pointer type for the image. */
  using ImageConstPointer = typename TInputImage::ConstPointer;

  /** Type definition for the input image pixel type. */
  using ImagePixelType = typename TInputImage::PixelType;
  using ImageIndexType = typename TInputImage::IndexType;
  using ImageSizeType = typename TInputImage::SizeType;
  using ImageRegionType = typename TInputImage::RegionType;
  using SlabRegionVectorType = std::vector<ImageRegionType>;

  /** Set/Get the input image. */
  /** @ITKStartGrouping */
  itkSetConstObjectMacro(Image, ImageType);
  itkGetConstObjectMacro(Image, ImageType);
  /** @ITKEndGrouping */
  /** Set/Get the number of minimum intensity pixels per slice. */
  /** @ITKStartGrouping */
  itkSetMacro(NumberOfSamples, unsigned int);
  itkGetConstReferenceMacro(NumberOfSamples, unsigned int);
  /** @ITKEndGrouping */
  /** Set/Get the minimum threshold value for the background pixels */
  /** @ITKStartGrouping */
  itkSetMacro(BackgroundMinimumThreshold, ImagePixelType);
  itkGetConstReferenceMacro(BackgroundMinimumThreshold, ImagePixelType);
  /** @ITKEndGrouping */
  /** Set/Get the tolerance value. */
  /** @ITKStartGrouping */
  itkSetMacro(Tolerance, double);
  itkGetConstReferenceMacro(Tolerance, double);
  /** @ITKEndGrouping */
  /** Set/Get the direction of slicing/
   * 0 - x axis, 1 - y axis, 2 - z axis */
  /** @ITKStartGrouping */
  itkSetMacro(SlicingDirection, int);
  itkGetConstReferenceMacro(SlicingDirection, int);
  /** @ITKEndGrouping */
  /** Compute the average values of minimum intensity pixels for each slice and
   * compare the average values with overall averages. */
  void
  GenerateSlabRegions();

  /** Get slab regions. */
  SlabRegionVectorType
  GetSlabRegionVector();

protected:
  MRASlabIdentifier();
  ~MRASlabIdentifier() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  /** Target image pointer that MRASlabIdentifier will use. */
  ImageConstPointer m_Image{};

  /** The number of pixels per slice which will be included
   * for average calculation. In a sense, it's sampling size per slice. */
  unsigned int m_NumberOfSamples{};

  int                  m_SlicingDirection{};
  ImagePixelType       m_BackgroundMinimumThreshold{};
  double               m_Tolerance{};
  SlabRegionVectorType m_Slabs{};
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkMRASlabIdentifier.hxx"
#endif

#endif /* itkMRASlabIdentifier_h */
