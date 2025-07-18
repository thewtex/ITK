/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkWeightedMeanSampleFilter_h
#define itkWeightedMeanSampleFilter_h

#include "itkMeanSampleFilter.h"
#include "itkFunctionBase.h"
#include "itkDataObjectDecorator.h"

namespace itk::Statistics
{
/**
 * \class WeightedMeanSampleFilter
 * \brief Given a sample where each measurement vector has
 * associated weight value, this filter computes the sample mean
 *
 * To run this algorithm, you have plug in the target sample data
 * using SetInput method and provides weight by an array or function.
 *. Then call the Update method to run the algorithm.
 *
 * \sa MeanSampleFilter
 *
 * \ingroup ITKStatistics
 */
template <typename TSample>
class ITK_TEMPLATE_EXPORT WeightedMeanSampleFilter : public MeanSampleFilter<TSample>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(WeightedMeanSampleFilter);

  /**Standard class type aliases. */
  using Self = WeightedMeanSampleFilter;
  using Superclass = MeanSampleFilter<TSample>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Standard Macros */
  /** @ITKStartGrouping */
  itkOverrideGetNameOfClassMacro(WeightedMeanSampleFilter);
  itkNewMacro(Self);
  /** @ITKEndGrouping */
  /** Types derived from the base class */
  using typename Superclass::SampleType;
  using typename Superclass::MeasurementVectorType;
  using typename Superclass::MeasurementVectorSizeType;
  using typename Superclass::MeasurementType;

  /** Types derived from the base class */
  using typename Superclass::MeasurementVectorRealType;
  using typename Superclass::MeasurementRealType;


  /** Type of weight values */
  using WeightValueType = double;


  /** Array type for weights */
  using WeightArrayType = Array<WeightValueType>;

  /** Type of DataObjects to use for the weight array type */
  using InputWeightArrayObjectType = SimpleDataObjectDecorator<WeightArrayType>;

  /** Method to set/get the input value of the weight array */
  itkSetGetDecoratedInputMacro(Weights, WeightArrayType);


  /** Weight calculation function type */
  using WeightingFunctionType = FunctionBase<MeasurementVectorType, WeightValueType>;

  /** Type of DataObjects to use for Weight function */
  using InputWeightingFunctionObjectType = DataObjectDecorator<WeightingFunctionType>;

  /** Method to set/get the weighting function */
  itkSetGetDecoratedObjectInputMacro(WeightingFunction, WeightingFunctionType);


  /** Types derived from the base class */
  using typename Superclass::MeasurementVectorDecoratedType;
  using typename Superclass::OutputType;

protected:
  WeightedMeanSampleFilter();
  ~WeightedMeanSampleFilter() override = default;

  void
  GenerateData() override;

  // compute mean with weight array
  void
  ComputeMeanWithWeights();

  // compute mean using a weighting function
  void
  ComputeMeanWithWeightingFunction();
}; // end of class
} // namespace itk::Statistics

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkWeightedMeanSampleFilter.hxx"
#endif

#endif
