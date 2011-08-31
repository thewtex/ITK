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

#ifndef __itkJaccardImageFilter_hxx
#define __itkJaccardImageFilter_hxx

#include "itkJaccardImageFilter.h"

namespace itk
{
template<class TImage>
JaccardImageFilter<TImage>::JaccardImageFilter()
{
  this->SetNumberOfRequiredInputs(2);
}

template< class TImage>
void JaccardImageFilter<TImage>:: SetInputImage1(const TImage* image)
{
  SetNthInput(0, const_cast<TImage*>(image));
}

template< class TImage>
void JaccardImageFilter<TImage>:: SetInputImage2(const TImage* image)
{
  SetNthInput(1, const_cast<TImage*>(image));
}

template<class TImage>
void JaccardImageFilter<TImage>::BeforeThreadedGenerateData()
{
  ThreadIdType numberOfThreads = this->GetNumberOfThreads();

  m_Count_intersection.SetSize(numberOfThreads);
  m_Count_union.SetSize(numberOfThreads);

  m_Count_intersection.Fill(NumericTraits<long>::Zero);
  m_Count_union.Fill(NumericTraits<long>::Zero);
}

template<class TImage>
void JaccardImageFilter<TImage>
::AfterThreadedGenerateData()
{
  //Calculate Jaccard distance
  long int countIntersection;
  long int countUnion;
  ThreadIdType numberOfThreads = this->GetNumberOfThreads();
  countIntersection = 0;
  countUnion = 0;

  for(ThreadIdType i = 0; i < numberOfThreads; i++)
    {
    countIntersection += m_Count_intersection[i];
    countUnion += m_Count_union[i];
    }
  if(countUnion > 0)
    {
    m_JaccardDistance = (1 - (double(countIntersection)/double(countUnion)));
    }
  else
    {
    //default Jaccard distance
    m_JaccardDistance= -1;
    }
}

template<class TImage>
void JaccardImageFilter<TImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       ThreadIdType threadId)
{
  typename TImage::ConstPointer input1 = this->GetInput(0);
  typename TImage::ConstPointer input2 = this->GetInput(1);
  typename TImage::Pointer output = this->GetOutput();

  ImageRegionIterator<TImage> outputIterator(output, outputRegionForThread);
  ImageRegionConstIterator<TImage> inputIterator1(input1, outputRegionForThread);
  ImageRegionConstIterator<TImage> inputIterator2(input2, outputRegionForThread);

  while(!outputIterator.IsAtEnd())
    {
    if((inputIterator1.Get() != 0) || (inputIterator2.Get() != 0))
      {
      outputIterator.Set(128);
      m_Count_union[threadId]++;
      }

    if((inputIterator1.Get() != 0) && (inputIterator2.Get() != 0))
      {
      outputIterator.Set(0);
      m_Count_intersection[threadId]++;
      }
    ++inputIterator1;
    ++inputIterator2;
    ++outputIterator;
    }
}

// Returns the calculated  Jaccard distance
template<class TImage>
double JaccardImageFilter<TImage>:: GetJaccardDistance() const
{
  return  m_JaccardDistance;
};
}// end namespace
#endif //__itkJaccardImageFilter_hxx
