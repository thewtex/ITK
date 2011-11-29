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
#ifndef __itkGaussianKernelFunction_h
#define __itkGaussianKernelFunction_h

#include "itkKernelFunction.h"
#include "vnl/vnl_math.h"
#include <math.h>

namespace itk
{
/** \class GaussianKernelFunction
 * \brief Gaussian kernel used for density estimation and nonparameteric
 *  regression.
 *
 * This class encapsulates a Gaussian smoothing kernel for
 * density estimation or nonparameteric regression.
 * See documentation for KernelFunction for more details.
 *
 * \sa KernelFunction
 *
 * \ingroup Functions
 * \ingroup ITKCommon
 */
template< typename TValueType = double >
class ITKCommon_EXPORT GaussianKernelFunction:public KernelFunction<TValueType>
{
public:
  /** Standard class typedefs. */
  typedef GaussianKernelFunction     Self;
  typedef KernelFunction<TValueType> Superclass;
  typedef SmartPointer< Self >       Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GaussianKernelFunction, KernelFunction);

  /** Evaluate the function. */
  inline TValueType Evaluate(const TValueType & u) const
  { return ( vcl_exp( static_cast< TValueType >(-0.5) * vnl_math_sqr(u) ) * m_Factor ); }
protected:
  GaussianKernelFunction(): m_Factor(  NumericTraits< TValueType >::One / vcl_sqrt(static_cast< TValueType >(2.0 * vnl_math::pi )) ) {};
  virtual ~GaussianKernelFunction() {};
  void PrintSelf(std::ostream & os, Indent indent) const
  { Superclass::PrintSelf(os, indent); }
private:
  GaussianKernelFunction(const Self &); //purposely not implemented
  void operator=(const Self &);         //purposely not implemented

  const TValueType m_Factor;
};
} // end namespace itk

#endif
