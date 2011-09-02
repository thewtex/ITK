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
#ifndef __itkDiceImageFilter_hxx
#define __itkDiceImageFilter_hxx

#include "itkDiceImageFilter.h"

namespace itk
{
template<class TImage>
itkDiceImageFilter<TImage>::itkDiceImageFilter()
{
  this->SetNumberOfRequiredInputs(2);
}

template< class TImage>
void itkDiceImageFilter<TImage>:: SetInputImage1(const TImage* image)
{
  SetNthInput(0, const_cast<TImage*>(image));
}

template< class TImage>
void itkDiceImageFilter<TImage>:: SetInputImage2(const TImage* image)
{
  SetNthInput(1, const_cast<TImage*>(image));
}

template<class TImage>
void itkDiceImageFilter<TImage>::BeforeThreadedGenerateData()
{
  int numberOfThreads = this->GetNumberOfThreads();
  // Resize the thread temporaries

  m_Count_intersection.SetSize(numberOfThreads);
  m_Count_union.SetSize(numberOfThreads);
  m_Count_image1.SetSize(numberOfThreads);
  m_Count_image2.SetSize(numberOfThreads);

  // Initialize the temporaries

  m_Count_intersection.Fill(NumericTraits<long>::Zero);
  m_Count_union.Fill(NumericTraits<long>::Zero);
  m_Count_image1.Fill(NumericTraits<long>::Zero);

}

template<class TImage>
void itkDiceImageFilter<TImage>
::AfterThreadedGenerateData()
{
  //Calcualte Dice distance
  long int countIntersection;
  long int countUnion;
  int numberOfThreads = this->GetNumberOfThreads();
  countIntersection = 0;
  countUnion = 0;
  long int countImage1 = 0;
  long int countImage2 = 0;

  for(int i = 0; i < numberOfThreads; i++)
    {
    countIntersection += m_Count_intersection[i];
    countImage1 += m_Count_image1[i];
    countImage2 += m_Count_image2[i];
    }

  //Calculation for dice
  if((countImage1 + countImage2 > 0))
    {
    m_DiceDistance = 1- double(2) *(countIntersection)/(countImage1 + countImage2);
    }
  else
    {
    m_DiceDistance= -1;
    }
}


template<class TImage>
void itkDiceImageFilter<TImage>
::ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
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
    //find the number of 'on'  pixels in image1
    if(inputIterator1.Get() != 0)
      {
      m_Count_image1[threadId]++;
      }
    //find the number of 'on'  pixels in image2
    if(inputIterator2.Get() != 0)
      {
      m_Count_image2[threadId]++;
      }
    //or
    if((inputIterator1.Get() != 0) || (inputIterator2.Get() != 0))
      {
      outputIterator.Set(128);
      }
    //and
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

// Function to get back the Dice distance
template<class TImage>
double  itkDiceImageFilter<TImage>:: GetDiceDistance() const
{
  return  m_DiceDistance;
};

}// end namespace
#endif //__itkDiceImageFilter_hxx
