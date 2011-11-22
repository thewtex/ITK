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
#ifndef __itkCompensatedSummation_h
#define __itkCompensatedSummation_h

namespace itk
{

/** \class CompensatedSummation
 * \brief Perform more precise accumulation of floating point numbers.
 *
 * The \c float and \c double datatypes only have finite precision.  When
 * performing a running sum of floats, the accumulated errors get progressively
 * worse as the magnitude of the sum gets large relative to new elements.
 *
 * From Wikipedia, http://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 * "In numerical analysis, the Kahan summation algorithm (also known as
 * compensated summation) significantly reduces the numerical error in the total
 * obtained by adding a sequence of finite precision floating point numbers,
 * compared to the obvious approach. This is done by keeping a separate running
 * compensation (a variable to accumulate small errors)."
 *
 * For example, instead of
 * \code
 *   double sum = 0.0;
 *   for( unsigned int i = 0; i < array.Size(); ++i )
 *     {
 *     sum += array.GetElement(i);
 *     }
 * \endcode
 *
 * do
 *
 * \code
 *   typedef CompensatedSummation<double> CompensatedSummationType;
 *   CompensatedSummationType compensatedSummation;
 *   for( unsigned int i = 0; i < array.Size(); ++i )
 *     {
 *     compensatedSummation += array.GetElement(i);
 *     }
 *   double sum = compensatedSummation.GetSum();
 * \endcode
 */
template < class TFloat >
class CompensatedSummation
{
public:
  typedef TFloat FloatType;

  /** Standard class typedefs. */
  typedef CompensatedSummation Self;

  /** Constructor. */
  CompensatedSummation();

  /** Add an element to the sum. */
  void AddElement( const FloatType & element );
  CompensatedSummation & operator+=( const FloatType & rhs );
  /** Subtract an element from the sum. */
  CompensatedSummation & operator-=( const FloatType & rhs );

  /** Reset the sum and compensation to zero. */
  void ResetToZero();

  /** Get the sum. */
  const FloatType & GetSum() const;

private:
  FloatType m_Sum;
  FloatType m_Compensation;

  CompensatedSummation( const Self & ); // purposely not implemented
  void operator=( const Self & );   // purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCompensatedSummation.hxx"
#endif

#endif
