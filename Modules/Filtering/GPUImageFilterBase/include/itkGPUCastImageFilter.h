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
#ifndef itkGPUCastImageFilter_h
#define itkGPUCastImageFilter_h

#include "itkGPUInPlaceImageFilter.h"
#include "itkCastImageFilter.h"

namespace itk
{
/**
 * \class GPUCastImageFilter
 * \brief A class to cast between itk::Image and itk::GPUImage
 *
 * \ingroup ITKGPUImageFilterBase
 */

template <typename TInputImage,
          typename TOutputImage,
          typename TParentImageFilter = CastImageFilter<TInputImage, TOutputImage>>
class GPUCastImageFilter : public GPUInPlaceImageFilter<TInputImage, TOutputImage, TParentImageFilter>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(GPUCastImageFilter);

  /** Standard class type aliases. */
  using Self = GPUCastImageFilter;
  using GPUSuperclass = GPUInPlaceImageFilter<TInputImage, TOutputImage, TParentImageFilter>;
  using CPUSuperclass = TParentImageFilter;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(GPUCastImageFilter, GPUInPlaceImageFilter);

protected:
  GPUCastImageFilter() {}
  ~GPUCastImageFilter() override {}

  void
  PrintSelf(std::ostream & os, Indent indent) const override
  {
    GPUSuperclass::PrintSelf(os, indent);
  }
};
} // namespace itk

#endif
