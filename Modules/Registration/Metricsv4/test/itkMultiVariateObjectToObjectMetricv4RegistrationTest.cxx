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

#include "itkCorrelationImageToImageMetricv4.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkGaussianImageSource.h"
#include "itkCyclicShiftImageFilter.h"
#include "itkRegistrationParameterScalesFromShift.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMultiVariateObjectToObjectMetricv4.h"

/* This test performs a simple registration test using
 * a single metric and a multivariate metric containing
 * two copies of the metric, testing
 * that the results are the same.
 */

template<class TImage>
void MultiVariateObjectToObjectMetricv4RegistrationTestCreateImages( typename TImage::Pointer & fixedImage, typename TImage::Pointer & movingImage )
{
  typedef typename TImage::PixelType  PixelType;
  typedef PixelType                   CoordinateRepresentationType;

  // Create two simple images
  itk::SizeValueType ImageSize = 100;
  itk::OffsetValueType boundary = 6;

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
  fixedImage = fixedImageSource->GetOutput();

  // zero-out the boundary
  itk::ImageRegionIteratorWithIndex<TImage> it( fixedImage, fixedImage->GetLargestPossibleRegion() );
  for( it.GoToBegin(); ! it.IsAtEnd(); ++it )
    {
    for( itk::SizeValueType n=0; n < TImage::ImageDimension; n++ )
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
  movingImage = shiftFilter->GetOutput();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename TMetric>
int MultiVariateObjectToObjectMetricv4RegistrationTestRun( typename TMetric::Pointer & metric, int numberOfIterations,
                                        typename TMetric::MeasureType & valueResult, typename TMetric::DerivativeType & derivativeResult )
{
  // calculate initial metric value
  metric->Initialize();
  typename TMetric::MeasureType initialValue = metric->GetValue();

  // scales estimator
// not ready yet for multi-variate metric
//  typedef itk::RegistrationParameterScalesFromShift< TMetric > RegistrationParameterScalesFromShiftType;
//  typename RegistrationParameterScalesFromShiftType::Pointer shiftScaleEstimator = RegistrationParameterScalesFromShiftType::New();
//  shiftScaleEstimator->SetMetric(metric);

  //
  // optimizer
  //
  typedef itk::GradientDescentOptimizerv4  OptimizerType;
  typename OptimizerType::Pointer  optimizer = OptimizerType::New();

  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
// not ready yet for multi-variate metric
//  optimizer->SetScalesEstimator( shiftScaleEstimator );
optimizer->SetLearningRate( 0.2 );

  optimizer->StartOptimization();

  std::cout << "# of iterations: " << optimizer->GetNumberOfIterations() << std::endl;

  derivativeResult = optimizer->GetCurrentPosition();
  std::cout << "Transform final parameters: " << derivativeResult << " mag: " << derivativeResult.magnitude() << std::endl;

  // final metric value
  valueResult = metric->GetValue();
  std::cout << "metric value: initial: " << initialValue << ", final: " << valueResult << std::endl;

  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////
int itkMultiVariateObjectToObjectMetricv4RegistrationTest(int , char *[])
{
  const int Dimension = 2;
  typedef itk::Image< double, Dimension > ImageType;

  int numberOfIterations = 10;

  // create an affine transform
  typedef itk::TranslationTransform<double, Dimension> TranslationTransformType;
  TranslationTransformType::Pointer translationTransform = TranslationTransformType::New();
  translationTransform->SetIdentity();

  // create images
  ImageType::Pointer fixedImage, movingImage;
  MultiVariateObjectToObjectMetricv4RegistrationTestCreateImages<ImageType>( fixedImage, movingImage );

  typedef itk::CorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
  MetricType::Pointer metric = MetricType::New();
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetMovingTransform( translationTransform );

  translationTransform->SetIdentity();

  std::cout << std::endl << "*** Single image metric: " << std::endl;
  MetricType::MeasureType singleValueResult;
  MetricType::DerivativeType singleDerivativeResult;
  MultiVariateObjectToObjectMetricv4RegistrationTestRun<MetricType>( metric, 40, singleValueResult, singleDerivativeResult );

  std::cout << "*** multi-variate metric: " << std::endl;
  typedef itk::MultiVariateObjectToObjectMetricv4<Dimension,Dimension> MultiVariateType;
  MultiVariateType::Pointer multiMetric = MultiVariateType::New();
  multiMetric->AddMetric( metric );
  multiMetric->AddMetric( metric );
  translationTransform->SetIdentity();

  MetricType::MeasureType multiValueResult;
  MetricType::DerivativeType multiDerivativeResult;
  MultiVariateObjectToObjectMetricv4RegistrationTestRun<MultiVariateType>( multiMetric, 40, multiValueResult, multiDerivativeResult );

  // compare results
  // Comparison between single-metric and multi-variate metric registrations will have to
  // wait until support for scale estimators is added
  MetricType::DerivativeValueType tolerance = static_cast<MetricType::DerivativeValueType>(0.1);
  if( vcl_fabs( multiDerivativeResult[0] - 2.0 ) / 2.0 > tolerance ||
      vcl_fabs( multiDerivativeResult[1] - 5.0 ) / 5.0 > tolerance )
      {
      std::cerr << "multi-variate registration did not produce expected results of [2,5]: " << multiDerivativeResult << std::endl;
      return EXIT_FAILURE;
      }

  return EXIT_SUCCESS;
}
