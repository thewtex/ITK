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

/**
 * Test program for JointHistogramMutualInformationImageToImageObjectMetric and
 * RegularStepGradientDescentObjectOptimizer classes.
 *
 * Perform a registration using user-supplied images.
 * No numerical verification is performed. Test passes as long
 * as no exception occurs.
 *
 */
#include "itkJointHistogramMutualInformationImageToImageObjectMetric.h"
#include "itkRegularStepGradientDescentObjectOptimizer.h"
#include "itkRegistrationParameterScalesFromShift.h"

#include "itkIdentityTransform.h"
#include "itkTranslationTransform.h"
#include "itkAffineTransform.h"
#include "itkEuler2DTransform.h"
#include "itkCompositeTransform.h"
#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransform.h"
#include "itkRegistrationParameterScalesFromJacobian.h"

#include "itkCastImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itksys/SystemTools.hxx"
#include "itkResampleImageFilter.h"
#include "itkShrinkImageFilter.h"

//FIXME We need these as long as we have to define ImageToData and
// Array1DToData as a fwd-declare in itkImageToImageObjectMetric.h
#include "itkImageToData.h"
#include "itkArray1DToData.h"


class RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationCommandIterationUpdate : public itk::Command
{
public:
  typedef  RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationCommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );

protected:
  RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationCommandIterationUpdate() {};

public:

  typedef itk::RegularStepGradientDescentObjectOptimizer     OptimizerType;
  typedef const OptimizerType *                              OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer =
                         dynamic_cast< OptimizerPointer >( object );

    if( itk::IterationEvent().CheckEvent( &event ) )
      {
      if( optimizer->GetCurrentIteration() % 10 == 1 )
        {
        std::cout << "Iteration " << optimizer->GetCurrentIteration()
                  << " = " << optimizer->GetValue()
                  << " (showing every 10). " << std::endl;
        }
      }
    if( itk::StartEvent().CheckEvent( &event ) )
      {
      std::cout << "Estimated scales: " << optimizer->GetScales() << std::endl;
      }
  }

};

/* Helper method to shrink images for faster testing as needed. */
template<class TImageType>
void RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationTestShrink( typename TImageType::Pointer & image, unsigned int shrinkFactor)
{
  typedef itk::ShrinkImageFilter<TImageType,TImageType> ShrinkFilterType;

  typename ShrinkFilterType::Pointer filter = ShrinkFilterType::New();

  filter->SetInput( image );
  filter->SetShrinkFactors( shrinkFactor );
  filter->Update();
  image = filter->GetOutput();
}

int itkRegularStepJointHistogramMutualInformationImageToImageObjectRegistrationTest(int argc, char *argv[])
{

  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile movingImageFile ";
    std::cerr << " outputImageFile ";
    std::cerr << " [numberOfIterations numberOfDisplacementIterations] ";
    std::cerr << " [affineMaxStepLength = 10] [displacementMaxStepLength = 10]";
    std::cerr << " [sparseSamplingStep = 1 (== dense sampling)] ";
    std::cerr << " [imageShrinkFactor = 1 ] ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << argc << std::endl;
  unsigned int numberOfIterations = 10;
  unsigned int numberOfDisplacementIterations = 10;
  unsigned int sparseSamplingStep = 1; //dense sampling
  double affineMaxStepLength = 10.0;
  double displacementMaxStepLength = 10.0;
  unsigned int imageShrinkFactor = 1;
  if( argc >= 5 )
    {
    numberOfIterations = atoi( argv[4] );
    }
  if( argc >= 6 )
    {
    numberOfDisplacementIterations = atof( argv[5] );
    }
  if( argc >= 7 )
    {
    affineMaxStepLength = atof( argv[6] );
    }
  if( argc >= 8 )
    {
    displacementMaxStepLength = atof( argv[7] );
    }
  if( argc >= 9 )
    {
    sparseSamplingStep = atoi( argv[8] );
    }
  if( argc >= 10 )
    {
    imageShrinkFactor = atoi( argv[9] );
    }

  bool useSparseSampling = sparseSamplingStep > 1;
  if( sparseSamplingStep < 1 )
    {
    std::cerr << "sparseSamplingStep must be >=1. It is " << sparseSamplingStep
              << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << " affine iterations: "<< numberOfIterations
    << " displacementIterations: " << numberOfDisplacementIterations << std::endl
    << " affineMaxStepLength: " << affineMaxStepLength
    << " displacementMaxStepLength: " << displacementMaxStepLength << std::endl
    << " sparseSamplingStep: " << sparseSamplingStep
    << " useSparseSampling: " << useSparseSampling
    << " imageShrinkFactor: " << imageShrinkFactor << std::endl;

  const unsigned int Dimension = 2;
  typedef double PixelType; //I assume png is unsigned short

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer fixedImageReader   = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName( argv[1] );
  movingImageReader->SetFileName( argv[2] );

  //get the images
  fixedImageReader->Update();
  FixedImageType::Pointer  fixedImage = fixedImageReader->GetOutput();
  movingImageReader->Update();
  MovingImageType::Pointer movingImage = movingImageReader->GetOutput();

  // resize the images for faster computation during default tests
  RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationTestShrink<
    FixedImageType>( fixedImage, imageShrinkFactor );
  RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationTestShrink<
    MovingImageType>( movingImage, imageShrinkFactor );

  /** define a resample filter that will ultimately be used to deform the image */
  typedef itk::ResampleImageFilter<
                            MovingImageType,
                            FixedImageType >    ResampleFilterType;
  ResampleFilterType::Pointer resample = ResampleFilterType::New();


  /** create a composite transform holder for other transforms  */
  typedef itk::CompositeTransform<double, Dimension>    CompositeType;
  typedef CompositeType::ScalarType                     ScalarType;

  CompositeType::Pointer compositeTransform = CompositeType::New();

  //create an affine transform
  typedef itk::AffineTransform<double, Dimension>
                                                    AffineTransformType;
  AffineTransformType::Pointer affineTransform = AffineTransformType::New();
  affineTransform->SetIdentity();
  std::cout <<" affineTransform params " << affineTransform->GetParameters() << std::endl;
  typedef itk::GaussianSmoothingOnUpdateDisplacementFieldTransform<
                                                    double, Dimension>
                                                     DisplacementTransformType;
  DisplacementTransformType::Pointer displacementTransform =
                                              DisplacementTransformType::New();
  typedef DisplacementTransformType::DisplacementFieldType
                                                         DisplacementFieldType;
  DisplacementFieldType::Pointer field = DisplacementFieldType::New();

  //set the field to be the same as the fixed image region, which will
  // act by default as the virtual domain in this example.
  field->SetRegions( fixedImage->GetLargestPossibleRegion() );
  //make sure the field has the same spatial information as the image
  field->CopyInformation( fixedImage );
  std::cout << "fixedImage->GetLargestPossibleRegion(): "
            << fixedImage->GetLargestPossibleRegion() << std::endl
            << "fixedImage->GetBufferedRegion(): "
            << fixedImage->GetBufferedRegion() << std::endl;
  field->Allocate();
  // Fill it with 0's
  DisplacementTransformType::OutputVectorType zeroVector;
  zeroVector.Fill( 0 );
  field->FillBuffer( zeroVector );
  // Assign to transform
  displacementTransform->SetDisplacementField( field );
  displacementTransform->SetGaussianSmoothingVarianceForTheUpdateField( 6 );
  displacementTransform->SetGaussianSmoothingVarianceForTheTotalField( 6 );

  //identity transform for fixed image
  typedef itk::IdentityTransform<double, Dimension> IdentityTransformType;
  IdentityTransformType::Pointer identityTransform = IdentityTransformType::New();
  identityTransform->SetIdentity();

  // The metric
  typedef itk::JointHistogramMutualInformationImageToImageObjectMetric
    < FixedImageType, MovingImageType >         MetricType;
  typedef MetricType::FixedSampledPointSetType  PointSetType;
  MetricType::Pointer metric = MetricType::New();
  metric->SetNumberOfHistogramBins(20);
  if( useSparseSampling )
    {
    std::cout << "Affine: testing metric with sparse point set..." << std::endl;
    typedef PointSetType::PointType     PointType;
    PointSetType::Pointer               pset(PointSetType::New());
    unsigned long ind=0,ct=0;
    itk::ImageRegionIteratorWithIndex<FixedImageType>
      It(fixedImage, fixedImage->GetLargestPossibleRegion() );
    for( It.GoToBegin(); !It.IsAtEnd(); ++It )
      {
      // take every N^th point
      if ( ct % sparseSamplingStep == 0  )
        {
          PointType pt;
          fixedImage->TransformIndexToPhysicalPoint( It.GetIndex(), pt);
          pset->SetPoint(ind, pt);
          ind++;
        }
        ct++;
      }
    std::cout << "Setting point set with " << ind << " points of "
              << fixedImage->GetLargestPossibleRegion().GetNumberOfPixels()
              << " total " << std::endl;
    metric->SetFixedSampledPointSet( pset );
    }
  else
    {
    std::cout << "Affine: testing metric with dense sampling..." << std::endl;
    }
  metric->SetUseFixedSampledPointSet( useSparseSampling );


  // Assign images and transforms.
  // By not setting a virtual domain image or virtual domain settings,
  // the metric will use the fixed image for the virtual domain.
  //  metric->SetVirtualDomainImage( fixedImage );
  metric->SetFixedImage( fixedImage );
  metric->SetMovingImage( movingImage );
  metric->SetFixedTransform( identityTransform );
  metric->SetMovingTransform( affineTransform );
  metric->SetDoFixedImagePreWarp( ! useSparseSampling );
  metric->SetDoMovingImagePreWarp( ! useSparseSampling );
  bool gaussian = false;
  metric->SetUseMovingImageGradientFilter( gaussian );
  metric->SetUseFixedImageGradientFilter( gaussian );
  metric->Initialize();

  // Create a scales estimator to use in the optimizer.
  // The RegularStepGradientDescentOptimizer will use this to
  // estimate the parameter scales at the begin of optimization,
  // but will not use it to estimate the learning rate since it
  // has its own learning rate estimation method.
  typedef itk::RegistrationParameterScalesFromShift< MetricType >
    RegistrationParameterScalesFromShiftType;
  RegistrationParameterScalesFromShiftType::Pointer shiftScaleEstimator
    = RegistrationParameterScalesFromShiftType::New();
  shiftScaleEstimator->SetMetric(metric);

  std::cout << "First do an affine registration " << std::endl;
  typedef itk::RegularStepGradientDescentObjectOptimizer  OptimizerType;
  OptimizerType::Pointer  optimizer = OptimizerType::New();
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetScalesEstimator( shiftScaleEstimator );
  optimizer->SetGradientMagnitudeTolerance( 1e-6 );
  optimizer->SetMaximumStepLength( affineMaxStepLength );
  optimizer->SetMinimumStepLength( 1e-6 );
  optimizer->SetRelaxationFactor( 0.5 );

  // Connect an observer
  typedef RegularStepJointHistogramMutualInformationImageToImageObjectRegistrationCommandIterationUpdate UpdateType;
  UpdateType::Pointer observer = UpdateType::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  // Optimize
  optimizer->StartOptimization();

  std::cout << "Optimization completed with stop condition: "
            << optimizer->GetStopConditionDescription() << std::endl
            << "Scales: " << optimizer->GetScales() << std::endl
            << "Final current-step: " << optimizer->GetCurrentStepLength()
            << std::endl;

  std::cout << "Follow affine with deformable registration " << std::endl;

  // now add the displacement field to the composite transform
  compositeTransform->AddTransform( affineTransform );
  compositeTransform->AddTransform( displacementTransform );
  // set back to optimize all.
  compositeTransform->SetAllTransformsToOptimizeOn();
  // set to optimize the displacement field
  compositeTransform->SetOnlyMostRecentTransformToOptimizeOn();
  metric->SetMovingTransform( compositeTransform );
  metric->SetDoFixedImagePreWarp( ! useSparseSampling );
  // CompositeTransform needs a fix for TransformCovariantVector before this
  // can be set to false. ALSO, with very sparse point set, results are
  // bad, so be ware. If a point set of 50% sparsity is used, you can double
  // the number of iterations and get good result.
  metric->SetDoMovingImagePreWarp( true );
  metric->SetUseFixedSampledPointSet( useSparseSampling );
  if( useSparseSampling )
    {
    std::cout << "DisplacementField: testing metric with sparse point set..."
              << std::endl;
    }
  else
    {
    std::cout << "DisplacementField: testing metric with dense sampling..."
              << std::endl;
    }
  metric->Initialize();

  // Optimizer
  optimizer->SetMetric( metric );
  optimizer->SetNumberOfIterations( numberOfDisplacementIterations );
  optimizer->SetScalesEstimator( shiftScaleEstimator );
  optimizer->SetMaximumStepLength( displacementMaxStepLength );
  try
    {
    optimizer->StartOptimization();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during deformation Optimization:" << std::endl;
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
    std::cout << e.what()    << std::endl;
    std::cout << "Test FAILED." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Optimization completed with stop condition: "
            << optimizer->GetStopConditionDescription() << std::endl
            << "Scales: " << optimizer->GetScales() << std::endl
            << "Final current-step: " << optimizer->GetCurrentStepLength()
            << std::endl;

  //warp the image with the displacement field
  resample->SetTransform( compositeTransform );
  resample->SetInput( movingImageReader->GetOutput() );
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 0 );
  resample->Update();

  //write out the displacement field
  typedef itk::ImageFileWriter< DisplacementFieldType >  DisplacementWriterType;
  DisplacementWriterType::Pointer      displacementwriter =  DisplacementWriterType::New();
  std::string outfilename( argv[3] );
  std::string ext = itksys::SystemTools::GetFilenameExtension( outfilename );
  std::string name = itksys::SystemTools::GetFilenameWithoutExtension( outfilename );
  std::string defout = name + std::string("_def") + ext;
  displacementwriter->SetFileName( defout.c_str() );
  displacementwriter->SetInput( displacementTransform->GetDisplacementField() );
  displacementwriter->Update();

  //write the warped image into a file
  typedef double                                    OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
  typedef itk::CastImageFilter<
                        MovingImageType,
                        OutputImageType >           CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >   WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( argv[3] );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput() );
  writer->Update();

  std::cout << "Test PASSED." << affineTransform->GetParameters() << std::endl;
  return EXIT_SUCCESS;

}
