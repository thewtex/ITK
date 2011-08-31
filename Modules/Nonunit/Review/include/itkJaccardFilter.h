/*
  Program: Insight Toolkit
  Module:  JaccardFilter
  Language: C++
  Date: Thu Aug 25 11:36:08 EDT 2011
  Author: Arindam Bhattacharya
  The Ohio State University
*/
#ifndef __itkJaccardFilter_h
#define __itkJaccardFilter_h

#include "itkImageToImageFilter.h"
#include "itkNeighborhoodIterator.h"
#include "itkArray.h"

namespace itk
{
//This filter expects as input
//two binary images and then finds the Jaccard similarity co-efficient between them.
//It outputs a floating value (0-1) and an (grayscale)image where the intersection of the two input image
//black ( is grayscale value '0') the union is 'gray' (grayscale value '127').
//The Jaccard coefficient measures similarity between the images,
//and is defined as the size of the intersection divided by the size of the union of the images.
//The distance which is a 'metric' is then given by subtracting the coefficient from 1.

template< class TImage>
class JaccardFilter:public ImageToImageFilter< TImage, TImage >
{
public:
  /** Standard class typedefs. */
  typedef JaccardFilter             Self;
  typedef ImageToImageFilter< TImage, TImage > Superclass;
  typedef SmartPointer< Self >        Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(JaccardFilter, ImageToImageFilter);

  //
  void SetInputImage1(const TImage* image);
  void SetInputImage2(const TImage* image);

  double getJaccardDistance();

protected:
  JaccardFilter();
  ~JaccardFilter(){}

virtual void BeforeThreadedGenerateData();

void AfterThreadedGenerateData ();

  /** Does the real work. */
  virtual void ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
  int threadId);

private:
  JaccardFilter(const Self &); //purposely not implemented
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
#include "itkJaccardFilter.txx"
#endif

#endif // __itkJaccardFilter_h
