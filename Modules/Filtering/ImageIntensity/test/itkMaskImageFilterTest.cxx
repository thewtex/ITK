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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include "itkImage.h"
#include "itkVectorImage.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkMaskImageFilter.h"

int itkMaskImageFilterTest(int, char* [] )
{

  // Define the dimension of the images
  const unsigned int myDimension = 3;

  // Declare the types of the images
  typedef itk::Image<float, myDimension>           myImageType1;
  typedef itk::Image<unsigned short, myDimension>  myImageType2;
  typedef itk::Image<float, myDimension>           myImageType3;

  // Declare the type of the index to access images
  typedef itk::Index<myDimension>         myIndexType;

  // Declare the type of the size
  typedef itk::Size<myDimension>          mySizeType;

  // Declare the type of the Region
  typedef itk::ImageRegion<myDimension>        myRegionType;

  // Create two images
  myImageType1::Pointer inputImageA  = myImageType1::New();
  myImageType2::Pointer inputImageB  = myImageType2::New();

  // Define their size, and start index
  mySizeType size;
  size[0] = 2;
  size[1] = 2;
  size[2] = 2;

  myIndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  myRegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  // Initialize Image A
  inputImageA->SetLargestPossibleRegion( region );
  inputImageA->SetBufferedRegion( region );
  inputImageA->SetRequestedRegion( region );
  inputImageA->Allocate();

  // Initialize Image B
  inputImageB->SetLargestPossibleRegion( region );
  inputImageB->SetBufferedRegion( region );
  inputImageB->SetRequestedRegion( region );
  inputImageB->Allocate();


  // Declare Iterator types apropriated for each image
  typedef itk::ImageRegionIteratorWithIndex<myImageType1>  myIteratorType1;
  typedef itk::ImageRegionIteratorWithIndex<myImageType2>  myIteratorType2;
  typedef itk::ImageRegionIteratorWithIndex<myImageType3>  myIteratorType3;

  // Create one iterator for Image A (this is a light object)
  myIteratorType1 it1( inputImageA, inputImageA->GetBufferedRegion() );

  // Initialize the content of Image A
  std::cout << "First operand " << std::endl;
  while( !it1.IsAtEnd() )
  {
    it1.Set( 255.0 );
    std::cout << it1.Get() << std::endl;
    ++it1;
  }

  // Create one iterator for Image B (this is a light object)
  myIteratorType2 it2( inputImageB, inputImageB->GetBufferedRegion() );

  // Initialize the content of Image B
  // Set to mask first 2 pixels and last 2 pixels and leave the rest as is
  std::cout << "Second operand " << std::endl;
  for(unsigned int i = 0; i<2; ++i, ++it2) it2.Set( 0 );

  while( !it2.IsAtEnd() )
  {
    it2.Set( 3 );
    ++it2;
  }

  for(unsigned int i = 0; i< 3; ++i, --it2) it2.Set( 0 );

  it2.GoToBegin();
  while( !it2.IsAtEnd() )
    {
    std::cout << it2.Get() << std::endl;
    ++it2;
    }

  // Declare the type for the Mask image filter
  typedef itk::MaskImageFilter<
                           myImageType1,
                           myImageType2,
                           myImageType3  >       myFilterType;


  // Create a mask  Filter
  myFilterType::Pointer filter = myFilterType::New();


  // Connect the input images
  filter->SetInput1( inputImageA );
  filter->SetInput2( inputImageB );
  filter->SetOutsideValue( 50 );

  // Get the Smart Pointer to the Filter Output
  myImageType3::Pointer outputImage = filter->GetOutput();


  // Execute the filter
  filter->Update();
  filter->SetFunctor(filter->GetFunctor());

  // Create an iterator for going through the image output
  myIteratorType3 it3(outputImage, outputImage->GetBufferedRegion());

  //  Print the content of the result image
  std::cout << " Result " << std::endl;
  while( !it3.IsAtEnd() )
    {
    std::cout << it3.Get() << std::endl;
    ++it3;
    }


  filter->Print( std::cout );

  // Vector image tests
  typedef itk::VectorImage<float, myDimension>
    myVectorImageType;
  typedef itk::ImageRegionIteratorWithIndex<myVectorImageType>
    myVectorIteratorType;

  myVectorImageType::Pointer inputVectorImage  = myVectorImageType::New();
  inputVectorImage->SetLargestPossibleRegion( region );
  inputVectorImage->SetBufferedRegion( region );
  inputVectorImage->SetRequestedRegion( region );
  inputVectorImage->SetNumberOfComponentsPerPixel(3);
  inputVectorImage->Allocate();

  typedef itk::MaskImageFilter< myVectorImageType,
                                myImageType2,
                                myVectorImageType> myVectorFilterType;

  myVectorFilterType::Pointer vectorFilter = myVectorFilterType::New();
  vectorFilter->SetInput1( inputVectorImage );
  vectorFilter->SetMaskImage( inputImageB );

  myVectorImageType::PixelType outsideValue = vectorFilter->GetOutsideValue();
  if ( outsideValue.GetSize() != 0 )
    {
    std::cerr << "Default outside value prior to running filter does not "
              << "have 0 components." << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    vectorFilter->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cout << "Caught unexpected exception when default outside value "
              << "is used." << std::endl;
    }

  // Check that the outside value consists of three zeros.
  myVectorImageType::PixelType threeZeros = vectorFilter->GetOutsideValue();
  if ( threeZeros.GetSize() != 3 )
    {
    std::cerr << "Default outside value after running filter does not have "
              << "3 components." << std::endl;
    return EXIT_FAILURE;
    }

  for ( int i = 0; i < 3; i++ )
    {
    if ( threeZeros[i] != 0.0f )
      {
      std::cerr << "Default outside value does not have all zeros." << std::endl;
      return EXIT_FAILURE;
      }
    }

  outsideValue = myVectorImageType::PixelType( 3 );
  outsideValue[0] = 1.0;
  outsideValue[1] = 2.0;
  outsideValue[2] = 3.0;
  vectorFilter->SetOutsideValue( outsideValue );

  try
    {
    vectorFilter->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Caught unexpected exception." << std::endl;
    return EXIT_FAILURE;
    }

  // Add components to image
  inputVectorImage->Initialize();
  inputVectorImage->SetLargestPossibleRegion( region );
  inputVectorImage->SetBufferedRegion( region );
  inputVectorImage->SetRequestedRegion( region );
  inputVectorImage->SetNumberOfComponentsPerPixel( 5 );
  inputVectorImage->Allocate();

  try
    {
    vectorFilter->Update();
    std::cerr << "Expected exception not caught when number of components in."
              << "outside value is not the same as number of components in "
              << "image." << std::endl;
    return EXIT_FAILURE;
    }
  catch ( itk::ExceptionObject e )
    {
    std::cout << "Caught exception expected when outside value not updated "
              << "to have the same number of components as image."
              << std::endl;
    }

  // Reset the outside value to zero-length vector.
  outsideValue = myVectorImageType::PixelType( 0 );
  vectorFilter->SetOutsideValue( outsideValue );

  try
    {
    vectorFilter->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Caught unexpected exception when resetting outside value "
              << "to  vector of length 0." << std::endl;
    return EXIT_FAILURE;
    }

  // Check size of outsideValue.
  myVectorImageType::PixelType fiveZeros = vectorFilter->GetOutsideValue();
  if ( fiveZeros.GetSize() != 5 )
    {
    std::cerr << "Outside value was not resized correctly." << std::endl;
    return EXIT_FAILURE;
    }

  // Check that the new outside value consists of all zeros.
  for ( int i = 0; i < 5; i++ )
    {
    if ( fiveZeros[i] != 0.0f )
      {
      std::cerr << "Element " << i << " of updated outside vector is not 0"
                << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << "[PASSED]" << std::endl;

  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;

}
