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
#ifndef __itkRGBToYUV2ColorSpacePixelAccessor_h
#define __itkRGBToYUV2ColorSpacePixelAccessor_h


#include "itkRGBPixel.h"
#include "itkVector.h"
#include "vnl/vnl_math.h"
#include "itkNumericTraits.h"

namespace itk
{
namespace Accessor
{
/**
 * \class RGBToYUV2ColorSpacePixelAccessor
 * \brief Give access to a RGBPixel as if it were in Yuv Color Space as a Vector type.
 *
 * This class is intended to be used as parameter of
 * an ImageAdaptor to make an RGBPixel image appear as being
 * an image of Vector pixel type in Yuv Color Space.
 *
 * \sa ImageAdaptor
 * \ingroup ImageAdaptors
 *
 */

template <class TInput, class TOutput>
class ITK_EXPORT RGBToYUV2ColorSpacePixelAccessor
{
public:
  /** Standard class typedefs. */
  typedef   RGBToYUV2ColorSpacePixelAccessor        Self;

 /** External typedef. It defines the external aspect
   * that this class will exhibit */
  typedef  Vector<TOutput,3>     ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data */
  typedef   RGBPixel<TInput>    InternalType;

  /** Write access to the RGBToYUV2ColorSpace component */
  inline void Set( InternalType & output, const ExternalType & input ) const
    {
    // Normalize RGB values.
    double r = (double)input[0] / (double)NumericTraits<TInput>::max();
    double g = (double)input[1] / (double)NumericTraits<TInput>::max();
    double b = (double)input[2] / (double)NumericTraits<TInput>::max();

    double Y =  0.299 * r + 0.587 * g + 0.114 * b;
    double U = -0.147 * r - 0.289 * g + 0.437 * b;
    double V =  0.615 * r - 0.515 * g - 0.100 * b;

    output[0] = static_cast<TInput>(Y); // Y
    output[1] = static_cast<TInput>(U); // u
    output[2] = static_cast<TInput>(V); // v

    return output;
    }

  /** Read access to the RGBToYuvColorSpace component */
  inline ExternalType Get( const InternalType & input ) const
    {
    // Normalize RGB values.
    double r = (double)input[0] / (double)NumericTraits<TInput>::max();
    double g = (double)input[1] / (double)NumericTraits<TInput>::max();
    double b = (double)input[2] / (double)NumericTraits<TInput>::max();

    double Y =  0.299 * r + 0.587 * g + 0.114 * b;
    double U = -0.147 * r - 0.289 * g + 0.437 * b;
    double V =  0.615 * r - 0.515 * g - 0.100 * b;

    ExternalType output;
    output[0] = static_cast<TOutput>(Y); // Y
    output[1] = static_cast<TOutput>(U); // U
    output[2] = static_cast<TOutput>(V); // V

    return output;
    }

private:
};

}  // end namespace Accessor
}  // end namespace itk

#endif
