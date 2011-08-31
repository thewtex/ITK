/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:
  Language:
  Date:
  Version:

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSupervisedBayesianClassifierInitializationImageFilter_h
#define __itkSupervisedBayesianClassifierInitializationImageFilter_h
#include <vector>
#include "itkVectorImage.h"
#include "itkVectorContainer.h"
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkDensityFunction.h"

namespace itk
{

template< class TInputImage, class TProbabilityPrecisionType=float >
class ITK_EXPORT SupervisedBayesianClassifierInitializationImageFilter :
    public
ImageToImageFilter<TInputImage, VectorImage< TProbabilityPrecisionType,
                  ::itk::GetImageDimension< TInputImage >::ImageDimension > >
{
public:
  /** Standard class typedefs. */
  typedef SupervisedBayesianClassifierInitializationImageFilter  Self;
  typedef TInputImage                                  InputImageType;
  typedef TProbabilityPrecisionType                    ProbabilityPrecisionType;

  /** Dimension of the input image */
  itkStaticConstMacro( Dimension, unsigned int,
                     ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< ProbabilityPrecisionType,
          itkGetStaticConstMacro(Dimension) >                   OutputImageType;
  typedef ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SupervisedBayesianClassifierInitializationImageFilter, ImageToImageFilter);

  /** Input image iterators */
  typedef ImageRegionConstIterator< InputImageType > InputImageIteratorType;

  /** Pixel types. */
  typedef typename InputImageType::PixelType  InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;

  /** Image Type and Pixel type for the images representing the membership of a
   *  pixel to a particular class. This image has arrays as pixels, the number of
   *  elements in the array is the same as the number of classes to be used.    */
  typedef VectorImage< ProbabilityPrecisionType,
          itkGetStaticConstMacro(Dimension) >             MembershipImageType;
  typedef typename MembershipImageType::PixelType         MembershipPixelType;
  typedef typename MembershipImageType::Pointer           MembershipImagePointer;
  typedef ImageRegionIterator< MembershipImageType >      MembershipImageIteratorType;

  /** Type of the Measurement */
  typedef Vector< InputPixelType, 1 >                     MeasurementVectorType;

  /** Type of the density functions */
  typedef Statistics::MembershipFunctionBase< MeasurementVectorType > MembershipFunctionType;

  typedef typename MembershipFunctionType::Pointer      MembershipFunctionPointer;

  /** Membership function container */
  typedef VectorContainer< unsigned int,
       MembershipFunctionPointer >  MembershipFunctionContainerType;
  typedef typename MembershipFunctionContainerType::Pointer
                                    MembershipFunctionContainerPointer;

  itkGetObjectMacro(MembershipFunctionContainer, MembershipFunctionContainerType );

  /** Set/Get methods for the number of classes. The user must supply this. */
  itkSetMacro( NumberOfClasses, unsigned int );
  itkGetConstMacro( NumberOfClasses, unsigned int );

  virtual void GenerateOutputInformation();
//co
  void AddMean(double);
  void AddCov(double);
#endif

protected:
  SupervisedBayesianClassifierInitializationImageFilter();
  virtual ~SupervisedBayesianClassifierInitializationImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Initialize the membership functions.
   */
  virtual void InitializeMembershipFunctions();

  /** Here is where the prior and membership probability vector images are created.*/
  virtual void GenerateData();

private:
  SupervisedBayesianClassifierInitializationImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  //
  unsigned int              m_NumberOfClasses;
  std::vector<double> mean;
  std::vector<double> cov;
  typename MembershipFunctionContainerType::Pointer m_MembershipFunctionContainer;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSupervisedBayesianClassifierInitializationImageFilter.txx"
#endif
