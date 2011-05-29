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

#include "itkImageRegistrationMethod.h"
#include "itkAffineTransform.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkAdaptiveGradientDescentOptimizer.h"

#include "itkOptimizerParameterEstimator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegistrationMethodImageSource.h"

/**
 *  This program tests the automatic estimation of parameter scales in optimizers
 *
 *
 */

int itkOptimizerParameterEstimatorTest_Func( int argc,
                                           char* argv[],
                                           bool subtractMean )
{
  bool pass = true;

  const unsigned int dimension = 2;

  // Fixed Image Type
  typedef itk::Image<float,dimension>               FixedImageType;

  // Moving Image Type
  typedef itk::Image<float,dimension>               MovingImageType;

  // Size Type
  typedef MovingImageType::SizeType                 SizeType;


  // ImageSource
  typedef itk::testhelper::ImageRegistrationMethodImageSource<
                                  FixedImageType::PixelType,
                                  MovingImageType::PixelType,
                                  dimension >         ImageSourceType;

  // Transform Type
  typedef itk::AffineTransform< double, dimension > TransformType;
  typedef TransformType::ParametersType             ParametersType;

  // Optimizer Type
  typedef itk::AdaptiveGradientDescentOptimizer            OptimizerType;

  // Metric Type
  typedef itk::NormalizedCorrelationImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType >    MetricType;

  // Interpolation technique
  typedef itk:: LinearInterpolateImageFunction<
                                    MovingImageType,
                                    double >             InterpolatorType;

  // Registration Method
  typedef itk::ImageRegistrationMethod<
                                    FixedImageType,
                                    MovingImageType >    RegistrationType;

  typedef itk::CommandIterationUpdate<
                                  OptimizerType >    CommandIterationType;


  MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  TransformType::Pointer      trasform      = TransformType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  ImageSourceType::Pointer    imageSource   = ImageSourceType::New();

  SizeType size;
  size[0] = 100;
  size[1] = 100;

  imageSource->GenerateImages( size );

  FixedImageType::ConstPointer     fixedImage    = imageSource->GetFixedImage();
  MovingImageType::ConstPointer    movingImage   = imageSource->GetMovingImage();

  //
  // Connect all the components required for Registratio
  //
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedImage(    fixedImage    );
  registration->SetMovingImage(   movingImage   );
  registration->SetInterpolator(  interpolator  );

  // Select the Region of Interest over which the Metric will be computed
  // Registration time will be proportional to the number of pixels in this region.
  metric->SetFixedImageRegion( fixedImage->GetBufferedRegion() );

  // Turn on/off subtract mean flag.
  metric->SetSubtractMean( subtractMean );

  // Instantiate an Observer to report the progress of the Optimization
  CommandIterationType::Pointer iterationCommand = CommandIterationType::New();
  iterationCommand->SetOptimizer(  optimizer.GetPointer() );

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );
  scales.Fill( 1.0 );

  unsigned long   numberOfIterations =   100;
  double          learningRate       = 1e-4;

  if( subtractMean )
    {
    // Factoring out the mean causes the optimization valley
    // to be more narrow and hence we need to reduce the
    // the learning rate.
    learningRate *= 0.2;
    }

  if( argc > 1 )
    {
    numberOfIterations = atol( argv[1] );
    std::cout << "numberOfIterations = " << numberOfIterations << std::endl;
    }
  if( argc > 2 )
    {
    learningRate = atof( argv[2] );
    std::cout << "learningRate = " << learningRate << std::endl;
    }

  //optimizer->SetLearningRate( learningRate );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetMaximize(false);

  // Start from an Identity transform (in a normal case, the user
  // can probably provide a better guess than the identity...
  transform->SetIdentity();
  registration->SetInitialTransformParameters( transform->GetParameters() );

  // Testing parameter scale estimator
  typedef itk::OptimizerParameterEstimator< FixedImageType,
                                        MovingImageType,
                                        TransformType > OptimizerParameterEstimatorType;
  OptimizerParameterEstimatorType::Pointer parameterEstimator = OptimizerParameterEstimatorType::New();

  parameterEstimator->SetFixedImage(fixedImage);
  parameterEstimator->SetMovingImage(movingImage);
  parameterEstimator->SetScaleStrategy(OptimizerParameterEstimatorType::ScaleWithShift);
  //parameterEstimator->SetScaleStrategy(OptimizerParameterEstimatorType::ScaleWithJacobian);
  parameterEstimator->SetLearningRateStrategy(OptimizerParameterEstimatorType::LearningWithShift);

  optimizer->SetOptimizerHelper( parameterEstimator );
  // Setting parameter scales done

  // Initialize the internal connections of the registration method.
  // This can potentially throw an exception
  try
    {
    registration->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << e << std::endl;
    pass = false;
    }

  ParametersType actualParameters = imageSource->GetActualParameters();
  ParametersType finalParameters  = registration->GetLastTransformParameters();

  const unsigned int numbeOfParameters = actualParameters.Size();

  // We know that for the Affine transform the Translation parameters are at
  // the end of the list of parameters.
  const unsigned int offsetOrder = finalParameters.Size()-actualParameters.Size();

  const double tolerance = 1.0;  // equivalent to 1 pixel.
  std::cout << "Estimated scales = " << optimizer->GetScales() << std::endl;
  std::cout << "Estimated learningRate = " << optimizer->GetLearningRate() << std::endl;
  std::cout << "finalParameters = " << finalParameters << std::endl;
  std::cout << "actualParameters = " << actualParameters << std::endl;

  for(unsigned int i=0; i<numbeOfParameters; i++)
    {
    // the parameters are negated in order to get the inverse transformation.
    // this only works for comparing translation parameters....
    std::cout << finalParameters[i+offsetOrder] << " == " << -actualParameters[i] << std::endl;
    if( vnl_math_abs ( finalParameters[i+offsetOrder] - (-actualParameters[i]) ) > tolerance )
      {
      std::cout << "Tolerance exceeded at component " << i << std::endl;
      pass = false;
      }
    }

  if( !pass )
    {
    std::cout << "Test FAILED." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test PASSED." << std::endl;
  return EXIT_SUCCESS;


}

int itkOptimizerParameterEstimatorTest( int argc, char* argv[] )
{
  // test metric without factoring out the mean.
  int fail1 = itkOptimizerParameterEstimatorTest_Func( argc, argv, false );

  // test metric with factoring out the mean.
  int fail2 = itkOptimizerParameterEstimatorTest_Func( argc, argv, true );

  if( fail1 || fail2 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
