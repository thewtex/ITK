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
#include "itkAffineTransform.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkSpecialCoordinatesImage.h"
#include "itkGaussianImageSource.h"

int itkSpecialCoordinatesImageTest(int, char* [])
{
  const unsigned int                          Dim = 3;

  typedef itk::AffineTransform< double, Dim > TransformType;
  typedef float                               PixelType;

  typedef itk::SpecialCoordinatesImage< PixelType, Dim, TransformType >
    ImageType;

  // Create test image
  ImageType::Pointer testImage = ImageType::New();
  ImageType::IndexType  imageIndex;  imageIndex.Fill(0);
  ImageType::SizeType   imageSize;   imageSize.Fill(20);
  ImageType::RegionType imageRegion;
  imageRegion.SetIndex(imageIndex);
  imageRegion.SetSize(imageSize);

  testImage->SetRegions(imageRegion);
  testImage->Allocate();

  // Create and set transform
  TransformType::Pointer transform = TransformType::New();

  // Let's make it an interesting transform
  TransformType::OutputVectorType scale;
  scale[0] = 1.0; scale[1] = 2.0; scale[2] = 0.5;
  transform->Scale(scale);

  transform->Shear(0, 2, 0.1);
  transform->Shear(1, 2, 0.11);

  TransformType::OutputVectorType translation;
  translation[0] = -3.0; translation[1] = 10.0; translation[2] = 4.0;
  transform->SetTranslation(translation);

  TransformType::OutputVectorType axis;
  axis[0] = 0.47; axis[1] = 0.23; axis[2] = -0.6; axis.Normalize();
  transform->Rotate3D(axis, 1.2);
  testImage->SetIndexToPhysicalPointTransform(transform);

  ImageType::IndexType  testRegionIndex; testRegionIndex.Fill(5);
  ImageType::SizeType   testRegionSize;  testRegionSize.Fill(5);
  ImageType::RegionType testRegion;
  testRegion.SetIndex(testRegionIndex);
  testRegion.SetSize(testRegionSize);

  itk::ImageRegionConstIteratorWithIndex< ImageType > iterator( testImage, testRegion );
  while ( !iterator.IsAtEnd() )
    {
    ImageType::PointType point;
    ImageType::IndexType originalIndex = iterator.GetIndex();
    testImage->TransformIndexToPhysicalPoint(originalIndex, point);

    ImageType::IndexType recoveredIndex;
    testImage->TransformPhysicalPointToIndex(point, recoveredIndex);

    // We really do want to check equality here
    if ( originalIndex != recoveredIndex )
      {
      std::cerr << "Original index does not match the recovered index." << std::endl;
      std::cerr << "Original index: " << originalIndex << std::endl;
      std::cerr << "Transformed point: " << point << std::endl;
      std::cerr << "Recovered index: " << recoveredIndex << std::endl;
      std::cerr << "SpecialCoordinatesImage: " << testImage << std::endl;

      return EXIT_FAILURE;
      }

    itk::ContinuousIndex< double, Dim > originalContinuousIndex(originalIndex);
    originalContinuousIndex[0] += 0.2; // "random" numbers
    originalContinuousIndex[1] -= 0.13433;
    originalContinuousIndex[2] += 0.5331;
    testImage->
      TransformContinuousIndexToPhysicalPoint(originalContinuousIndex, point);

    itk::ContinuousIndex< double, Dim > recoveredContinuousIndex;
    testImage->
      TransformPhysicalPointToContinuousIndex(point, recoveredContinuousIndex);

    if ( (originalContinuousIndex - recoveredContinuousIndex).GetNorm() > 1e-5 )
      {
      std::cerr << "Original continuous index does not match the recovered continuous index." << std::endl;
      std::cerr << "Original continuous index: " << originalContinuousIndex << std::endl;
      std::cerr << "Transformed point: " << point << std::endl;
      std::cerr << "Recovered continuous index: " << recoveredContinuousIndex << std::endl;
      std::cerr << "SpecialCoordinatesImage: " << testImage << std::endl;

      return EXIT_FAILURE;
      }

    ++iterator;
    }

  return EXIT_SUCCESS;
}
