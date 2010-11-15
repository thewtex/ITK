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
#ifndef __itkNumericTraitsCovariantVectorPixel_h
#define __itkNumericTraitsCovariantVectorPixel_h

#include "itkNumericTraits.h"
#include "itkCovariantVector.h"

namespace itk
{
/** \class NumericTraits<CovariantVector< T > >
 * \brief Define numeric traits for CovariantVector.
 *
 * We provide here a generic implementation based on creating types of
 * CovariantVector whose components are the types of the NumericTraits from
 * the original CovariantVector components. This implementation require
 * support for partial specializations, since it is based on the
 * concept that:
 *   NumericTraits<CovariantVector< T > >  is defined piecewise by
 *   CovariantVector< NumericTraits< T > >
 *
 * \sa NumericTraits
 * \ingroup DataRepresentation
 */
template< typename T, unsigned int D >
class NumericTraits< CovariantVector< T, D > >
{
private:

  typedef  typename NumericTraits< T >::AbsType        ElementAbsType;
  typedef  typename NumericTraits< T >::AccumulateType ElementAccumulateType;
  typedef  typename NumericTraits< T >::FloatType      ElementFloatType;
  typedef  typename NumericTraits< T >::PrintType      ElementPrintType;
  typedef  typename NumericTraits< T >::RealType       ElementRealType;
public:

  /** Return the type of the native component type. */
  typedef T ValueType;

  typedef CovariantVector< T, D > Self;

  /** Unsigned component type */
  typedef CovariantVector< ElementAbsType, D > AbsType;

  /** Accumulation of addition and multiplication. */
  typedef CovariantVector< ElementAccumulateType, D > AccumulateType;

  /** Typedef for operations that use floating point instead of real precision
    */
  typedef CovariantVector< ElementFloatType, D > FloatType;

  /** Return the type that can be printed. */
  typedef CovariantVector< ElementPrintType, D > PrintType;

  /** Type for real-valued scalar operations. */
  typedef CovariantVector< ElementRealType, D > RealType;

  /** Type for real-valued scalar operations. */
  typedef ElementRealType ScalarRealType;

  /** Measurement vector type */
  typedef Self MeasurementVectorType;

  /** Component wise defined element
   *
   * \note minimum value for floating pointer types is defined as
   * minimum positive normalize value.
   */
  static const Self max(const Self &)
  {
    return Self( NumericTraits< T >::max() );
  }

  static const Self min(const Self &)
  {
    return Self( NumericTraits< T >::min() );
  }

  static const Self max()
  {
    return Self( NumericTraits< T >::max() );
  }

  static const Self min()
  {
    return Self( NumericTraits< T >::min() );
  }

  static const Self NonpositiveMin()
  {
    return Self( NumericTraits< T >::NonpositiveMin() );
  }

  static const Self ZeroValue()
  {
    return Self( NumericTraits< T >::ZeroValue() );
  }

  static const Self OneValue()
  {
    return Self( NumericTraits< T >::OneValue() );
  }

  /** Fixed length vectors cannot be resized, so an exception will
   *  be thrown if the input size is not valid.  If the size is valid
   *  the vector will be filled with zeros. */
  static void SetLength(CovariantVector< T, D > & m, const unsigned int s)
  {
    if ( s != D )
      {
      itkGenericExceptionMacro(<< "Cannot set the size of a CovariantVector of length "
                               << D << " to " << s);
      }
    m.Fill(NumericTraits< T >::Zero);
  }

  /** Return the length of the vector. */
  static unsigned int GetLength(const CovariantVector< T, D > &)
  {
    return D;
  }

  /** \note: the functions are prefered over the member variables as
   * they are defined for all partial specialization
   */
  static const Self ITKCommon_EXPORT Zero;
  static const Self ITKCommon_EXPORT One;
};
} // end namespace itk

#endif // __itkNumericTraitsCovariantVectorPixel_h
