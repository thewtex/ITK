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
#ifndef itkBinaryMedianImageFilter_hxx
#define itkBinaryMedianImageFilter_hxx

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"
#include "itkTotalProgressReporter.h"

#include <vector>
#include <algorithm>
#include "itkMath.h"

namespace itk
{
template <typename TInputImage, typename TOutputImage>
BinaryMedianImageFilter<TInputImage, TOutputImage>::BinaryMedianImageFilter()
  : m_ForegroundValue(NumericTraits<InputPixelType>::max())
  , m_BackgroundValue(InputPixelType{})
{
  m_Radius.Fill(1);
  this->ThreaderUpdateProgressOff();
}

template <typename TInputImage, typename TOutputImage>
void
BinaryMedianImageFilter<TInputImage, TOutputImage>::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  const typename Superclass::InputImagePointer  inputPtr = const_cast<TInputImage *>(this->GetInput());
  const typename Superclass::OutputImagePointer outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius(m_Radius);

  // crop the input requested region at the input's largest possible region
  if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()))
  {
    inputPtr->SetRequestedRegion(inputRequestedRegion);
    return;
  }

  // Couldn't crop the region (requested region is outside the largest
  // possible region).  Throw an exception.

  // store what we tried to request (prior to trying to crop)
  inputPtr->SetRequestedRegion(inputRequestedRegion);

  // build an exception
  InvalidRequestedRegionError e(__FILE__, __LINE__);
  e.SetLocation(ITK_LOCATION);
  e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
  e.SetDataObject(inputPtr);
  throw e;
}

template <typename TInputImage, typename TOutputImage>
void
BinaryMedianImageFilter<TInputImage, TOutputImage>::DynamicThreadedGenerateData(
  const OutputImageRegionType & outputRegionForThread)
{
  ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;

  ConstNeighborhoodIterator<InputImageType> bit;
  ImageRegionIterator<OutputImageType>      it;

  // Allocate output
  const typename OutputImageType::Pointer     output = this->GetOutput();
  const typename InputImageType::ConstPointer input = this->GetInput();

  // Find the data-set boundary "faces"
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                              bC;
  const typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList =
    bC(input, outputRegionForThread, m_Radius);

  TotalProgressReporter progress(this, output->GetRequestedRegion().GetNumberOfPixels());

  // Process each of the boundary faces.  These are N-d regions which border
  // the edge of the buffer.
  for (const auto & face : faceList)
  {
    bit = ConstNeighborhoodIterator<InputImageType>(m_Radius, input, face);
    it = ImageRegionIterator<OutputImageType>(output, face);
    bit.OverrideBoundaryCondition(&nbc);
    bit.GoToBegin();

    const unsigned int neighborhoodSize = bit.Size();

    // All of our neighborhoods have an odd number of pixels, so there is
    // always a median index (if there where an even number of pixels
    // in the neighborhood we have to average the middle two values).
    const unsigned int medianPosition = neighborhoodSize / 2;

    while (!bit.IsAtEnd())
    {
      // count the pixels in the neighborhood
      unsigned int count = 0;
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
      {
        const InputPixelType value = bit.GetPixel(i);
        if (Math::ExactlyEquals(value, m_ForegroundValue))
        {
          ++count;
        }
      }

      if (count > medianPosition)
      {
        it.Set(static_cast<OutputPixelType>(m_ForegroundValue));
      }
      else
      {
        it.Set(static_cast<OutputPixelType>(m_BackgroundValue));
      }

      ++bit;
      ++it;
      progress.CompletedPixel();
    }
  }
}


template <typename TInputImage, typename TOutput>
void
BinaryMedianImageFilter<TInputImage, TOutput>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Radius: " << m_Radius << std::endl;
  os << indent << "Foreground value : " << m_ForegroundValue << std::endl;
  os << indent << "Background value : " << m_BackgroundValue << std::endl;
}
} // end namespace itk

#endif
