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


#include <iostream>
#include <time.h>

#include "itkAffineTransform.h"
#include "itkImage.h"
#include "itkResampleImageFilterTransduceFunction.h"
#include "itkResampleImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkPixelValueSpatialTransformationTransduceFunctionBase.h"
#include "itkPixelValueSpatialTransformationTransduceFunctionJacobianDeterminant.h"

int itkResampleImageFilterTransduceFunctionTest(int argc, char* argv[] )
{
  /* Test3 - test using a different TransduceFunction, and basic speed profiling.
   * Arguments: iterations - number of iterations to run for the speed profiling.
   */

  if( argc < 2 )
    {
    std::cout << "Error. Usage: " << argv[0] << " IterationsForSpeedProfiling"
              << std::endl;
    return EXIT_FAILURE;
    }
  unsigned int profilingIterations = atoi( argv[1] );
  if( profilingIterations == 0 )
    {
    std::cout << "Error parsing iterations, or zero-value passed."
              << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int NDimensions = 2;

  typedef float                                 PixelType;
  typedef itk::Image<PixelType, NDimensions>    ImageType;
  typedef ImageType::IndexType                  ImageIndexType;
  typedef ImageType::Pointer                    ImagePointerType;
  typedef ImageType::RegionType                 ImageRegionType;
  typedef ImageType::SizeType                   ImageSizeType;
  typedef double                                CoordRepType;

  typedef itk::AffineTransform<CoordRepType,NDimensions>   AffineTransformType;
  typedef itk::LinearInterpolateImageFunction<ImageType,CoordRepType>
    InterpolatorType;

  // Create and configure an image
  ImagePointerType image = ImageType::New();
  ImageIndexType  index = {{0,  0}};
  ImageSizeType   size  = {{1000, 1000}};
  ImageRegionType region;
  region.SetSize ( size );
  region.SetIndex( index );
  image->SetLargestPossibleRegion( region );
  image->SetBufferedRegion( region );
  image->Allocate();

  // Fill image with a ramp
  itk::ImageRegionIteratorWithIndex<ImageType> iter(image, region);
  PixelType value;
  for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
    {
    index = iter.GetIndex();
    value = index[0] + index[1];
    iter.Set(value);
    }

  // Create an affine transformation
  AffineTransformType::Pointer aff = AffineTransformType::New();
  double affineScale = 0.5;
  aff->Scale( affineScale );

  // Create a linear interpolation image function
  InterpolatorType::Pointer interp = InterpolatorType::New();
  interp->SetInputImage(image);

  // Define the JacobianDeterminant transduce function.
  typedef itk::PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant
                                              <
                                              PixelType,
                                              PixelType,
                                              ImageType::PointType,
                                              ImageType::ImageDimension,
                                              ImageType::ImageDimension
                                              > TransduceFunctionJacobianType;

  // Create and configure a resampling filter
  typedef itk::ResampleImageFilterTransduceFunction
    < ImageType, ImageType, CoordRepType, TransduceFunctionJacobianType >
    ResampleJacobianType;
  ResampleJacobianType::Pointer resampleJac = ResampleJacobianType::New();
  resampleJac->SetInput(image);
  resampleJac->SetSize(size);
  resampleJac->SetTransform(aff);
  resampleJac->SetInterpolator(interp);
  index.Fill( 0 );
  resampleJac->SetOutputStartIndex( index );

  ImageType::PointType origin;
  origin.Fill( 0.0 );
  resampleJac->SetOutputOrigin( origin );
  ImageType::SpacingType spacing;
  spacing.Fill( 1.0 );
  resampleJac->SetOutputSpacing( spacing );
  // Run the resampling filter
  resampleJac->Update();

  // Check if desired results were obtained.
  // See ...TransduceFunctionJacobianDeterminant.h for the transformation
  // that's performed.
  bool passed = true;
  ImageType::RegionType region2;
  region2 = resampleJac->GetOutput()->GetRequestedRegion();
  itk::ImageRegionIteratorWithIndex<ImageType>
      iter2(resampleJac->GetOutput(), region2);
  PixelType pixval;
  AffineTransformType::MatrixValueType determinant =
    vnl_determinant( aff->GetMatrix().GetVnlMatrix() );
  const double tolerance = 1e-30;
  for (iter2.GoToBegin(); !iter2.IsAtEnd(); ++iter2)
    {
    index  = iter2.GetIndex();
    value  = iter2.Get();
    pixval = value;
    PixelType expectedValue = static_cast<PixelType>
      ( (index[0] + index[1]) * affineScale * determinant );
    if ( vcl_fabs( expectedValue - pixval ) > tolerance )
      {
      std::cout << "Error in resampled JacobianDeterminant image: Pixel "
                << index << " "
                << "determinant = " << determinant << "  "
                << "value    = " << value << "  "
                << "pixval   = " << pixval << "  "
                << "expected = " << expectedValue << std::endl;
      passed = false;
      }
    }
  // Report success or failure
  if (!passed)
    {
    std::cout << "Resampling test failed" << std::endl;
    return EXIT_FAILURE;
    }

  // Exercise other member functions
  std::cout << "TransduceFunction: " << resampleJac->GetTransduceFunction()
            << std::endl;

  /* Perform some basic speed profiling on identity transduce function,
   * comparing with original ResampleImageFilter.
   */
  //...TransducFunctionBase performs identity operation.
  typedef itk::PixelValueSpatialTransformationTransduceFunctionBase
                                              <
                                              PixelType,
                                              ImageType::PointType,
                                              ImageType::ImageDimension,
                                              ImageType::ImageDimension
                                              > TransduceFunctionBaseType;
  typedef itk::ResampleImageFilterTransduceFunction
    < ImageType, ImageType, CoordRepType, TransduceFunctionBaseType >
    ResampleBaseType;
  ResampleBaseType::Pointer resampleTransduceBase = ResampleBaseType::New();
  resampleTransduceBase->SetInput(image);
  resampleTransduceBase->SetSize(size);
  resampleTransduceBase->SetTransform(aff);
  resampleTransduceBase->SetInterpolator(interp);
  index.Fill( 0 );
  resampleTransduceBase->SetOutputStartIndex( index );
  origin.Fill( 0.0 );
  resampleTransduceBase->SetOutputOrigin( origin );
  spacing.Fill( 1.0 );
  resampleTransduceBase->SetOutputSpacing( spacing );

  typedef itk::ResampleImageFilter
    < ImageType, ImageType, CoordRepType > ResampleOrigType;
  ResampleOrigType::Pointer resampleOrig = ResampleOrigType::New();
  resampleOrig->SetInput(image);
  resampleOrig->SetSize(size);
  resampleOrig->SetTransform(aff);
  resampleOrig->SetInterpolator(interp);
  index.Fill( 0 );
  resampleOrig->SetOutputStartIndex( index );
  origin.Fill( 0.0 );
  resampleOrig->SetOutputOrigin( origin );
  spacing.Fill( 1.0 );
  resampleOrig->SetOutputSpacing( spacing );

  //Run the profiling
  clock_t tTransduce=0, tOrig=1, tJacobian=0;
  std::cout << "Profiling test: " << std::endl;
  std::cout << " Image size: " << size << std::endl;
  std::cout << " Iterations: " << profilingIterations << std::endl;

  std::cout << " resampleOrig...: " << std::endl;
  itk::MultiThreader * threader = resampleOrig->GetMultiThreader();
  std::cout << " # of threads: " << threader->GetNumberOfThreads() << std::endl;
  tOrig = clock();
  for( unsigned int it=0; it < profilingIterations; it++ )
    {
    resampleOrig->Update();
    resampleOrig->Modified();
    }
  tOrig = clock() - tOrig;

  std::cout << " resampleTransduceBase...: " << std::endl;
  threader = resampleTransduceBase->GetMultiThreader();
  std::cout << " # of threads: " << threader->GetNumberOfThreads() << std::endl;
  tTransduce = clock();
  //resampleTransduceBase->m_TransduceFunction->SetTransform( aff );
  //resampleTransduceBase->m_TransduceFunction->Prepare();
  for( unsigned int it=0; it < profilingIterations; it++ )
    {
    resampleTransduceBase->Update();
    resampleTransduceBase->Modified();
    }
  tTransduce = clock() - tTransduce;

  /** NOTE **/
  /* When this code is included, at least in no-threading build, then the
   * profiling time reported for TransduceBase also changes. Weird! */
  /*
  std::cout << " resampleTransduceJacobian...: " << std::endl;
  threader = resampleJac->GetMultiThreader();
  std::cout << " # of threads: " << threader->GetNumberOfThreads() << std::endl;
  tJacobian = clock();
  for( unsigned int it=0; it < profilingIterations; it++ )
    {
    resampleJac->Update();
    resampleJac->Modified();
    }
  tJacobian = clock() - tJacobian;
  */
  std::cout << " id transduce func filter clocks:  " << tTransduce << std::endl
            << " orignal resample filter clocks:   " << tOrig << std::endl
            << " jacobian transduce filter clocks: " << tJacobian << std::endl
            << " id / original:       " << tTransduce / (double)tOrig
            << std::endl
            << " jacobian / original: " << tJacobian / (double)tOrig
            << std::endl;

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;

}
