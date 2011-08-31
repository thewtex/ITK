/*
Program: Insight Toolkit
Module:  JaccardImageFilter
Language: C++
Date: Thu Aug 25 11:36:08 EDT 2011
Author: Arindam Bhattacharya
The Ohio State University
*/
#ifndef __itkJaccardImageFilter_hxx
#define __itkJaccardImageFilter_hxx

#include "itkJaccardImageFilter.h"
using namespace std;
namespace itk
{
    template<class TImage>
        JaccardImageFilter<TImage>::JaccardImageFilter()
        {
            this->SetNumberOfRequiredInputs(2);
        }


    //Set the first image
    template< class TImage>
        void JaccardImageFilter<TImage>:: SetInputImage1(const TImage* image)
        {
            SetNthInput(0, const_cast<TImage*>(image));
        }


    //Set second image
    template< class TImage>
        void JaccardImageFilter<TImage>:: SetInputImage2(const TImage* image)
        {
            SetNthInput(1, const_cast<TImage*>(image));
        }

    //Before threaded
    template<class TImage>
        void JaccardImageFilter<TImage>::BeforeThreadedGenerateData()
        {
            int numberOfThreads = this->GetNumberOfThreads();

            // Resize the thread temporaries
            m_Count_intersection.SetSize(numberOfThreads);
            m_Count_union.SetSize(numberOfThreads);

            // Initialize the temporaries
            m_Count_intersection.Fill(NumericTraits<long>::Zero);
            m_Count_union.Fill(NumericTraits<long>::Zero);

        }
    //After Thread generate
    template<class TImage>
        void JaccardImageFilter<TImage>
        ::AfterThreadedGenerateData()
        {
            //Calculate Jaccard distance
            long int countIntersection;
            long int countUnion;
            int numberOfThreads = this->GetNumberOfThreads();
            countIntersection = 0;
            countUnion = 0;

            for(int i = 0; i < numberOfThreads; i++)
            {
                countIntersection += m_Count_intersection[i];
                countUnion += m_Count_union[i];
            }
            if(countUnion > 0)
            {
                this->JaccardDistance = (1 - (double(countIntersection)/double(countUnion)));
            }
            else
            {
                //default Jaccard distance
                this->JaccardDistance= -1;
            }
        }


    template<class TImage>
        void JaccardImageFilter<TImage>
        ::ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
                ThreadIdType threadId)
        {
            typename TImage::ConstPointer input1 = this->GetInput(0);
            typename TImage::ConstPointer input2 = this->GetInput(1);
            typename TImage::Pointer output = this->GetOutput();

            itk::ImageRegionIterator<TImage> outputIterator(output, outputRegionForThread);
            itk::ImageRegionConstIterator<TImage> inputIterator1(input1, outputRegionForThread);
            itk::ImageRegionConstIterator<TImage> inputIterator2(input2, outputRegionForThread);

            while(!outputIterator.IsAtEnd())
            {
                if((inputIterator1.Get()!=0) || (inputIterator2.Get() != 0))
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
        const double  JaccardImageFilter<TImage>:: getJaccardDistance()
        {
            return  this->JaccardDistance;
        };
}// end namespace
#endif //__itkJaccardImageFilter_hxx
