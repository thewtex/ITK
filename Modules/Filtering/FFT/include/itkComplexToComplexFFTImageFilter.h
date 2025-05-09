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
#ifndef itkComplexToComplexFFTImageFilter_h
#define itkComplexToComplexFFTImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkMacro.h"
#include "ITKFFTExport.h"
#include <complex>

namespace itk
{
/** \class ComplexToComplexFFTImageFilterEnums
 * \brief Contains enum classes used by ComplexToComplexFFTImageFilter class
 * \ingroup ITKFFT
 * \ingroup FourierTransform
 */
class ComplexToComplexFFTImageFilterEnums
{
public:
  /**
   * \ingroup ITKFFT
   * */
  enum class TransformDirection : uint8_t
  {
    FORWARD = 1,
    INVERSE = 2
  };
};
// Define how to print enumeration
extern ITKFFT_EXPORT std::ostream &
operator<<(std::ostream & out, const ComplexToComplexFFTImageFilterEnums::TransformDirection value);

/**
 * \class ComplexToComplexFFTImageFilter
 *
 * \brief Implements an API to enable the Fourier transform or the inverse
 * Fourier transform of images with complex valued voxels to be computed.
 *
 * \author Simon K. Warfield simon.warfield\@childrens.harvard.edu
 *
 * \note Attribution Notice. This research work was made possible by
 * Grant Number R01 RR021885 (PI Simon K. Warfield, Ph.D.) from
 * the National Center for Research Resources (NCRR), a component of the
 * National Institutes of Health (NIH).  Its contents are solely the
 * responsibility of the authors and do not necessarily represent the
 * official view of NCRR or NIH.
 *
 * This class was taken from the Insight Journal paper:
 * https://doi.org/10.54294/h4j7t7
 *
 * \ingroup FourierTransform
 * \ingroup ITKFFT
 *
 * \sa ForwardFFTImageFilter
 */
template <typename TInputImage, typename TOutputImage = TInputImage>
class ITK_TEMPLATE_EXPORT ComplexToComplexFFTImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ComplexToComplexFFTImageFilter);

  /** Input and output image types. */
  using ImageType = TInputImage;
  using InputImageType = TInputImage;
  using OutputImageType = TOutputImage;

  /** Standard class type aliases. */
  using Self = ComplexToComplexFFTImageFilter;
  using Superclass = ImageToImageFilter<InputImageType, OutputImageType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  static constexpr unsigned int ImageDimension = InputImageType::ImageDimension;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ComplexToComplexFFTImageFilter);

  /** Customized object creation methods that support configuration-based
   * selection of FFT implementation.
   *
   * Default implementation is VnlFFT.
   */
  itkFactoryOnlyNewMacro(Self);

  using TransformDirectionEnum = ComplexToComplexFFTImageFilterEnums::TransformDirection;
#if !defined(ITK_LEGACY_REMOVE)
  /**Exposes enums values for backwards compatibility*/
  static constexpr TransformDirectionEnum FORWARD = TransformDirectionEnum::FORWARD;
  static constexpr TransformDirectionEnum INVERSE = TransformDirectionEnum::INVERSE;
#endif

  /** Image type alias support */
  using ImageSizeType = typename ImageType::SizeType;

  /** Set/Get the direction in which the transform will be applied.
   * By selecting FORWARD, this filter will perform a direct, i.e. forward, Fourier Transform,
   * By selecting INVERSE, this filter will perform an inverse, i.e. backward, Fourier Transform,
   */
  /** @ITKStartGrouping */
  itkSetEnumMacro(TransformDirection, TransformDirectionEnum);
  itkGetConstMacro(TransformDirection, TransformDirectionEnum);
  /** @ITKEndGrouping */
protected:
  ComplexToComplexFFTImageFilter() = default;

  void
  GenerateInputRequestedRegion() override;

private:
  TransformDirectionEnum m_TransformDirection{ TransformDirectionEnum::FORWARD };
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkComplexToComplexFFTImageFilter.hxx"
#endif

#ifdef ITK_FFTIMAGEFILTERINIT_FACTORY_REGISTER_MANAGER
#  include "itkFFTImageFilterInitFactoryRegisterManager.h"
#endif

#endif
