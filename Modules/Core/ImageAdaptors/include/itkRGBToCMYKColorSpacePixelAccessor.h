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
#ifndef __itkRGBToCMYKColorSpacePixelAccessor_h
#define __itkRGBToCMYKColorSpacePixelAccessor_h


#include "itkRGBPixel.h"
#include "itkVector.h"
#include "vnl/vnl_math.h"
#include "itkNumericTraits.h"

namespace itk
{
namespace Accessor
{
/**
 * \class RGBToCMYKColorSpacePixelAccessor
 * \brief Give access to a RGBPixel as if it were in CMYK Color Space as a Vector type.
 *
 * This class is intended to be used as parameter of
 * an ImageAdaptor to maKe an RGBPixel image appear as being
 * an image of Vector pixel type in CMYK Color Space.
 *
 * \sa ImageAdaptor
 * \ingroup ImageAdaptors
 *
 */

template <class TInput, class TOutput>
class ITK_EXPORT RGBToCMYKColorSpacePixelAccessor
{
public:
  /** Standard class typedefs. */
  typedef   RGBToCMYKColorSpacePixelAccessor        Self;

 /** External typedef. It defines the external aspect
   * that this class will exhibit */
  typedef  Vector<TOutput,4>     ExternalType;

  /** Internal typedef. It defines the internal real
   * representation of data */
  typedef   RGBPixel<TInput>    InternalType;

  /** Write access to the RGBToCMYKColorSpace component */
  inline void Set( InternalType & output, const ExternalType & input ) const
    {
    // Normalize RGB values.
    double r = (double)input[0] / (double)NumericTraits<TInput>::max();
    double g = (double)input[1] / (double)NumericTraits<TInput>::max();
    double b = (double)input[2] / (double)NumericTraits<TInput>::max();

    double C = 1 - r;
    double M = 1 - g;
    double Y = 1 - b;

    double K = 1.;
    if(C < K)
      {
      K = C;
      }
    if(M < K)
      {
      K = M;
      }
    if(Y < K)
      {
      K = Y;
      }

    if(K == 1)
      {
      C = 0.;
      M = 0.;
      Y = 0.;
      }
    else
      {
      double invOneMinusK = 1. / ( 1. - K );
      C = (C - K) * invOneMinusK;
      M = (M - K) * invOneMinusK;
      Y = (Y - K) * invOneMinusK;
      }

    output[0] = static_cast<TInput>(C); // C
    output[1] = static_cast<TInput>(M); // M
    output[2] = static_cast<TInput>(Y); // Y
    output[3] = static_cast<TInput>(K); // K

    return output;
    }

  /** Read access to the RGBToCMYKColorSpace component */
  inline ExternalType Get( const InternalType & input ) const
    {
    // Normalize RGB values.
    double r = (double)input[0] / (double)NumericTraits<TInput>::max();
    double g = (double)input[1] / (double)NumericTraits<TInput>::max();
    double b = (double)input[2] / (double)NumericTraits<TInput>::max();

    double C = 1 - r;
    double M = 1 - g;
    double Y = 1 - b;

    double K = 1;
    if(C < K)
      K = C;
    if(M < K)
      K = M;
    if(Y < K)
      K = Y;

    if(K == 1)
      {
      C = 0;
      M = 0;
      Y = 0;
      }
    else
      {
      C = (C - K) / (1 - K);
      M = (M - K) / (1 - K);
      Y = (Y - K) / (1 - K);
      }

    ExternalType output;
    output[0] = static_cast<TOutput>(C); // C
    output[1] = static_cast<TOutput>(M); // M
    output[2] = static_cast<TOutput>(Y); // Y
    output[3] = static_cast<TOutput>(K); // K

    return output;
    }

private:
};

}  // end namespace Accessor
}  // end namespace itk

#endif
