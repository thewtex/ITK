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

#include <iostream>
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkMaskFeaturePointSelection3D.h"

int
itkMaskFeaturePointSelection3DTest(int, char *[])
{
  try
    {
      typedef unsigned char PixelType;
      typedef itk::MaskFeaturePointSelection3D<float, PixelType, PixelType> FilterType;
      typedef FilterType::InputImageType ImageType;

      // Set up filter
      FilterType::Pointer filter = FilterType::New();

      // Generate input image and mask
      ImageType::Pointer image = ImageType::New();
      ImageType::Pointer mask = ImageType::New();

      ImageType::IndexType start;
      start[0] = 0;
      start[1] = 0;
      start[2] = 0;

      ImageType::SizeType size;
      size[0] = 10;
      size[1] = 10;
      size[2] = 10;

      ImageType::RegionType region;
      region.SetSize(size);
      region.SetIndex(start);

      image->SetRegions(region);
      image->Allocate();

      mask->SetRegions(region);
      mask->Allocate();

      typedef itk::ImageRegionIterator<ImageType> IteratorType;
      ImageType::IndexType idx;

      // Set pixels with all same indices to 100 while set the rest to 1
      IteratorType itImage(image, image->GetRequestedRegion());

      for (itImage.GoToBegin(); !itImage.IsAtEnd(); ++itImage)
        {
        idx = itImage.GetIndex();
        itImage.Set(idx[0] == idx[1] && idx[1] == idx[2] ? 100 : 1);
      }

    filter->SetInputImage(image);

    // Fill half of the mask with 0s
    IteratorType itMask(mask, mask->GetRequestedRegion());

    for (itMask.GoToBegin(); !itMask.IsAtEnd(); ++itMask)
      {
      idx = itMask.GetIndex();
      itMask.Set(idx[1] < 5);
    }

  filter->SetMaskImage(mask);

  std::cout << "filter: " << filter;

  filter->Update();

  std::cout << std::endl << "Selected points: "
      << filter->GetOutput()->GetNumberOfPoints() << std::endl;
}
catch (itk::ExceptionObject &err)
{
  (&err)->Print(std::cerr);
  return EXIT_FAILURE;
}
return EXIT_SUCCESS;
}
