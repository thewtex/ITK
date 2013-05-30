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
/**
 *
 *  This program illustrates the use of Adaptors and
 *  NthElementPixelAccessor
 *
 *  The example shows how an Adaptor can be used to
 *  get acces only to the Nth component of a vector image
 *  giving the appearance of being just a 'float' image
 *
 *  That will allow to pass the Nth component of this
 *  image as input or output to any filter that expects
 *  a float image
 *
 */

#include "itkImageAdaptor.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkNthElementPixelAccessor.h"
#include "itkTestingMacros.h"
#include "itkVariableLengthVector.h"

//-------------------------------------
//     Typedefs for convenience
//-------------------------------------
const unsigned int Dimension = 2;
const unsigned int VectorLength = 3;
typedef itk::VariableLengthVector< float >  PixelType;
typedef itk::Image< PixelType,  Dimension > myImageType;
typedef itk::NthElementPixelAccessor< float, PixelType >       myNthAccessorType;
typedef itk::ImageAdaptor< myImageType, myNthAccessorType >    myNthAdaptorType;
typedef itk::ImageRegionIteratorWithIndex< myImageType >       myIteratorType;
typedef itk::ImageRegionIteratorWithIndex< myNthAdaptorType>  myNthIteratorType;

//-------------------------
//
//   Main code
//
//-------------------------
int itkNthElementPixelAccessorTest2(int, char* []) {
  // 2) test on variable length vector image
  myImageType::SizeType size;
  size[0] = 2;
  size[1] = 2;

  myImageType::IndexType index;
  index[0] = 0;
  index[1] = 0;

  myImageType::RegionType region;
  region.SetIndex( index );
  region.SetSize(  size  );

  myImageType::Pointer myImage = myImageType::New();

  myImage->SetLargestPossibleRegion( region );
  myImage->SetBufferedRegion( region );
  myImage->SetRequestedRegion( region );
  myImage->Allocate();
  PixelType f(VectorLength);
  myImage->FillBuffer(f);

  myIteratorType it1( myImage, myImage->GetRequestedRegion() );

  // Value to initialize the pixels
  PixelType color(3);
  color[0] = 1.0f;
  color[1] = 0.5f;
  color[2] = 0.5f;

  // Initializing all the pixel in the image
  it1.GoToBegin();
  while( !it1.IsAtEnd() )
    {
    it1.Set(color);
    ++it1;
    }

  // Reading the values to verify the image content
  std::cout << "--- Before --- " << std::endl;
  it1.GoToBegin();
  while( !it1.IsAtEnd() )
    {
    std::cout << it1.Get()[0]   << "  ";
    std::cout << it1.Get()[1] << "  ";
    std::cout << it1.Get()[2]  << std::endl;
    ++it1;
    }

  myNthAdaptorType::Pointer myAdaptor = myNthAdaptorType::New();
  myAdaptor->SetImage( myImage);

  myNthAccessorType myNthAccessor;
  myNthAccessor.SetVectorLength(VectorLength);
  myNthAccessor.SetElementNumber(0);

  myAdaptor->SetPixelAccessor(myNthAccessor);


  myNthIteratorType  it2( myAdaptor, myAdaptor->GetRequestedRegion() );

  // Set the values of the first component of myImage, using myAdaptor
  const float MY_VALUE = float(0.4);
  it2.GoToBegin();
  while( !it2.IsAtEnd() )
    {
    it2.Set(MY_VALUE);
    ++it2;
    }


  std::cout << "--- After --- " << std::endl;

  it1.GoToBegin();
  while( !it1.IsAtEnd() )
    {
    std::cout << it1.Get()[0] << "  ";
    std::cout << it1.Get()[1] << "  ";
    std::cout << it1.Get()[2] << std::endl;

    TEST_EXPECT_EQUAL( it1.Get()[0], MY_VALUE);

    ++it1;
    }

  return EXIT_SUCCESS;
}
