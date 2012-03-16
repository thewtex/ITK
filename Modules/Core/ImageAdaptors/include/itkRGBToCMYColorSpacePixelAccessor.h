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

#ifndef __itkRGBToCMYColorSpacePixelAccessor_h
#define __itkRGBToCMYColorSpacePixelAccessor_h


#include "itkRGBPixel.h"
#include "itkVector.h"
#include "vnl/vnl_math.h"
#include "itkNumericTraits.h"

namespace itk
{
namespace Accessor
{
/**
 * \class RGBToCMYColorSpacePixelAccessor
 * \brief Give access to a RGBPixel as if it were in CMY Color Space as a Vector type.
 *
 * This class is intended to be used as parameter of
 * an ImageAdaptor to make an RGBPixel image appear as being
 * an image of Vector pixel type in CMY Color Space.
 *
 * \sa ImageAdaptor
 * \ingroup ImageAdaptors
 *
 */
template <class TInput, class TOutput>
class ITK_EXPORT RGBToCMYColorSpacePixelAccessor
{
public:
  /** Standard class typedefs. */
  typedef   RGBToCMYColorSpacePixelAccessor        Self;

 /** External typedef. It defines the external aspect
   * that this class will exhibit */
  typedef  Vector<TOutput,3>     ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data */
  typedef   RGBPixel<TInput>    InternalType;

  /** Write access to the RGBToCMYColorSpace component */
  inline void Set( InternalType & output, const ExternalType & input ) const
    {
    const double invMaxValue = 1. / static_cast< double >( NumericTraits< TInput >::max() );

    // Normalize RGB values.
    double r = static_cast< double >( input[0] ) * invMaxValue;
    double g = static_cast< double >( input[1] ) * invMaxValue;
    double b = static_cast< double >( input[2] ) * invMaxValue;

    double C = 1. - r;
    double M = 1. - g;
    double Y = 1. - b;

    output[0] = static_cast< TInput >( C ); // C
    output[1] = static_cast< TInput >( M ); // M
    output[2] = static_cast< TInput >( Y ); // Y
    }

  /** Read access to the RGBToCMYColorSpace component */
  inline ExternalType Get( const InternalType & input ) const
    {
    const double invMaxValue = 1. / static_cast< double >( NumericTraits< TInput >::max() );

    // Normalize RGB values.
    double r = static_cast< double >( input[0] ) * invMaxValue;
    double g = static_cast< double >( input[1] ) * invMaxValue;
    double b = static_cast< double >( input[2] ) * invMaxValue;

    double C = 1 - r;
    double M = 1 - g;
    double Y = 1 - b;

    ExternalType output;
    output[0] = static_cast< TOutput >( C ); // C
    output[1] = static_cast< TOutput >( M ); // M
    output[2] = static_cast< TOutput >( Y ); // Y

    return output;
    }

private:
};

}  // end namespace Accessor
}  // end namespace itk

#endif
