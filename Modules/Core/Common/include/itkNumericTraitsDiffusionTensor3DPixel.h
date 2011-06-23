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
#ifndef __itkNumericTraitsDiffusionTensor3DPixel_h
#define __itkNumericTraitsDiffusionTensor3DPixel_h

#include "itkNumericTraits.h"
#include "itkDiffusionTensor3D.h"

// This file is meant to define numeric traits for tensor pixels types in itk

namespace itk
{
/**
 * \brief Define numeric traits for DiffusionTensor3D.
 * \tparam T Component type of DiffusionTensor3D
 *
 * We provide here a generic implementation based on creating types of
 * DiffusionTensor3D whose components are the types of the NumericTraits from
 * the original DiffusionTensor3D components. This implementation require
 * support for partial specializations, since it is based on the
 * concept that:
 *   NumericTraits<DiffusionTensor3D< T > >  is defined piecewise by
 *   DiffusionTensor3D< NumericTraits< T > >
 *
 * \sa NumericTraits
 * \ingroup DataRepresentation
 * \ingroup ITK_Common
 */
template< typename T >
class NumericTraits< DiffusionTensor3D< T > >
{
private:

  typedef typename NumericTraits< T >::AbsType        ElementAbsType;
  typedef typename NumericTraits< T >::AccumulateType ElementAccumulateType;
  typedef typename NumericTraits< T >::FloatType      ElementFloatType;
  typedef typename NumericTraits< T >::PrintType      ElementPrintType;
  typedef typename NumericTraits< T >::RealType       ElementRealType;
public:

  /** Return the type of the native component type. */
  typedef T ValueType;

  typedef DiffusionTensor3D< T > Self;

  /** Unsigned component type */
  typedef DiffusionTensor3D< ElementAbsType > AbsType;

  /** Accumulation of addition and multiplication. */
  typedef DiffusionTensor3D< ElementAccumulateType > AccumulateType;

  /** Typedef for operations that use floating point instead of real precision
    */
  typedef DiffusionTensor3D< ElementFloatType > FloatType;

  /** Return the type that can be printed. */
  typedef DiffusionTensor3D< ElementPrintType > PrintType;

  /** Type for real-valued scalar operations. */
  typedef DiffusionTensor3D< ElementRealType > RealType;

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
   *  be thrown if the input size is not valid.  In this case, the
   *  only valid size is 6. If the size is valid the tensor will be
   *  filled with zeros. */
  static void SetLength(DiffusionTensor3D< T > & m, const unsigned int s)
  {
    if ( s != 6 )
      {
      itkGenericExceptionMacro(<< "Cannot set the size of a DiffusionTensor3D "
                               "to anything other than 6.");
      }
    m.Fill(NumericTraits< T >::Zero);
  }

  /** Return the size of the tensor. Always returns 6. */
  static unsigned int GetLength(const DiffusionTensor3D< T > &)
  {
    return 6;
  }

  /** Return the size of the tensor. Always returns 6. */
  static unsigned int GetLength()
  {
    return 6;
  }

  static void AssignToArray( const Self & v, MeasurementVectorType & mv )
  {
    mv = v;
  }

  template<class TArray>
  static void AssignToArray( const Self & v, TArray & mv )
  {
    for( unsigned int i=0; i<6; i++ )
      {
      mv[i] = v[i];
      }
  }

  /** \note: the functions are prefered over the member variables as
   * they are defined for all partial specialization
   */
  static const Self ITKCommon_EXPORT Zero;
  static const Self ITKCommon_EXPORT One;
};
} // end namespace itk

#endif // __itkNumericTraitsTensorPixel_h
