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
#ifndef __itkImageToData_hxx
#define __itkImageToData_hxx
#include "itkImageToData.h"

#include "vnl/vnl_math.h"

namespace itk
{

/**
 *
 */
template <unsigned int VDimension, class TDataHolder, typename TInputObject>
ImageToData<VDimension, TDataHolder, TInputObject>
::ImageToData()
{
}

//----------------------------------------------------------------------------
template <unsigned int VDimension, class TDataHolder, typename TInputObject>
ThreadIdType
ImageToData<VDimension, TDataHolder, TInputObject>
::SplitRequestedObject(ThreadIdType i, ThreadIdType requestedTotal,
                       InputObjectType &overallRegion,
                       InputObjectType& splitRegion) const
{
  // Get the output pointer
//  ImageType * outputPtr = this->GetOutput();
//  const typename TOutputImage::SizeType& requestedRegionSize
//    = outputPtr->GetRequestedRegion().GetSize();

  const SizeType requestedRegionSize = overallRegion.GetSize();

  int splitAxis;
  IndexType splitIndex;
  SizeType splitSize;

  // Initialize the splitRegion to the output requested region
  splitRegion = overallRegion;
  splitIndex = splitRegion.GetIndex();
  splitSize = splitRegion.GetSize();

  // split on the outermost dimension available
  splitAxis = this->ImageDimension - 1;
  while (requestedRegionSize[splitAxis] == 1)
    {
    --splitAxis;
    if (splitAxis < 0)
      { // cannot split
      itkDebugMacro("  Cannot Split");
      return 1;
      }
    }

  // determine the actual number of pieces that will be generated
  typename SizeType::SizeValueType range = requestedRegionSize[splitAxis];
  ThreadIdType valuesPerThread =
    Math::Ceil<ThreadIdType>(range/(double)requestedTotal);
  ThreadIdType maxThreadIdUsed =
    Math::Ceil<ThreadIdType>(range/(double)valuesPerThread) - 1;

  // Split the region
  if (i < maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
    }
  if (i == maxThreadIdUsed)
    {
    splitIndex[splitAxis] += i*valuesPerThread;
    // last thread needs to process the "rest" dimension being split
    splitSize[splitAxis] = splitSize[splitAxis] - i*valuesPerThread;
    }

  // set the split region ivars
  splitRegion.SetIndex( splitIndex );
  splitRegion.SetSize( splitSize );

  itkDebugMacro("  Split Piece: " << splitRegion );

  return maxThreadIdUsed + 1;
}

} // end namespace itk

#endif
