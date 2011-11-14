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
  this->m_MaximumIterationsWithoutProgress = 10;
  this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;

  // m_MaximumNewtonStepSizeInPhysicalSpaceUnits is used for automatic learning
  // rate estimation. it may be initialized either by calling
  // SetMaximumNewtonStepSizeInPhysicalSpaceUnits manually or by using m_ScalesEstimator
  // automatically. and the former has higher priority than the latter.
  this->m_MaximumNewtonStepSizeInPhysicalSpaceUnits = NumericTraits<InternalComputationValueType>::Zero;

  /** Threader for Quasi-Newton method */
  this->m_EstimateNewtonStepThreader = QuasiNewtonThreaderType::New();
  this->m_EstimateNewtonStepThreader->SetThreadedGenerateData(
                                                Self::EstimateNewtonStepThreaded );
  this->m_EstimateNewtonStepThreader->SetHolder( this );

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
  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();
  const SizeValueType numLocals = this->m_Metric->GetNumberOfParameters() / numLocalPara;

  /* Set up the data for Quasi-Newton method */
  this->m_NewtonStep.SetSize(numPara);

  this->m_NewtonStepValidFlags.resize(numLocals);

  this->m_HessianArray.resize(numLocals);
  for (SizeValueType loc=0; loc<numLocals; loc++)
    {
    this->m_HessianArray[loc].SetSize(numLocalPara, numLocalPara);
    }

  if ( this->m_ScalesEstimator.IsNotNull() )
    {
    if ( this->m_MaximumNewtonStepSizeInPhysicalSpaceUnits <=
      NumericTraits<InternalComputationValueType>::epsilon())
      {
      // Newton step size might be bigger than one voxel spacing.
      // emperically, we set it to 5 voxel spacings.
      this->m_MaximumNewtonStepSizeInPhysicalSpaceUnits =
        3.0 * this->m_ScalesEstimator->EstimateMaximumStepSize();
      }
    }

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
 */
void
QuasiNewtonObjectOptimizer
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");
  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();
  this->m_CurrentPosition = this->m_Metric->GetParameters();

  if (this->GetCurrentIteration() == 0)
    {
    // initialize some information
    m_PreviousValue = this->GetValue();
    m_PreviousPosition = this->GetCurrentPosition();
    m_PreviousGradient = this->GetGradient();

    m_BestValue = this->m_Value;
    m_BestPosition = this->m_CurrentPosition;
    m_BestIteration = this->GetCurrentIteration();
    }
  else if (m_BestValue > this->m_Value)
    {
    // store the best value and related information
    m_BestValue = this->m_Value;
    m_BestPosition = this->m_CurrentPosition;
    m_BestIteration = this->GetCurrentIteration();
    }

  if ( this->GetCurrentIteration() - m_BestIteration
            > m_MaximumIterationsWithoutProgress )
    {
    ParametersType backStep;
    backStep = m_BestPosition - this->m_Metric->GetParameters();
    this->m_Metric->UpdateTransformParameters( backStep );

    this->m_CurrentPosition = this->m_BestPosition;
    this->m_Value = this->m_BestValue;

    m_StopCondition = STEP_TOO_SMALL;
    m_StopConditionDescription << "Optimization stops after "
                               << this->GetCurrentIteration()
                               << " iterations since"
                               << " there is no progress in the last "
                               << m_MaximumIterationsWithoutProgress
                               << " steps." << std::endl
                               << " The best value is from Iteration "
                               << m_BestIteration << ".";
    this->StopOptimization();
    return;
    }

  if (this->GetCurrentIteration() > 0)
    {
    ParametersType lastStep(numPara);
    lastStep = this->m_CurrentPosition - this->m_PreviousPosition;
    if (lastStep.squared_magnitude() <
      NumericTraits<InternalComputationValueType>::epsilon())
      {
      m_StopCondition = STEP_TOO_SMALL;
      m_StopConditionDescription << "Optimization stops after "
                                 << this->GetCurrentIteration()
                                 << " iterations since"
                                 << " the last step is almost zero.";
      this->StopOptimization();
      return;
      }
    }

  this->EstimateNewtonStep();

  /** Save for the next iteration */
  m_PreviousValue = this->GetValue();
  m_PreviousPosition = this->GetCurrentPosition();
  m_PreviousGradient = this->GetGradient();

  this->ModifyGradient();
  this->ModifyNewtonStep();

  this->CombineGradientNewtonStep();

  try
    {
    /* Pass graident to transform and let it do its own updating */
    this->m_Metric->UpdateTransformParameters( this->m_Gradient );
    }
  catch ( ExceptionObject & err )
    {
    this->m_StopCondition = UPDATE_PARAMETERS_ERROR;
    this->m_StopConditionDescription << "UpdateTransformParameters error";
    this->StopOptimization();

    // Pass exception to caller
    throw err;
    }

  this->InvokeEvent( IterationEvent() );
}

/**
 * Modify the gradient over a given index range.
 */
void
QuasiNewtonObjectOptimizer
::CombineGradientNewtonStep()
{
  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();
  const SizeValueType numLocals = this->m_Metric->GetNumberOfParameters() / numLocalPara;

  for (SizeValueType loc=0; loc<numLocals; loc++)
    {
    if (this->m_NewtonStepValidFlags[loc])
      {
      // Using Newton step
      IndexValueType offset = loc * numLocalPara;
      for (SizeValueType p=0; p<numLocalPara; p++)
        {
        this->m_Gradient[offset+p] = this->m_NewtonStep[offset+p];
        }
      }
    }
}

void QuasiNewtonObjectOptimizer
::ModifyNewtonStep()
{
  if (this->m_ScalesEstimator.IsNull())
    {
    return;
    }

  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();
  const SizeValueType numLocals = this->m_Metric->GetNumberOfParameters() / numLocalPara;

  if (!this->m_Metric->HasLocalSupport())
    {
    InternalComputationValueType stepScale
      = this->m_ScalesEstimator->EstimateStepScale(this->m_NewtonStep);

    if (stepScale <= NumericTraits<InternalComputationValueType>::epsilon())
      {
      this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
      }
    else
      {
      this->m_LearningRate = this->m_MaximumNewtonStepSizeInPhysicalSpaceUnits / stepScale;
      if (this->m_LearningRate > NumericTraits<InternalComputationValueType>::One)
        {
        // learning rate is at most 1 for a newton step
        this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
        }
      }
    this->m_NewtonStep *= this->m_LearningRate;
    }
  else
    {
    ScalesType localStepScales;
    this->m_ScalesEstimator->EstimateLocalStepScales(this->m_NewtonStep, localStepScales);
    InternalComputationValueType localLearningRate;

    for (SizeValueType loc=0; loc<numLocals; loc++)
      {
      if (this->m_NewtonStepValidFlags[loc])
        {
        if (localStepScales[loc] <= NumericTraits<InternalComputationValueType>::epsilon())
          {
          localLearningRate = NumericTraits<InternalComputationValueType>::One;
          }
        else
          {
          localLearningRate = this->m_MaximumNewtonStepSizeInPhysicalSpaceUnits / localStepScales[loc];
          if (localLearningRate > NumericTraits<InternalComputationValueType>::One)
            {
            // learning rate is at most 1 for a newton step
            localLearningRate = NumericTraits<InternalComputationValueType>::One;
            }
          }

        // Apply the local learning rate
        for (SizeValueType p=0; p<numLocalPara; p++)
          {
          this->m_NewtonStep[loc*numLocalPara+p] *= localLearningRate;
          }
        }
      } // for loc

    }
}

void QuasiNewtonObjectOptimizer
::ResetNewtonStep(IndexValueType loc)
{
  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();

  // Initialize Hessian to identity matrix
  m_HessianArray[loc].Fill(NumericTraits<InternalComputationValueType>::Zero);

  for (unsigned int i=0; i<numLocalPara; i++)
    {
    m_HessianArray[loc][i][i] = NumericTraits<InternalComputationValueType>::One; //identity matrix
    }

  IndexValueType offset = loc * numLocalPara;
  for (SizeValueType p=0; p<numLocalPara; p++)
    {
    // Set to zero for invalid Newton steps.
    // They must be defined since they will be used during step scale estimation.
    this->m_NewtonStep[offset+p] = NumericTraits<InternalComputationValueType>::Zero;
    }
}

/**
 * Estimate the quasi-newton step.
 */
void
QuasiNewtonObjectOptimizer
::EstimateNewtonStep()
{
  IndexRangeType fullrange;
  fullrange[0] = 0;
  fullrange[1] = this->m_Gradient.GetSize()-1; //range is inclusive
  /* Perform the modification either with or without threading */
  if( this->m_Metric->HasLocalSupport() )
    {
    this->m_EstimateNewtonStepThreader->SetOverallIndexRange( fullrange );
    /* This ends up calling EstimateNewtonStepThreaded from each thread */
    this->m_EstimateNewtonStepThreader->StartThreadedExecution();
    }
  else
    {
    this->EstimateNewtonStepOverSubRange( fullrange );
    }
}

/**
 * Estimate the quasi-newton step in a thread.
 */
void
QuasiNewtonObjectOptimizer
::EstimateNewtonStepThreaded( const IndexRangeType& rangeForThread,
                          ThreadIdType,
                          Self *holder )
{
  holder->EstimateNewtonStepOverSubRange( rangeForThread );
}

/**
 * Estimate the quasi-newton step over a given index range.
 */
void QuasiNewtonObjectOptimizer
::EstimateNewtonStepOverSubRange( const IndexRangeType& subrange )
{
  // Three cases for m_NewtonStepIsValid = false:
  // 1. at the first iteration
  // 2. exception happened during Newton step estimation
  // 3. the Newton step is on the opposite direction of the gradient step

  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();

  IndexValueType low = subrange[0] / numLocalPara;
  IndexValueType high = subrange[1] / numLocalPara;

  for (IndexValueType loc = low; loc <= high; loc++)
    {

    this->m_NewtonStepValidFlags[loc] = true;

    if (this->GetCurrentIteration() == 0)
      {
      this->m_NewtonStepValidFlags[loc] = false;
      }
    else
      {
      this->m_NewtonStepValidFlags[loc] = this->ComputeHessianAndStepWithBFGS(loc);
      }

    if (!this->m_NewtonStepValidFlags[loc])
      {
      this->ResetNewtonStep(loc);
      }

    } // end for loc
}
/**
 * Estimate Hessian matrix with BFGS method described
 * at http://en.wikipedia.org/wiki/BFGS_method
 */
bool QuasiNewtonObjectOptimizer
::ComputeHessianAndStepWithBFGS(IndexValueType loc)
{
  if (this->GetCurrentIteration() == 0)
    {
    return false;
    }

  const SizeValueType numLocalPara = this->m_Metric->GetNumberOfLocalParameters();
  IndexValueType offset = loc * numLocalPara;

  ParametersType dx(numLocalPara);  //delta of position x: x_k+1 - x_k
  DerivativeType dg(numLocalPara);  //delta of gradient: g_k+1 - g_k
  DerivativeType edg(numLocalPara); //estimated delta of gradient: hessian_k * dx

  for (SizeValueType p=0; p<numLocalPara; p++)
    {
    dx[p] = this->m_CurrentPosition[offset+p] - this->m_PreviousPosition[offset+p];
    // gradient is already negated
    dg[p] = this->m_PreviousGradient[offset+p] - this->m_Gradient[offset+p];
    }

  edg = this->m_HessianArray[loc] * dx;

  double dot_dg_dx = inner_product(dg, dx);
  double dot_edg_dx = inner_product(edg, dx);

  if (vcl_abs(dot_dg_dx) <= NumericTraits<InternalComputationValueType>::epsilon()
    || vcl_abs(dot_edg_dx) <= NumericTraits<InternalComputationValueType>::epsilon())
    {
    return false;
    }

  vnl_matrix<double> plus  = outer_product(dg, dg) / dot_dg_dx;
  vnl_matrix<double> minus = outer_product(edg, edg) / dot_edg_dx;
  vnl_matrix<double> newHessian = this->m_HessianArray[loc] + plus - minus;

  this->m_HessianArray[loc] = newHessian;

  for (SizeValueType p=0; p<numLocalPara; p++)
    {
    if (newHessian[p][p] < 0)
      {
      return false;
      }
    }

  InternalComputationValueType threshold
    = NumericTraits<InternalComputationValueType>::epsilon();

  if ( vcl_abs(vnl_determinant(newHessian)) <= threshold )
    {
    return false;
    }
  else
    {
    vnl_matrix<double> hessianInverse = vnl_matrix_inverse<double>(newHessian);
    DerivativeType gradient(numLocalPara);
    DerivativeType newtonStep(numLocalPara);
    for (SizeValueType p=0; p<numLocalPara; p++)
      {
      gradient[p] = this->m_Gradient[offset+p];
      }
    // gradient is already negated
    newtonStep = hessianInverse * gradient;

    for (SizeValueType p=0; p<numLocalPara; p++)
      {
      this->m_NewtonStep[offset+p] = newtonStep[p];
      }

    }

  return true;
}

} // end namespace itk
