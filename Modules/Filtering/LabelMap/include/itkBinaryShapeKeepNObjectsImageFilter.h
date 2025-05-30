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
#ifndef itkBinaryShapeKeepNObjectsImageFilter_h
#define itkBinaryShapeKeepNObjectsImageFilter_h

#include "itkShapeLabelObject.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"
#include "itkShapeKeepNObjectsLabelMapFilter.h"
#include "itkLabelMapToBinaryImageFilter.h"

namespace itk
{
/**
 * \class BinaryShapeKeepNObjectsImageFilter
 * \brief keep N objects according to their shape attributes
 *
 * BinaryShapeKeepNObjectsImageFilter keep the N objects in a binary image
 * with the highest (or lowest) attribute value. The attributes are the ones
 * of the ShapeLabelObject.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * https://doi.org/10.54294/q6auw4
 *
 * \sa ShapeLabelObject, LabelShapeKeepNObjectsImageFilter, BinaryStatisticsKeepNObjectsImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 * \ingroup ITKLabelMap
 */
template <typename TInputImage>
class ITK_TEMPLATE_EXPORT BinaryShapeKeepNObjectsImageFilter : public ImageToImageFilter<TInputImage, TInputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(BinaryShapeKeepNObjectsImageFilter);

  /** Standard class type aliases. */
  using Self = BinaryShapeKeepNObjectsImageFilter;
  using Superclass = ImageToImageFilter<TInputImage, TInputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Some convenient type alias. */
  using InputImageType = TInputImage;
  using OutputImageType = TInputImage;
  using InputImagePointer = typename InputImageType::Pointer;
  using InputImageConstPointer = typename InputImageType::ConstPointer;
  using InputImageRegionType = typename InputImageType::RegionType;
  using InputImagePixelType = typename InputImageType::PixelType;
  using OutputImagePointer = typename OutputImageType::Pointer;
  using OutputImageConstPointer = typename OutputImageType::ConstPointer;
  using OutputImageRegionType = typename OutputImageType::RegionType;
  using OutputImagePixelType = typename OutputImageType::PixelType;

  /** ImageDimension constants */
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;

  using LabelType = SizeValueType;

  using LabelObjectType = ShapeLabelObject<LabelType, Self::ImageDimension>;
  using LabelMapType = LabelMap<LabelObjectType>;
  using LabelizerType = BinaryImageToLabelMapFilter<InputImageType, LabelMapType>;
  using ShapeLabelFilterOutput = Image<typename OutputImageType::PixelType, Self::OutputImageDimension>;
  using LabelObjectValuatorType = ShapeLabelMapFilter<LabelMapType, ShapeLabelFilterOutput>;
  using AttributeType = typename LabelObjectType::AttributeType;
  using KeepNObjectsType = ShapeKeepNObjectsLabelMapFilter<LabelMapType>;
  using BinarizerType = LabelMapToBinaryImageFilter<LabelMapType, OutputImageType>;

  /** Standard New method. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(BinaryShapeKeepNObjectsImageFilter);

  /**
   * Set/Get whether the connected components are defined strictly by
   * face connectivity or by face+edge+vertex connectivity.  Default is
   * FullyConnectedOff.  For objects that are 1 pixel wide, use
   * FullyConnectedOn.
   */
  /** @ITKStartGrouping */
  itkSetMacro(FullyConnected, bool);
  itkGetConstReferenceMacro(FullyConnected, bool);
  itkBooleanMacro(FullyConnected);
  /** @ITKEndGrouping */
  itkConceptMacro(InputEqualityComparableCheck, (Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(IntConvertibleToInputCheck, (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(InputOStreamWritableCheck, (Concept::OStreamWritable<InputImagePixelType>));

  /**
   * Set/Get the value used as "background" in the output image.
   * Defaults to NumericTraits<PixelType>::NonpositiveMin().
   */
  /** @ITKStartGrouping */
  itkSetMacro(BackgroundValue, OutputImagePixelType);
  itkGetConstMacro(BackgroundValue, OutputImagePixelType);
  /** @ITKEndGrouping */
  /**
   * Set/Get the value used as "foreground" in the output image.
   * Defaults to NumericTraits<PixelType>::max().
   */
  /** @ITKStartGrouping */
  itkSetMacro(ForegroundValue, OutputImagePixelType);
  itkGetConstMacro(ForegroundValue, OutputImagePixelType);
  /** @ITKEndGrouping */
  /**
   * Set/Get the number of objects to keep
   */
  /** @ITKStartGrouping */
  itkGetConstMacro(NumberOfObjects, SizeValueType);
  itkSetMacro(NumberOfObjects, SizeValueType);
  /** @ITKEndGrouping */
  /**
   * Set/Get the ordering of the objects. By default, the ones with the
   * highest value are kept. Turming ReverseOrdering to true make this filter
   * keep the objects with the smallest values
   */
  /** @ITKStartGrouping */
  itkGetConstMacro(ReverseOrdering, bool);
  itkSetMacro(ReverseOrdering, bool);
  itkBooleanMacro(ReverseOrdering);
  /** @ITKEndGrouping */
  /**
   * Set/Get the attribute to use to select the object to keep. The default
   * is "NumberOfPixels".
   */
  /** @ITKStartGrouping */
  itkGetConstMacro(Attribute, AttributeType);
  itkSetMacro(Attribute, AttributeType);
  void
  SetAttribute(const std::string & s)
  {
    this->SetAttribute(LabelObjectType::GetAttributeFromName(s));
  }
  /** @ITKEndGrouping */
protected:
  BinaryShapeKeepNObjectsImageFilter();
  ~BinaryShapeKeepNObjectsImageFilter() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** BinaryShapeKeepNObjectsImageFilter needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void
  GenerateInputRequestedRegion() override;

  /** BinaryShapeKeepNObjectsImageFilter will produce the entire output. */
  void
  EnlargeOutputRequestedRegion(DataObject * itkNotUsed(output)) override;

  /** Single-threaded version of GenerateData.  This filter delegates
   * to GrayscaleGeodesicErodeImageFilter. */
  void
  GenerateData() override;

private:
  bool                 m_FullyConnected{ false };
  OutputImagePixelType m_BackgroundValue{};
  OutputImagePixelType m_ForegroundValue{};
  SizeValueType        m_NumberOfObjects{ 0 };
  bool                 m_ReverseOrdering{ false };
  AttributeType        m_Attribute{};
}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkBinaryShapeKeepNObjectsImageFilter.hxx"
#endif

#endif
