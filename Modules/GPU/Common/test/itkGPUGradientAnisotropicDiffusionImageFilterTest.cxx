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
 * Test program for GPUGradientAnisotropicDiffusionImageFilter class
 */
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkTimeProbe.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUMeanImageFilter.h"
#include "itkGPUGradientAnisotropicDiffusionImageFilter.h"

int itkGPUGradientAnisotropicDiffusionImageFilterTest(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUGradientAnisotropicDiffusionImageFilterFactory::New() );

  typedef float InputPixelType;
  typedef float OutputPixelType;

  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  if( argc <  3 )
  {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "inputfile outputfile " << std::endl;
    return EXIT_FAILURE;
  }

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );//"C:/Users/wkjeong/Proj/ITK/Examples/Data/BrainProtonDensitySlice.png" );

  //
  // Note: We use regular itk filter type here but factory will automatically create
  //       GPU filter for Median filter and CPU filter for threshold filter.
  //
  typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > AnisoDiffFilterType;

  AnisoDiffFilterType::Pointer anisoFilter = AnisoDiffFilterType::New();

  // GPU test
  reader->Update();

  itk::TimeProbe timer;
  timer.Start();

  anisoFilter->SetInput( reader->GetOutput() );
  anisoFilter->SetNumberOfIterations( 10 );
  anisoFilter->SetTimeStep( 0.125 );
  anisoFilter->SetConductanceParameter( 3.0 );
  anisoFilter->UseImageSpacingOn();
  anisoFilter->Update();

  timer.Stop();
  std::cerr << "Anisotropic diffusion took " << timer.GetMeanTime() << " seconds.\n";

  //
  //  The output of the filter is rescaled here and then sent to a writer.
  //
  typedef unsigned char WritePixelType;
  typedef itk::Image< WritePixelType, 2 > WriteImageType;
  typedef itk::RescaleIntensityImageFilter< OutputImageType, WriteImageType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );

  typedef itk::ImageFileWriter< WriteImageType >  WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );

  rescaler->SetInput( anisoFilter->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
