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

#ifndef __itkMultiVariateObjectToObjectMetricv4_h
#define __itkMultiVariateObjectToObjectMetricv4_h

#include "itkObjectToObjectMetric.h"
#include "itkArray.h"
#include <deque>

namespace itk
{
/** \class MultiVariateObjectToObjectMetricv4
  * \brief This class takes one ore more ObjectToObject metrics and assigns weights to their derivatives
  * to compute a single result.
  *
  * This class takes N ObjectToObject-derived component metrics and assigns a weight to each of the metrics'
  * derivatives. It then computes a weighted measure of the metric. The GetValue() and
  * GetValueAndDerivative() methods compute the measure and derivative using the following calculation:
  *
  * metric value = Sum_j ( w_j * M_j ) (see important note below)
  *
  * and the GetDerivative() method computes the derivative by computing:
  *
  * derivative = Sum_j (w_j * dM_j / ||dM_j|| )
  *
  * \note The metric value is unit-less, and thus it is difficult to compute a combined metric.
  * This metric returns the metric value in three ways:
  *  1) GetValue() returns the computed value of only the *first* component metric. This result
  *     is stored in m_Value and also returned by GetCurrentValue().
  *  2) GetValueArray() returns an itkArray of metric values, one for each component metric. It
  *     only has meaning after a call to GetValue(), GetDerivative() or GetValueAndDerivative().
  *  3) GetWeightedValue() returns a combined metric value of all component metrics, using the
  *     assigned weights. It only has meaning after a call to GetValue(), GetDerivative()
  *     or GetValueAndDerivative().
  *
  * The assigned weights are normalized internally to sum to one before use, and the weights
  * default to 1/N, where N is the number of component metrics.
  *
  * \note Each component metric must be setup independently. That is, each component's images,
  * transforms and options must be set independently. There are no methods in this metric
  * to assign these settings.
  *
  * Each component will be initialized by this metric in the call to Initialize().
  *
  * \note Each component metric must use the same transform parameters object. That is, each metric
  * must be evaluating the same parameters by evaluating the same transform. Except, if a component
  * transform is a CompositeTransform, in which case it must be set to optimize a single transform,
  * and that transform must be the same as the transform in other component metrics.
  *
  * This metric's m_MovingTransform and m_FixedTransform members are assigned to the transforms assigned
  * to the first component metric.
  */

template<unsigned int TFixedDimension, unsigned int TMovingDimension, unsigned int TVirtualDimension = TFixedDimension>
class ITK_EXPORT MultiVariateObjectToObjectMetricv4:
  public ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualDimension>
{
public:
  /** Standard class typedefs */
  typedef MultiVariateObjectToObjectMetricv4                                          Self;
  typedef ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualDimension>  Superclass;
  typedef SmartPointer<Self>                                                          Pointer;
  typedef SmartPointer<const Self>                                                    ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiVariateObjectToObjectMetricv4, ObjectToObjectMetric);

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Types inherited from Superclass. */
  typedef typename Superclass::MeasureType                  MeasureType;
  typedef typename Superclass::DerivativeType               DerivativeType;
  typedef typename Superclass::DerivativeValueType          DerivativeValueType;
  typedef typename Superclass::ParametersType               ParametersType;
  typedef typename Superclass::ParametersValueType          ParametersValueType;
  typedef typename Superclass::NumberOfParametersType       NumberOfParametersType;
  typedef typename Superclass::CoordinateRepresentationType CoordinateRepresentationType;
  typedef typename Superclass::MovingTransformType          MovingTransformType;

  /** typedefs related to the metric queue */
  typedef Superclass                               MetricType;
  typedef typename MetricType::Pointer             MetricBasePointer;
  typedef typename MetricType::ConstPointer        MetricBaseConstPointer;
  typedef std::deque<MetricBasePointer>            MetricQueueType;

  typedef typename DerivativeType::ValueType       WeightValueType;
  typedef Array<WeightValueType>                   WeightsArrayType;
  typedef Array<MeasureType>                       MetricValueArrayType;

  itkSetMacro(MetricWeights,WeightsArrayType);
  itkGetMacro(MetricWeights,WeightsArrayType);

  /** Add a metric to the queue */
  void AddMetric( MetricType* metric );

  /** Clear the metric queue */
  void ClearMetricQueue( void );

  /** Get the number of metrics */
  SizeValueType GetNumberOfMetrics() const;

  void Initialize(void) throw ( itk::ExceptionObject );

  /** Evaluate the metrics and return the value of only the *first* metric.
   * \sa GetValueArray
   * \sa GetWeightedValue
   */
  MeasureType GetValue() const;

  virtual void GetDerivative( DerivativeType & ) const;

  /** Evaluate the metric value and derivative.
   * \note \param value will contain the value of only the *first* metric.
   * \sa GetValueArray
   * \sa GetWeightedValue */
  void GetValueAndDerivative(MeasureType & value, DerivativeType & derivative) const;

  /** Returns an itkArray of metric values, one for each component metric. It
   *  only has meaning after a call to GetValue(), GetDerivative() or GetValueAndDerivative(). */
  MetricValueArrayType GetValueArray() const;

  /**  Returns a combined metric value of all component metrics, using the
   *   assigned weights. It only has meaning after a call to GetValue(), GetDerivative() or GetValueAndDerivative(). */
  MeasureType GetWeightedValue() const;

  /** Get the metrics queue */
  const MetricQueueType & GetMetricQueue();

protected:

  MultiVariateObjectToObjectMetricv4();
  virtual ~MultiVariateObjectToObjectMetricv4();
  void PrintSelf(std::ostream & os, Indent indent) const;

private:

  //purposely not implemented
  MultiVariateObjectToObjectMetricv4(const Self &);
  void operator=(const Self &);

  MetricQueueType               m_MetricQueue;
  WeightsArrayType              m_MetricWeights;
  mutable MetricValueArrayType  m_MetricValueArray;
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiVariateObjectToObjectMetricv4.hxx"
#endif

#endif
