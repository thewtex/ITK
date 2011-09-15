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
#ifndef __itkHighLevelFeatureMatchingFilter_h
#define __itkHighLevelFeatureMatchingFilter_h

#include "itkVectorImage.h"
#include "itkImageToImageFilter.h"
#include "itkLabelGeometryImageFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include <vector>
const unsigned int dimension = 1;
namespace itk
{

template< class TImage, class TProbabilityPrecisionType=float >
class HighLevelFeatureMatchingFilter
:public ImageToImageFilter< TImage,
  VectorImage<TProbabilityPrecisionType, dimension> >
  //TImage >
{
public:
/** Standard class typedefs. */
  typedef VectorImage<TProbabilityPrecisionType,dimension > OutputImageType;
  typedef HighLevelFeatureMatchingFilter                    Self;
  typedef ImageToImageFilter< TImage, OutputImageType>      Superclass;
  typedef SmartPointer< Self >                              Pointer;

/** Method for creation through the object factory. */
  itkNewMacro(Self);
/** Run-time type information (and related methods). */
  itkTypeMacro(HighLevelFeatureMatchingFilter,ImageToImageFilter);

/**Pixel types.*/
  typedef typename TImage::PixelType          InputPixelType;
  typedef typename OutputImageType::PixelType OutputPixelType;


/** BinaryImageToLabelMapFilter*/
  typedef itk::BinaryImageToLabelMapFilter<TImage>
     BinaryImageToLabelMapFilterType;
  typedef typename BinaryImageToLabelMapFilterType::Pointer
     BTLFilterPointer;
  typedef typename BinaryImageToLabelMapFilterType::OutputImageType
     BTLFilterOutputType;
  //the output ImageType need to be fixed
  typedef itk::LabelMapToLabelImageFilter< BTLFilterOutputType, TImage>
     L2IType;
  typedef typename L2IType::Pointer
     L2IPointer;

  typedef  itk::LabelGeometryImageFilter<TImage>
     LabelGeometryImageFilterType;
  typename LabelGeometryImageFilterType::Pointer
     m_GeoFilter;
  typename LabelGeometryImageFilterType::Pointer
     m_GeoFilterFloat;
  typename LabelGeometryImageFilterType::LabelsType
     m_AllLabels;
  typename LabelGeometryImageFilterType::LabelsType::iterator
     m_AllLabelsIt;
  typename LabelGeometryImageFilterType::LabelPixelType
     m_LabelValue;

  typedef  VectorImage<TProbabilityPrecisionType, dimension>
      MembershipImageType;
  typedef  ImageRegionIterator<MembershipImageType>
      MembershipImageIteratorType;

  void SetInputImage1(const TImage* image);
  void SetInputImage2(const TImage* image);
protected:
HighLevelFeatureMatchingFilter();
~HighLevelFeatureMatchingFilter(){}

virtual void GenerateOutputInformation();
virtual void GenerateData();

private:
HighLevelFeatureMatchingFilter(const Self &); //purposely not implemented

void operator=(const Self &);  //purposely not implemented
//void BITLfilter();

std::vector<float>                                   *m_Feature;
float                                                 m_SizeTol;
float                                                 m_EccenTol;
float                                                 m_OrientTol;
float                                                 m_DistTol;
typename LabelGeometryImageFilterType::LabelPointType m_LabelPoint;
};

}//namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHighLevelFeatureMatchingFilter.hxx"
#endif

#endif
