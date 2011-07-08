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


#include "itkVectorImage.h"
#include "itkResampleVectorImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateVectorImageFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileWriter.h"
#include "itkFilterWatcher.h"


int itkResampleVectorImageFilterTest( int argc, char * argv[] )
{

  if( argc < 2 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " outputImage" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 2;
  typedef   unsigned char  PixelComponentType;

  typedef itk::VectorImage< PixelComponentType,  Dimension >   ImageType;

  typedef itk::ResampleVectorImageFilter<
                            ImageType, ImageType >  FilterType;

  FilterType::Pointer filter = FilterType::New();
  FilterWatcher watcher(filter);

  typedef itk::LinearInterpolateVectorImageFunction<
                       ImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  filter->SetInterpolator( interpolator );


  typedef itk::IdentityTransform< double, Dimension >  TransformType;
  TransformType::Pointer transform = TransformType::New();

  filter->SetTransform( transform );


  ImageType::SpacingType spacing;
  spacing.Fill( 1.0 );

  ImageType::PointType origin;
  origin.Fill( 0.0 );

  ImageType::RegionType     region;
  ImageType::SizeType       size;
  ImageType::IndexType      start;

  size[0] = 128;
  size[1] = 128;

  start[0] = 0;
  start[1] = 0;

  region.SetSize( size );
  region.SetIndex( start );

  ImageType::Pointer image = ImageType::New();

  image->SetOrigin( origin );
  image->SetSpacing( spacing );
  image->SetRegions( region );
  image->SetNumberOfComponentsPerPixel(3);
  image->Allocate();

  ImageType::PixelType pixelValue;
  pixelValue.SetSize(3);

  itk::ImageRegionIteratorWithIndex< ImageType > it( image, region );

  // Fill the image with some color pattern
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    ImageType::IndexType index = it.GetIndex();
    pixelValue[0] =  index[0] * 2;
    pixelValue[1] =  index[0] + index[1];
    pixelValue[2] =  index[1] * 2;
    it.Set( pixelValue );
    ++it;
    }

{
  typedef itk::ImageFileWriter<  ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput (image);
  writer->SetFileName( "/home/ddoria/input.png");
  writer->Update();
}
  ImageType::PixelType blackValue;
  blackValue.SetSize(3);
  blackValue.Fill( 0 );

  filter->SetDefaultPixelValue( blackValue );

  // Set the spacing for the resampling
  spacing[0] *= 2.0;
  spacing[1] *= 2.0;

  filter->SetOutputSpacing( spacing );


  // keep the origin
  filter->SetOutputOrigin( origin );

  // set the size
  size[0] /= 2;
  size[1] /= 2;

  filter->SetSize( size );


  filter->SetInput( image );


  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    }

  // Write an image for regression testing
  typedef itk::ImageFileWriter<  ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput (filter->GetOutput());
  writer->SetFileName( argv[1] );
  try
    {
  writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown by writer" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
