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
#ifndef __itkBSplineKernelFunction_h
#define __itkBSplineKernelFunction_h

#include "itkKernelFunction.h"
#include "vnl/vnl_math.h"

namespace itk
{
/** \class BSplineKernelFunction
 * \brief BSpline kernel used for density estimation and nonparameteric
 *  regression.
 *
 * This class enscapsulates BSpline kernel for
 * density estimation or nonparameteric regression.
 * See documentation for KernelFunction for more details.
 *
 * This class is templated over the spline order.
 * \warning Evaluate is only implemented for spline order 0 to 3
 *
 * \sa KernelFunction
 *
 * \ingroup Functions
 * \ingroup ITKCommon
 */
template< unsigned int VSplineOrder = 3, typename TValueType = double >
class ITK_EXPORT BSplineKernelFunction:public KernelFunction<TValueType>
{
public:
  /** Standard class typedefs. */
  typedef BSplineKernelFunction      Self;
  typedef KernelFunction<TValueType> Superclass;
  typedef SmartPointer< Self >       Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BSplineKernelFunction, KernelFunction);

  /** Enum of for spline order. */
  itkStaticConstMacro(SplineOrder, unsigned int, VSplineOrder);

  /** Evaluate the function. */
  inline TValueType Evaluate(const TValueType & u) const
  {
    return this->Evaluate(Dispatch< VSplineOrder >(), u);
  }

protected:
  BSplineKernelFunction(){}
  ~BSplineKernelFunction(){}
  void PrintSelf(std::ostream & os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent  << "Spline Order: " << SplineOrder << std::endl;
  }

private:
  BSplineKernelFunction(const Self &); //purposely not implemented
  void operator=(const Self &);        //purposely not implemented

  /** Structures to control overloaded versions of Evaluate */
  struct DispatchBase {};
  template< unsigned int >
  struct Dispatch: public DispatchBase {};

  /** Zeroth order spline. */
  inline TValueType Evaluate(const Dispatch< 0 > &, const TValueType & u) const
  {
    const TValueType absValue = vnl_math_abs(u);
    if ( absValue  < 0.5 )
      {
      return 1.0;
      }
    else if ( absValue == 0.5 )
      {
      return 0.5;
      }
    else
      {
      return 0.0;
      }
  }

  /** First order spline */
  inline TValueType Evaluate(const Dispatch< 1 > &, const TValueType & u) const
  {
    const TValueType absValue = vnl_math_abs(u);
    if ( absValue  < 1.0 )
      {
      return 1.0 - absValue;
      }
    else
      {
      return 0.0;
      }
  }

  /** Second order spline. */
  inline TValueType Evaluate(const Dispatch< 2 > &, const TValueType & u) const
  {
    const TValueType absValue = vnl_math_abs(u);
    if ( absValue  < 0.5 )
      {
      const TValueType sqrValue = vnl_math_sqr(absValue);
      return 0.75 - sqrValue;
      }
    else if ( absValue < 1.5 )
      {
      const TValueType sqrValue = vnl_math_sqr(absValue);
      return ( 9.0 - 12.0 * absValue + 4.0 * sqrValue ) / 8.0;
      }
    else
      {
      return 0.0;
      }
  }

  /**  Third order spline. */
  inline TValueType Evaluate(const Dispatch< 3 > &, const TValueType & u) const
  {
    const TValueType absValue = vnl_math_abs(u);
    if ( absValue  < 1.0 )
      {
      const TValueType sqrValue = vnl_math_sqr(absValue);
      return ( 4.0 - 6.0 * sqrValue + 3.0 * sqrValue * absValue ) / 6.0;
      }
    else if ( absValue < 2.0 )
      {
      const TValueType sqrValue = vnl_math_sqr(absValue);
      return ( 8.0 - 12 * absValue + 6.0 * sqrValue
               - sqrValue * absValue ) / 6.0;
      }
    else
      {
      return 0.0;
      }
  }

  /** Unimplemented spline order */
  inline TValueType Evaluate(const DispatchBase &, const TValueType &) const
  {
    itkExceptionMacro( "Evaluate not implemented for spline order "
      << SplineOrder);
    return 0.0; // This is to avoid compiler warning about missing
                // return statement.  It should never be evaluated.
  }
};
} // end namespace itk

#endif
