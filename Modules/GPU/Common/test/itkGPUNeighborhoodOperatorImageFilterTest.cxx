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
 * Test program for itkGPUMeanImageFilter class
 *
 * This program creates a GPU Mean filter test pipelining.
 */

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

/**
 * Testing GPU Neighborhood Operator Image Filter
 */

#define ImageDimension 3 //2

int itkGPUNeighborhoodOperatorImageFilterTest(int argc, char *argv[])
{
  if(!IsGPUAvailable())
  {
    std::cerr << "OpenCL-enabled GPU is not present." << std::endl;
    return EXIT_FAILURE;
  }

  // register object factory for GPU image and filter
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUImageFactory::New() );
  //itk::ObjectFactoryBase::RegisterFactory( itk::GPUMeanImageFilterFactory::New() );

  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;


  //typedef itk::Image< InputPixelType,  3 >   InputImageType;
  //typedef itk::Image< OutputPixelType, 3 >   OutputImageType;

  typedef itk::GPUImage< InputPixelType,  ImageDimension >   InputImageType;
  typedef itk::GPUImage< OutputPixelType, ImageDimension >   OutputImageType;

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
  //
  // Note: We use regular itk filter type here but factory will automatically create
  //       GPU filter for Median filter and CPU filter for threshold filter.
  //
  typedef itk::NeighborhoodOperatorImageFilter< InputImageType, OutputImageType > NeighborhoodFilterType;
  typedef itk::GPUNeighborhoodOperatorImageFilter< InputImageType, OutputImageType > GPUNeighborhoodFilterType;

  if( ImageDimension == 3 )
  {
    reader->SetFileName( "E:/proj/CUDA_ITK/ITK_CUDA_TEST/data/AD63368_tensor_double_up.nhdr" );
  }
  else
  {
      exit(0);
  }

  // Create a series of  1D Gaussian operators
  typedef itk::GaussianOperator< float, ImageDimension > OperatorType;

  std::vector< OperatorType > oper;
  oper.resize( ImageDimension );

  // Set up the operators
  unsigned int i;
  for ( i = 0; i < ImageDimension; ++i )
  {
    // we reverse the direction to minimize computation while, because
    // the largest dimension will be split slice wise for streaming
    unsigned int reverse_i = ImageDimension - i - 1;

    // Set up the operator for this dimension
    oper[reverse_i].SetDirection(i);
    oper[reverse_i].SetVariance( 3.0 );
    oper[reverse_i].SetMaximumKernelWidth( 20 );
    oper[reverse_i].SetMaximumError( 0.1 );
    oper[reverse_i].CreateDirectional();
  }

  // test 1~8 threads for CPU
  for(int nThreads = 1; nThreads <= 8; nThreads++)
  {
    NeighborhoodFilterType::Pointer CPUFilter = NeighborhoodFilterType::New();

    itk::TimeProbe cputimer;
    cputimer.Start();

    CPUFilter->SetNumberOfThreads( nThreads );

    CPUFilter->SetInput( reader->GetOutput() );
    CPUFilter->SetOperator( oper[0] );
    CPUFilter->Update();

    cputimer.Stop();

    std::cout << "CPU NeighborhoodFilter took " << cputimer.GetMeanTime() << " seconds with "
              << CPUFilter->GetNumberOfThreads() << " threads.\n" << std::endl;

    // -------

    if( nThreads == 8 )
    {
      GPUNeighborhoodFilterType::Pointer GPUFilter = GPUNeighborhoodFilterType::New();

      itk::TimeProbe gputimer;
      gputimer.Start();

      GPUFilter->SetInput( reader->GetOutput() );
      GPUFilter->SetOperator( oper[0] );
      GPUFilter->Update();

      GPUFilter->GetOutput()->MakeUpToDate(); // synchronization point (GPU->CPU memcpy)

      gputimer.Stop();
      std::cout << "GPU NeighborhoodFilter took " << gputimer.GetMeanTime() << " seconds.\n" << std::endl;

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
