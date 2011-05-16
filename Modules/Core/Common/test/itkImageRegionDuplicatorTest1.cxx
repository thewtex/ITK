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

#include "itkImageRegionDuplicator.h"
#include "itkRandomImageSource.h"

namespace
{

template < typename TInputImageType, typename TOutputImageType >
void Test1()
{
  typedef TInputImageType InputImageType;
  typedef itk::RandomImageSource< InputImageType > RandomSourceType;
  typename RandomSourceType::Pointer rndSource = RandomSourceType::New();
  typename RandomSourceType::SizeType size;
  size.Fill( 100 );
  rndSource->SetSize( size );
  rndSource->UpdateLargestPossibleRegion();

  typename InputImageType::Pointer inImage = rndSource->GetOutput();

  typename InputImageType::RegionType r = inImage->GetLargestPossibleRegion();

  typedef TOutputImageType OutputImageType;
  typename OutputImageType::Pointer outImage = OutputImageType::New();
  outImage->SetRegions( r );
  outImage->Allocate();


  itk::ImageRegionDuplicator<InputImageType, OutputImageType>::Copy(inImage.GetPointer(), outImage, r, r );
}

void Test2()
{
  typedef itk::Image< itk::Vector<double, 3> , 3 > ImageV3Type;
}

}

int itkImageRegionDuplicatorTest1( int, char*[] )
{
  typedef itk::Image< unsigned char, 3 > ImageUInt8Type;
  typedef itk::Image< float, 3 > ImageFloatType;
  typedef itk::Image< itk::Vector<double, 3> , 3 > ImageV3Type;

  Test1<ImageUInt8Type, ImageUInt8Type>();
  Test1<ImageFloatType, ImageFloatType>();
  Test1<ImageUInt8Type, ImageFloatType>();


  return EXIT_SUCCESS;
}
