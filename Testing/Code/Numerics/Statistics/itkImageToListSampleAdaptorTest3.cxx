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
// The example tests the class itk::Statistics::ImageToListSampleAdaptor.

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToListSampleAdaptor.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkVectorImage.h"

int itkImageToListSampleAdaptorTest3(int, char* [] )
{
  const unsigned int MeasurementVectorSize = 8;
  typedef unsigned long ComponentPixelType;

  const unsigned int ImageDimension = 3;
  typedef itk::VectorImage< ComponentPixelType, ImageDimension > ImageType;

  typedef ImageType::PixelType    PixelType;

  ImageType::Pointer image = ImageType::New();

  ImageType::IndexType start;
  ImageType::SizeType  size;

  start.Fill( 0 );
  size.Fill( 10 );

  ImageType::RegionType region( start, size );
  image->SetRegions( region );
  image->SetNumberOfComponentsPerPixel( MeasurementVectorSize );
  image->Allocate();

  typedef itk::ImageRegionIteratorWithIndex< ImageType > IteratorType;
  IteratorType it( image, region );
  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    PixelType value;
    for( unsigned int i=0; i< MeasurementVectorSize; i++ )
      {
      value[i] = i + it.GetIndex()[0];
      }
    it.Set( value );
    ++it;
    }

  //define an adaptor type
  typedef itk::Statistics::ImageToListSampleAdaptor<
    ImageType > ImageToListSampleAdaptorType;
  ImageToListSampleAdaptorType::Pointer adaptor
                              = ImageToListSampleAdaptorType::New();

  adaptor->SetImage( image );

  ImageType::IndexType index;
  ImageType::PixelType pixel;

  ImageToListSampleAdaptorType::InstanceIdentifier id;

  for ( unsigned int i=0; i < size[2]; i++ )
    for ( unsigned int j=0; j < size[1]; j++ )
      for ( unsigned int k=0; k < size[0]; k++ )
      {
      index[0]=k;
      index[1]=j;
      index[2]=i;

      pixel = image->GetPixel( index );
      id = image->ComputeOffset( index );
      for ( unsigned int m=0; m < adaptor->GetMeasurementVectorSize(); m++ )
        {
        if ( adaptor->GetMeasurementVector(id)[m] != pixel[m] )
          {
          std::cerr << "Error in pixel value accessed using the adaptor" << std::endl;
          return EXIT_FAILURE;
          }
        }
      }

  //
  // Exercise the iterators
  //
  typedef ImageToListSampleAdaptorType::ConstIterator ConstIterator;

  ConstIterator itrBegin = adaptor->Begin();
  ConstIterator itrEnd   = adaptor->End();

  ConstIterator citr = itrBegin;

  double frequencySum = 0.0;

  while( citr != itrEnd )
    {
    frequencySum += citr.GetFrequency();
    ++citr;
    }

  if( citr == itrEnd )
    {
    std::cout << "Reached the end successfully" << std::endl;
    }

  std::cout << "Frequency Sum = " << frequencySum << std::endl;

  typedef itk::VariableLengthVector< float > VariableLengthPixelType;

  typedef itk::Image< VariableLengthPixelType, ImageDimension > VariableLengthImageType;

  const unsigned int vMeasurementVectorSize = 4;

  VariableLengthImageType::Pointer vImage = VariableLengthImageType::New();

  VariableLengthImageType::IndexType vStart;
  VariableLengthImageType::SizeType  vSize;

  vStart.Fill(0);
  vSize.Fill(10);

  VariableLengthImageType::RegionType vRegion( vStart, vSize );
  vImage->SetRegions( vRegion );
  vImage->Allocate();

  typedef itk::ImageRegionIteratorWithIndex< VariableLengthImageType > VariableIteratorType;

  VariableIteratorType ivt( vImage, vRegion );

  ivt.GoToBegin();

  while( !ivt.IsAtEnd() )
    {
    VariableLengthPixelType value(vMeasurementVectorSize);

    for( unsigned int i=0; i< vMeasurementVectorSize; i++ )
      {
      value[i] = i + ivt.GetIndex()[0];
      }
    ivt.Set( value );
    ++ivt;
    }

  //define an adaptor for the image with variable length vector type
  typedef itk::Statistics::ImageToListSampleAdaptor<
    VariableLengthImageType > VariableLengthImageToListSampleAdaptorType;

  VariableLengthImageToListSampleAdaptorType::Pointer vAdaptor
                              = VariableLengthImageToListSampleAdaptorType::New();

  vAdaptor->SetImage( vImage );

  VariableLengthImageType::IndexType vIndex;
  VariableLengthImageType::PixelType vPixel;

  VariableLengthImageToListSampleAdaptorType::InstanceIdentifier vId;

  for ( unsigned int i=0; i < size[2]; i++ )
    {
    for ( unsigned int j=0; j < size[1]; j++ )
      {
      for ( unsigned int k=0; k < size[0]; k++ )
        {
        vIndex[0]=k;
        vIndex[1]=j;
        vIndex[2]=i;

        vPixel = vImage->GetPixel( vIndex );
        vId = vImage->ComputeOffset( vIndex );
        for ( unsigned int m=0; m < vAdaptor->GetMeasurementVectorSize(); m++ )
          {
          if ( vAdaptor->GetMeasurementVector(vId)[m] != vPixel[m] )
            {
            std::cerr << "Error in vPixel value accessed using the vAdaptor" << std::endl;
            return EXIT_FAILURE;
            }
          }
        }
      }
    }


  std::cerr << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
