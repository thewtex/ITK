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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUNeighborhoodOperatorImageFilter.h"

#include "itkRescaleIntensityImageFilter.h"
#include "itkTimeProbe.h"
#include "itkGaussianOperator.h"

#include "itkDiscreteGaussianImageFilter.h"
#include "itkGPUDiscreteGaussianImageFilter.h"

/**
 * Testing GPU Discrete Gaussian Image Filter
 */

#define ImageDimension 3 //2

int itkGPUDiscreteGaussianImageFilterTest(int argc, char *argv[])
{
  if(!IsGPUAvailable())
  {
    std::cerr << "OpenCL-enabled GPU is not present." << std::endl;
    return EXIT_FAILURE;
  }

  // register object factory for GPU image and filter
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUMeanImageFilterFactory::New() );

  typedef float InputPixelType;
  typedef float OutputPixelType;


  //typedef itk::Image< InputPixelType,  3 >   InputImageType;
  //typedef itk::Image< OutputPixelType, 3 >   OutputImageType;

  typedef itk::GPUImage< InputPixelType,  ImageDimension >   InputImageType;
  typedef itk::GPUImage< OutputPixelType, ImageDimension >   OutputImageType;

  typedef itk::DiscreteGaussianImageFilter< InputImageType, OutputImageType> CPUFilterType;
  typedef itk::GPUDiscreteGaussianImageFilter< InputImageType, OutputImageType> GPUFilterType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

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

  float variance = 4.0;

  // test 1~8 threads for CPU
  for(int nThreads = 1; nThreads <= 8; nThreads++)
  {
    CPUFilterType::Pointer CPUFilter = CPUFilterType::New();

    itk::TimeProbe cputimer;
    cputimer.Start();

    CPUFilter->SetNumberOfThreads( nThreads );

    CPUFilter->SetInput( reader->GetOutput() );
    CPUFilter->SetVariance( variance );
    CPUFilter->Update();

    cputimer.Stop();

    std::cout << "CPU Gaussian Filter took " << cputimer.GetMeanTime() << " seconds with "
              << CPUFilter->GetNumberOfThreads() << " threads.\n" << std::endl;

    // -------

    if( nThreads == 8 )
    {
      GPUFilterType::Pointer GPUFilter = GPUFilterType::New();

      itk::TimeProbe gputimer;
      gputimer.Start();

      GPUFilter->SetInput( reader->GetOutput() );
      GPUFilter->SetVariance( variance );
      GPUFilter->Update();

      GPUFilter->GetOutput()->UpdateBuffers(); // synchronization point (GPU->CPU memcpy)

      gputimer.Stop();
      std::cout << "GPU Gaussian Filter took " << gputimer.GetMeanTime() << " seconds.\n" << std::endl;

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
        if(err > 0.1 || (double)cit.Get() < 0.1) std::cout << "CPU : " << (double)(cit.Get()) << ", GPU : " << (double)(git.Get()) << std::endl;
        diff += err*err;
        nPix++;
      }

      std::cout << "RMS Error : " << sqrt( diff / (double)nPix ) << std::endl;
    }

  }

  return EXIT_SUCCESS;
}
