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
#ifndef __itkDemonsImageToImageObjectMetric_h
#define __itkDemonsImageToImageObjectMetric_h

#include "itkImageToImageObjectMetric.h"

#include "itkDemonsImageToImageObjectMetricGetValueAndDerivativeThreader.h"

namespace itk
{

/** \class DemonsImageToImageObjectMetric
 *
 *  \brief Class implementing demons metric.
 *
 *  The implementation is taken from itkDemonsRegistrationFunction.
 *
 *  The metric derivative can be calculated using image derivatives
 *  either from the fixed or moving images. The default is to use fixed-image
 *  gradients. See ImageToImageObjectMetric::SetGradientSource to change
 *  this behavior.
 *
 *  An intensity threshold is used, below which image pixels are considered
 *  equal for the purpose of derivative calculation. The threshold can be
 *  changed by calling SetIntensityDifferenceThreshold.
 *
 *  This metric requires that the transform assigned to the image used as the
 *  gradient source has local-support, i.e. is a dense transform. An exception
 *  is thrown during initialization if not.
 *
 *  See
 *  DemonsImageToImageObjectMetricGetValueAndDerivativeThreader::ProcessPoint for algorithm implementation.
 *
 * \sa itkImageToImageObjectMetric
 * \ingroup ITKHighDimensionalMetrics
 */
template <class TFixedImage, class TMovingImage, class TVirtualImage = TFixedImage >
class ITK_EXPORT DemonsImageToImageObjectMetric :
public ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
{
public:

  /** Standard class typedefs. */
  typedef DemonsImageToImageObjectMetric                      Self;
  typedef ImageToImageObjectMetric<TFixedImage, TMovingImage, TVirtualImage>
                                                              Superclass;
  typedef SmartPointer<Self>                                  Pointer;
  typedef SmartPointer<const Self>                            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DemonsImageToImageObjectMetric, ImageToImageObjectMetric);

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
  typedef typename Superclass::VirtualSampledPointSetType
                                                       VirtualSampledPointSetType;
  typedef typename Superclass::NumberOfParametersType  NumberOfParametersType;

  typedef typename Superclass::InternalComputationValueType InternalComputationValueType;

  typedef typename Superclass::ImageDimensionType      ImageDimensionType;

  /* Image dimension accessors */
  itkStaticConstMacro(VirtualImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TVirtualImage>::ImageDimension);
  itkStaticConstMacro(FixedImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TFixedImage>::ImageDimension);
  itkStaticConstMacro(MovingImageDimension, ImageDimensionType,
      ::itk::GetImageDimension<TMovingImage>::ImageDimension);

  virtual void Initialize(void) throw ( itk::ExceptionObject );

  /** Accessors for the image intensity difference threshold use
   *  in derivative calculation */
  itkGetConstMacro(IntensityDifferenceThreshold, InternalComputationValueType);
  itkSetMacro(IntensityDifferenceThreshold, InternalComputationValueType);

  /** Get the denominator threshold used in derivative calculation. */
  itkGetConstMacro(DenominatorThreshold, InternalComputationValueType);

protected:
  itkGetConstMacro(Normalizer, InternalComputationValueType);

  DemonsImageToImageObjectMetric();
  virtual ~DemonsImageToImageObjectMetric();

  friend class DemonsImageToImageObjectMetricGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >;
  friend class DemonsImageToImageObjectMetricGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner, Superclass, Self >;
  typedef DemonsImageToImageObjectMetricGetValueAndDerivativeThreader< ThreadedImageRegionPartitioner< Superclass::VirtualImageDimension >, Superclass, Self >
    DemonsDenseGetValueAndDerivativeThreaderType;
  typedef DemonsImageToImageObjectMetricGetValueAndDerivativeThreader< ThreadedIndexedContainerPartitioner, Superclass, Self >
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

  DemonsImageToImageObjectMetric(const Self &); //purposely not implemented
  void operator = (const Self &); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDemonsImageToImageObjectMetric.hxx"
#endif

#endif
