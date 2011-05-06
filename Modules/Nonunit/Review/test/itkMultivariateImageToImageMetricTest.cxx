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

#include "itkMultivariateImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkGaussianImageSource.h"

/** This test illustrates the use of the MultivariateImageToImageMetric class, which
    takes N Image to Image metrics and assigns a weight to each metric's
    gradient.
 */
int itkMultivariateImageToImageMetricTest (int argc, char *argv[])
{
  try
    {
    // Create two simple images
    const unsigned int Dimension = 2;
    typedef unsigned char PixelType;
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
    FixedImageType::SizeValueType fixedImageSize[]     = {  100,  100 };
    MovingImageType::SizeValueType movingImageSize[]    = {  100,  100 };

    FixedImageType::SpacingValueType fixedImageSpacing[]  = { 1.0f, 1.0f };
    MovingImageType::SpacingValueType movingImageSpacing[] = { 1.0f, 1.0f };

    FixedImageType::PointValueType fixedImageOrigin[]   = { 0.0f, 0.0f };
    MovingImageType::PointValueType movingImageOrigin[]  = { 0.0f, 0.0f };

    MovingImageSourceType::Pointer movingImageSource = MovingImageSourceType::New();
    FixedImageSourceType::Pointer  fixedImageSource  = FixedImageSourceType::New();

    fixedImageSource->SetSize(    fixedImageSize    );
    fixedImageSource->SetOrigin(  fixedImageOrigin  );
    fixedImageSource->SetSpacing( fixedImageSpacing );
    fixedImageSource->SetNormalized( false );
    fixedImageSource->SetScale( 250.0f );

    movingImageSource->SetSize(    movingImageSize    );
    movingImageSource->SetOrigin(  movingImageOrigin  );
    movingImageSource->SetSpacing( movingImageSpacing );
    movingImageSource->SetNormalized( false );
    movingImageSource->SetScale( 250.0f );

    movingImageSource->Update();  // Force the filter to run
    fixedImageSource->Update();   // Force the filter to run

    MovingImageType::Pointer movingImage = movingImageSource->GetOutput();
    FixedImageType::Pointer  fixedImage  = fixedImageSource->GetOutput();

    // Set up the metric.
    typedef itk::MultivariateImageToImageMetric<FixedImageType,
             MovingImageType> MultivariateMetricType;
    typedef MultivariateMetricType::WeightsArrayType WeightsArrayType;
    typedef MultivariateMetricType::ParametersType   ParametersType;

    MultivariateMetricType::Pointer multivariatemetric = MultivariateMetricType::New();

    // Instantiate and Add metrics to the queue
    typedef itk::MeanSquaresImageToImageMetric <FixedImageType,MovingImageType> MeanSquaresMetricType;
    typedef itk::MattesMutualInformationImageToImageMetric <FixedImageType,MovingImageType> MattesMutualInformationMetricType;

    MeanSquaresMetricType::Pointer msq = MeanSquaresMetricType::New();
    MattesMutualInformationMetricType::Pointer mi = MattesMutualInformationMetricType::New();

    multivariatemetric->AddMetric(msq);
    multivariatemetric->AddMetric(mi);

    // Plug the images into the metric
    msq->SetFixedImage(fixedImage);
    msq->SetMovingImage(movingImage);
    mi->SetFixedImage(fixedImage);
    mi->SetMovingImage(movingImage);

    // Set up a transform
    typedef itk::TranslationTransform<
                        CoordinateRepresentationType,
                        Dimension >         TransformType;

    TransformType::Pointer transform = TransformType::New();

    msq->SetTransform( transform.GetPointer() );
    mi->SetTransform( transform.GetPointer() );

    // Set up an interpolator
    typedef itk::LinearInterpolateImageFunction<MovingImageType,
      CoordinateRepresentationType> InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetInputImage(movingImage.GetPointer());

    msq->SetInterpolator(interpolator.GetPointer() );
    mi->SetInterpolator(interpolator.GetPointer() );

    // Define the region over which the metric will be computed.
    msq->SetFixedImageRegion(fixedImage->GetBufferedRegion());
    mi->SetFixedImageRegion(fixedImage->GetBufferedRegion());


    // Set up transform parameters.
    const unsigned int numberOfParameters = transform->GetNumberOfParameters();
    ParametersType parameters( numberOfParameters );
    for (unsigned int k = 0; k < numberOfParameters; k++)
      {
      parameters[k] = 1.0;
      }

    msq->Initialize();
    mi->Initialize();

    // Set Derivative scales
    WeightsArrayType derivativeScales (multivariatemetric->GetNumberOfMetrics() );
    derivativeScales[0] = 0.1;
    derivativeScales[1] = 2.0;
    multivariatemetric->SetMetricDerivativeWeights(derivativeScales);

    // Print out metric value and derivative.
    MultivariateMetricType::MeasureType measure = 0;
    MultivariateMetricType::DerivativeType derivative;
    multivariatemetric->GetValueAndDerivative(parameters, measure, derivative);

    MultivariateMetricType::MeasureType metricvalue = 0;
    MultivariateMetricType::DerivativeType metricderivative;
    for (unsigned int i = 0; i < multivariatemetric->GetNumberOfMetrics() ; i++)
      {
      (multivariatemetric->GetMetricsQueue()[i])->GetValueAndDerivative(parameters, metricvalue, metricderivative);
      std::cout << " Metric " << i << " value : " << metricvalue << std::endl;
      std::cout << " Metric " << i << " derivative : " << metricderivative << std::endl << std::endl;
      }

    std::cout << "Multivariate measure: " << measure << std::endl
              << "Derivative : " << derivative << std::endl << std::endl;

    // Exercising the Print function
    multivariatemetric->Print(std::cout);

    std::cout << "Test passed." << std::endl;
    }
  catch (itk::ExceptionObject& exp)
    {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
