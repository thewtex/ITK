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

#include "itkImageFunctionImageFilter.h"
#include "itkImage.h"
#include "itkImage.h"
#include "itkIndex.h"
#include "itkGaussianBlurImageFunction.h"

#include <iostream>

namespace // anonymous
{
  typedef itk::Image< unsigned char, 2 > UnsignedCharImageType;
  typedef itk::Image< float, 2 > FloatImageType;

  static void CreateImage(UnsignedCharImageType::Pointer image);
}
//-------------------------
//
//   This file test the interface to the ImageFunctionImageFilter class
//
//-------------------------
int itkImageFunctionImageFilterTest(int, char* [] )
{
  UnsignedCharImageType::Pointer image = UnsignedCharImageType::New();
  CreateImage(image);

  typedef itk::GaussianBlurImageFunction< UnsignedCharImageType > GaussianBlurImageFunctionType;
  GaussianBlurImageFunctionType::Pointer gaussianFunction = GaussianBlurImageFunctionType::New();
  GaussianBlurImageFunctionType::ErrorArrayType setError;
  setError.Fill( 0.01 );
  gaussianFunction->SetMaximumError( setError );
  gaussianFunction->SetSigma( 1.0);
  gaussianFunction->SetMaximumKernelWidth( 3 );

  typedef itk::ImageFunctionImageFilter<UnsignedCharImageType,FloatImageType,GaussianBlurImageFunctionType> ImageFunctionImageFilterType;
  ImageFunctionImageFilterType::Pointer imageFunctionImageFilter = ImageFunctionImageFilterType::New();
  imageFunctionImageFilter->SetInput(image);
  imageFunctionImageFilter->SetFunction(gaussianFunction);
  imageFunctionImageFilter->Update();

  return EXIT_SUCCESS;
}

namespace // anonymous
{
void CreateImage(UnsignedCharImageType::Pointer image)
{
  itk::Index<2> start;
  start.Fill(0);

  itk::Size<2> size;
  size.Fill(100);

  itk::ImageRegion<2> region(start,size);

  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  itk::ImageRegionIterator<UnsignedCharImageType> imageIterator(image,region);

  while(!imageIterator.IsAtEnd())
    {
    if(imageIterator.GetIndex()[0] >= 50 && imageIterator.GetIndex()[1] >= 50 &&
       imageIterator.GetIndex()[0] <= 70 && imageIterator.GetIndex()[1] <= 70)
      {
      imageIterator.Set(255);
      }

    ++imageIterator;
    }

}
} // end namespace
