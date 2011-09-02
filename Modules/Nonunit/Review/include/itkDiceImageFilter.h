/*
 Program: Insight Toolkit
 Module:  itkDiceImageFilter
 Language: C++
 Date: Thu Aug 25 11:36:08 EDT 2011
 Author: Arindam Bhattacharya
 The Ohio State University
 */
#ifndef __itkDiceImageFilter_h
#define __itkDiceImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkArray.h"

namespace itk
{

    //This example introduces the use of the Dice Filter.This filter expects as input
    //two binary images and then finds the Dice similarity co-efficient between them.
    //It outputs a floating value (0-1) and an (grayscale)image where the intersection of the two input image
    //black ( is grayscale value '0') the union is 'gray' (grayscale value '127').
    //The formula for the calculations is given 'as twice the
    //shared information (intersection) over the sum of cardinalities'.The Dice distance which measures dissimilarities
    //is given subtracting the value from 1.


  template< class TImage>
  class ITK_EXPORT itkDiceImageFilter:public ImageToImageFilter< TImage, TImage >
  {
  public:
    /** Standard class typedefs. */
    typedef itkDiceImageFilter             Self;
    typedef ImageToImageFilter< TImage, TImage > Superclass;
    typedef SmartPointer< Self >        Pointer;


    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(itkDiceImageFilter, ImageToImageFilter);

    void SetInputImage1(const TImage* image);
    void SetInputImage2(const TImage* image);

    const double getDiceDistance();



  protected:
    itkDiceImageFilter();
    ~itkDiceImageFilter(){}

    virtual void BeforeThreadedGenerateData();

    void AfterThreadedGenerateData ();

    /** Does the real work. */
      //virtual void ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
      //                                 int threadId);
    void ThreadedGenerateData(const typename Superclass::OutputImageRegionType& outputRegionForThread,
                              ThreadIdType threadId);
  private:
    itkDiceImageFilter(const Self &); //purposely not implemented
    void operator=(const Self &);  //purposely not implemented


      //Count of the number of non-zero pixels in image1.
    Array<long>      m_Count_image1;
      //Count of the number of non-zero pixels in image2.
    Array<long>      m_Count_image2;
      //Count of the number of intersection pixels.
    Array<long>      m_Count_intersection;
      //Count of the number of union pixels.
    Array<long>      m_Count_union;
    double DiceDistance;
  };
} //namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiceImageFilter.hxx"
#endif

#endif // __itkDiceImageFilter_h
