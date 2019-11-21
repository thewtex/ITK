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
#ifndef itkGPUBinaryThresholdImageFilterFactory_h
#define itkGPUBinaryThresholdImageFilterFactory_h

#include "itkGPUBinaryThresholdImageFilter.h"

namespace itk
{

/**
 * \class GPUBinaryThresholdImageFilterFactory
 * Object Factory implementation for GPUBinaryThresholdImageFilter
 *
 * \ingroup ITKGPUThresholding
 */
class GPUBinaryThresholdImageFilterFactory : public ObjectFactoryBase
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(GPUBinaryThresholdImageFilterFactory);

  using Self = GPUBinaryThresholdImageFilterFactory;
  using Superclass = ObjectFactoryBase;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Class methods used to interface with the registered factories. */
  const char *
  GetITKSourceVersion() const override
  {
    return ITK_SOURCE_VERSION;
  }
  const char *
  GetDescription() const override
  {
    return "A Factory for GPUBinaryThresholdImageFilter";
  }

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GPUBinaryThresholdImageFilterFactory, itk::ObjectFactoryBase);

  /** Register one factory of this type  */
  static void
  RegisterOneFactory()
  {
    GPUBinaryThresholdImageFilterFactory::Pointer factory = GPUBinaryThresholdImageFilterFactory::New();

    itk::ObjectFactoryBase::RegisterFactory(factory);
  }

private:
#define OverrideThresholdFilterTypeMacro(ipt, opt, dm)                                                                 \
  {                                                                                                                    \
    using InputImageType = itk::Image<ipt, dm>;                                                                        \
    using OutputImageType = itk::Image<opt, dm>;                                                                       \
    this->RegisterOverride(                                                                                            \
      typeid(itk::BinaryThresholdImageFilter<InputImageType, OutputImageType>).name(),                                 \
      typeid(itk::GPUBinaryThresholdImageFilter<InputImageType, OutputImageType>).name(),                              \
      "GPU Binary Threshold Image Filter Override",                                                                    \
      true,                                                                                                            \
      itk::CreateObjectFunction<GPUBinaryThresholdImageFilter<InputImageType, OutputImageType>>::New());               \
  }

  GPUBinaryThresholdImageFilterFactory()
  {
    if (IsGPUAvailable())
    {
      OverrideThresholdFilterTypeMacro(unsigned char, unsigned char, 1);
      OverrideThresholdFilterTypeMacro(char, char, 1);
      OverrideThresholdFilterTypeMacro(float, float, 1);
      OverrideThresholdFilterTypeMacro(int, int, 1);
      OverrideThresholdFilterTypeMacro(unsigned int, unsigned int, 1);
      OverrideThresholdFilterTypeMacro(double, double, 1);

      OverrideThresholdFilterTypeMacro(unsigned char, unsigned char, 2);
      OverrideThresholdFilterTypeMacro(char, char, 2);
      OverrideThresholdFilterTypeMacro(float, float, 2);
      OverrideThresholdFilterTypeMacro(int, int, 2);
      OverrideThresholdFilterTypeMacro(unsigned int, unsigned int, 2);
      OverrideThresholdFilterTypeMacro(double, double, 2);

      OverrideThresholdFilterTypeMacro(unsigned char, unsigned char, 3);
      OverrideThresholdFilterTypeMacro(unsigned short, unsigned short, 3);
      OverrideThresholdFilterTypeMacro(char, char, 3);
      OverrideThresholdFilterTypeMacro(float, float, 3);
      OverrideThresholdFilterTypeMacro(int, int, 3);
      OverrideThresholdFilterTypeMacro(unsigned int, unsigned int, 3);
      OverrideThresholdFilterTypeMacro(double, double, 3);
    }
  }
};

} // end of namespace itk

#endif
