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

#include "itkMultiVariateObjectToObjectMetricv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkJointHistogramMutualInformationImageToImageMetricv4.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkGaussianImageSource.h"
#include "itkShiftScaleImageFilter.h"
#include "itkTestingMacros.h"
#include "itkCompositeTransform.h"
#include "itkEuclideanDistancePointSetToPointSetMetricv4.h"
#include "itkExpectationBasedPointSetToPointSetMetricv4.h"

/** This test illustrates the use of the MultivariateImageToImageMetric class, which
    takes N metrics and assigns a weight to each metric's result.
    This first test uses image metrics only.
 */

const unsigned int MultiVariateObjectToObjectMetricv4TestDimension = 2;
typedef itk::MultiVariateObjectToObjectMetricv4<MultiVariateObjectToObjectMetricv4TestDimension,MultiVariateObjectToObjectMetricv4TestDimension>
                                                                                    MultiVariateObjectToObjectMetricv4TestMultivariateMetricType;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int itkMultiVariateObjectToObjectMetricv4TestRun ( MultiVariateObjectToObjectMetricv4TestMultivariateMetricType::Pointer & multiVariateMetric )
{
  typedef MultiVariateObjectToObjectMetricv4TestMultivariateMetricType    MultivariateMetricType;

  // Setup weights
  MultivariateMetricType::WeightsArrayType origMetricWeights( multiVariateMetric->GetNumberOfMetrics() );
  MultivariateMetricType::WeightValueType weightSum = 0;
  for( itk::SizeValueType n = 0; n < multiVariateMetric->GetNumberOfMetrics(); n++ )
    {
    origMetricWeights[n] = static_cast<MultivariateMetricType::WeightValueType>( n + 1 );
    weightSum += origMetricWeights[n];
    }
  multiVariateMetric->SetMetricWeights( origMetricWeights );

  // Initialize. This initializes all the component metrics.
  std::cout << "Initialize" << std::endl;
  multiVariateMetric->Initialize();

  // Print out metric value and derivative.
  typedef MultivariateMetricType::MeasureType MeasureType;
  MeasureType measure = 0;
  MultivariateMetricType::DerivativeType combinedDerivative;
  std::cout << "GetValueAndDerivative" << std::endl;
  try
    {
    multiVariateMetric->GetValueAndDerivative( measure, combinedDerivative );
    }
  catch (itk::ExceptionObject& exp)
    {
    std::cerr << "Exception caught during call to GetValueAndDerivative:" << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Multivariate measure: " << measure << std::endl
            << "  Derivative : " << combinedDerivative << std::endl << std::endl;

  // Test GetDerivative
  MultivariateMetricType::DerivativeType combinedDerivative2;
  multiVariateMetric->GetDerivative( combinedDerivative2 );
  for( MultivariateMetricType::NumberOfParametersType p = 0; p < multiVariateMetric->GetNumberOfParameters(); p++ )
    {
    if( combinedDerivative2[p] != combinedDerivative[p] )
      {
      std::cerr << "Results do not match between GetValueAndDerivative and GetDerivative." << std::endl;
      return EXIT_FAILURE;
      }
    }

  // Test GetValue method
  MeasureType measure2 = 0;
  std::cout << "GetValue" << std::endl;
  try
    {
    measure2 = multiVariateMetric->GetValue();
    }
  catch (itk::ExceptionObject& exp)
    {
    std::cerr << "Exception caught during call to GetValue:" << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }
  if( measure2 != measure )
    {
    std::cerr << "measure does not match between calls to GetValue and GetValueAndDerivative: "
              << "measure: " << measure << " measure2: " << measure2 << std::endl;
    return EXIT_FAILURE;
    }

  // Evaluate individually
  MeasureType metricValue = itk::NumericTraits<MeasureType>::Zero;
  MeasureType weightedMetricValue = itk::NumericTraits<MeasureType>::Zero;
  MultivariateMetricType::DerivativeType metricDerivative;
  MultivariateMetricType::DerivativeType combinedDerivativeTruth( multiVariateMetric->GetNumberOfParameters() );
  combinedDerivativeTruth.Fill( itk::NumericTraits<MultivariateMetricType::DerivativeValueType>::Zero );
  for (itk::SizeValueType i = 0; i < multiVariateMetric->GetNumberOfMetrics(); i++)
    {
    std::cout << "GetValueAndDerivative on component metrics" << std::endl;
    multiVariateMetric->GetMetricQueue()[i]->GetValueAndDerivative( metricValue, metricDerivative );
    std::cout << " Metric " << i << " value : " << metricValue << std::endl;
    std::cout << " Metric " << i << " derivative : " << metricDerivative << std::endl << std::endl;
    if( metricValue != multiVariateMetric->GetValueArray()[i] )
      {
      std::cerr << "Individual metric value " << metricValue
                << " does not match that returned from multi-variate metric: " << multiVariateMetric->GetValueArray()[i]
                << std::endl;
      return EXIT_FAILURE;
      }
    weightedMetricValue += metricValue * origMetricWeights[i] / weightSum;
    for( MultivariateMetricType::NumberOfParametersType p = 0; p < multiVariateMetric->GetNumberOfParameters(); p++ )
      {
      combinedDerivativeTruth[p] += metricDerivative[p] * ( origMetricWeights[i] / weightSum ) / metricDerivative.magnitude();
      }
    }

  if( vcl_fabs( weightedMetricValue - multiVariateMetric->GetWeightedValue() ) > 1e-6 )
    {
    std::cerr << "Computed weighted metric value " << weightedMetricValue << " does match returned value "
              << multiVariateMetric->GetWeightedValue() << std::endl;
    return EXIT_FAILURE;
    }

  for( MultivariateMetricType::NumberOfParametersType p = 0; p < multiVariateMetric->GetNumberOfParameters(); p++ )
    {
    MultivariateMetricType::DerivativeValueType tolerance = static_cast<MultivariateMetricType::DerivativeValueType> (1e-6);
    if( vcl_fabs(combinedDerivativeTruth[p] - combinedDerivative[p]) > tolerance )
      {
      std::cerr << "Error: combinedDerivative does not match expected result." << std::endl
                << "  combinedDerivative: " << combinedDerivative << std::endl
                << "  combinedDerivativeTruth: " << combinedDerivativeTruth << std::endl;
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////

int itkMultiVariateObjectToObjectMetricv4Test (int , char *[])
{
  // Create two simple images
  const unsigned int Dimension = MultiVariateObjectToObjectMetricv4TestDimension;
  typedef double PixelType;
  typedef double CoordinateRepresentationType;

  // Allocate Images
  typedef itk::Image<PixelType,Dimension> FixedImageType;
  typedef itk::Image<PixelType,Dimension> MovingImageType;

   // Declare Gaussian Sources
  typedef itk::GaussianImageSource< MovingImageType >  MovingImageSourceType;
  typedef itk::GaussianImageSource< FixedImageType  >  FixedImageSourceType;
  typedef MovingImageSourceType::Pointer               MovingImageSourcePointer;
  typedef FixedImageSourceType::Pointer                FixedImageSourcePointer;

  // Note: the following declarations are classical arrays
  FixedImageType::SizeValueType     fixedImageSize[]     = {  100,  100 };
  FixedImageType::SpacingValueType  fixedImageSpacing[]  = { 1.0f, 1.0f };
  FixedImageType::PointValueType    fixedImageOrigin[]   = { 0.0f, 0.0f };
  FixedImageSourceType::Pointer     fixedImageSource     = FixedImageSourceType::New();

  fixedImageSource->SetSize(    fixedImageSize    );
  fixedImageSource->SetOrigin(  fixedImageOrigin  );
  fixedImageSource->SetSpacing( fixedImageSpacing );
  fixedImageSource->SetNormalized( false );
  fixedImageSource->SetScale( 1.0f );
  fixedImageSource->Update();   // Force the filter to run
  FixedImageType::Pointer  fixedImage  = fixedImageSource->GetOutput();

  typedef itk::ShiftScaleImageFilter<FixedImageType, MovingImageType> ShiftScaleFilterType;
  ShiftScaleFilterType::Pointer shiftFilter = ShiftScaleFilterType::New();
  shiftFilter->SetInput( fixedImage );
  shiftFilter->SetShift( 2.0 );
  shiftFilter->Update();
  MovingImageType::Pointer movingImage = shiftFilter->GetOutput();

  // Set up the metric.
  typedef MultiVariateObjectToObjectMetricv4TestMultivariateMetricType    MultivariateMetricType;
  typedef MultivariateMetricType::WeightsArrayType                      WeightsArrayType;
  typedef MultivariateMetricType::ParametersType                        ParametersType;

  MultivariateMetricType::Pointer multiVariateMetric = MultivariateMetricType::New();

  // Instantiate and Add metrics to the queue
  typedef itk::JointHistogramMutualInformationImageToImageMetricv4<FixedImageType,MovingImageType> JointHistorgramMetrictype;
  typedef itk::MeanSquaresImageToImageMetricv4<FixedImageType,MovingImageType>  MeanSquaresMetricType;
  typedef itk::MattesMutualInformationImageToImageMetricv4 <FixedImageType,MovingImageType> MattesMutualInformationMetricType;
  typedef itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<FixedImageType,MovingImageType> ANTSNCMetricType;

  MeanSquaresMetricType::Pointer              m1 = MeanSquaresMetricType::New();
  MattesMutualInformationMetricType::Pointer  m2 = MattesMutualInformationMetricType::New();
  JointHistorgramMetrictype::Pointer          m3 = JointHistorgramMetrictype::New();
  ANTSNCMetricType::Pointer                   m4 = ANTSNCMetricType::New();
  // Need to do this until upcoming patch 5489 has been merged in
  m4->SetVirtualDomainImage( fixedImage );

  // Set up a transform
  typedef itk::TranslationTransform<CoordinateRepresentationType,Dimension> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

  // Plug the images and transform into the metrics
  std::cout << "Setup metrics" << std::endl;
  m1->SetFixedImage(fixedImage);
  m1->SetMovingImage(movingImage);
  m1->SetMovingTransform( transform );
  m2->SetFixedImage(fixedImage);
  m2->SetMovingImage(movingImage);
  m2->SetMovingTransform( transform );
  m3->SetFixedImage(fixedImage);
  m3->SetMovingImage(movingImage);
  m3->SetMovingTransform( transform );
  m4->SetFixedImage(fixedImage);
  m4->SetMovingImage(movingImage);
  m4->SetMovingTransform( transform );

  // Add the component metrics
  std::cout << "Add component metrics" << std::endl;
  multiVariateMetric->AddMetric(m1);
  multiVariateMetric->AddMetric(m2);
  multiVariateMetric->AddMetric(m3);
  multiVariateMetric->AddMetric(m4);

  if( multiVariateMetric->GetMetricQueue()[0] != m1 || multiVariateMetric->GetMetricQueue()[3] != m4 )
    {
    std::cerr << "GetMetricQueue failed." << std::endl;
    }

  std::cout << "*** Test image metrics *** " << std::endl;
  if( itkMultiVariateObjectToObjectMetricv4TestRun( multiVariateMetric ) != EXIT_SUCCESS )
    {
    return EXIT_FAILURE;
    }

  std::cout << "*** Test with mismatched transforms *** " << std::endl;
  TransformType::Pointer transform2 = TransformType::New();
  m4->SetMovingTransform( transform2 );
  TRY_EXPECT_EXCEPTION( multiVariateMetric->Initialize() );
  m4->SetMovingTransform( transform );

  std::cout << "*** Test with proper CompositeTransform ***" << std::endl;
  typedef itk::CompositeTransform<CoordinateRepresentationType,Dimension> CompositeTransformType;
  CompositeTransformType::Pointer compositeTransform = CompositeTransformType::New();
  compositeTransform->AddTransform( transform2 );
  compositeTransform->AddTransform( transform );
  compositeTransform->SetOnlyMostRecentTransformToOptimizeOn();
  m4->SetMovingTransform( compositeTransform );
  if( itkMultiVariateObjectToObjectMetricv4TestRun( multiVariateMetric ) != EXIT_SUCCESS )
    {
    return EXIT_FAILURE;
    }

  std::cout << "*** Test with CompositeTransform - too many active transforms ***" << std::endl;
  compositeTransform->SetAllTransformsToOptimizeOn();
  TRY_EXPECT_EXCEPTION( multiVariateMetric->Initialize() );

  std::cout << "*** Test with CompositeTransform - one active transform, but wrong one ***" << std::endl;
  compositeTransform->SetAllTransformsToOptimizeOff();
  compositeTransform->SetNthTransformToOptimizeOn( 0 );
  TRY_EXPECT_EXCEPTION( multiVariateMetric->Initialize() );

  // Reset
  m4->SetMovingTransform( transform );

  //
  // Test with point set metrics
  //
  typedef itk::PointSet<float, Dimension> PointSetType;
  PointSetType::Pointer fixedPoints = PointSetType::New();
  PointSetType::Pointer movingPoints = PointSetType::New();
  fixedPoints->Initialize();
  movingPoints->Initialize();

  PointSetType::PointType point;
  for( itk::SizeValueType n = 0; n < 100; n++ )
    {
    point[0] = n * 1.0;
    point[1] = n * 2.0;
    fixedPoints->SetPoint( n, point );
    point[0] += 0.5;
    point[1] += 0.5;
    movingPoints->SetPoint( n, point );
    }

  typedef itk::ExpectationBasedPointSetToPointSetMetricv4<PointSetType> ExpectationPointSetMetricType;
  typedef itk::EuclideanDistancePointSetToPointSetMetricv4<PointSetType> EuclideanPointSetMetricType;
  ExpectationPointSetMetricType::Pointer expectationPointSetMetric = ExpectationPointSetMetricType::New();
  EuclideanPointSetMetricType::Pointer   euclideanPointSetMetric = EuclideanPointSetMetricType::New();

  expectationPointSetMetric->SetFixedPointSet( fixedPoints );
  expectationPointSetMetric->SetMovingPointSet( movingPoints );
  expectationPointSetMetric->SetMovingTransform( transform );
  euclideanPointSetMetric->SetFixedPointSet( fixedPoints );
  euclideanPointSetMetric->SetMovingPointSet( movingPoints );
  euclideanPointSetMetric->SetMovingTransform( transform );

  multiVariateMetric->AddMetric( expectationPointSetMetric );
  multiVariateMetric->AddMetric( euclideanPointSetMetric );

  std::cout << "*** Test with PointSet metrics and Image metrics *** " << std::endl;
  if( itkMultiVariateObjectToObjectMetricv4TestRun( multiVariateMetric ) != EXIT_SUCCESS )
    {
    return EXIT_FAILURE;
    }

  // Exercising the Print function
  std::cout << "Print: " << std::endl;
  multiVariateMetric->Print(std::cout);

  multiVariateMetric->ClearMetricQueue();
  if( multiVariateMetric->GetNumberOfMetrics() != 0 )
    {
    std::cerr << "ClearMetricQueue() failed. Number of metrics is not zero." << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
