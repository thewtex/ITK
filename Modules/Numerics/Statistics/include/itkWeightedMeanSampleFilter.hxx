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
#ifndef __itkWeightedMeanSampleFilter_hxx
#define __itkWeightedMeanSampleFilter_hxx

#include "itkWeightedMeanSampleFilter.h"

#include <vector>
#include "itkCompensatedSummation.h"
#include "itkMeasurementVectorTraits.h"

namespace itk
{
namespace Statistics
{
template< typename TSample >
WeightedMeanSampleFilter< TSample >
::WeightedMeanSampleFilter()
{
  this->ProcessObject::SetNthInput(1, NULL);
}

template< typename TSample >
WeightedMeanSampleFilter< TSample >
::~WeightedMeanSampleFilter()
{}

template< typename TSample >
void
WeightedMeanSampleFilter< TSample >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  // m_Weights
  os << indent << "Weights: " << this->GetWeightsInput() << std::endl;
  // m_WeightingFunction
  os << indent << "Weighting Function: " << this->GetWeightingFunctionInput() << std::endl;
}

template< typename TSample >
void
WeightedMeanSampleFilter< TSample >
::GenerateData()
{
  // if weighting function is specifed, use it to compute the mean
  const InputWeightingFunctionObjectType *functionObject =
    this->GetWeightingFunctionInput();

  if ( functionObject != NULL )
    {
    this->ComputeWeightedMean( true );
    return;
    }

  // if weight array is specified use it to compute the mean
  const InputWeightArrayObjectType *weightArrayObject =
    this->GetWeightsInput();

  if ( weightArrayObject != NULL )
    {
    this->ComputeWeightedMean( false );
    return;
    }

  // Otherwise compute the regular mean ( without weight coefficients)
  Superclass::GenerateData();
}

template< typename TSample >
void
WeightedMeanSampleFilter< TSample >
::ComputeWeightedMean(bool useWeightingFunction)
{
  // set up input / output
  const SampleType *input = this->GetInput();

  const MeasurementVectorSizeType measurementVectorSize =
    input->GetMeasurementVectorSize();

  MeasurementVectorDecoratedType *decoratedOutput =
    itkDynamicCastInDebugMode< MeasurementVectorDecoratedType * >(
      this->ProcessObject::GetOutput(0) );

  MeasurementVectorRealType output = decoratedOutput->Get();

  NumericTraits<MeasurementVectorRealType>::SetLength( output, this->GetMeasurementVectorSize() );

  // algorithm start
  typedef CompensatedSummation< MeasurementRealType > MeasurementRealAccumulateType;
  std::vector< MeasurementRealAccumulateType > sum( measurementVectorSize );

  WeightValueType totalWeight = NumericTraits< WeightValueType >::Zero;

  typename SampleType::ConstIterator iter = input->Begin();
  typename SampleType::ConstIterator end =  input->End();

  for ( unsigned int sampleVectorIndex = 0;
        iter != end;
        ++iter, ++sampleVectorIndex )
    {
    const MeasurementVectorType & measurement = iter.GetMeasurementVector();

    const typename SampleType::AbsoluteFrequencyType frequency = iter.GetFrequency();

    WeightValueType rawWeight;
    if ( useWeightingFunction )
      {
      rawWeight = this->GetWeightingFunction()->Evaluate( measurement );
      }
    else
      {
      rawWeight = this->GetWeights().GetElement( sampleVectorIndex );
      }

    const WeightValueType weight = ( rawWeight * static_cast< WeightValueType >( frequency ) );
    totalWeight += weight;

    for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
      {
      const MeasurementRealType component =
        static_cast< MeasurementRealType >( measurement[dim] );

      sum[dim] += ( component * weight );
      }
    }

  if ( totalWeight > vnl_math::eps )
    {
    for ( unsigned int dim = 0; dim < measurementVectorSize; dim++ )
      {
      output[dim] = ( sum[dim].GetSum() / static_cast< MeasurementRealType >( totalWeight ) );
      }
    }
  else
    {
    itkExceptionMacro("Total weight was too close to zero. Value = " << totalWeight );
    }

  decoratedOutput->Set( output );
}
} // end of namespace Statistics
} // end of namespace itk

#endif
