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

#include "itkRGBPixel.h"
#include "itkNumericTraits.h"
#include "itkRGBToHSVColorSpacePixelAccessor.h"

template< typename TOutput >
int AdaptorTest()
{
  typedef unsigned char                       InputComponentType;
  typedef itk::RGBPixel< InputComponentType > RGBPixelType;

  typedef itk::Accessor::RGBToHSVColorSpacePixelAccessor< InputComponentType, TOutput > ConverterType;
  ConverterType Converter;

  InputComponentType maxValue = itk::NumericTraits< InputComponentType >::max();

  RGBPixelType rgb;

  for( InputComponentType r = 0; r < maxValue; r++ )
    {
    rgb.SetRed( r );

    for( InputComponentType g = 0; g < maxValue; g++ )
      {
      rgb.SetGreen( g );

      for( InputComponentType b = 0; b < maxValue; b++ )
        {
        rgb.SetBlue( b );

        typename ConverterType::ExternalType hsv = Converter.Get( rgb );

        std::cout << rgb << " -> " << hsv << std::endl;
        }
      }
    }

  return EXIT_SUCCESS;
}

int itkRGBToHSVColorSpaceAdaptorTest( int , char* [] )
{
  return AdaptorTest< float >();
}
