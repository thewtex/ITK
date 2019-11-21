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
#ifndef itkGPUBinaryThresholdImageFilter_h
#define itkGPUBinaryThresholdImageFilter_h

#include "itkOpenCLUtil.h"
#include "itkGPUFunctorBase.h"
#include "itkGPUKernelManager.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkGPUUnaryFunctorImageFilter.h"

namespace itk
{

namespace Functor
{
template <typename TInput, typename TOutput>
class ITK_TEMPLATE_EXPORT GPUBinaryThreshold : public GPUFunctorBase
{
public:
  GPUBinaryThreshold()
  {
    m_LowerThreshold = NumericTraits<TInput>::NonpositiveMin();
    m_UpperThreshold = NumericTraits<TInput>::max();
    m_OutsideValue = NumericTraits<TOutput>::ZeroValue();
    m_InsideValue = NumericTraits<TOutput>::max();
  }

  ~GPUBinaryThreshold() {}

  void
  SetLowerThreshold(const TInput & thresh)
  {
    m_LowerThreshold = thresh;
  }
  void
  SetUpperThreshold(const TInput & thresh)
  {
    m_UpperThreshold = thresh;
  }
  void
  SetInsideValue(const TOutput & value)
  {
    m_InsideValue = value;
  }
  void
  SetOutsideValue(const TOutput & value)
  {
    m_OutsideValue = value;
  }

  /** Setup GPU kernel arguments for this functor.
   * Returns current argument index to set additional arguments in the GPU kernel */
  int
  SetGPUKernelArguments(GPUKernelManager::Pointer KernelManager, int KernelHandle)
  {
    KernelManager->SetKernelArg(KernelHandle, 0, sizeof(TInput), &(m_LowerThreshold));
    KernelManager->SetKernelArg(KernelHandle, 1, sizeof(TInput), &(m_UpperThreshold));
    KernelManager->SetKernelArg(KernelHandle, 2, sizeof(TOutput), &(m_InsideValue));
    KernelManager->SetKernelArg(KernelHandle, 3, sizeof(TOutput), &(m_OutsideValue));
    return 4;
  }

private:
  TInput  m_LowerThreshold;
  TInput  m_UpperThreshold;
  TOutput m_InsideValue;
  TOutput m_OutsideValue;
};
} // end of namespace Functor

/** Create a helper GPU Kernel class for GPUBinaryThresholdImageFilter */
itkGPUKernelClassMacro(GPUBinaryThresholdImageFilterKernel);

/**
 * \class GPUBinaryThresholdImageFilter
 *
 * \brief GPU version of binary threshold image filter.
 *
 * \ingroup ITKGPUThresholding
 */
template <typename TInputImage, typename TOutputImage>
class ITK_TEMPLATE_EXPORT GPUBinaryThresholdImageFilter
  : public GPUUnaryFunctorImageFilter<
      TInputImage,
      TOutputImage,
      Functor::GPUBinaryThreshold<typename TInputImage::PixelType, typename TOutputImage::PixelType>,
      BinaryThresholdImageFilter<TInputImage, TOutputImage>>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(GPUBinaryThresholdImageFilter);

  /** Standard class type aliases. */
  using Self = GPUBinaryThresholdImageFilter;
  using GPUSuperclass = GPUUnaryFunctorImageFilter<
    TInputImage,
    TOutputImage,
    Functor::GPUBinaryThreshold<typename TInputImage::PixelType, typename TOutputImage::PixelType>,
    BinaryThresholdImageFilter<TInputImage, TOutputImage>>;
  using CPUSuperclass = BinaryThresholdImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GPUBinaryThresholdImageFilter, GPUUnaryFunctorImageFilter);

  /** Pixel types. */
  using InputPixelType = typename TInputImage::PixelType;
  using OutputPixelType = typename TOutputImage::PixelType;

  /** Type of DataObjects to use for scalar inputs */
  using InputPixelObjectType = SimpleDataObjectDecorator<InputPixelType>;

  /** Get OpenCL Kernel source as a string, creates a GetOpenCLSource method */
  itkGetOpenCLSourceFromKernelMacro(GPUBinaryThresholdImageFilterKernel);

protected:
  GPUBinaryThresholdImageFilter();
  ~GPUBinaryThresholdImageFilter() override {}

  /** This method is used to set the state of the filter before
   * multi-threading. */
  // virtual void BeforeThreadedGenerateData();

  /** Unlike CPU version, GPU version of binary threshold filter is not
    multi-threaded */
  void
  GPUGenerateData() override;
};

} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkGPUBinaryThresholdImageFilter.hxx"
#endif

#endif
