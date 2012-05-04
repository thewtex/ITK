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
#ifndef __itkMultiVariateObjectToObjectMetricv4_hxx
#define __itkMultiVariateObjectToObjectMetricv4_hxx

#include "itkMultiVariateObjectToObjectMetricv4.h"
#include "itkCompositeTransform.h"

namespace itk
{

/** Constructor */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::MultiVariateObjectToObjectMetricv4()
{
  this->m_MetricQueue.clear();
}

/** Destructor */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::~MultiVariateObjectToObjectMetricv4()
{
}

/** Add a metric to the queue. */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::AddMetric (MetricType* metric)
{
  this->m_MetricQueue.push_back(metric);
}

/** Clear the queue */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::ClearMetricQueue()
{
  this->m_MetricQueue.clear();
}

/** Get the number of metrics */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
itk::SizeValueType
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetNumberOfMetrics() const
{
  return this->m_MetricQueue.size();
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::Initialize() throw ( ExceptionObject )
{
  if( this->GetNumberOfMetrics() == 0 )
    {
    itkExceptionMacro("No metrics are assigned. Cannot evaluate.");
    }

  /* Verify derivative weights and initialize if appropriate */
  if( this->m_MetricWeights.Size() > 0 )
    {
    if( this->m_MetricWeights.Size() != this->GetNumberOfMetrics() )
      {
      itkExceptionMacro("The derivative weights are not of the proper size. "
                        "Number of metrics: " << this->GetNumberOfMetrics() << ", "
                        "Number of weights: " << this->m_MetricWeights.Size() );
      }
    /* normalize the weights */
    WeightValueType sum = NumericTraits<WeightValueType>::Zero;
    for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
      {
      sum += this->m_MetricWeights[j];
      }
    if( ! sum > NumericTraits<WeightValueType>::epsilon() )
      {
      itkExceptionMacro("The derivative weights are too small: " << this->m_MetricWeights );
      }
    for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
      {
      this->m_MetricWeights[j] = this->m_MetricWeights[j] / sum;
      }
    }
  else
    {
    /* Initialize to defaults */
    this->m_MetricWeights.SetSize( this->GetNumberOfMetrics() );
    this->m_MetricWeights.Fill( NumericTraits<WeightValueType>::One / static_cast<WeightValueType>(this->GetNumberOfMetrics()) );
    }

  /* resize */
  this->m_MetricValueArray.SetSize( this->GetNumberOfMetrics() );

  /* Verify the same transform is in all metrics. */
  const MovingTransformType * firstTransform = NULL;
  for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    const MovingTransformType * transform = this->m_MetricQueue[j]->GetMovingTransform();
    //Check if it's a composite. If so, there must be only one transform set to be
    // optimized, and it must be the same as in other metrics.
    typedef CompositeTransform<typename MovingTransformType::ScalarType, TFixedDimension> CompositeType;
    const CompositeType * composite = dynamic_cast<const CompositeType*>(transform);
    if( composite != NULL )
      {
      SizeValueType count = 0;
      for( size_t n = 0; n < composite->GetNumberOfTransforms(); n++ )
        {
        if( composite->GetNthTransformToOptimize( n ) )
          {
          count++;
          transform = composite->GetNthTransform( n );
          }
        }
      if( count != 1 )
        {
        itkExceptionMacro("Expected exactly one transform set to be optimized within the composite transform. Error with metric " << j << ".");
        }
      }

    if( j == 0 )
      {
      firstTransform = transform;
      }
    else
      {
      if( transform != firstTransform )
        {
        itkExceptionMacro("One or more component metrics have different active transforms. "
                          "Each metric must be using the same transform object. For CompositeTransform, "
                          "there must be only one transform set to optimize, and it must be the same "
                          "as other metric transforms." );
        }
      }
    }

  /* Assign local pointers to common transforms */
  this->SetMovingTransform( const_cast<MovingTransformType*>(this->m_MetricQueue[0]->GetMovingTransform()) );
  this->SetFixedTransform(  const_cast<MovingTransformType*>(this->m_MetricQueue[0]->GetFixedTransform()) );

  /* Initialize */
  Superclass::Initialize();
  for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    try
      {
      this->m_MetricQueue[j]->Initialize();
      }
    catch(ExceptionObject exc)
      {
      std::string msg("Caught exception initializing metric: \n");
      msg += exc.what();
      ExceptionObject err(__FILE__, __LINE__, msg);
      throw err;
      }
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
typename MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>::MeasureType
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetValue() const
{
  for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    this->m_MetricValueArray[j] = this->m_MetricQueue[j]->GetValue();
    }

  MeasureType firstValue = this->m_MetricValueArray[0];
  this->m_Value = firstValue;
  return firstValue;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetDerivative(DerivativeType & derivativeResult) const
{
  MeasureType firstValue;
  this->GetValueAndDerivative( firstValue, derivativeResult );
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetValueAndDerivative(MeasureType & firstValue, DerivativeType & derivativeResult) const
{
  if ( derivativeResult.GetSize() != this->GetNumberOfParameters() )
    {
    derivativeResult.SetSize( this->GetNumberOfParameters() );
    }
  derivativeResult.Fill( NumericTraits<DerivativeValueType>::Zero );

  DerivativeType  metricDerivative;
  MeasureType     metricValue = NumericTraits<MeasureType>::Zero;

  // Loop over metrics
  for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    this->m_MetricQueue[j]->GetValueAndDerivative( metricValue, metricDerivative);
    this->m_MetricValueArray[j] = metricValue;

    DerivativeValueType magnitude = metricDerivative.magnitude();
    DerivativeValueType weightOverMagnitude = NumericTraits<DerivativeValueType>::Zero;
    if( magnitude > NumericTraits<DerivativeValueType>::epsilon() )
      {
      weightOverMagnitude = this->m_MetricWeights[j] / magnitude;
      }
    // derivative = \sum_j w_j * (dM_j / ||dM_j||)
    for( NumberOfParametersType p = 0; p < this->GetNumberOfParameters(); p++ )
      {
      // roll our own loop to avoid temporary variable that could be large when using displacement fields.
      derivativeResult[p] += ( metricDerivative[p] * weightOverMagnitude );
      }
    }

  firstValue = this->m_MetricValueArray[0];
  this->m_Value = firstValue;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
typename MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>::MetricValueArrayType
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetValueArray() const
{
  return this->m_MetricValueArray;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
typename MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>::MeasureType
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetWeightedValue() const
{
  MeasureType value = NumericTraits<MeasureType>::Zero;

  for (SizeValueType j = 0; j < this->GetNumberOfMetrics(); j++)
    {
    // value = sum_j w_j * M_j
    value += this->m_MetricValueArray[j] * this->m_MetricWeights[j];
    }
  return value;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
const typename MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>::MetricQueueType &
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::GetMetricQueue()
{
  return this->m_MetricQueue;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension>
void
MultiVariateObjectToObjectMetricv4<TFixedDimension, TMovingDimension, TVirtualDimension>
::PrintSelf(std::ostream & os, Indent indent) const
{
  os << indent << "Weights of metric derivatives: " << this->m_MetricWeights << std::endl;
  os << indent << "The multivariate contains the following metrics: " << std::endl << std::endl;
  for (SizeValueType i = 0; i < this->GetNumberOfMetrics(); i++)
    {
    os << indent << "~~~ Metric " << i << " ~~~" << std::endl;
    this->m_MetricQueue[i]->Print(os, indent );
    }
}

} // end namespace itk

#endif //__itkMultiVariateObjectToObjectMetricv4_hxx
