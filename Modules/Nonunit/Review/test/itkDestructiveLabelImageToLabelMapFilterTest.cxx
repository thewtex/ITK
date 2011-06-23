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

template< class TPixel, class TFunctor >
int Test( int argc, char* argv[] )
{
  if( argc != 3 )
    {
    std::cerr << "usage: " << argv[0] << "input output" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 2;

//  typedef itk::RGBPixel< unsigned char> PixelType;
//  typedef unsigned char PixelType;

  typedef TPixel PixelType;

  typedef itk::Image< PixelType, dim > InputImageType;

  typedef itk::ImageFileReader< InputImageType > ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
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

  typedef itk::DestructiveLabelImageToLabelMapFilter< InputImageType, LabelMapType, TFunctor > FilterType;

  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );

  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef typename FilterType::OutputImageType OutputLabelMapType;

  typedef itk::Image<unsigned char, dim> OutputImageType;

  typedef itk::LabelMapToLabelImageFilter< OutputLabelMapType, OutputImageType > LM2OutputImageType;
  typename LM2OutputImageType::Pointer lm2i = LM2OutputImageType::New();
  lm2i->SetInput( filter->GetOutput() );

  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
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

int itkDestructiveLabelImageToLabelMapFilterTest(int argc, char * argv[])
{
  typedef itk::RGBPixel< unsigned char> RGBPixelType;

  if( Test< RGBPixelType, std::less< RGBPixelType > >( argc, argv ) == EXIT_FAILURE )
    {
    std::cerr << "VectorBasedPixelTest< RGBPixelType, std::less< RGBPixelType > > FAILURE!!!" << std::endl;
    return EXIT_FAILURE;
    }

  if( Test< RGBPixelType, RGBPixelType::LexicographicCompare >( argc, argv ) == EXIT_FAILURE )
    {
    std::cerr << "VectorBasedPixelTest< RGBPixelType, RGBPixelType::LexicographicCompare > FAILURE!!!" << std::endl;
    return EXIT_FAILURE;
    }

  typedef unsigned char PixelType;
  if( Test< PixelType, std::less< PixelType > >( argc, argv ) == EXIT_FAILURE )
    {
    std::cerr << "VectorBasedPixelTest< unsigned char, std::less< unsigned char > > FAILURE!!!" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
