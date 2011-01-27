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

//
// Test program for itkGPUImageToImageFilter class
//

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkGPUImage.h"
#include "itkGPUKernelManager.h"
#include "itkGPUContextManager.h"
#include "itkGPUImageToImageFilter.h"
#include "itkGPUMeanImageFilter.h"

using namespace itk;

//
// OpenCL source file path
//
char* gpuSrcPath = "../../../ITK/Code/GPU/ImageOps.cl";


int main()
{

  ObjectFactoryBase::RegisterFactory( GPUImageFactory::New() );
  ObjectFactoryBase::RegisterFactory( GPUMeanImageFilterFactory::New() );

  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;

  typedef itk::Image< InputPixelType,  2 >   InputImageType;
  typedef itk::Image< OutputPixelType, 2 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  //reader->SetFileName( argv[1] );
  //writer->SetFileName( argv[2] );
  reader->SetFileName( "C:/Users/wkjeong/Proj/ITK/Examples/Data/BrainProtonDensitySlice.png" );
  writer->SetFileName( "testout.png" );

  //typedef itk::GPUMeanImageFilter< InputImageType, OutputImageType > MeanFilterType;
  typedef itk::MeanImageFilter< InputImageType, OutputImageType > MeanFilterType;
  typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType > ThresholdFilterType;

  MeanFilterType::Pointer filter1 = MeanFilterType::New();
  MeanFilterType::Pointer filter2 = MeanFilterType::New();
  ThresholdFilterType::Pointer filter3 = ThresholdFilterType::New();

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

  // build pipeline
  filter1->SetInput( reader->GetOutput() );
  filter2->SetInput( filter1->GetOutput() );
  filter3->SetInput( filter2->GetOutput() );
  writer->SetInput( filter3->GetOutput() );

  // execute pipeline filter and write output
  writer->Update();


  return EXIT_SUCCESS;
}


/*
int main()
{
  typedef   unsigned char  InputPixelType;
  typedef   unsigned char  OutputPixelType;

  typedef itk::GPUImage< InputPixelType,  2 >   InputImageType;
  typedef itk::GPUImage< OutputPixelType, 2 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  //reader->SetFileName( argv[1] );
  //writer->SetFileName( argv[2] );
  reader->SetFileName( "C:/Users/wkjeong/Proj/ITK/Examples/Data/BrainProtonDensitySlice.png" );
  writer->SetFileName( "testout.png" );

  typedef itk::GPUMeanImageFilter< InputImageType, OutputImageType > MeanFilterType;
  typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType > ThresholdFilterType;

  MeanFilterType::Pointer filter1 = MeanFilterType::New();
  MeanFilterType::Pointer filter2 = MeanFilterType::New();
  ThresholdFilterType::Pointer filter3 = ThresholdFilterType::New();

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

  // build pipeline
  filter1->SetInput( reader->GetOutput() );
  filter2->SetInput( filter1->GetOutput() );
  filter3->SetInput( filter2->GetOutput() );
  writer->SetInput( filter3->GetOutput() );

  // execute pipeline filter and write output
  writer->Update();


  return EXIT_SUCCESS;
}
*/
