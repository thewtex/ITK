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

#include <iostream>
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkLabelMap.h"
#include "itkImageFileReader.h"
#include "itkDestructiveLabelImageToLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

int itkDestructiveLabelImageToLabelMapFilterTest(int argc, char * argv[])
{

  if( argc != 3 )
    {
    std::cerr << "usage: " << argv[0] << "input output" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 2;

  typedef itk::RGBPixel< unsigned char> PType;
//  typedef unsigned char PType;

  typedef itk::Image< PType, dim > VIType;

  typedef itk::ImageFileReader< VIType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Caught expected error." << std::endl;
    std::cout << e << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::LabelObject< unsigned long, 2 >  LabelObjectType;
  typedef itk::LabelMap< LabelObjectType >      LabelMapType;

  typedef itk::DestructiveLabelImageToLabelMapFilter< VIType, LabelMapType > FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef FilterType::OutputImageType LabelMapType;

  typedef itk::Image<unsigned char, dim> IType;

  typedef itk::LabelMapToLabelImageFilter< LabelMapType, IType > LM2IType;
  LM2IType::Pointer lm2i = LM2IType::New();
  lm2i->SetInput( filter->GetOutput() );

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( lm2i->GetOutput() );
  writer->SetFileName( argv[2] );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "Caught expected error." << std::endl;
    std::cout << e << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
