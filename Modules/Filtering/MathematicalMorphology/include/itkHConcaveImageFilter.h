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
#ifndef itkHConcaveImageFilter_h
#define itkHConcaveImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
/** \class HConcaveImageFilter
 * \brief Identify local minima whose depth below the baseline is greater than h.
 *
 * HConcaveImageFilter extract local minima that are more than h
 * intensity units below the (local) background. This has the effect
 * of extracting objects that are darker than the background by at least
 * h intensity units.
 *
 * This filter uses the HMinimaImageFilter.
 *
 * Geodesic morphology and the H-Convex algorithm is described in
 * Chapter 6 of Pierre Soille's book "Morphological Image Analysis:
 * Principles and Applications", Second Edition, Springer, 2003.
 *
 * \sa GrayscaleGeodesicDilateImageFilter, HMaximaImageFilter,
 * \sa MorphologyImageFilter, GrayscaleDilateImageFilter, GrayscaleFunctionDilateImageFilter, BinaryDilateImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 * \ingroup ITKMathematicalMorphology
 */
template <typename TInputImage, typename TOutputImage>
class ITK_TEMPLATE_EXPORT HConcaveImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(HConcaveImageFilter);

  /** Standard class type aliases. */
  using Self = HConcaveImageFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Some convenient type alias. */
  using InputImageType = TInputImage;
  using InputImagePointer = typename InputImageType::Pointer;
  using InputImageConstPointer = typename InputImageType::ConstPointer;
  using InputImageRegionType = typename InputImageType::RegionType;
  using InputImagePixelType = typename InputImageType::PixelType;
  using OutputImageType = TOutputImage;
  using OutputImagePointer = typename OutputImageType::Pointer;
  using OutputImageConstPointer = typename OutputImageType::ConstPointer;
  using OutputImageRegionType = typename OutputImageType::RegionType;
  using OutputImagePixelType = typename OutputImageType::PixelType;

  /** ImageDimension constants */
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

  /** Standard New method. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(HConcaveImageFilter);

  /** Set/Get the height that a local maximum must be above the local
   * background (local contrast) in order to survive the
   * processing. Local maxima below this value are replaced with an
   * estimate of the local background. */
  itkSetMacro(Height, InputImagePixelType);
  itkGetConstMacro(Height, InputImagePixelType);

  /**
   * Set/Get whether the connected components are defined strictly by
   * face connectivity or by face+edge+vertex connectivity.  Default is
   * FullyConnectedOff.  For objects that are 1 pixel wide, use
   * FullyConnectedOn.
   */
  itkSetMacro(FullyConnected, bool);
  itkGetConstReferenceMacro(FullyConnected, bool);
  itkBooleanMacro(FullyConnected);

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro(InputEqualityComparableCheck, (Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(IntConvertibleToInputCheck, (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(InputOStreamWritableCheck, (Concept::OStreamWritable<InputImagePixelType>));
  // End concept checking
#endif

protected:
  HConcaveImageFilter();
  ~HConcaveImageFilter() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** HConcaveImageFilter needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void
  GenerateInputRequestedRegion() override;

  /** HConcaveImageFilter will produce the entire output. */
  void
  EnlargeOutputRequestedRegion(DataObject * itkNotUsed(output)) override;

  /** Single-threaded version of GenerateData.  This filter delegates
   * to GrayscaleGeodesicErodeImageFilter. */
  void
  GenerateData() override;

private:
  InputImagePixelType m_Height{};
  unsigned long       m_NumberOfIterationsUsed{ 1 };
  bool                m_FullyConnected{ false };
}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkHConcaveImageFilter.hxx"
#endif

#endif
