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

/*
 * Test itkGradientRecursiveGaussianFilter with various types
 * of images of vector and VectorImage.
 */

template< class TImageType, class TGradImageType, unsigned int TComponents >
int itkGradientRecursiveGaussianFilterTest3Run( typename TImageType::PixelType & myPixelBorder, typename TImageType::PixelType & myPixelFill, char * outputFilename )
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
  while( !it.IsAtEnd() )
    {
    it.Set( myPixelBorder );
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
  while( !itb.IsAtEnd() )
    {
    itb.Set( myPixelFill );
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

  // Write the output to file
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
  if( argc != 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " outputImageFile1 outputImageFile2 outputImageFile3 outputImageFile4 outputImageFile5 " << std::endl;
    return EXIT_FAILURE;
    }

  int result = EXIT_SUCCESS;

  const unsigned int myDimension = 3;
  const unsigned int myComponents1D = 1;
  typedef itk::Vector<float,myDimension*myComponents1D>   myGrad1DType;
  typedef itk::Image<myGrad1DType, myDimension>           myGradImage1DType;

  // Test with Image of 1D Vector
  typedef itk::Vector<float, myComponents1D>          myVector1DType;
  typedef itk::Image<myVector1DType, myDimension>     myImageVector1DType;

  myVector1DType vector1Dborder;
  myVector1DType vector1Dfill;
  vector1Dborder.Fill( 0.0 );
  vector1Dfill.Fill( 100.0 );
  int runResult = itkGradientRecursiveGaussianFilterTest3Run<myImageVector1DType, myGradImage1DType, myComponents1D>( vector1Dborder, vector1Dfill, argv[1] );
  if( runResult == EXIT_FAILURE )
    {
    std::cerr << "Failed with Image<1D-Vector> type." << std::endl;
    result = runResult;
    }

  // Test with Image of *scalar* pixels to verify same results
  typedef float                                      myPixelType;
  typedef itk::Image<myPixelType, myDimension>       myImageScalarType;

  myPixelType pixelBorder;
  myPixelType pixelFill;
  pixelBorder = itk::NumericTraits<myPixelType>::Zero;
  pixelFill = static_cast<myPixelType>(100.0);
  runResult = itkGradientRecursiveGaussianFilterTest3Run<myImageScalarType, myGradImage1DType, myComponents1D>( pixelBorder, pixelFill, argv[2] );
  if( runResult == EXIT_FAILURE )
    {
    std::cerr << "Failed with scalar pixel type." << std::endl;
    result = runResult;
    }

  // Test with Image of 2D Vector
  const unsigned int myComponents2D = 2;
  typedef itk::Vector<float,myDimension*myComponents2D> myGrad2DType;
  typedef itk::Image<myGrad2DType, myDimension>         myGradImage2DType;
  typedef itk::Vector<float, myComponents2D>            myVector2DType;
  typedef itk::Image<myVector2DType, myDimension>       myImage2DType;
  typedef itk::VariableLengthVector<float>              myVarVector2DType;
  typedef itk::Image<myVarVector2DType, myDimension>    myImageVar2DType;

  myVector2DType vector2Dborder;
  myVector2DType vector2Dfill;
  vector2Dborder.Fill( 0.0 );
  vector2Dfill[0] = 100.0;
  vector2Dfill[1] = 50.0;
  runResult = itkGradientRecursiveGaussianFilterTest3Run<myImage2DType, myGradImage2DType, myComponents2D>( vector2Dborder, vector2Dfill, argv[3] );
  if( runResult == EXIT_FAILURE )
    {
    std::cerr << "Failed with 2D Vector type." << std::endl;
    result = runResult;
    }

  // Test with Image of 2D VariableLengthVector
  myVarVector2DType varVector2Dborder;
  myVarVector2DType varVector2Dfill;
  varVector2Dborder.SetSize( myComponents2D );
  varVector2Dfill.SetSize( myComponents2D );
  varVector2Dborder.Fill( 0.0 );
  varVector2Dfill[0] = 100.0;
  varVector2Dfill[1] = 50.0;
  runResult = itkGradientRecursiveGaussianFilterTest3Run<myImageVar2DType, myGradImage2DType, myComponents2D>( varVector2Dborder, varVector2Dfill, argv[4] );
  if( runResult == EXIT_FAILURE )
    {
    std::cerr << "Failed with 2D VariableLengthVector type." << std::endl;
    result = runResult;
    }

  // Test with VectorImage
  typedef itk::VectorImage<float, myDimension> myVecImageType;
  runResult = itkGradientRecursiveGaussianFilterTest3Run<myVecImageType, myGradImage2DType, myComponents2D>( varVector2Dborder, varVector2Dfill, argv[5] );
  if( runResult == EXIT_FAILURE )
    {
    std::cerr << "Failed with 2D-vector VectorImage type." << std::endl;
    result = runResult;
    }

  return result;
}
