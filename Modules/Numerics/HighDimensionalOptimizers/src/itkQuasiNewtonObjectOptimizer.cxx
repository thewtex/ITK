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
#include "itkQuasiNewtonObjectOptimizer.h"

namespace itk
{
/**
 * Constructor
 */
QuasiNewtonObjectOptimizer
::QuasiNewtonObjectOptimizer()
{
  this->m_MaximumIterationsWithoutProgress = 50;
}

/**
 * Destructor
 */
QuasiNewtonObjectOptimizer
::~QuasiNewtonObjectOptimizer()
{
}

void
QuasiNewtonObjectOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/**
 * Start and run the optimization
 */
void
QuasiNewtonObjectOptimizer
::StartOptimization()
{
  itkDebugMacro("StartOptimization");

  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();

  /* Set up the data for Quasi-Newton method */
  this->m_NewtonStep.SetSize(numPara);
  this->m_Hessian.SetSize(numPara, numPara);
  this->m_HessianInverse.SetSize(numPara, numPara);

  /* Must call the superclass version for basic validation and setup */
  Superclass::StartOptimization();
}

/**
 * Advance one step using the Quasi-Newton step or the gradient step.
 *
 * There are a few occasions to use the gradient step:
 * 1) at the first iteration,
 * 2) exceptions thrown at Quasi-Newton step computation, ex. division-by-zero,
 * 3) the Quasi-Newton step is on the opposite direction of the gradient step.
 *    This happens when the metric function is a convex locally when a convex
 *    is expected, or the other way around.
 */
void
QuasiNewtonObjectOptimizer
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");

  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();
  this->m_CurrentPosition = this->m_Metric->GetParameters();

  ParametersType scales(numPara);

  //To be fixed: assuming maximization for optimizing the metric function
  //We probably need to fix the demons metric such that the value is negated
  //when the gradient is negated.
  this->m_Value = - this->m_Value;

  if (this->GetCurrentIteration() == 0)
    {
    m_BestValue = this->m_Value;
    m_BestPosition = this->m_CurrentPosition;
    m_BestIteration = this->GetCurrentIteration();
    }

  if (m_BestValue < this->m_Value)
    {
    m_BestValue = this->m_Value;
    m_BestPosition = this->m_CurrentPosition;
    m_BestIteration = this->GetCurrentIteration();
    }
  else if ( this->GetCurrentIteration() - m_BestIteration
            > m_MaximumIterationsWithoutProgress )
    {
    ParametersType backStep;
    backStep = m_BestPosition - this->m_Metric->GetParameters();
    this->m_Metric->UpdateTransformParameters( backStep );
    this->m_CurrentPosition = m_BestPosition;
    this->m_Value = m_BestValue;

    m_StopCondition = STEP_TOO_SMALL;
    m_StopConditionDescription << "Optimization stops after "
                               << this->GetCurrentIteration()
                               << " iterations since"
                               << " there is no progress in the last "
                               << m_MaximumIterationsWithoutProgress
                               << " steps."
                               << " The best value is from Iteration "
                               << m_BestIteration << ".";
    this->StopOptimization();
    return;
    }

  /** Flag to use a Newton step or a gradient step */
  bool useNewtonStep = true;

  if (this->GetCurrentIteration() == 0)
    {
    useNewtonStep = false;
    }
  else
    {
    try
      {
      this->EstimateNewtonStep();
      }
    catch ( ExceptionObject &ex )
      {
      std::cout << "Warning: Exception in estimating Newton step - "
        << ex.GetDescription() << std::endl;
      useNewtonStep = false;
      }
    }

  /** Save for the next iteration */
  m_PreviousValue = this->GetValue();
  m_PreviousPosition = this->GetCurrentPosition();
  m_PreviousGradient = this->GetGradient();

  if (useNewtonStep)
    {
    /**
     * If a Newton step is on the opposite direction of a gradient step,
     * we will use the gradient step. This happens when the second order
     * approximation produces a convex when a concave is expected, or the
     * other way around.
     */
    double gradientNewtonProduct = inner_product(m_Gradient, m_NewtonStep);
    if (gradientNewtonProduct < 0)
      {
      useNewtonStep = false;
      }
    }

  if (!useNewtonStep)
    {
    this->ResetHessian();

    // Using gradient step
    std::cout << "Using Gradient" << std::endl;

    this->ModifyGradient();
    this->m_Metric->UpdateTransformParameters( this->m_Gradient );
    }
  else
    {
    // Using Newton step
    std::cout << "Using Newton" << std::endl;

    this->ModifyNewtonStep();
    this->m_Metric->UpdateTransformParameters( this->m_NewtonStep);
    }

  this->InvokeEvent( IterationEvent() );
}
/**
 * Modify the gradient over a given index range.
 */
void
QuasiNewtonObjectOptimizer
::ModifyNewtonStep()
{
  /* Estimate the learning rate */
  this->InvokeEvent( NewtonStepLearningRateEvent() );

  this->m_NewtonStep *= this->m_LearningRate;
}

void QuasiNewtonObjectOptimizer
::ResetHessian()
{
  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();
  // Initialize Hessian to identity matrix
  m_Hessian.Fill(NumericTraits<InternalComputationValueType>::Zero);
  m_HessianInverse.Fill(NumericTraits<InternalComputationValueType>::Zero);

  for (unsigned int i=0; i<numPara; i++)
    {
    m_Hessian[i][i] = NumericTraits<InternalComputationValueType>::One; //identity matrix
    m_HessianInverse[i][i] = NumericTraits<InternalComputationValueType>::One; //identity matrix
    }
}

/** Estimate the quasi-newton step */
void QuasiNewtonObjectOptimizer
::EstimateNewtonStep()
{
  // Estimate Hessian
  this->EstimateHessian();

  this->m_NewtonStep = m_HessianInverse * m_Gradient;
}

/**
 * Estimate Hessian matrix with BFGS method described
 * at http://en.wikipedia.org/wiki/BFGS_method
 */
void QuasiNewtonObjectOptimizer
::EstimateHessian()
{
  if (this->GetCurrentIteration() == 0)
    {
    itkExceptionMacro("Please use the gradient step for the first iteration.");
    }

  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();

  ParametersType dx(numPara);  //delta of position x: x_k+1 - x_k
  ParametersType dg(numPara);  //delta of gradient: g_k+1 - g_k
  ParametersType edg(numPara); //estimated delta of gradient: hessian_k * dx

  dx = this->m_CurrentPosition - this->m_PreviousPosition;
  //dg = this->m_Gradient - this->m_PreviousGradient; //minimize
  dg = this->m_PreviousGradient - this->m_Gradient; //maximize
  edg = m_Hessian * dx;

  double dot_dg_dx = inner_product(dg, dx);
  double dot_edg_dx = inner_product(edg, dx);

  if (vcl_abs(dot_dg_dx) <= NumericTraits<InternalComputationValueType>::epsilon()
    || vcl_abs(dot_edg_dx) <= NumericTraits<InternalComputationValueType>::epsilon())
    {
    itkExceptionMacro("A denominator is too small in Hessian estimation.");
    return;
    }

  vnl_matrix<double> plus  = outer_product(dg, dg) / dot_dg_dx;
  vnl_matrix<double> minus = outer_product(edg, edg) / dot_edg_dx;
  vnl_matrix<double> newHessian = m_Hessian + plus - minus;

  m_Hessian         = newHessian;

  if ( vcl_abs(vnl_determinant(newHessian)) == 0 )
    {
    itkExceptionMacro("The Hessian matrix is almost singular.");
    }
  else
    {
    m_HessianInverse = vnl_matrix_inverse<double>(newHessian);
    }
}

} // end namespace itk
