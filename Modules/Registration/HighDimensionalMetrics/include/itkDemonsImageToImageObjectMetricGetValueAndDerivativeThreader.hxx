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
#ifndef __itkDemonsImageToImageObjectMetricGetValueAndDerivativeThreader_hxx
#define __itkDemonsImageToImageObjectMetricGetValueAndDerivativeThreader_hxx

#include "itkDemonsImageToImageObjectMetricGetValueAndDerivativeThreader.h"

namespace itk
{

template< class TDomainPartitioner, class TImageToImageMetric, class TDemonsMetric >
bool
DemonsImageToImageObjectMetricGetValueAndDerivativeThreader< TDomainPartitioner, TImageToImageMetric, TDemonsMetric >
::ProcessPoint( const VirtualPointType &,
                const FixedImagePointType &,
                const FixedImagePixelType &        fixedImageValue,
                const FixedImageGradientType &     fixedImageGradient,
                const MovingImagePointType &,
                const MovingImagePixelType &       movingImageValue,
                const MovingImageGradientType &    movingImageGradient,
                MeasureType &                      metricValueReturn,
                DerivativeType &                   localDerivativeReturn,
                const ThreadIdType ) const
{
   TDemonsMetric * associate = dynamic_cast< TDemonsMetric * >( this->m_Associate );

  /* Metric value */
  const InternalComputationValueType speedValue = fixedImageValue - movingImageValue;
  const InternalComputationValueType sqr_speedValue = vnl_math_sqr( speedValue );
  metricValueReturn = sqr_speedValue;

  /* Derivative */
  InternalComputationValueType gradientSquaredMagnitude = 0;
  FixedImageGradientType gradient;
  if( associate->GetGradientSourceIncludesFixed() )
    {
    gradient = fixedImageGradient;
    }
    else
    {
    gradient = movingImageGradient;
    }

  for ( ImageDimensionType j = 0; j < ImageToImageObjectMetricType::FixedImageDimension; j++ )
    {
    gradientSquaredMagnitude += vnl_math_sqr( gradient[j] );
    }

  /*
   * In the original equation the denominator is defined as (g-f)^2 + grad_mag^2.
   * However there is a mismatch in units between the two terms.
   * The units for the second term is intensity^2/mm^2 while the
   * units for the first term is intensity^2. This mismatch is particularly
   * problematic when the fixed image does not have unit spacing.
   * In this implemenation, we normalize the first term by a factor K,
   * such that denominator = (g-f)^2/K + grad_mag^2
   * where K = mean square spacing to compensate for the mismatch in units.
   */
  const InternalComputationValueType denominator =
    sqr_speedValue / associate->m_Normalizer + gradientSquaredMagnitude;

  if ( vnl_math_abs(speedValue) < associate->GetIntensityDifferenceThreshold() || denominator < associate->GetDenominatorThreshold() )
    {
    localDerivativeReturn.Fill( NumericTraits<DerivativeValueType>::Zero );
    return true;
    }

  DerivativeValueType floatingPointCorrectionResolution = associate->GetFloatingPointCorrectionResolution();

  for ( NumberOfParametersType j = 0;
        j < associate->GetNumberOfLocalParameters(); j++ )
    {
    localDerivativeReturn[j] = speedValue * gradient[j] / denominator;

    intmax_t test = static_cast< intmax_t >
            ( localDerivativeReturn[j] * floatingPointCorrectionResolution );

    localDerivativeReturn[j] = static_cast<DerivativeValueType>
                                  ( test / floatingPointCorrectionResolution );
    }

  return true;
}

} // end namespace itk

#endif
