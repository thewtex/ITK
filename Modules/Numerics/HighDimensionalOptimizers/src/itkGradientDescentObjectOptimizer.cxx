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

#include "itkGradientDescentObjectOptimizer.h"

namespace itk
{

/**
 * Default constructor
 */
GradientDescentObjectOptimizer
::GradientDescentObjectOptimizer()
{
  // m_TrustedStepScale is used for automatic learning rate estimation.
  // it will be initialized later either by user or by m_ScalesEstimator.
  this->m_TrustedStepScale = NumericTraits<InternalComputationValueType>::Zero;
}

/**
 * Destructor
 */
GradientDescentObjectOptimizer
::~GradientDescentObjectOptimizer()
{}


/**
 *PrintSelf
 */
void
GradientDescentObjectOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/**
 * Start and run the optimization
 */
void
GradientDescentObjectOptimizer
::StartOptimization()
{
  itkDebugMacro("StartOptimization");

  /* Must call the superclass version for basic validation and setup */
  Superclass::StartOptimization();

  /* Get the default step size if user has not set one */
  if ( this->m_ScalesEstimator.IsNotNull() )
    {
    if ( this->m_TrustedStepScale <=
      NumericTraits<InternalComputationValueType>::epsilon())
      {
      this->m_TrustedStepScale = this->m_ScalesEstimator->EstimateTrustedStepScale();
      }
    }

  this->ResumeOptimization();
}

/**
 * Advance one Step following the gradient direction
 */
void
GradientDescentObjectOptimizer
::AdvanceOneStep()
{
  itkDebugMacro("AdvanceOneStep");

  /* Begin threaded gradient modification. Work is done in
   * ProcessGradientOverSubRange */
  this->ProcessGradient();

  /* Estimate the learning rate, if user has supplied a ScalesEstimator.
   * Otherwise, it's left as-is. */
  this->EstimateLearningRate();
}

/**
 * Modify the gradient over a given index range.
 */
void
GradientDescentObjectOptimizer
::ProcessGradientOverSubRange( const IndexRangeType& subrange,
  const ThreadIdType )
{
  /* If scales are identity, there's no work to do */
  if (this->m_ScalesAreIdentity)
    {
    return;
    }

  const ScalesType& scales = this->GetScales();

  /* Loop over the range. It is inclusive. */
  for ( IndexValueType j = subrange[0]; j <= subrange[1]; j++ )
    {
    // scales is checked during StartOptmization for values <=
    // machine epsilon.
    // Take the modulo of the index to handle gradients from transforms
    // with local support. The gradient array stores the gradient of local
    // parameters at each local index with linear packing.
    IndexValueType scalesIndex = j % scales.Size();
    this->m_Gradient[j] = this->m_Gradient[j] / scales[scalesIndex];
    }
}

/**
 * Estimate the learning rate.
 */
void
GradientDescentObjectOptimizer
::EstimateLearningRate()
{
  if (this->m_ScalesEstimator.IsNotNull())
    {
    InternalComputationValueType stepScale
      = this->m_ScalesEstimator->EstimateStepScale(this->m_Gradient);

    if (stepScale <= NumericTraits<InternalComputationValueType>::epsilon())
      {
      this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
      }
    else
      {
      this->m_LearningRate = this->m_TrustedStepScale / stepScale;
      }
    }
}

}//namespace itk
