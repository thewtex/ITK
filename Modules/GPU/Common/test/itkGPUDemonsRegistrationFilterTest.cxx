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
 * Test program for itkGPUDemonsRegistrationFilter class
 *
 * This program creates a GPU Mean filter and a CPU threshold filter using
 * object factory framework and test pipelining of GPU and CPU filters.
 */
//#include "pathToOpenCLSourceCode.h"

#include "itkGPUDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkCommand.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUDemonsRegistrationFilter.h"
#include "itkGPUMeanImageFilter.h"

namespace{
// The following class is used to support callbacks
// on the filter in the pipeline that follows later
template<typename TRegistration>
class ShowProgressObject
{
public:
  ShowProgressObject(TRegistration* o)
    {m_Process = o;}
  void ShowProgress()
    {
    std::cout << "Progress: " << m_Process->GetProgress() << "  ";
    std::cout << "Iter: " << m_Process->GetElapsedIterations() << "  ";
    std::cout << "Metric: "   << m_Process->GetMetric()   << "  ";
    std::cout << "RMSChange: " << m_Process->GetRMSChange() << "  ";
    std::cout << std::endl;
    if ( m_Process->GetElapsedIterations() == 150 )
      { m_Process->StopRegistration(); }
    }
  typename TRegistration::Pointer m_Process;
};
}

int itkGPUDemonsRegistrationFilterTest(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUMeanImageFilterFactory::New() );

  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile movingImageFile ";
    std::cerr << " outputImageFile " << std::endl;
    std::cerr << " [outputDeformationFieldFile] " << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;
  typedef unsigned short PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer fixedImageReader   = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName( argv[1] );
  movingImageReader->SetFileName( argv[2] );

  //casting pixel type from short to float
  typedef float                                      InternalPixelType;
  typedef itk::GPUImage< InternalPixelType, Dimension > InternalImageType;
  typedef itk::CastImageFilter< FixedImageType,
                                InternalImageType >  FixedImageCasterType;
  typedef itk::CastImageFilter< MovingImageType,
                                InternalImageType >  MovingImageCasterType;

  FixedImageCasterType::Pointer fixedImageCaster   = FixedImageCasterType::New();
  MovingImageCasterType::Pointer movingImageCaster = MovingImageCasterType::New();

  fixedImageCaster->SetInput( fixedImageReader->GetOutput() );
  movingImageCaster->SetInput( movingImageReader->GetOutput() );

  //maching intensity histogram
  typedef itk::HistogramMatchingImageFilter<
                                    InternalImageType,
                                    InternalImageType >   MatchingFilterType;
  MatchingFilterType::Pointer matcher = MatchingFilterType::New();


  matcher->SetInput( movingImageCaster->GetOutput() );
  matcher->SetReferenceImage( fixedImageCaster->GetOutput() );

  matcher->SetNumberOfHistogramLevels( 1024 );
  matcher->SetNumberOfMatchPoints( 7 );

  matcher->ThresholdAtMeanIntensityOn();

  //demons registration
  typedef itk::Vector< float, Dimension >             VectorPixelType;
  typedef itk::GPUImage<  VectorPixelType, Dimension >   DeformationFieldType;
  typedef itk::GPUDemonsRegistrationFilter<
                                InternalImageType,
                                InternalImageType,
                                DeformationFieldType> RegistrationFilterType;

  RegistrationFilterType::Pointer filter = RegistrationFilterType::New();

  typedef ShowProgressObject<RegistrationFilterType> ProgressType;
  ProgressType progressWatch(filter);
  itk::SimpleMemberCommand<ProgressType>::Pointer command;
  command = itk::SimpleMemberCommand<ProgressType>::New();
  command->SetCallbackFunction(&progressWatch,
                               &ProgressType::ShowProgress);
  filter->AddObserver( itk::ProgressEvent(), command);

  filter->SetFixedImage( fixedImageCaster->GetOutput() );
  filter->SetMovingImage( matcher->GetOutput() );

  filter->SetNumberOfIterations( 50 );
  filter->SetStandardDeviations( 1.0 );
  filter->Update();


  //warp the image with the deformation field
  typedef itk::WarpImageFilter<
                          MovingImageType,
                          MovingImageType,
                          DeformationFieldType  >     WarperType;
  typedef itk::LinearInterpolateImageFunction<
                                   MovingImageType,
                                   double          >  InterpolatorType;
  WarperType::Pointer warper = WarperType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  warper->SetInput( movingImageReader->GetOutput() );
  warper->SetInterpolator( interpolator );
  warper->SetOutputSpacing( fixedImage->GetSpacing() );
  warper->SetOutputOrigin( fixedImage->GetOrigin() );
  warper->SetOutputDirection( fixedImage->GetDirection() );

  warper->SetDeformationField( filter->GetOutput() );

  //write the warped image into a file
  typedef  unsigned char                           OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter<
                        MovingImageType,
                        OutputImageType > CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();

  writer->SetFileName( argv[3] );

  caster->SetInput( warper->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();

  return EXIT_SUCCESS;
}
