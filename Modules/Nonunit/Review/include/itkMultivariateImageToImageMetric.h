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

#ifndef __itkMultivariateImageToImageMetric_h
#define __itkMultivariateImageToImageMetric_h

#include "itkImageToImageMetric.h"
#include "itkArray.h"
#include <deque>

namespace itk
{
/** \class MultivariateImageToImageMetric
  * \brief This class takes multiple Image to Image metrics and assigns weights to their derivatives
  *
  * This class takes N ImageToImage metrics and assigns a weight to each of the metrics'
  * derivatives.It then computes a weighted measure of the metric. The GetValue() method computes
  * the measure of the multivariate metric and derivative using the following calculation
  *
  * metric value = Sum_j ( w_j * M_j / ||dM_j|| )
  *
  * and the GetDerivative() method computes the derivative by computing:
  *
  * derivative = Sum_j (w_j * dM_j / ||dM_j|| )
  *
  */

template <class TFixedImage, class TMovingImage>
class ITK_EXPORT MultivariateImageToImageMetric:
  public ImageToImageMetric<TFixedImage,TMovingImage>
{
public:
  /** Standard class typedefs */
  typedef MultivariateImageToImageMetric               Self;
  typedef ImageToImageMetric<TFixedImage,TMovingImage> Superclass;
  typedef SmartPointer<Self>                           Pointer;
  typedef SmartPointer<const Self>                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultivariateImageToImageMetric, ImageToImageMetric);

  /** Types inherited from Superclass. */
  typedef typename Superclass::TransformType                TransformType;
  typedef typename Superclass::TransformPointer             TransformPointer;
  typedef typename Superclass::TransformJacobianType        TransformJacobianType;
  typedef typename Superclass::InterpolatorType             InterpolatorType;
  typedef typename Superclass::MeasureType                  MeasureType;
  typedef typename Superclass::DerivativeType               DerivativeType;
  typedef typename Superclass::ParametersType               ParametersType;
  typedef typename Superclass::FixedImageType               FixedImageType;
  typedef typename Superclass::MovingImageType              MovingImageType;
  typedef typename Superclass::MovingImagePointType         MovingImagePointType;
  typedef typename Superclass::FixedImageConstPointer       FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer      MovingImageConstPointer;
  typedef typename Superclass::CoordinateRepresentationType CoordinateRepresentationType;
  typedef typename Superclass::FixedImageSampleContainer    FixedImageSampleContainer;
  typedef typename Superclass::ImageDerivativesType         ImageDerivativesType;
  typedef typename Superclass::WeightsValueType             WeightsValueType;
  typedef typename Superclass::IndexValueType               IndexValueType;

  /** typedefs related to the metric queue */
  typedef Superclass                                        MetricType;
  typedef typename MetricType::Pointer                      MetricPointerType;
  typedef std::deque <MetricPointerType>                    MetricQueueType;
  typedef Array <double> WeightsArrayType;

  itkSetMacro(MetricDerivativeWeights,WeightsArrayType);
  itkGetMacro(MetricDerivativeWeights,WeightsArrayType);

  /** Add a metric to the queue */
  void AddMetric (MetricType* metric);

  /** Get the number of metrics */
  size_t GetNumberOfMetrics() const;

  /**  Computes the value of the metric after assigning weights
    *  to the derivatives of the metrics in the queue */
  MeasureType GetValue(const ParametersType & parameters) const;


  /** Get the derivatives of the match measure. */
  void GetDerivative(const ParametersType & parameters,
                     DerivativeType & Derivative) const;


  /** Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative(const ParametersType & parameters,
                             MeasureType & Value,
                             DerivativeType & Derivative) const;

  /** Get the metrics queue */
  MetricQueueType GetMetricsQueue ();

protected:

  MultivariateImageToImageMetric();
  virtual ~MultivariateImageToImageMetric();
  void PrintSelf(std::ostream & os, Indent indent) const;

private:

  //purposely not implemented
  MultivariateImageToImageMetric(const Self &);
  void operator=(const Self &);

  MetricQueueType   m_MetricsQueue;
  WeightsArrayType  m_MetricDerivativeWeights;
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultivariateImageToImageMetric.txx"
#endif

#endif
