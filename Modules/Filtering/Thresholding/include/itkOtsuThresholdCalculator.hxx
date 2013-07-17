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
#ifndef __itkOtsuThresholdCalculator_hxx
#define __itkOtsuThresholdCalculator_hxx

#include "itkOtsuThresholdCalculator.h"
#include "itkOtsuMultipleThresholdsCalculator.h"
#include "vnl/vnl_math.h"

namespace itk
{
template< class THistogram, class TOutput >
void
OtsuThresholdCalculator< THistogram, TOutput >
::GenerateData(void)
{
  this->UpdateProgress(0.0);
  // Compute the Otsu threshold using the OtsuMultipleThresholdsCalculator to ensure code reusability.
  typedef itk::OtsuMultipleThresholdsCalculator<THistogram> CalculatorType;
  typename CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetInputHistogram( this->GetInput() );
  calculator->SetNumberOfThresholds( 1 );
  calculator->Compute();
  this->GetOutput()->Set( static_cast<OutputType>( calculator->GetOutput()[0] ) );
  this->UpdateProgress(1.0);
}
} // end namespace itk

#endif
