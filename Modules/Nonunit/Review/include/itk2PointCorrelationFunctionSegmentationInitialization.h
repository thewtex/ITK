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
#ifndef __itk2PointCorrelationFunctionSegmentationInitialization_h
#define __itk2PointCorrelationFunctionSegmentationInitialization_h

#include "itkVectorImage.h"
#include "itkVectorContainer.h"
#include "itkImageToImageFilter.h"
#include "itkMembershipFunctionBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include <vector>

namespace itk
{
/**\class TPointCorrelationFunctionSegmentationInitialization
 *
 * \brief This filter is intended to be used as a Feature Extraction
 * for the 2pcf tissue segmentation algorithm. The goal of this filter
 * is to generate a vector image which contain the feature vector of
 * the input image.
 *
 * Parameters
 * Number Of Classes: The number of texture of the input label image
 * rays:
 * fwin: The window size of the 2pcf feature extraction.
 *
 * Input image and Output image
 * This filter will take an lable image as input and generate Vectorimage
 * each component represent a featrue vector of that pixel.
 *
 * Template parameters
 * This filter is templated over the input image type and the data type used
 * to represent the probabilities (defaults to double).
 *
 * For more information user can check
 * Two-Point Correlation as a Feature for Histology Images: Feature Space Structure and Correlation Updating
 * Lee Cooper* (Emory University), Joel Saltz (Emory University), Raghu Machiraju (Ohio State University), Kun Huang (Ohio State University)
*/

template< class TInputImage, class TProbabilityPrecisionType=double  >
class ITK_EXPORT TPointCorrelationFunctionSegmentationInitialization :
  public
ImageToImageFilter<TInputImage,VectorImage< TProbabilityPrecisionType,
                      ::itk::GetImageDimension< TInputImage >::ImageDimension > >
{
public:
  /** Standard class typedefs. */
  typedef TPointCorrelationFunctionSegmentationInitialization Self;
  typedef TInputImage                                          InputImageType;
  typedef TProbabilityPrecisionType                    probabilityPrecisionType;

  itkStaticConstMacro( Dimension, unsigned int,
                      ::itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef VectorImage< probabilityPrecisionType,
         itkGetStaticConstMacro(Dimension) >                   OutputImageType;
  typedef ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TPointCorrelationFunctionSegmentationInitialization, ImageToImageFilter);

  /** Input image iterators */
  typedef ImageRegionConstIterator< InputImageType > InputImageIteratorType;

  /** Pixel types. */
  typedef typename InputImageType::PixelType  InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;

        typedef VectorImage<probabilityPrecisionType,
                itkGetStaticConstMacro(Dimension) >             MembershipImageType;
        typedef typename MembershipImageType::PixelType         MembershipPixelType;
        typedef typename MembershipImageType::Pointer           MembershipImagePointer;
        typedef ImageRegionIterator< MembershipImageType >      MembershipImageIteratorType;

  /** Type of the Measurement */
  typedef Vector< InputPixelType, 1 >                     MeasurementVectorType;

   /** Set/Get methods for the number of classes. The user must supply this. */
  itkSetMacro( NumberOfClasses, unsigned int );
  itkGetConstMacro( NumberOfClasses, unsigned int );

  /** Set/Get methods for the rays. The user must supply this. */
  itkSetMacro(rays, unsigned int);
  itkGetConstMacro( rays, unsigned int);

  /** Set/Get methods for the fwin. The user must supply this. */
  itkSetMacro(fwin, unsigned int);
  itkGetConstMacro(fwin, unsigned int);

  virtual void GenerateOutputInformation();
protected:

  TPointCorrelationFunctionSegmentationInitialization();
  virtual ~TPointCorrelationFunctionSegmentationInitialization(){}
  virtual void GenerateData();

private:
  TPointCorrelationFunctionSegmentationInitialization(const Self&);
  void operator=(const Self&);

  /**Check if the input number of class match with the input label image */
  void GetNumOfClass();
  /**Generate Mask image for feature extraction*/
  void GenerateMask();
  void GetSampleCoordinates();
  /*Calculate the feature*/
  double Get_feature(int, int, int, int);

  /**Samples coordinates*/
  std::vector<int> *x;
  std::vector<int> *y;

  unsigned int m_rays;
  unsigned int m_fwin;

  std::vector<InputPixelType> lable_type;

  unsigned int TempDemension;
  unsigned int inputImage_x;
  unsigned int inputImage_y;
  unsigned int m_NumberOfClasses;
  typedef itk::Image<unsigned int, 3> MaskImageType;
  MaskImageType::Pointer MaskImage;

};


}// end namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itk2PointCorrelationFunctionSegmentationInitialization.hxx"
#endif


#endif
