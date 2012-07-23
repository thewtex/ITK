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

#include "itkCentralDifferenceImageFunction.h"
#include "itkImageRegionIterator.h"

int itkCentralDifferenceImageFunctionOnVectorTest(int, char* [] )
{
  int result = EXIT_SUCCESS;

  const unsigned int                            ImageDimension = 2;
  typedef itk::Vector<float,2>                  PixelType;
  typedef itk::Image<PixelType,ImageDimension>  ImageType;

  ImageType::Pointer image = ImageType::New();
  ImageType::SizeType size;
  size.Fill( 16 );
  ImageType::RegionType region( size );

  image->SetRegions( region );
  image->Allocate();

  // make a test image
  typedef itk::ImageRegionIterator<ImageType> Iterator;
  Iterator iter( image, region );
  iter.GoToBegin();
  unsigned int counter = 0;

  while ( !iter.IsAtEnd() )
    {
    PixelType pix;
    pix[0] = counter * counter;
    pix[1] = counter;
    iter.Set( pix );
    ++counter;
    ++iter;
    }

  // set up central difference calculator
  typedef float                   CoordRepType;
  typedef itk::Matrix<double,2,2> DerivativeType;
  typedef ImageType::PixelType    PixelType;

  typedef itk::CentralDifferenceImageFunction<ImageType,CoordRepType,DerivativeType>  FunctionType;
  typedef FunctionType::OutputType                                                    OutputType;

  FunctionType::Pointer function = FunctionType::New();

  function->SetInputImage( image );

  ImageType::IndexType index;

  // pick an index inside the image
  index.Fill( 8 );
  OutputType indexOutput = function->EvaluateAtIndex( index );
  std::cout << "Index: " << index << " Derivative: ";
  std::cout << indexOutput << std::endl;

  // verify the output
  OutputType truthOutput;
  for( unsigned int i = 0; i < 2; i++ )
    {
    PixelType deriv;
    ImageType::IndexType indexTest = index;
    indexTest[i] = indexTest[i] + 1.0;
    deriv = image->GetPixel( indexTest );
    indexTest[i] = indexTest[i] - 2.0;
    deriv -= image->GetPixel( indexTest );
    deriv /= 2.0;
    truthOutput[0][i] = deriv[0];
    truthOutput[1][i] = deriv[1];
    }

  if( indexOutput != truthOutput )
    {
    std::cerr << "ERROR: indexOutput " << indexOutput << " does not match truth: " << truthOutput << std::endl;
    result = EXIT_FAILURE;
    }

  if ( function->IsInsideBuffer( index ) )
    {
    std::cout << "Index: " << index << " is inside the BufferedRegion." << std::endl;
    }
  else
    {
    std::cerr << "Expected index " << index << " to be inside BufferedRegion. " << std::endl;
    result = EXIT_FAILURE;
    }

  // test continuous index
  FunctionType::ContinuousIndexType cindex;
  cindex.Fill( 8.0 );
  OutputType continuousIndexOutput = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: ";
  std::cout << continuousIndexOutput << std::endl;

  if( indexOutput != continuousIndexOutput )
    {
    std::cerr << "ERROR: Output of EvaluateAtIndex and EvaluateAtContinuousIndex do not match." << std::endl;
    result = EXIT_FAILURE;
    }

  FunctionType::PointType point;
  point.Fill( 8.0 );
  OutputType pointOutput = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: ";
  std::cout << pointOutput << std::endl;

  // this should be the same as output from EvaluateAtIndex as long as
  // image is setup with default spatial information.
  if( indexOutput != pointOutput )
    {
    std::cerr << "ERROR: Output of EvaluateAtIndex and Evaluate do not match." << std::endl;
    result = EXIT_FAILURE;
    }

  // pick an index on the image edge
  index.Fill( 8 );
  index[0] = 15;
  indexOutput = function->EvaluateAtIndex( index );
  std::cout << "Index: " << index << " Derivative: ";
  std::cout << indexOutput << std::endl;

  if ( function->IsInsideBuffer( index ) )
    {
    std::cout << "Index: " << index << " is inside the BufferedRegion." << std::endl;
    }

  cindex.Fill( 8.0 );
  cindex[0] = 15.0;
  continuousIndexOutput = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: ";
  std::cout << continuousIndexOutput << std::endl;

  if( indexOutput != continuousIndexOutput )
    {
    std::cerr << "ERROR: Output of EvaluateAtIndex and EvaluateAtContinuousIndex "
              << "do not match at boundary." << std::endl;
    result = EXIT_FAILURE;
    }

  point.Fill( 8.0 );
  point[0] = 15.0;
  pointOutput = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: ";
  std::cout << pointOutput << std::endl;

  if( indexOutput != pointOutput )
    {
    std::cerr << "ERROR: Output of EvaluateAtIndex and EvaluateAtContinuousIndex "
              << "do not match at boundary." << std::endl;
    result = EXIT_FAILURE;
    }

  //
  // test out-of-bounds indecies
  //
  std::cout << "Test out of bounds indecies: " << std::endl;
  index.Fill( size[0] );
  indexOutput = function->EvaluateAtIndex( index );
  std::cout << "Index: " << index << " Derivative: " << indexOutput << std::endl;
  OutputType zeroVector;
  zeroVector.Fill( itk::NumericTraits<OutputType::ValueType>::ZeroValue() );

  if ( function->IsInsideBuffer( index ) )
    {
    std::cerr << "ERROR: Expected index: " << index << " to be outside the BufferedRegion." << std::endl;
    result = EXIT_FAILURE;
    }

  if( indexOutput != zeroVector )
    {
    std::cerr << "ERROR: Expected zeroVector return for indexOutput, but got: " << indexOutput << std::endl;
    result = EXIT_FAILURE;
    }

  // test out-of-bounds in a single dimension
  index.Fill( 8 );
  index[1] = size[1];
  OutputType halfOutOutput = function->EvaluateAtIndex( index );
  std::cout << "Index: " << index << " Derivative: " << indexOutput << std::endl;

  if( halfOutOutput != zeroVector )
    {
    std::cerr << "ERROR: Expected zero for return for halfOutOutput, but got: " << halfOutOutput << std::endl;
    result = EXIT_FAILURE;
    }

  // test out-of-bounds with continuous index
  cindex.Fill( size[0] );
  continuousIndexOutput = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: " << continuousIndexOutput << std::endl;

  if( continuousIndexOutput != zeroVector )
    {
    std::cerr << "ERROR: Expected zeroVector return for continuousIndexOutput, but got: " << indexOutput << std::endl;
    result = EXIT_FAILURE;
    }

  // test out-of-bounds with a point
  point.Fill( size[0] );
  pointOutput = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: " << pointOutput << std::endl;

  if( pointOutput != zeroVector )
    {
    std::cerr << "ERROR: Expected zeroVector return for pointOutput, but got: " << indexOutput << std::endl;
    result = EXIT_FAILURE;
    }

  // test results at non-interger positions
  std::cout << "Test non-integer position for EvaluateAtContinuousIndex. "
            << std::endl;
  cindex.Fill( 8.0 );
  cindex[0] = 8.0;
  OutputType center = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: ";
  std::cout << center << std::endl;
  cindex[0] = 7.5;
  OutputType left = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: ";
  std::cout << left << std::endl;
  cindex[0] = 8.5;
  OutputType right = function->EvaluateAtContinuousIndex( cindex );
  std::cout << "ContinuousIndex: " << cindex << " Derivative: ";
  std::cout << right << std::endl;

  if( center == left || center == right )
    {
    std::cerr << "ERROR: Failed for EvaluateAtContinuousIndex at non-interger indecies. "
              << "Results are unexpectedly identical." << std::endl;
    result = EXIT_FAILURE;
    }

  if( fabs( ( right[0][0] + left[0][0] ) / 2.0 - center[0][0] ) > 1e-06 )
    {
    std::cerr << "ERROR: Failed for EvaluateAtContinuousIndex at non-integer incecies. Center index result is not average of left and right." << std::endl;
    result = EXIT_FAILURE;
    }

  std::cout << "Test non-integer position for Evaluate. " << std::endl;
  point.Fill( 8.0 );
  point[0] = 8.0;
  center = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: " << center << std::endl;
  point[0] = 7.5;
  left = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: " << left << std::endl;
  point[0] = 8.5;
  right = function->Evaluate( point );
  std::cout << "Point: " << point << " Derivative: " << right << std::endl;

  if( center == left || center == right )
    {
    std::cerr << "ERROR: Failed for Evaluate at non-interger indecies. Results are unexpectedly identical." << std::endl;
    result = EXIT_FAILURE;
    }

  if( fabs( ( right[0][0] + left[0][0] ) / 2.0 - center[0][0] ) > 1e-06 )
    {
    std::cerr << "ERROR: Failed for Evaluate at non-integer incecies. Center index result is not average of left and right." << std::endl;
    result = EXIT_FAILURE;
    }

  // test image direction and Evaluate
  point.Fill( 8.0 );
  OutputType origDerivative = function->Evaluate( point );
  std::cout << "Point: " << point << " origDerivative: " << origDerivative << std::endl;

  ImageType::DirectionType direction;
  direction[0][0] = -1.0;
  direction[0][1] = 0.0;
  direction[1][0] = 0.0;
  direction[1][1] = -1.0;
  point.Fill( -8.0 );
  image->SetDirection( direction );
  function->SetUseImageDirection( true );
  OutputType directionOnDerivative = function->Evaluate( point );
  std::cout << "Point: " << point << " directionOnDerivative: " << directionOnDerivative << std::endl;

  if( directionOnDerivative[0][0] != -origDerivative[0][0] ||
      directionOnDerivative[0][1] != -origDerivative[0][1] )
    {
    std::cerr << "ERROR: Expected origDerivative and directionOnDerivative to be opposite." << std::endl;
    result = EXIT_FAILURE;
    }

  // with image direction disabled, result should be same as with
  // identity direction
  function->SetUseImageDirection( false );
  OutputType directionOffDerivative = function->Evaluate( point );
  std::cout << "Point: " << point << " directionOffDerivative: " << directionOffDerivative << std::endl;

  if( directionOffDerivative != origDerivative )
    {
    std::cerr << "Expected origDerivative == directionOffDerivative." << std::endl;
    result = EXIT_FAILURE;
    }

  if( result == EXIT_SUCCESS )
    {
    std::cout << "Test passed." << std::endl;
    }
  return result;

}
