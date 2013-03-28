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
#ifndef __itkDemonsImageToImageMetricComputationTypeTemplatev4_h
#define __itkDemonsImageToImageMetricComputationTypeTemplatev4_h

#include "itkImageToImageMetricComputationTypeTemplatev4.h"

#include "itkDemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate.h"

namespace itk
{

/** \class DemonsImageToImageMetricComputationTypeTemplatev4
 *
 *  \brief Class implementing demons metric.
 *
 *  The implementation is taken from itkDemonsRegistrationFunction.
 *
 *  The metric derivative can be calculated using image derivatives
 *  either from the fixed or moving images. The default is to use fixed-image
 *  gradients. See ObjectToObjectMetric::SetGradientSource to change
 *  this behavior.
 *
 *  An intensity threshold is used, below which image pixels are considered
 *  equal for the purpose of derivative calculation. The threshold can be
 *  changed by calling SetIntensityDifferenceThreshold.
 *
 *  \note This metric supports only moving transforms with local support and
 *  with a number of local parameters that matches the moving image dimension.
 *  In particular, it's meant to be used with itkDisplacementFieldTransform and
 *  derived classes.
 *
 *  See DemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate::ProcessPoint
 *  for algorithm implementation.
 *
 * \sa ImageToImageMetricComputationTypeTemplatev4
 * \ingroup ITKMetricsv4
 */
template <class InternalComputationValueType, class TFixedImage, class TMovingImage, class TVirtualImage = TFixedImage >
class ITK_EXPORT DemonsImageToImageMetricComputationTypeTemplatev4 :
public ImageToImageMetricComputationTypeTemplatev4<InternalComputationValueType, TFixedImage, TMovingImage, TVirtualImage>
{
public:
  /** Standard class typedefs. */
  typedef DemonsImageToImageMetricComputationTypeTemplatev4                          Self;
  typedef ImageToImageMetricComputationTypeTemplatev4
          <InternalComputationValueType, TFixedImage, TMovingImage, TVirtualImage>  Superclass;
  typedef SmartPointer<Self>                                                        Pointer;
  typedef SmartPointer<const Self>                                                  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DemonsImageToImageMetricComputationTypeTemplatev4, ImageToImageMetricComputationTypeTemplatev4);

  /** Superclass types */
  typedef typename Superclass::MeasureType             MeasureType;
  typedef typename Superclass::DerivativeType          DerivativeType;

  typedef typename Superclass::FixedImagePointType     FixedImagePointType;
  typedef typename Superclass::FixedImagePixelType     FixedImagePixelType;
  typedef typename Superclass::FixedImageGradientType  FixedImageGradientType;

  typedef typename Superclass::MovingImagePointType    MovingImagePointType;
  typedef typename Superclass::MovingImagePixelType    MovingImagePixelType;
  typedef typename Superclass::MovingImageGradientType MovingImageGradientType;

  typedef typename Superclass::MovingTransformType     MovingTransformType;
  typedef typename Superclass::JacobianType            JacobianType;
  typedef typename Superclass::VirtualImageType        VirtualImageType;
  typedef typename Superclass::VirtualIndexType        VirtualIndexType;
  typedef typename Superclass::VirtualPointType        VirtualPointType;
  typedef typename Superclass::VirtualPointSetType     VirtualSPointSetType;
  typedef typename Superclass::NumberOfParametersType  NumberOfParametersType;

  typedef typename Superclass::ImageDimensionType      ImageDimensionType;

  /* Image dimension accessors */
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType,
      TVirtualImage::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, ImageDimensionType,
      TFixedImage::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType,
      TMovingImage::ImageDimension);

  virtual void Initialize(void) throw ( itk::ExceptionObject );

  /** Accessors for the image intensity difference threshold use
   *  in derivative calculation */
  itkGetConstMacro(IntensityDifferenceThreshold, InternalComputationValueType);
  itkSetMacro(IntensityDifferenceThreshold, InternalComputationValueType);

  /** Get the denominator threshold used in derivative calculation. */
  itkGetConstMacro(DenominatorThreshold, InternalComputationValueType);

protected:
  itkGetConstMacro(Normalizer, InternalComputationValueType);

  DemonsImageToImageMetricComputationTypeTemplatev4();
  virtual ~DemonsImageToImageMetricComputationTypeTemplatev4();

  friend class DemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >;
  friend class DemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedIndexedContainerPartitioner, Superclass, Self >;
  typedef DemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >
    DemonsDenseGetValueAndDerivativeThreaderType;
  typedef DemonsImageToImageMetricv4GetValueAndDerivativeThreaderTemplate< ThreadedIndexedContainerPartitioner, Superclass, Self >
    DemonsSparseGetValueAndDerivativeThreaderType;

  void PrintSelf(std::ostream& os, Indent indent) const;

private:

  /** Threshold below which the denominator term is considered zero.
   *  Fixed programatically in constructor. */
  InternalComputationValueType   m_DenominatorThreshold;

  /** Threshold below which two intensity value are assumed to match. */
  InternalComputationValueType   m_IntensityDifferenceThreshold;

  /* Used to normalize derivative calculation. Automatically calculated */
  InternalComputationValueType   m_Normalizer;

  DemonsImageToImageMetricComputationTypeTemplatev4(const Self &); //purposely not implemented
  void operator = (const Self &); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDemonsImageToImageMetricComputationTypeTemplatev4.hxx"
#endif

#endif
