/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkDanielssonDistanceMapImageFilter_hxx
#define itkDanielssonDistanceMapImageFilter_hxx

#include <iostream>

#include "itkReflectiveImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include <algorithm> // For max.

namespace itk
{

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::DanielssonDistanceMapImageFilter()

{
  // Make the outputs (distance map, voronoi map, distance vectors).
  ProcessObject::MakeRequiredOutputs(*this, 3);
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::MakeOutput(
  DataObjectPointerArraySizeType idx) -> DataObjectPointer
{
  if (idx == 1)
  {
    return VoronoiImageType::New().GetPointer();
  }

  if (idx == 2)
  {
    return VectorImageType::New().GetPointer();
  }

  return Superclass::MakeOutput(idx);
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetDistanceMap() -> OutputImageType *
{
  return dynamic_cast<OutputImageType *>(this->ProcessObject::GetOutput(0));
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetVoronoiMap() -> VoronoiImageType *
{
  return dynamic_cast<VoronoiImageType *>(this->ProcessObject::GetOutput(1));
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetVectorDistanceMap() -> VectorImageType *
{
  return dynamic_cast<VectorImageType *>(this->ProcessObject::GetOutput(2));
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::PrepareData()
{
  itkDebugMacro("PrepareData Start");
  const VoronoiImagePointer voronoiMap = this->GetVoronoiMap();

  const InputImagePointer inputImage = dynamic_cast<const InputImageType *>(ProcessObject::GetInput(0));

  voronoiMap->SetLargestPossibleRegion(inputImage->GetLargestPossibleRegion());

  voronoiMap->SetBufferedRegion(inputImage->GetBufferedRegion());

  voronoiMap->SetRequestedRegion(inputImage->GetRequestedRegion());

  voronoiMap->Allocate();

  const OutputImagePointer distanceMap = this->GetDistanceMap();

  distanceMap->SetLargestPossibleRegion(inputImage->GetLargestPossibleRegion());

  distanceMap->SetBufferedRegion(inputImage->GetBufferedRegion());

  distanceMap->SetRequestedRegion(inputImage->GetRequestedRegion());

  distanceMap->Allocate();

  const typename OutputImageType::RegionType region = voronoiMap->GetRequestedRegion();

  // find the largest of the image dimensions
  SizeType      size = region.GetSize();
  SizeValueType maxLength = 0;

  for (unsigned int dim = 0; dim < InputImageDimension; ++dim)
  {
    maxLength = std::max(maxLength, size[dim]);
  }

  ImageRegionConstIteratorWithIndex<InputImageType> it(inputImage, region);
  ImageRegionIteratorWithIndex<VoronoiImageType>    ot(voronoiMap, region);

  itkDebugMacro("PrepareData: Copy input to output");
  if (m_InputIsBinary)
  {
    constexpr VoronoiPixelType npt = 1;
    while (!ot.IsAtEnd())
    {
      if (it.Get())
      {
        ot.Set(npt);
      }
      else
      {
        ot.Set(0);
      }
      ++it;
      ++ot;
    }
  }
  else
  {
    while (!ot.IsAtEnd())
    {
      ot.Set(static_cast<VoronoiPixelType>(it.Get()));
      ++it;
      ++ot;
    }
  }

  const VectorImagePointer distanceComponents = GetVectorDistanceMap();

  distanceComponents->SetLargestPossibleRegion(inputImage->GetLargestPossibleRegion());

  distanceComponents->SetBufferedRegion(inputImage->GetBufferedRegion());

  distanceComponents->SetRequestedRegion(inputImage->GetRequestedRegion());

  distanceComponents->Allocate();

  ImageRegionIteratorWithIndex<VectorImageType> ct(distanceComponents, region);

  OffsetType maxValue;
  OffsetType minValue;

  for (unsigned int j = 0; j < InputImageDimension; ++j)
  {
    maxValue[j] = 2 * maxLength;
    minValue[j] = 0;
  }

  itkDebugMacro("PrepareData: Copy output to ct");

  // Iterate over the input image and distanceComponents image.
  // Wherever the input image is non-zero, initialize the distanceComponents image to the minValue.
  // Wherever the input image is zero, initialize the distanceComponents image to the maxValue.
  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    if (it.Get())
    {
      ct.Set(minValue);
    }
    else
    {
      ct.Set(maxValue);
    }
    ++it;
    ++ct;
  }
  itkDebugMacro("PrepareData End");
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::ComputeVoronoiMap()
{
  itkDebugMacro("ComputeVoronoiMap Start");
  const VoronoiImagePointer voronoiMap = this->GetVoronoiMap();
  const OutputImagePointer  distanceMap = this->GetDistanceMap();
  const VectorImagePointer  distanceComponents = this->GetVectorDistanceMap();

  const typename OutputImageType::RegionType region = voronoiMap->GetRequestedRegion();

  ImageRegionIteratorWithIndex<VoronoiImageType> ot(voronoiMap, region);
  ImageRegionIteratorWithIndex<VectorImageType>  ct(distanceComponents, region);
  ImageRegionIteratorWithIndex<OutputImageType>  dt(distanceMap, region);

  itkDebugMacro("ComputeVoronoiMap Region: " << region);
  while (!ot.IsAtEnd())
  {
    const IndexType index = ct.GetIndex() + ct.Get();
    if (region.IsInside(index))
    {
      ot.Set(voronoiMap->GetPixel(index));
    }

    OffsetType distanceVector = ct.Get();
    double     distance = 0.0;
    if (m_UseImageSpacing)
    {
      for (unsigned int i = 0; i < InputImageDimension; ++i)
      {
        const double component = distanceVector[i] * static_cast<double>(m_InputSpacingCache[i]);
        distance += component * component;
      }
    }
    else
    {
      for (unsigned int i = 0; i < InputImageDimension; ++i)
      {
        distance += distanceVector[i] * distanceVector[i];
      }
    }

    if (m_SquaredDistance)
    {
      dt.Set(static_cast<OutputPixelType>(distance));
    }
    else
    {
      dt.Set(static_cast<OutputPixelType>(std::sqrt(distance)));
    }
    ++ot;
    ++ct;
    ++dt;
  }
  itkDebugMacro("ComputeVoronoiMap End");
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::UpdateLocalDistance(
  VectorImageType *  components,
  const IndexType &  here,
  const OffsetType & offset)
{
  const IndexType there = here + offset;
  OffsetType      offsetValueHere = components->GetPixel(here);
  OffsetType      offsetValueThere = components->GetPixel(there) + offset;

  double norm1 = 0.0;
  double norm2 = 0.0;
  for (unsigned int i = 0; i < InputImageDimension; ++i)
  {
    auto v1 = static_cast<double>(offsetValueHere[i]);
    auto v2 = static_cast<double>(offsetValueThere[i]);

    if (m_UseImageSpacing)
    {
      auto spacingComponent = static_cast<double>(m_InputSpacingCache[i]);
      v1 *= spacingComponent;
      v2 *= spacingComponent;
    }

    norm1 += v1 * v1;
    norm2 += v2 * v2;
  }

  if (norm1 > norm2)
  {
    components->SetPixel(here, offsetValueThere);
  }
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GenerateData()
{
  this->PrepareData();

  this->m_InputSpacingCache = this->GetInput()->GetSpacing();

  // Specify images and regions.

  const VoronoiImagePointer voronoiMap = this->GetVoronoiMap();
  const VectorImagePointer  distanceComponents = this->GetVectorDistanceMap();

  const RegionType region = voronoiMap->GetRequestedRegion();

  itkDebugMacro("Region to process: " << region);

  // Instantiate reflective iterator

  ReflectiveImageRegionConstIterator<VectorImageType> it(distanceComponents, region);

  typename VectorImageType::OffsetType voffset;
  for (unsigned int dim = 0; dim < InputImageDimension; ++dim)
  {
    if (region.GetSize()[dim] > 1)
    {
      voffset[dim] = 1;
    }
    else
    {
      voffset[dim] = 0;
    }
  }
  it.SetBeginOffset(voffset);
  it.SetEndOffset(voffset);
  it.GoToBegin();

  // Set up an iterator for the input image.
  // In this image, non-zero values are the background and zero values are the foreground.
  // The foreground values are where the distance map should be solved.
  // We iterate over this input image so that all background (non-zero) values are ignored in the
  // distance map computation.
  const InputImagePointer inputImage = dynamic_cast<const InputImageType *>(ProcessObject::GetInput(0));
  ReflectiveImageRegionConstIterator<const InputImageType> inputIt(inputImage, region);
  inputIt.SetBeginOffset(voffset);
  inputIt.SetEndOffset(voffset);
  inputIt.GoToBegin();

  // Support progress methods/callbacks.

  // Each pixel is visited 2^InputImageDimension times, and the number
  // of visits per pixel needs to be computed for progress reporting.
  const SizeValueType visitsPerPixel = (1 << InputImageDimension);
  SizeValueType       updateVisits = region.GetNumberOfPixels() * visitsPerPixel / 10;
  if (updateVisits < 1)
  {
    updateVisits = 1;
  }
  const float updatePeriod = static_cast<float>(updateVisits) * 10.0;

  // Process image.

  OffsetType offset{};

  SizeValueType i = 0;

  itkDebugMacro("GenerateData: Computing distance transform");
  while (!it.IsAtEnd())
  {
    if (!(i % updateVisits))
    {
      this->UpdateProgress(static_cast<float>(i) / updatePeriod);
    }

    // The background is the region from which we are growing.
    // The region we want to solve for is the foreground.
    // The background pixels are set to a non-zero value.
    // We can ignore these pixels in the update step.
    if (!inputIt.Get())
    {
      const IndexType here = it.GetIndex();
      for (unsigned int dim = 0; dim < InputImageDimension; ++dim)
      {
        if (region.GetSize()[dim] <= 1)
        {
          continue;
        }
        if (it.IsReflected(dim))
        {
          offset[dim]++;
          UpdateLocalDistance(distanceComponents, here, offset);
          offset[dim] = 0;
        }
        else
        {
          offset[dim]--;
          UpdateLocalDistance(distanceComponents, here, offset);
          offset[dim] = 0;
        }
      }
    }
    ++it;
    ++i;
    ++inputIt;
  }

  itkDebugMacro("GenerateData: ComputeVoronoiMap");

  this->ComputeVoronoiMap();
}

template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
DanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::PrintSelf(std::ostream & os,
                                                                                      Indent         indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Danielson Distance: " << std::endl;
  os << indent << "Input Is Binary   : " << m_InputIsBinary << std::endl;
  os << indent << "Use Image Spacing : " << m_UseImageSpacing << std::endl;
  os << indent << "Squared Distance  : " << m_SquaredDistance << std::endl;
}
} // end namespace itk

#endif
