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

#include "itkRegularStepGradientDescentObjectOptimizer.h"

namespace itk
{

/**
 * Default constructor
 */
RegularStepGradientDescentObjectOptimizer
::RegularStepGradientDescentObjectOptimizer()
{
  this->m_MaximumStepLength = static_cast<InternalComputationValueType>(1.0);
  this->m_MinimumStepLength = static_cast<InternalComputationValueType>(1e-3);
  this->m_GradientMagnitudeTolerance = static_cast<InternalComputationValueType>(1e-4);
  this->m_NumberOfIterations = static_cast<SizeValueType>(100);
  this->m_CurrentIteration   = static_cast<SizeValueType>(0);
  this->m_CurrentStepLength  = static_cast<InternalComputationValueType>(0);
  this->m_RelaxationFactor   = static_cast<InternalComputationValueType>(0.5);
}

/**
 * Destructor
 */
RegularStepGradientDescentObjectOptimizer
::~RegularStepGradientDescentObjectOptimizer()
{}


/**
 *PrintSelf
 */
void
RegularStepGradientDescentObjectOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "MaximumStepLength: " << this->GetMaximumStepLength()
               << std::endl;
  os << indent << "MinimuStepLength: " << this->GetMinimumStepLength()
               << std::endl;
  os << indent << "RelaxationFactor: " << this->GetRelaxationFactor()
               << std::endl;
  os << indent << "GradientMagnitudeTolerance: "
               << this->GetGradientMagnitudeTolerance() << std::endl;
  os << indent << "CurrentStepLength: " << this->GetCurrentStepLength()
               << std::endl;
}

/**
 * Start and run the optimization
 */
void
RegularStepGradientDescentObjectOptimizer
::StartOptimization()
{
  itkDebugMacro("StartOptimization");

  /* Must call the superclass version for basic validation and setup */
  Superclass::StartOptimization();

  m_CurrentStepLength = m_MaximumStepLength;

  // validity check for the value of GradientMagnitudeTolerance
  if ( this->m_GradientMagnitudeTolerance <
        NumericTraits<InternalComputationValueType>::Zero )
    {
    itkExceptionMacro(<< "Gradient magnitude tolerance must be"
                         "greater or equal 0.0. Current value is "
                      <<   this->m_GradientMagnitudeTolerance);
    }

  /* Initialize previous-gradient storage */
  const NumberOfParametersType
    spaceDimension = this->m_Metric->GetNumberOfParameters();
  m_PreviousGradientScaled = DerivativeType(spaceDimension);
  typedef DerivativeType::ValueType  DerivativeValueType;
  m_PreviousGradientScaled.Fill(NumericTraits<DerivativeValueType>::Zero);

  /* Setup threading variables */
  this->m_MagnitudeSquaredPerThread.resize( this->GetNumberOfThreads() );
  this->m_ScalarProductPerThread.resize( this->GetNumberOfThreads() );

  this->ResumeOptimization();
}

/**
 * Advance one Step following the gradient direction
 */
void
RegularStepGradientDescentObjectOptimizer
::AdvanceOneStep()
{
  itkDebugMacro("AdvanceOneStep");

  InternalComputationValueType
    zero = NumericTraits<InternalComputationValueType>::Zero;
  /* Check relaxation factor. The previous implementation of this class
   * had this check here rather than in StartOptimization, so it's been
   * left here. */
  if ( this->m_RelaxationFactor < zero )
    {
    itkExceptionMacro(<< "Relaxation factor must be positive. Current value is "
                      << m_RelaxationFactor);
    return;
    }

  if ( this->m_RelaxationFactor >=
        NumericTraits<InternalComputationValueType>::One )
    {
    itkExceptionMacro(<< "Relaxation factor must less than 1.0. Current value is "
                      << m_RelaxationFactor);
    return;
    }

  /* Initialize threading memory */
  for( ThreadIdType i=0; i < this->GetNumberOfThreads(); i++ )
    {
    this->m_MagnitudeSquaredPerThread[i] = zero;
    this->m_ScalarProductPerThread[i] = zero;
    }

  /* Begin threaded gradient modification and gradient statistic
   * calculations. Work is done in ProcessGradientOverSubRange.
   * This will scale the gradient and calculate gradient magnitude and
   * scalar product over each threaded region. */
  this->ProcessGradient();

  /* Collect per-thread results */
  InternalComputationValueType magnitudeSquared = zero;
  InternalComputationValueType scalarProduct = zero;
  for( ThreadIdType i=0; i < this->GetNumberOfThreads(); i++ )
    {
    magnitudeSquared += this->m_MagnitudeSquaredPerThread[i];
    scalarProduct += this->m_ScalarProductPerThread[i];
    }

  /* Check for a direction change */
  if ( scalarProduct < 0 )
    {
    this->m_CurrentStepLength *= this->m_RelaxationFactor;
    }

  /* Check for minimum step length */
  if ( m_CurrentStepLength < m_MinimumStepLength )
    {
    m_StopCondition = STEP_TOO_SMALL;
    m_StopConditionDescription << "Step too small after "
                               << m_CurrentIteration
                               << " iterations. Current step ("
                               << m_CurrentStepLength
                               << ") is less than minimum step ("
                               << m_MinimumStepLength
                               << ").";
    this->StopOptimization();
    return;
    }

  /* Store the gradient for the next iteration. It's been modified
   * in-place by the scales. Store it before we modify it again to
   * act as the transform update. */
  this->m_PreviousGradientScaled = this->m_Gradient;

  /* The computation to be made in the next two steps is:
   *
   *  transformUpdate = transformedGradient * factor / scales[j]
   *
   * where transformedGradient is stored in m_Gradient. */

  /* Compute factor */
  const InternalComputationValueType gradientMagnitude =
    vcl_sqrt(magnitudeSquared);
  if ( gradientMagnitude < m_GradientMagnitudeTolerance )
    {
    m_StopCondition = GRADIENT_MAGNITUDE_TOLERANCE;
    m_StopConditionDescription << "Gradient magnitude tolerance met after "
                               << m_CurrentIteration
                               << " iterations. Gradient magnitude ("
                               << gradientMagnitude
                               << ") is less than gradient magnitude tolerance ("
                               << m_GradientMagnitudeTolerance
                               << ").";
    this->StopOptimization();
    return;
    }
  const InternalComputationValueType
    factor = this->m_CurrentStepLength / gradientMagnitude;

  /* Modify the current gradient again to act as the transform update. */
  const ScalesType& scales = this->GetScales();
  for ( IndexValueType j = 0; j < this->m_Gradient.Size(); j++ )
    {
    /* scales is checked during StartOptmization for values <=
     * machine epsilon.
     * Take the modulo of the index to account for gradients from transforms
     * with local support. */
    IndexValueType scalesIndex = j % scales.Size();
    this->m_Gradient[j] = this->m_Gradient[j] * factor / scales[scalesIndex];
    }
}

/**
 * Process the gradient over a given index range.
 */
void
RegularStepGradientDescentObjectOptimizer
::ProcessGradientOverSubRange( const IndexRangeType& subrange, const ThreadIdType threadId )
{
  const ScalesType& scales = this->GetScales();

  /* Loop over the range. It is inclusive. */
  this->m_MagnitudeSquaredPerThread[threadId] =
    NumericTraits<InternalComputationValueType>::Zero;
  this->m_ScalarProductPerThread[threadId] =
    NumericTraits<InternalComputationValueType>::Zero;
  for ( IndexValueType j = subrange[0]; j <= subrange[1]; j++ )
    {
    /* Modify gradient by scales.
     * scales is checked during StartOptmization for values <=
     * machine epsilon.
     * Take the modulo of the index to account for gradients from transforms
     * with local support. */
    IndexValueType scalesIndex = j % scales.Size();
    this->m_Gradient[j] = this->m_Gradient[j] / scales[scalesIndex];
    InternalComputationValueType previousGradientValueScaled =
      this->m_PreviousGradientScaled[j] / scales[scalesIndex];

    /* Part of computation of the gradient magnitude */
    this->m_MagnitudeSquaredPerThread[threadId] +=
      this->m_Gradient[j] * this->m_Gradient[j];

    /* Compute the scalar product */
    this->m_ScalarProductPerThread[threadId] +=
      this->m_Gradient[j] * previousGradientValueScaled;
    }

}

}//namespace itk
