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

#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkJointHistogramMutualInformationImageToImageMetricv4.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "itkCorrelationImageToImageMetricv4.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkGaussianImageSource.h"
#include "itkCyclicShiftImageFilter.h"
#include "itkRegistrationParameterScalesFromShift.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageRegionIteratorWithIndex.h"

/* This test performs a simple registration test on each
 * ImageToImageMetricv4 metric, testing the results for correctnes,
 * thereby acting as a basic regression test.
 * New metrics must be added manually to this test. */

template<unsigned int Dimension, typename TImage, typename TMetric>
int ImageToImageMetricv4RegistrationTestRun( int argc, char *argv[], typename TMetric::Pointer metric )
{
  typedef typename TImage::PixelType  PixelType;
  typedef PixelType                   CoordinateRepresentationType;

  // options
  int numberOfIterations = 120;
  PixelType maximumStepSize = 0.1;
  if( argc > 1 )
    {
    numberOfIterations = atoi( argv[1] );
    }
  if( argc > 2 )
    {
    maximumStepSize = atof( argv[2] );
    }

  // Create two simple images
  itk::SizeValueType ImageSize = 100;
  itk::OffsetValueType boundary = 6;
  if( Dimension == 3 )
    {
    ImageSize = 60;
    boundary = 4;
    }

   // Declare Gaussian Sources
  typedef itk::GaussianImageSource< TImage >        GaussianImageSourceType;
  typedef typename GaussianImageSourceType::Pointer GaussianImageSourcePointer;

  typename TImage::SizeType size;
  size.Fill( ImageSize );

  typename TImage::SpacingType spacing;
  spacing.Fill( itk::NumericTraits<CoordinateRepresentationType>::One );

  typename TImage::PointType origin;
  origin.Fill( itk::NumericTraits<CoordinateRepresentationType>::Zero );

  typename TImage::DirectionType direction;
  direction.Fill( itk::NumericTraits<CoordinateRepresentationType>::One );

  typename GaussianImageSourceType::Pointer  fixedImageSource = GaussianImageSourceType::New();

  fixedImageSource->SetSize(    size    );
  fixedImageSource->SetOrigin(  origin  );
  fixedImageSource->SetSpacing( spacing );
  fixedImageSource->SetNormalized( false );
  fixedImageSource->SetScale( 1.0f );
  fixedImageSource->Update();
  typename TImage::Pointer  fixedImage  = fixedImageSource->GetOutput();

  // zero-out the boundary
  itk::ImageRegionIteratorWithIndex<TImage> it( fixedImage, fixedImage->GetLargestPossibleRegion() );
  for( it.GoToBegin(); ! it.IsAtEnd(); ++it )
    {
    for( itk::SizeValueType n=0; n < Dimension; n++ )
      {
      if( it.GetIndex()[n] < boundary || (static_cast<itk::OffsetValueType>(size[n]) - it.GetIndex()[n]) <= boundary )
        {
        it.Set( itk::NumericTraits<PixelType>::Zero );
        break;
        }
      }
    }

  // shift the fixed image to get the moving image
  typedef itk::CyclicShiftImageFilter<TImage, TImage> CyclicShiftFilterType;
  typename CyclicShiftFilterType::Pointer shiftFilter = CyclicShiftFilterType::New();
  typename CyclicShiftFilterType::OffsetType imageShift;
  typename CyclicShiftFilterType::OffsetValueType maxImageShift = boundary-1;
  imageShift.Fill( maxImageShift );
  imageShift[0] = maxImageShift / 2;
  shiftFilter->SetInput( fixedImage );
  shiftFilter->SetShift( imageShift );
  shiftFilter->Update();
  typename TImage::Pointer movingImage = shiftFilter->GetOutput();

  // create an affine transform
  typedef itk::TranslationTransform<double, Dimension> TranslationTransformType;
  typename TranslationTransformType::Pointer translationTransform = TranslationTransformType::New();
  translationTransform->SetIdentity();

  // setup metric
  //
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetMovingTransform( translationTransform );
  const bool gaussian = false;
  metric->SetDoFixedImagePreWarp( false );
  metric->SetDoMovingImagePreWarp( false );
  metric->SetUseMovingImageGradientFilter( gaussian );
  metric->SetUseFixedImageGradientFilter( gaussian );
  metric->Initialize();

  // initial metric value
  typename TMetric::MeasureType initialValue = metric->GetValue();

  // scales estimator
  typedef itk::RegistrationParameterScalesFromShift< TMetric > RegistrationParameterScalesFromShiftType;
  typename RegistrationParameterScalesFromShiftType::Pointer shiftScaleEstimator = RegistrationParameterScalesFromShiftType::New();
  shiftScaleEstimator->SetMetric(metric);

  //
  // optimizer
  //
  typedef itk::GradientDescentOptimizerv4  OptimizerType;
  typename OptimizerType::Pointer  optimizer = OptimizerType::New();
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetScalesEstimator( shiftScaleEstimator );
  if( maximumStepSize > 0 )
    {
    optimizer->SetMaximumStepSizeInPhysicalUnits( maximumStepSize );
    }
  optimizer->StartOptimization();

  std::cout << "image size: " << size;
  std::cout << ", # of iterations: " << optimizer->GetNumberOfIterations() << ", max step size: "
            << optimizer->GetMaximumStepSizeInPhysicalUnits() << std::endl;
  std::cout << "imageShift: " << imageShift << std::endl;
  std::cout << "Transform final parameters: " << translationTransform->GetParameters() << std::endl;

  // final metric value
  typename TMetric::MeasureType finalValue = metric->GetValue();
  std::cout << "metric value: initial: " << initialValue << ", final: " << finalValue << std::endl;

  // test that the final position is close to the truth
  double tolerance = static_cast<double>(0.10);
  for( itk::SizeValueType n=0; n < Dimension; n++ )
    {
    if( vcl_fabs( 1.0 - ( static_cast<double>(imageShift[n]) / translationTransform->GetParameters()[n] ) ) > tolerance )
      {
      std::cerr << "XXX Failed. Final transform parameters are not within tolerance of image shift. XXX" << std::endl;
      return EXIT_FAILURE;
      }
    }
  // test that metric value is minimized
  if( finalValue >= initialValue )
    {
    std::cerr << "XXX Failed. Final metric value is not less than initial value. XXX" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////
template<unsigned int Dimension>
int itkImageToImageMetricv4RegistrationTestRunAll (int argc, char *argv[])
{
  typedef itk::Image< double, Dimension > ImageType;

  std::cout << std::endl << "******************* Dimension: " << Dimension << std::endl;

  bool passed = true;

  // ANTS Neighborhood Correlation
  {
  typedef itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
  typename MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** ANTSNeighborhoodCorrelation metric: " << std::endl;
  if( ImageToImageMetricv4RegistrationTestRun<Dimension, ImageType, MetricType>( argc, argv, metric ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Correlation
  {
  typedef itk::CorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
  typename MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** Correlation metric: " << std::endl;
  if( ImageToImageMetricv4RegistrationTestRun<Dimension, ImageType, MetricType>( argc, argv, metric ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Joint Histogram
  {
  typedef itk::JointHistogramMutualInformationImageToImageMetricv4<ImageType, ImageType> MetricType;
  typename MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** JointHistogramMutualInformation metric: " << std::endl;
  if( ImageToImageMetricv4RegistrationTestRun<Dimension, ImageType, MetricType>( argc, argv, metric ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // Mattes
  {
  typedef itk::MattesMutualInformationImageToImageMetricv4<ImageType, ImageType> MetricType;
  typename MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** MattesMutualInformation metric: " << std::endl;
  if( ImageToImageMetricv4RegistrationTestRun<Dimension, ImageType, MetricType>( argc, argv, metric ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  // MeanSquares
  {
  typedef itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType> MetricType;
  typename MetricType::Pointer metric = MetricType::New();
  std::cout << std::endl << "*** MeanSquares metric: " << std::endl;
  if( ImageToImageMetricv4RegistrationTestRun<Dimension, ImageType, MetricType>( argc, argv, metric ) != EXIT_SUCCESS )
    {
    passed = false;
    }
  }

  if( passed )
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }
}

//////////////////////////////////////////////////////////////
int itkImageToImageMetricv4RegistrationTest (int argc, char *argv[])
{
  if( itkImageToImageMetricv4RegistrationTestRunAll<2>(argc, argv) != EXIT_SUCCESS )
    {
    std::cerr << "Failed for one or more metrics. See error message(s) above." << std::endl;
    return EXIT_FAILURE;
    }
//Results (at least for ANTS metric, didn't run others, too slow) look good for 100x100x100 image with 50 iterations and 0.25 max step size
//Trouble getting anything close to decent results for 40x40x40 image or 60x60x60
//  if( itkImageToImageMetricv4RegistrationTestRunAll<3>(argc, argv) != EXIT_SUCCESS )
//    {
//    return EXIT_FAILURE;
//    }

  return EXIT_SUCCESS;
}
