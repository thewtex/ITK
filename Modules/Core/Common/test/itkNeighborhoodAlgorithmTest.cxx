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


#include "itkNeighborhoodAlgorithm.h"

template<class TImage>
bool ImageBoundaryFaceCalculatorTest(TImage * image, typename TImage::RegionType & region, const typename TImage::SizeType & radius)
{
  typedef itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TImage> FaceCalculatorType;
  typedef typename FaceCalculatorType::FaceListType FaceListType;
  FaceCalculatorType faceCalculator;
  FaceListType faceList;

  faceList = faceCalculator(image, region, radius);
  for(typename FaceListType::iterator fit = faceList.begin(); fit != faceList.end(); ++fit)
    {
    std::cout<<"Number of pixels : "<<fit->GetNumberOfPixels()<<std::endl;
    std::cout<<*fit<<std::endl;
    }

  if( !faceList.size() )
    return true;

  image->FillBuffer(0);
  for(typename FaceListType::iterator fit = faceList.begin(); fit != faceList.end(); ++fit)
    {
    itk::ImageRegionIterator<TImage> it(image, *fit);
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
      it.Value()++;
      }
    }

  //to test the case that region is outside of bufferedRegion
  region.Crop(image->GetBufferedRegion());

  itk::ImageRegionIterator<TImage> iter1(image, region);
  for(iter1.GoToBegin(); !iter1.IsAtEnd(); ++iter1)
    {
    if(iter1.Get() != 1)
      {
        std::cerr<<"pixel at Duplication or empty region found, pixel = "<<iter1.Get()<<std::endl;
      return false;
      }
    }
    return true;
}

template<class TPixel, unsigned int VDimension>
bool NeighborhoodAlgorithmTest()
{
  typedef itk::Image<TPixel, VDimension>      ImageType;
  typedef typename ImageType::RegionType      RegionType;
  typedef typename ImageType::IndexType       IndexType;
  typedef typename ImageType::SizeType        SizeType;

  IndexType ind;
  ind.Fill(0);

  SizeType size;
  size.Fill(5);

  SizeType radius;
  radius.Fill(1);

  RegionType region(ind, size);

  typename ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();

  //test 1: requestToProcessRegion match the bufferedRegion
  if ( !ImageBoundaryFaceCalculatorTest( image.GetPointer(), region, radius ))
    return false;

  ind.Fill(1);
  size.Fill(4);
  region.SetIndex(ind);
  region.SetSize(size);

  //test 2: requestToProcessRegion is part of bufferedRegion
  if ( !ImageBoundaryFaceCalculatorTest( image.GetPointer(), region, radius ))
    return false;

  ind.Fill(0);
  region.SetIndex(ind);

  size.Fill(5);
  if (VDimension > 1)
  {
    size[VDimension-1] = 1;
  }
  region.SetSize(size);
  image->SetRegions(region);

  //test 3: requestToProcessRegion match the bufferedRegion, but all the bufferedRegion is inside the boundary
  if ( !ImageBoundaryFaceCalculatorTest( image.GetPointer(), region, radius ))
    return false;

  size.Fill(5);
  image->SetRegions(region);

  ind.Fill(1);
  size.Fill(6);
  region.SetIndex(ind);
  region.SetSize(size);

  //test 4: bufferedRegion is part of the requestToProcessRegion
  if ( !ImageBoundaryFaceCalculatorTest( image.GetPointer(), region, radius ))
    return false;

  return true;
}

int itkNeighborhoodAlgorithmTest(int, char * [] )
{
  if( !NeighborhoodAlgorithmTest<int, 1>( ) )
      return EXIT_FAILURE;

  if( !NeighborhoodAlgorithmTest<int, 2>( ) )
      return EXIT_FAILURE;

  if( !NeighborhoodAlgorithmTest<int, 3>( ) )
      return EXIT_FAILURE;

  if( !NeighborhoodAlgorithmTest<int, 4>( ) )
      return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
