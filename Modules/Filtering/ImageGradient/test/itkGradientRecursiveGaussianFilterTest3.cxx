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

#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkVector.h"
#include "itkVariableLengthVector.h"
#include "itkVectorImage.h"
#include "itkImageFileWriter.h"

template< class TImageType, class TGradImageType, unsigned int TComponents >
int itkGradientRecursiveGaussianFilterTest3Run( typename TImageType::PixelType & myPixel, char * outputFilename )
{
  typedef TImageType      myImageType;
  typedef TGradImageType  myGradImageType;

  const unsigned int myComponents = TComponents;

  // Define the dimension of the images
  const unsigned int myDimension = myImageType::ImageDimension;

  // Declare the type of the index to access images
  typedef itk::Index<myDimension>             myIndexType;

  // Declare the type of the size
  typedef itk::Size<myDimension>              mySizeType;

  // Declare the type of the Region
  typedef itk::ImageRegion<myDimension>        myRegionType;

  // Create the image
  typename myImageType::Pointer inputImage  = myImageType::New();

  // Define their size, and start index
  mySizeType size;
  size[0] = 8;
  size[1] = 8;
  size[2] = 8;

  myIndexType start;
  start.Fill(0);

  myRegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  // Initialize Image A
  inputImage->SetLargestPossibleRegion( region );
  inputImage->SetBufferedRegion( region );
  inputImage->SetRequestedRegion( region );
  inputImage->SetNumberOfComponentsPerPixel( myComponents );
  inputImage->Allocate();

  // Declare Iterator type for the input image
  typedef itk::ImageRegionIteratorWithIndex<myImageType>  myIteratorType;

  // Create one iterator for the Input Image A (this is a light object)
  myIteratorType it( inputImage, inputImage->GetRequestedRegion() );

  // Initialize the content of Image A
  myPixel.Fill( 0.0 );
  while( !it.IsAtEnd() )
    {
    it.Set( myPixel );
    ++it;
    }

  size[0] = 4;
  size[1] = 4;
  size[2] = 4;

  start[0] = 2;
  start[1] = 2;
  start[2] = 2;

  // Create one iterator for an internal region
  region.SetSize( size );
  region.SetIndex( start );
  myIteratorType itb( inputImage, region );

  // Initialize the content the internal region
  myPixel.Fill( 100.0 );
  while( !itb.IsAtEnd() )
    {
    itb.Set( myPixel );
    ++itb;
    }

  // Declare the type for the
  typedef itk::GradientRecursiveGaussianImageFilter<myImageType, myGradImageType >  myFilterType;
  typedef typename myFilterType::OutputImageType                                    myGradientImageType;

  // Create a  Filter
  typename myFilterType::Pointer filter = myFilterType::New();

  // Connect the input images
  filter->SetInput( inputImage );

  // Select the value of Sigma
  filter->SetSigma( 2.5 );

  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  // It is important to do it AFTER the filter is Updated
  // Because the object connected to the output may be changed
  // by another during GenerateData() call
  typename myGradientImageType::Pointer outputImage = filter->GetOutput();

  // Declare Iterator type for the output image
  typedef itk::ImageRegionIteratorWithIndex<myGradientImageType>  myOutputIteratorType;

  // Create an iterator for going through the output image
  myOutputIteratorType itg( outputImage, outputImage->GetRequestedRegion() );

  //  Print the content of the result image
  /* dbg
  std::cout << " Result " << std::endl;
  itg.GoToBegin();
  while( !itg.IsAtEnd() )
    {
    std::cout << itg.Get();
    ++itg;
    }
  */

  typedef itk::ImageFileWriter< myGradientImageType >   WriterType;

  typename WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName( outputFilename );
  writer->SetInput( outputImage );
  writer->Update();

  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;
}

int itkGradientRecursiveGaussianFilterTest3(int argc, char *argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " outputImageFile1 outputImageFile2 ";
    return EXIT_FAILURE;
    }

  int result = EXIT_SUCCESS;

  const unsigned int myDimension = 3;
  const unsigned int myComponents = 2;
  typedef itk::Vector<float, myComponents>            myVectorType;
  typedef itk::Image<myVectorType, myDimension>       myImageType;
  typedef itk::Vector<float,myDimension*myComponents> myGradType;
  typedef itk::Image<myGradType, myDimension>         myGradImageType;

  myVectorType pixel;
  int runResult = itkGradientRecursiveGaussianFilterTest3Run<myImageType, myGradImageType, myComponents>( pixel, argv[1] );
  if( runResult == EXIT_FAILURE )
    {
    result = runResult;
    }

  typedef itk::VariableLengthVector<float>             myVector2Type;
  typedef itk::Image<myVector2Type, myDimension>       myImage2Type;

  myVector2Type pixel2;
  pixel2.SetSize( myComponents );
  runResult = itkGradientRecursiveGaussianFilterTest3Run<myImage2Type, myGradImageType, myComponents>( pixel2, argv[2] );
  if( runResult == EXIT_FAILURE )
    {
    result = runResult;
    }

  return result;
}
