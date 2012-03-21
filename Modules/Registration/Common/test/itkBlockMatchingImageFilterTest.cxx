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

#include <ctime>
#include <iostream>
#include "itkIndex.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkLineIterator.h"
#include "itkMaskFeaturePointSelectionFilter.h"
#include "itkBlockMatchingImageFilter.h"


int itkBlockMatchingImageFilterTest( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << " itkBlockMatchingImageFilterTest inputImageFile outputImageFile [Mask File] ";
    return EXIT_FAILURE;
    }

  typedef unsigned char                  InputPixelType;
  typedef itk::RGBPixel<InputPixelType>  OutputPixelType;

  typedef itk::Image< InputPixelType,  3 >       InputImageType;
  typedef itk::Image< OutputPixelType, 3 >       OutputImageType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  typedef itk::MaskFeaturePointSelectionFilter< InputImageType >  FilterType;
  typedef FilterType::FeaturePointsType                           PointSetType;

  typedef FilterType::PointType       PointType;
  typedef FilterType::InputImageType  ImageType;

  //Set up the reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  // Set up filter
  FilterType::Pointer filter = FilterType::New();

  //
  filter->SetInput( reader->GetOutput() );

  filter->SetSelectFraction( 0.0001 );

  // setting the same (as for BM) parameters ensures selection of feature points safe distance from image boundaries
  FilterType::SizeType s;
  s.Fill( 2 );
  filter->SetBlockRadius( s );
  //s.Fill( 7 );
  //filter->SetBlockHalfWindow( s );

  std::cout << "Feature selection: " << filter << std::endl;
  try
    {
      const clock_t begin = std::clock();

      filter->Update();

      const clock_t end = std::clock();

      std::cout << "Execution time: "<< ( end - begin ) /  CLOCKS_PER_SEC << "sec" << std::endl << std::endl;
    }
  catch ( itk::ExceptionObject &err )
    {
      ( &err )->Print( std::cerr );
      return EXIT_FAILURE;
    }

  //PointSetType::PointDataContainerIterator itPointData =
  //        filter->GetOutput()->GetPointData()->Begin();
  //PointSetType::PointDataContainerIterator end =
  //        filter->GetOutput()->GetPointData()->End();

  // duplicate input image (not really necessary but simpler)
  typedef itk::ImageDuplicator< InputImageType > DuplicatorType;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage( reader->GetOutput() );
  duplicator->Update();
//  InputImageType::Pointer dupImage = duplicator->GetOutput();

  // transform the dup image
  typedef itk::ImageRegionIteratorWithIndex< InputImageType >  DuplicateIteratorType;
  DuplicateIteratorType dupIterator( reader->GetOutput(), reader->GetOutput()->GetBufferedRegion() );

  duplicator->GetOutput()->FillBuffer( 0 );
  for ( dupIterator.GoToBegin(); !dupIterator.IsAtEnd(); ++dupIterator )
    {
      // move each pixel in input image 5 pixels along first(0) dimension
      InputImageType::IndexType idx = dupIterator.GetIndex();
      idx[0] += 5;
      if ( duplicator->GetOutput()->GetBufferedRegion().IsInside( idx ) )
        {
          duplicator->GetOutput()->SetPixel( idx, dupIterator.Value() );
        }
    }

  // at this time we have feature points
  typedef itk::BlockMatchingImageFilter< InputImageType >  BMFilterType;

  BMFilterType::Pointer BMFilter = BMFilterType::New();

  // inputs (all required)
  BMFilter->SetFixedImage( duplicator->GetOutput() );
  BMFilter->SetMovingImage( reader->GetOutput() );
  BMFilter->SetFeaturePoints( filter->GetOutput() );

  // parameters (all optional)
  s.Fill( 2 );
  BMFilter->SetBlockRadius( s );
  s.Fill( 7 );
  BMFilter->SetSearchRadius( s );
  s.Fill( 1 );
  BMFilter->SetBlockStep( s );

  std::cout << "Block matching: " << BMFilter << std::endl;
  try
    {
      const clock_t begin = std::clock();

      BMFilter->Update();

      const clock_t end = std::clock();

      std::cout << "Execution time: "<< ( end - begin ) /  CLOCKS_PER_SEC << "sec" << std::endl << std::endl;
    }
  catch ( itk::ExceptionObject &err )
    {
      ( &err )->Print( std::cerr );
      return EXIT_FAILURE;
    }

  //Set up the writer
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();

  typedef itk::ImageRegionConstIterator< InputImageType >         InputIteratorType;
  InputIteratorType inputIterator( reader->GetOutput(), reader->GetOutput()->GetBufferedRegion() );
  typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;

  OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->CopyInformation( reader->GetOutput() );
  outputImage->SetBufferedRegion( reader->GetOutput()->GetBufferedRegion() );
  outputImage->SetRequestedRegion( reader->GetOutput()->GetRequestedRegion() );
  outputImage->Allocate();

  OutputIteratorType outputIterator( outputImage, outputImage->GetBufferedRegion() );
  inputIterator.GoToBegin();
  outputIterator.GoToBegin();

  // Copy input image to output image
  while (!outputIterator.IsAtEnd())
    {
    OutputPixelType rgbPixel;
    rgbPixel.SetRed( inputIterator.Get() );
    rgbPixel.SetGreen( inputIterator.Get());
    rgbPixel.SetBlue( inputIterator.Get() );
    outputIterator.Set( rgbPixel );
    ++outputIterator;
    ++inputIterator;
    }

  //Highlight the feature points identified in the output image
  typedef PointSetType::PointsContainer::ConstIterator                        PointIteratorType;
  typedef BMFilterType::DisplacementsType::PointDataContainer::ConstIterator  PointDataIteratorType;

  PointIteratorType pointItr =
          filter->GetOutput()->GetPoints()->Begin();
  PointIteratorType pointEnd =
          filter->GetOutput()->GetPoints()->End();
  PointDataIteratorType displItr =
          BMFilter->GetOutput()->GetPointData()->Begin();

  // define colors
  OutputPixelType red;
  red.SetRed( 255.0 );
  red.SetGreen( 0.0 );
  red.SetBlue( 0.0 );

  OutputPixelType green;
  green.SetRed( 0.0 );
  green.SetGreen( 255.0 );
  green.SetBlue( 0.0 );

  OutputPixelType blue;
  blue.SetRed( 0.0 );
  blue.SetGreen( 0.0 );
  blue.SetBlue( 255.0 );

  OutputImageType::IndexType index;
  while ( pointItr != pointEnd )
    {
    if ( outputImage->TransformPhysicalPointToIndex(pointItr.Value(), index) )
      {
        OutputImageType::IndexType displ;
        outputImage->TransformPhysicalPointToIndex( pointItr.Value() + displItr.Value(), displ );

        // draw line between old and new location of a point in blue
        itk::LineIterator< OutputImageType > lineIter( outputImage, index, displ );
        for ( lineIter.GoToBegin(); !lineIter.IsAtEnd(); ++lineIter )
          {
            lineIter.Set( blue );
          }

        // mark old location of a point in green
        outputImage->SetPixel(index, green);

        // mark new location of a point in red
        outputImage->SetPixel(displ, red);
      }
    pointItr++;
    displItr++;
    }

  writer->SetFileName( argv[2] );
  writer->SetInput( outputImage );
  writer->Update();


  return EXIT_SUCCESS;
}
