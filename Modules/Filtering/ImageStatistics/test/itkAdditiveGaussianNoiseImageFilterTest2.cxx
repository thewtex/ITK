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
#include "itkSimpleFilterWatcher.h"


#include "itkPeakSignalToNoiseRatioCalculator.h"
#include "itkAdditiveGaussianNoiseImageFilter.h"
#include "itkTestingMacros.h"

int  itkAdditiveGaussianNoiseImageFilterTest2(int argc, char * argv[])
{

  if( argc < 3 )
    {
    std::cerr << "usage: " << argv[0] << " intput output [std_dev [mean]]" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;

  typedef unsigned char            PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::AdditiveGaussianNoiseImageFilter< IType, IType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  TEST_SET_GET_VALUE( 1.0, filter->GetStandardDeviation() );
  if( argc >= 4 )
    {
    filter->SetStandardDeviation( atof(argv[3]) );
    TEST_SET_GET_VALUE( atof(argv[3]), filter->GetStandardDeviation() );
    }
  TEST_SET_GET_VALUE( 0.0, filter->GetMean() );
  if( argc >= 5 )
    {
    filter->SetMean( atof(argv[4]) );
    TEST_SET_GET_VALUE( atof(argv[4]), filter->GetMean() );
    }

  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  typedef itk::PeakSignalToNoiseRatioCalculator< IType > CalculatorType;
  CalculatorType::Pointer psnr = CalculatorType::New();
  psnr->SetImage( reader->GetOutput() );
  psnr->SetNoisyImage( filter->GetOutput() );
  psnr->Compute();
  std::cout << "PSNR: " << psnr->GetOutput() << " dB" << std::endl;

  return 0;
}
