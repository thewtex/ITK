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
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkCompositeTransform.h"
#include "itkScaleTransform.h"
#include "itkTranslationTransform.h"
#include "itkSimilarity2DTransform.h"
#include "itkRigid2DTransform.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCommand.h"


class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::RegularStepGradientDescentOptimizer  OptimizerType;
  typedef   const OptimizerType *                   OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer =
      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;

    }
};

int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " outputImagefile  [differenceAfterRegistration] ";
    std::cerr << " [differenceBeforeRegistration] ";
    std::cerr << " [initialStepLength] "<< std::endl;
    return EXIT_FAILURE;
    }

  /* The inputs images that have been tested with this example are
     Fixed Image: BrainProtonDensitySlice.png
     Moving Image: BrainProtonDensitySliceR10X13Y17S12.png

     Here we add a rigid and a scale transform into the composite transform deque, which is
     the equivalent of a similarity transform.
     Results may be compared with the same images being used with ImageRegistration7 example, which
     uses a centered similarity transform in the registration.
  */
  const    unsigned int    Dimension = 2;
  typedef  unsigned char   PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
  typedef itk::MeanSquaresImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType >    MetricType;
  typedef itk:: LinearInterpolateImageFunction<
                                    MovingImageType,
                                    double          >    InterpolatorType;
  typedef itk::ImageRegistrationMethod<
                                    FixedImageType,
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );
  fixedImageReader->Update();

  registration->SetFixedImageRegion( fixedImageReader->GetOutput()->GetBufferedRegion() );

  fixedImageReader->Update();
  movingImageReader->Update();

  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  typedef itk::CompositeTransform<double,Dimension> CompositeType;
  CompositeType::Pointer compositeTransform = CompositeType::New();

  typedef itk::ScaleTransform <double, Dimension> ScaleType;
  ScaleType::Pointer scaleTransform = ScaleType::New();

  typedef itk::Rigid2DTransform <double> RigidType;
  RigidType::Pointer rigid1 = RigidType::New();

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType fixedParameters = rigid1->GetFixedParameters();
  fixedParameters[0] = 91.6129;
  fixedParameters[1] = 111.929;

  scaleTransform->SetFixedParameters(fixedParameters);
  rigid1->SetFixedParameters( fixedParameters );
  registration->SetTransform( compositeTransform );

  // Add a scale and rigid transform. The combination of these two is the equivalent of a similarity transform.
  compositeTransform->AddTransform ( scaleTransform );
  compositeTransform->AddTransform ( rigid1 );

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters(compositeTransform->GetNumberOfParameters() );
  initialParameters = compositeTransform->GetParameters();
  registration->SetInitialTransformParameters ( initialParameters );

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( compositeTransform->GetNumberOfParameters() );

  const double translationScale = 0.0001;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = translationScale;
  optimizerScales[2] = translationScale;
  optimizerScales[3] = 10.0;
  optimizerScales[4] = 10.0;

  ParametersType scalefixed (scaleTransform->GetFixedParameters() );
  std::cout << " scale fixed parameters: " << scalefixed << std::endl;

  optimizer->SetScales( optimizerScales );

  double initialStepLength = 2.00;

  if( argc > 6 )
    {
    initialStepLength = atof( argv[6] );
    }

  optimizer->SetMaximumStepLength( initialStepLength );
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetNumberOfIterations( 500 );

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  try
    {
    registration->StartRegistration();
    std::cout << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }


  OptimizerType::ParametersType finalParameters =
                    registration->GetLastTransformParameters();

  std::cout << finalParameters << std::endl;

  const double finalAngle1           = finalParameters[0];
  const double finalTranslationX1    = finalParameters[1];
  const double finalTranslationY1    = finalParameters[2];
  const double finalScaleX           = finalParameters[3];
  const double finalScaleY           = finalParameters[4];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  const double bestValue = optimizer->GetValue();

  // Print out results

  const double finalAngleInDegrees1 = finalAngle1 * 180.0 / vnl_math::pi;
  std::cout << "Result = " << std::endl;
  std::cout << " Angle (radians)   = " << finalAngle1  << std::endl;
  std::cout << " Angle (degrees)   = " << finalAngleInDegrees1  << std::endl;
  std::cout << " Translation X = "     << finalTranslationX1  << std::endl;
  std::cout << " Translation Y = "     << finalTranslationY1  << std::endl;
  std::cout << " Scalex = "            << finalScaleX << std::endl;
  std::cout << " Scaley = "            << finalScaleY << std::endl;


  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  typedef itk::ResampleImageFilter<
                            MovingImageType,
                            FixedImageType >    ResampleFilterType;

  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  resample->SetTransform( registration->GetOutput()->Get() );
  resample->SetInput( movingImageReader->GetOutput() );

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 100 );

  typedef itk::ImageFileWriter< FixedImageType >  WriterFixedType;

  WriterFixedType::Pointer      writer =  WriterFixedType::New();

  writer->SetFileName( argv[3] );

  writer->SetInput( resample->GetOutput()   );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "ExceptionObject while writing the resampled image !" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< float, Dimension > DifferenceImageType;

  typedef itk::SubtractImageFilter<
                           FixedImageType,
                           FixedImageType,
                           DifferenceImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  typedef  unsigned char  OutputPixelType;

  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  typedef itk::RescaleIntensityImageFilter<
                                  DifferenceImageType,
                                  OutputImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();

  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  difference->SetInput1( fixedImageReader->GetOutput() );
  difference->SetInput2( resample->GetOutput() );

  resample->SetDefaultPixelValue( 1 );

  intensityRescaler->SetInput( difference->GetOutput() );

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  WriterType::Pointer      writer2 =  WriterType::New();

  writer2->SetInput( intensityRescaler->GetOutput() );


  try
    {
    // Compute the difference image between the
    // fixed and moving image after registration.
    if( argc > 4 )
      {
      writer2->SetFileName( argv[4] );
      writer2->Update();
      }

    // Compute the difference image between the
    // fixed and resampled moving image after registration.
    RigidType::Pointer identityTransform = RigidType::New();
    identityTransform->SetIdentity();
    resample->SetTransform( identityTransform );
    if( argc > 5 )
      {
      writer2->SetFileName( argv[5] );
      writer2->Update();
      }
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error while writing difference images" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
