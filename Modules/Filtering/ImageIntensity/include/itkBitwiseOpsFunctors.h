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
#ifndef itkBitwiseOpsFunctors_h
#define itkBitwiseOpsFunctors_h

#include "itkMacro.h"

namespace itk::Functor
{

/**
 * \class AND
 * \brief bitwise AND functor
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT AND
{
public:
  bool
  operator==(const AND &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(AND);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A & B);
  }
};

/**
 * \class OR
 * \brief bitwise OR functor
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT OR
{
public:
  bool
  operator==(const OR &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(OR);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A | B);
  }
};

/**
 * \class XOR
 * \brief bitwise XOR functor
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT XOR
{
public:
  bool
  operator==(const XOR &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(XOR);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A ^ B);
  }
};

/**
 * \class BitwiseNot
 * \brief Performs the C++ unary bitwise NOT operator.
 * \ingroup ITKImageIntensity
 */
template <class TInput, class TOutput>
class BitwiseNot
{
public:
  // Use default copy, assigned and destructor
  // BitwiseNot() {} default constructor OK

  bool
  operator==(const BitwiseNot &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(BitwiseNot);

  inline TOutput
  operator()(const TInput & A) const
  {
    return static_cast<TOutput>(~A);
  }
};
} // namespace itk::Functor

#endif
