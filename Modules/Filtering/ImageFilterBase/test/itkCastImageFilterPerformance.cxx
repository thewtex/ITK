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
#include "itkTimeProbe.h"
#include "itkAddImageFilter.h"


template <class TImage, class TOutImage >
static void AverageTestCast( typename TImage::SizeType &size )
{

  std::cout << "--- For image size: " << size << " ---" << std::endl;

  typedef typename TImage::PixelType PixelType;
  typedef TImage                     ImageType;
  typename ImageType::RegionType     region;


  typename ImageType::IndexType index;
  index.Fill(0);
  region.SetSize(size);
  region.SetIndex(index);

  typename ImageType::Pointer inImage1 = ImageType::New();
  inImage1->SetRegions( region );
  inImage1->Allocate();
  inImage1->FillBuffer(0);

  typename ImageType::Pointer inImage2 = ImageType::New();
  inImage2->SetRegions( region );
  inImage2->Allocate();
  inImage2->FillBuffer(1);


  typedef itk::AddImageFilter<TImage, TImage, TOutImage> CastImageFilter;
  typename CastImageFilter::Pointer f = CastImageFilter::New();
  f->SetInput1(inImage1);
  f->SetInput2(inImage2);
  f->SetNumberOfThreads(1);

  itk::TimeProbe t;



  for ( unsigned int j = 0; j < 10; ++j )
    {
    t.Start();
    for ( unsigned int i = 0; i < 10; ++i )
      {
      inImage1->Modified();
      f->Update();
      }
    t.Stop();
    }

  std::cout << "\t+Add Average Time: " << t.GetMean()/10 << t.GetUnit() << std::endl;

  return;
}

int itkCastImageFilterPerformance( int, char *[] )
{
  typedef itk::Image<float, 3 > ImageType3D;
  typedef itk::Image<float, 3 > ImageTypeF3D;
  ImageType3D::SizeType size3d;


  size3d.Fill( 64 );
  AverageTestCast<ImageType3D,ImageTypeF3D>( size3d );

  size3d.Fill( 128 );
  AverageTestCast<ImageType3D,ImageTypeF3D>( size3d );

  size3d.Fill( 256 );
  AverageTestCast<ImageType3D,ImageTypeF3D>( size3d );


  typedef itk::Image<char, 2 > ImageType2D;
  typedef itk::Image<float, 2 > ImageTypeF2D;
  ImageType2D::SizeType size2d;


  size2d.Fill( 32 );
  AverageTestCast<ImageType2D,ImageTypeF2D>( size2d );

  size2d.Fill( 64 );
  AverageTestCast<ImageType2D,ImageTypeF2D>( size2d );


  size2d.Fill( 128 );
  AverageTestCast<ImageType2D,ImageTypeF2D>( size2d );

  size2d.Fill( 256 );
  AverageTestCast<ImageType2D,ImageTypeF2D>( size2d );



  return EXIT_SUCCESS;
}
