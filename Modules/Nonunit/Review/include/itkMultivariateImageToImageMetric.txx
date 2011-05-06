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

#ifndef __itkMultivariateImageToImageMetric_txx
#define __itkMultivariateImageToImageMetric_txx

#include "itkMultivariateImageToImageMetric.h"

namespace itk
{
/** Constructor */
template <class TFixedImage, class TMovingImage>
MultivariateImageToImageMetric<TFixedImage, TMovingImage>
::MultivariateImageToImageMetric()
{
  this->m_MetricsQueue.clear();
}

/** Destructor */
template <class TFixedImage, class TMovingImage>
MultivariateImageToImageMetric<TFixedImage, TMovingImage>
::~MultivariateImageToImageMetric()
{
}

/** Add a metric to the queue. This method should be called after setting
  * the fixed image, moving image, transform, interpolator and fixed image region
  * for the multivariate metric. */
template <class TFixedImage, class TMovingImage>
void
MultivariateImageToImageMetric<TFixedImage, TMovingImage>
::AddMetric (MetricType* metric)
{
  this->m_MetricsQueue.push_back (metric);
}

/** Get the number of metrics */
template <class TFixedImage, class TMovingImage>
size_t
MultivariateImageToImageMetric<TFixedImage, TMovingImage>
::GetNumberOfMetrics() const
{
    return this->m_MetricsQueue.size();
}

/**  Computes the value of the metric after assigning weights
    *  to the derivatives of the metrics in the queue */
template <class TFixedImage, class TMovingImage>
typename MultivariateImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
MultivariateImageToImageMetric<TFixedImage,TMovingImage>
::GetValue(const ParametersType & parameters) const
{
  MeasureType value = 0;
  MeasureType metricvalue = 0;
  DerivativeType metricderivative (parameters.Size() );

  for (unsigned int j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    metricvalue = (this->m_MetricsQueue[j])->GetValue(parameters);
    (this->m_MetricsQueue[j])->GetDerivative(parameters,metricderivative);

    // value = sum_j w_j*M_j / ||dM_j||
    value += (metricvalue*this->m_MetricDerivativeWeights[j])/metricderivative.magnitude();
    }

  return value;
}

template <class TFixedImage, class TMovingImage>
void
MultivariateImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative(const ParametersType & parameters,
                     DerivativeType & Derivative) const
{
  if ( Derivative.GetSize() != parameters.Size() )
    {
    Derivative = DerivativeType(parameters.Size() );
    }
  Derivative.Fill(0);
  DerivativeType metricderivative (parameters.Size() );

  for (unsigned int j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    (this->m_MetricsQueue[j])->GetDerivative(parameters,metricderivative);

    // derivative = \sum_j w_j * (dM_j / ||dM_j||)
    Derivative += this->m_MetricDerivativeWeights[j]*metricderivative.normalize();
    }
}

template <class TFixedImage, class TMovingImage>
void
MultivariateImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(const ParametersType & parameters,
                             MeasureType & Value,
                             DerivativeType & Derivative) const
{
  Value = this->GetValue(parameters);
  this->GetDerivative(parameters,Derivative);
}

template <class TFixedImage, class TMovingImage>
typename MultivariateImageToImageMetric<TFixedImage,TMovingImage>::MetricQueueType
MultivariateImageToImageMetric<TFixedImage,TMovingImage>
::GetMetricsQueue()
{
  return this->m_MetricsQueue;
}

template <class TFixedImage, class TMovingImage>
void
MultivariateImageToImageMetric<TFixedImage,TMovingImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  os << indent << "Weights of metric derivatives: " << this->m_MetricDerivativeWeights << std::endl;
  os << indent << "The multivariate contains the following metrics: " << std::endl << std::endl;
  for (unsigned int i = 0; i < this->GetNumberOfMetrics() ; i++)
    {
    os << indent << this->m_MetricsQueue[i] << std::endl;
    }
}

} // end namespace itk

#endif
