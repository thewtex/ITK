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

// Software Guide: BeginCommandLineArgs
//    INPUTS:  {BrainProtonDensitySlice.png}
//    INPUTS:  {BrainProtonDensitySliceR10X13Y17S12.png}
//    OUTPUTS: {CompositeTransformRegistration.png}
//    OUTPUTS: {CompositeTransformRegistrationBefore.png}
//    OUTPUTS: {CompositeTransformRegistrationAfter.png}
//  Software Guide : EndCommandLineArgs

// Software Guide : BeginLatex
//
//  The following simple example illustrates how to perform a registration
//  using the composite transform. The registration optimizes the parameters
//  for two transforms, a Rigid and a Scale transform, the addition of which
//  results in the equivalent of a similarity transform.
//
//  \index{itk::CompositeTransform!header}
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkCompositeTransform.h"
#include "itkRigid2DTransform.h"
#include "itkScaleTransform.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
//  The registration uses the Mean squares metric and the Regular Step Gradient
//  Descent optimizer.
//
// Software Guide: EndLatex

// Software Guide : BeginCodeSnippet
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
// Software Guide : EndCodeSnippet

#include "itkImageRegistrationMethod.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

//  The following section of code implements a Command observer
//  used to monitor the evolution of the registration process.
//
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

  // The types of each one of the components in the registration methods should
  // be instantiated. First, we select the image dimension and the type for
  // representing image pixels.

  const    unsigned int    Dimension = 2;
  typedef  unsigned char   PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  //  Software Guide : BeginLatex
  //
  //  The Transform class is instantiated using the code below. It is
  //  templated over the scalar type and the dimension.
  //
  //  \index{itk::CompositeTransform!Instantiation}
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::CompositeTransform<double,Dimension> CompositeType;
  CompositeType::Pointer compositeTransform = CompositeType::New();
  // Software Guide : EndCodeSnippet


  typedef itk::MeanSquaresImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType >    MetricType;
  typedef itk::LinearInterpolateImageFunction<
                                    MovingImageType,
                                    double          >    InterpolatorType;
  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
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

  // Software Guide : BeginLatex
  //
  //  We instantiate a scale and a rigid transform so that we can add
  //  them into the composite transform. The fixed parameters (center of rotation)
  //  are set and the composite transform is passed to the registration method.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::ScaleTransform <double, Dimension> ScaleType;
  ScaleType::Pointer scaleTransform = ScaleType::New();

  typedef itk::Rigid2DTransform <double> RigidType;
  RigidType::Pointer rigid = RigidType::New();

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType fixedParameters = rigid->GetFixedParameters();
  fixedParameters[0] = 91.6129;
  fixedParameters[1] = 111.929;

  scaleTransform->SetFixedParameters(fixedParameters);
  rigid->SetFixedParameters( fixedParameters );

  registration->SetTransform( compositeTransform );
  // Software Guide : EndCodeSnippet

  // Sofware Guide : BeginLatex
  //
  //  We add the scale and the rigid transform, necessarily in that order, into the composite
  //  transform as the most recently added transform's parameters will be outputed first.
  //
  // Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  compositeTransform->AddTransform ( scaleTransform );
  compositeTransform->AddTransform ( rigid );
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  We now pass the parameters of the composite transform as the initial
  //  parameters to be used when the registration process starts.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters(compositeTransform->GetNumberOfParameters() );
  initialParameters = compositeTransform->GetParameters();
  registration->SetInitialTransformParameters ( initialParameters );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Keeping in mind that the scale of units in scaling, rotation and
  //  translation are quite different, we take advantage of the scaling
  //  functionality provided by the optimizers. We know that the first element
  //  of the parameters array corresponds to the angle, the second and third
  //  correspond to the translation parameters, and the fourth and fifth correspond
  //  to the scale parameters. We use henceforth small factors in the scales
  //  associated with translations.
  //
  //  Software Guide : EndLatex


  // Software Guide : BeginCodeSnippet
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( compositeTransform->GetNumberOfParameters() );
  const double translationScale = 0.0001;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = translationScale;
  optimizerScales[2] = translationScale;
  optimizerScales[3] = 10.0;
  optimizerScales[4] = 10.0;

  optimizer->SetScales( optimizerScales );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  //  Below, we define the optimization parameters like initial step length,
  //  minimal step length and number of iterations. These last two act as
  //  stopping criteria for the optimization.
  //
  // Software Guide : EndLatex

  double initialStepLength = 2.00;

  if( argc > 6 )
    {
    initialStepLength = atof( argv[6] );
    }

  // Software Guide : BeginCodeSnippet
  optimizer->SetMaximumStepLength( initialStepLength );
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetNumberOfIterations( 500 );
  // Software Guide : EndCodeSnippet

  // Create the Command observer and register it with the optimizer.
  //
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

  //  Software Guide : BeginLatex
  //
  //  Let's execute this example over some of the images provided in
  //  \code{Examples/Data}, for example:
  //
  //  \begin{itemize}
  //  \item \code{BrainProtonDensitySlice.png}
  //  \item \code{BrainProtonDensitySliceR10X13Y17S12.png}
  //  \end{itemize}
  //
  //  The second image is the result of intentionally rotating the first image
  //  by $10$ degrees, scaling by $1/1.2$ and then translating by $(-13,-17)$.
  //  The registration takes $412$ iterations and produces:
  //
  //  \begin{center}
  //  \begin{verbatim}
  //  [-0.174502, 26.9376, 30.9931, 0.83325, 0.833205]
  //  \end{verbatim}
  //  \end{center}
  //
  //  That are interpreted as
  //
  //  \begin{itemize}
  //  \item Angle          =                     $0.174521$   radians
  //  \item Translation    = $( 26.9376    , 30.9931    )$ millimeters
  //  \item Scale factor X = $( 0.83325    , 0.833205   )$
  //  \end{itemize}
  //
  // Software Guide : EndLatex

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
