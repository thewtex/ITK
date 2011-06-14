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
 * Test program for itkGPUImageToImageFilter class
 *
 * This program creates a GPU Mean filter and a CPU threshold filter using
 * object factory framework and test pipelining of GPU and CPU filters.
 */
//#include "pathToOpenCLSourceCode.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUMeanImageFilter.h"

#include "itkRescaleIntensityImageFilter.h"

#include "itkTimeProbe.h"

// test
#include "itkGPUBinaryThresholdImageFilter.h"
#include "itkGPUGradientAnisotropicDiffusionImageFilter.h"

//#define BINARYTHRESHOLDTEST
#define ANISODIFFUSIONTEST

#ifdef BINARYTHRESHOLDTEST
/**
 * Testing GPU BInary Threshold Filter
 */
int itkGPUImageFilterTestTemp(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUMeanImageFilterFactory::New() );

  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;

  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  /*
  if( argc <  3 )
    {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "inputfile outputfile " << std::endl;
    return EXIT_FAILURE;
    }

  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );
*/

  reader->SetFileName( "C:/Users/wkjeong/Proj/ITK/Examples/Data/BrainProtonDensitySlice.png" );
  writer->SetFileName( "output-GPUImageFilterTest.png" );

  //
  // Note: We use regular itk filter type here but factory will automatically create
  //       GPU filter for Median filter and CPU filter for threshold filter.
  //
  typedef itk::MeanImageFilter< InputImageType, OutputImageType > MeanFilterType;
  typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType > ThresholdFilterType;
  typedef itk::GPUBinaryThresholdImageFilter< InputImageType, OutputImageType > GPUThresholdFilterType;

  MeanFilterType::Pointer filter1 = MeanFilterType::New();
  MeanFilterType::Pointer filter2 = MeanFilterType::New();
  ThresholdFilterType::Pointer filter3 = ThresholdFilterType::New();
  GPUThresholdFilterType::Pointer filter4 = GPUThresholdFilterType::New();

  // Mean filter kernel radius
  InputImageType::SizeType indexRadius;
  indexRadius[0] = 2; // radius along x
  indexRadius[1] = 2; // radius along y

  // threshold parameters
  const InputPixelType upperThreshold = 255;
  const InputPixelType lowerThreshold = 175;
  const OutputPixelType outsideValue = 0;
  const OutputPixelType insideValue  = 255;

  filter1->SetRadius( indexRadius );
  filter2->SetRadius( indexRadius );

  filter3->SetOutsideValue( outsideValue );
  filter3->SetInsideValue(  insideValue  );
  filter3->SetUpperThreshold( upperThreshold );
  filter3->SetLowerThreshold( lowerThreshold );

  filter4->SetOutsideValue( outsideValue );
  filter4->SetInsideValue(  insideValue  );
  filter4->SetUpperThreshold( upperThreshold );
  filter4->SetLowerThreshold( lowerThreshold );


  /*
  // build pipeline
  filter1->SetInput( reader->GetOutput() ); // copy CPU->GPU implicilty
  filter2->SetInput( filter1->GetOutput() );
  filter3->SetInput( filter2->GetOutput() );
  writer->SetInput( filter3->GetOutput() ); // copy GPU->CPU implicilty
  */

  /*
  // CPU test
  reader->Update();
  filter3->SetInPlace( true );
  filter3->SetInput( reader->GetOutput() );
  filter3->Update();
  writer->SetInput( filter3->GetOutput() ); // copy GPU->CPU implicilty
*/

  // GPU test
  reader->Update();
  filter4->SetInPlace( true );// false/*true*/ );
  filter4->SetInput( reader->GetOutput() );
  filter4->Update();
  writer->SetInput( filter4->GetOutput() ); // copy GPU->CPU implicilty

  // execute pipeline filter and write output
  writer->Update();

  return EXIT_SUCCESS;
}
#endif


#ifdef ANISODIFFUSIONTEST

#define DIM_2 // DIM_3 //

#ifdef DIM_3
int itkGPUImageFilterTestTemp(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUMeanImageFilterFactory::New() );

  typedef float InputPixelType;
  typedef float OutputPixelType;

  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::Image< OutputPixelType, 3 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( "E:/proj/CUDA_ITK/ITK_CUDA_TEST/data/AD63368_tensor_double_up.nhdr");

  //
  // Note: We use regular itk filter type here but factory will automatically create
  //       GPU filter for Median filter and CPU filter for threshold filter.
  //
  typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > AnisoDiffFilterType;
  //typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > AnisoDiffFilterType;

  AnisoDiffFilterType::Pointer anisoFilter = AnisoDiffFilterType::New();

  // GPU test
  reader->Update();

  itk::TimeProbe timer;
  timer.Start();

  //anisoFilter->SetInPlace( true );
  anisoFilter->SetInput( reader->GetOutput() );
  anisoFilter->SetNumberOfIterations( 10 );
  anisoFilter->SetTimeStep( 0.0625 );
  anisoFilter->SetConductanceParameter( 3.0 );
  anisoFilter->UseImageSpacingOn();
  //anisoFilter->SetFixedAverageGradientMagnitude( 10.0 );
  anisoFilter->Update();

  timer.Stop();
  std::cerr << "Anisotropic diffusion took " << timer.GetMeanTime() << " seconds.\n";

  //
  //  The output of the filter is rescaled here and then sent to a writer.
  //
  typedef unsigned char WritePixelType;
  typedef itk::Image< WritePixelType, 3 > WriteImageType;
  typedef itk::RescaleIntensityImageFilter< OutputImageType, WriteImageType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );

  typedef itk::ImageFileWriter< WriteImageType >  WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "input-testvolume.nrrd" );

  rescaler->SetInput( anisoFilter->GetOutput() );// anisoFilter->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}

#endif

#ifdef DIM_2
/**
 * Testing GPU BInary Threshold Filter
 */
int itkGPUImageFilterTestTemp(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GPUGradientAnisotropicDiffusionImageFilterFactory::New() );

  typedef float InputPixelType;
  typedef float OutputPixelType;

  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( "C:/Users/wkjeong/Proj/ITK/Examples/Data/BrainProtonDensitySlice.png" );

  //
  // Note: We use regular itk filter type here but factory will automatically create
  //       GPU filter for Median filter and CPU filter for threshold filter.
  //
  //typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > AnisoDiffFilterType;
  typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > AnisoDiffFilterType;

  AnisoDiffFilterType::Pointer anisoFilter = AnisoDiffFilterType::New();

  // GPU test
  reader->Update();

  itk::TimeProbe timer;
  timer.Start();

  //anisoFilter->SetInPlace( true );// false/*true*/ );
  anisoFilter->SetInput( reader->GetOutput() );
  anisoFilter->SetNumberOfIterations( 5 );
  anisoFilter->SetTimeStep( 0.125 );
  anisoFilter->SetConductanceParameter( 3.0 );
  anisoFilter->UseImageSpacingOn();
  //anisoFilter->SetFixedAverageGradientMagnitude( 10.0 );
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
  writer->SetFileName( "output-GPUImageFilterTest.png" );

  rescaler->SetInput( anisoFilter->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}
#endif

#endif
