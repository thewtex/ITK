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
#include "itkImageRegionIterator.h"

#include "itkOclUtil.h"
#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUMeanImageFilter.h"
#include "itkGPUGradientAnisotropicDiffusionImageFilter.h"

int itkGPUGradientAnisotropicDiffusionImageFilterTest(int argc, char *argv[])
{
  // register object factory for GPU image and filter
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUGradientAnisotropicDiffusionImageFilterFactory::New() );

  typedef float InputPixelType;
  typedef float OutputPixelType;

  typedef itk::GPUImage< InputPixelType,  3 >   InputImageType;
  typedef itk::GPUImage< OutputPixelType, 3 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  if(!IsGPUAvailable())
  {
    std::cerr << "OpenCL-enabled GPU is not present." << std::endl;
    return EXIT_FAILURE;
  }

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  if( argc <  3 )
  {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "inputfile outputfile " << std::endl;
    return EXIT_FAILURE;
    //reader->SetFileName( "C:/Users/wkjeong/Proj/ITK/Modules/GPU/Common/data/input-testvolume.nrrd" );
  }
  else
  {
    reader->SetFileName( argv[1] );
    writer->SetFileName( argv[2] );
  }

  // Create CPU/GPU anistorpic diffusion filter
  typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > CPUAnisoDiffFilterType;
  typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, OutputImageType > GPUAnisoDiffFilterType;

  CPUAnisoDiffFilterType::Pointer CPUFilter = CPUAnisoDiffFilterType::New();
  GPUAnisoDiffFilterType::Pointer GPUFilter = GPUAnisoDiffFilterType::New();

  reader->Update();

  // -------

  // test 1~8 threads for CPU
  for(int nThreads = 1; nThreads <= 8; nThreads++)
  {
    itk::TimeProbe cputimer;
    cputimer.Start();

    CPUFilter->SetNumberOfThreads( nThreads );

    CPUFilter->SetInput( reader->GetOutput() );
    CPUFilter->SetNumberOfIterations( 10 );
    CPUFilter->SetTimeStep( 0.0625 );//125 );
    CPUFilter->SetConductanceParameter( 3.0 );
    CPUFilter->UseImageSpacingOn();
    CPUFilter->Update();

    cputimer.Stop();

    std::cout << "CPU Anisotropic diffusion took " << cputimer.GetMeanTime() << " seconds with "
              << CPUFilter->GetNumberOfThreads() << " threads.\n" << std::endl;

    // -------

    if( nThreads == 8 )
    {
      itk::TimeProbe gputimer;
      gputimer.Start();

      GPUFilter->SetInput( reader->GetOutput() );
      GPUFilter->SetNumberOfIterations( 10 );
      GPUFilter->SetTimeStep( 0.0625 );//125 );
      GPUFilter->SetConductanceParameter( 3.0 );
      GPUFilter->UseImageSpacingOn();
      GPUFilter->Update();

      GPUFilter->GetOutput()->UpdateBuffers(); // synchronization point

      gputimer.Stop();
      std::cout << "GPU Anisotropic diffusion took " << gputimer.GetMeanTime() << " seconds.\n" << std::endl;

      // ---------------
      // RMS Error check
      // ---------------

      double diff = 0;
      unsigned int nPix = 0;
      itk::ImageRegionIterator<OutputImageType> cit(CPUFilter->GetOutput(), CPUFilter->GetOutput()->GetLargestPossibleRegion());
      itk::ImageRegionIterator<OutputImageType> git(GPUFilter->GetOutput(), GPUFilter->GetOutput()->GetLargestPossibleRegion());

      for(cit.GoToBegin(), git.GoToBegin(); !cit.IsAtEnd(); ++cit, ++git)
      {
        double err = (double)(cit.Get()) - (double)(git.Get());
        diff += err*err;
        nPix++;
      }

      std::cout << "RMS Error : " << sqrt( diff / (double)nPix ) << std::endl;
    }

  }

  return EXIT_SUCCESS;
}


/*
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
*/
