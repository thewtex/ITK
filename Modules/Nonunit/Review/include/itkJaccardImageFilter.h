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
/*
 Program: Insight Toolkit
 Module:  JaccardImageFilter
 Language: C++
 Date: Thu Aug 25 11:36:08 EDT 2011
 Author: Arindam Bhattacharya
 The Ohio State University
 */
#ifndef __itkJaccardImageFilter_h
#define __itkJaccardImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkArray.h"

namespace itk
{

//This filter implements the JaccardImageFilter
//The Filter  expects as input,
//two binary images and then finds the Jaccard similarity co-efficient between them.
//It outputs a floating value (0-1) and an (grayscale)image where the intersection of the two input image
//black ( is grayscale value '0') the union is 'gray' (grayscale value '127').
//The Jaccard coefficient measures similarity between the images,
//and is defined as the size of the intersection divided by the size of the union of the images.
//The distance which is a 'metric' is then given by subtracting the coefficient from 1.

template< class TImage>
class JaccardImageFilter:public ImageToImageFilter< TImage, TImage>
{
public:
  /** Standard class typedefs. */
  typedef JaccardImageFilter                   Self;
  typedef ImageToImageFilter< TImage, TImage > Superclass;
  typedef SmartPointer< Self >                 Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(JaccardImageFilter, ImageToImageFilter);

  typedef TImage                               OutputImageType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  void SetInputImage1(const TImage* image);
  void SetInputImage2(const TImage* image);

  double getJaccardDistance() const;

protected:
  JaccardImageFilter();
  ~JaccardImageFilter(){}

  virtual void BeforeThreadedGenerateData();

  void AfterThreadedGenerateData ();

  /** Does the real work. */
  void ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
                            ThreadIdType threadId);

private:
  JaccardImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented

  //Count of the number of intersections between the two images.
  Array<long>      m_Count_intersection;
  //Count of the number of Union between the two images.
  Array<long>      m_Count_union;
  //The output dis-similarity is stored here
  double JaccardDistance;
};
} //namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkJaccardImageFilter.hxx"
#endif

#endif // __itkJaccardImageFilter_h
