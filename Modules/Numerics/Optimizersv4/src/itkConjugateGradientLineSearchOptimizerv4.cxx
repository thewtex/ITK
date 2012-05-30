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

#include "itkConjugateGradientLineSearchOptimizerv4.h"

namespace itk
{

/**
 * Default constructor
 */
ConjugateGradientLineSearchOptimizerv4
::ConjugateGradientLineSearchOptimizerv4()
{
}

/**
 * Destructor
 */
ConjugateGradientLineSearchOptimizerv4
::~ConjugateGradientLineSearchOptimizerv4()
{}


/**
 *PrintSelf
 */
void
ConjugateGradientLineSearchOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/**
 * StartOptimization
 */
void
ConjugateGradientLineSearchOptimizerv4
::StartOptimization(void)
{
  this->m_ConjugateGradient.SetSize( this->m_Metric->GetNumberOfParameters() );
  this->m_ConjugateGradient.Fill( itk::NumericTraits< InternalComputationValueType >::Zero );
  this->m_LastGradient.SetSize( this->m_Metric->GetNumberOfParameters() );
  this->m_LastGradient.Fill( itk::NumericTraits< InternalComputationValueType >::Zero );

  Superclass::StartOptimization();
}

/**
 * ModifyGradientByDirection
 */
void
ConjugateGradientLineSearchOptimizerv4
::ComputeSearchDirection(void)
{
  /** the nonlinear cg update */
  InternalComputationValueType gamma = itk::NumericTraits< InternalComputationValueType >::Zero;
  InternalComputationValueType gammaDenom = inner_product( this->m_LastGradient , this->m_LastGradient );
  if ( gammaDenom > itk::NumericTraits< InternalComputationValueType >::epsilon() )
    {
    gamma = inner_product( this->m_Gradient - this->m_LastGradient , this->m_Gradient ) / gammaDenom;
    }

  /** Modified Polak-Ribiere restart conditions */
  if ( gamma < 0 || gamma > 10 )
    {
    gamma = 0;
    }
  this->m_LastGradient = this->m_Gradient;
  if( gamma > 0 )
    {
    this->m_ConjugateGradient = this->m_Gradient + this->m_ConjugateGradient * gamma;
    this->m_Gradient = this->m_ConjugateGradient;
    }
}

}//namespace itk
