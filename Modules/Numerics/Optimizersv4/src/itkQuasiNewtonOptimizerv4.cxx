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
#include "itkQuasiNewtonOptimizerv4.h"

namespace itk
{

QuasiNewtonOptimizerv4
::QuasiNewtonOptimizerv4()
{
  this->m_MaximumIterationsWithoutProgress = 30;
  this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;

  // m_MaximumNewtonStepSizeInPhysicalUnits is used for automatic learning
  // rate estimation. it may be initialized either by calling
  // SetMaximumNewtonStepSizeInPhysicalUnits manually or by using m_ScalesEstimator
  // automatically. and the former has higher priority than the latter.
  this->m_MaximumNewtonStepSizeInPhysicalUnits = NumericTraits<InternalComputationValueType>::Zero;

  /** Threader for Quasi-Newton method */
  this->m_EstimateNewtonStepThreader = QuasiNewtonOptimizerv4EstimateNewtonStepThreader::New();
}

QuasiNewtonOptimizerv4
::~QuasiNewtonOptimizerv4()
{
}

void
QuasiNewtonOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

void
QuasiNewtonOptimizerv4
::StartOptimization( bool doOnlyInitialization )
{
  itkDebugMacro("StartOptimization");

  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();
  const SizeValueType numLocalPara = this->m_Metric->GetAggregateNumberOfLocalParameters();
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
    if ( this->m_MaximumNewtonStepSizeInPhysicalUnits <=
      NumericTraits<InternalComputationValueType>::epsilon())
      {
      // Newton step size might be bigger than one voxel spacing.
      // emperically, we set it to 1~5 voxel spacings.
      this->m_MaximumNewtonStepSizeInPhysicalUnits =
        3.0 * this->m_ScalesEstimator->EstimateMaximumStepSize();
      }
    }

  /* Must call the superclass version for basic validation, setup,
   * and to start the optimization loop. */
  Superclass::StartOptimization( doOnlyInitialization );
}

void
QuasiNewtonOptimizerv4
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");
  const SizeValueType numPara = this->m_Metric->GetNumberOfParameters();
  this->m_CurrentPosition = this->m_Metric->GetParameters();

  if (this->GetCurrentIteration() == 0)
    {
    // initialize some information
    m_PreviousValue = this->GetCurrentMetricValue();
    m_PreviousPosition = this->GetCurrentPosition();
    m_PreviousGradient = this->GetGradient();

    m_BestValue = this->m_CurrentMetricValue;
    m_BestPosition = this->m_CurrentPosition;
    m_BestIteration = this->GetCurrentIteration();
    }
  else if (m_BestValue > this->m_CurrentMetricValue)
    {
    // store the best value and related information
    m_BestValue = this->m_CurrentMetricValue;
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
    this->m_CurrentMetricValue = this->m_BestValue;

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
  m_PreviousValue = this->GetCurrentMetricValue();
  m_PreviousPosition = this->GetCurrentPosition();
  m_PreviousGradient = this->GetGradient();

  this->CombineGradientNewtonStep();
  this->ModifyCombinedNewtonStep();

  try
    {
    /* Pass graident to transform and let it do its own updating */
    this->m_Metric->UpdateTransformParameters( this->m_NewtonStep );
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

void
QuasiNewtonOptimizerv4
::CombineGradientNewtonStep()
{
  const SizeValueType numLocalPara = this->m_Metric->GetAggregateNumberOfLocalParameters();
  const SizeValueType numLocals = this->m_Metric->GetNumberOfParameters() / numLocalPara;

  bool validNewtonStepExists = false;
  for (SizeValueType loc=0; loc<numLocals; loc++)
    {
    if (this->m_NewtonStepValidFlags[loc])
      {
      validNewtonStepExists = true;
      break;
      }
    }

  InternalComputationValueType ratio = NumericTraits<InternalComputationValueType>::One;
  if (validNewtonStepExists)
    {
    InternalComputationValueType gradStepScale
      = this->m_ScalesEstimator->EstimateStepScale(this->m_Gradient);
    InternalComputationValueType newtonStepScale
      = this->m_ScalesEstimator->EstimateStepScale(this->m_NewtonStep);
    if (gradStepScale > NumericTraits<InternalComputationValueType>::epsilon())
      {
      ratio = newtonStepScale / gradStepScale;
      }
    }

  for (SizeValueType loc=0; loc<numLocals; loc++)
    {
    if (!this->m_NewtonStepValidFlags[loc])
      {
      // Using the Gradient step
      IndexValueType offset = loc * numLocalPara;
      for (SizeValueType p=0; p<numLocalPara; p++)
        {
        this->m_NewtonStep[offset+p] = this->m_Gradient[offset+p] * ratio;
        }
      }
    }
}

void QuasiNewtonOptimizerv4
::ModifyCombinedNewtonStep()
{
  // If m_ScalesEstimator is not set, we will not change the Newton step.
  if (this->m_ScalesEstimator.IsNull())
    {
    return;
    }

  InternalComputationValueType stepScale
    = this->m_ScalesEstimator->EstimateStepScale(this->m_NewtonStep);

  if (stepScale <= NumericTraits<InternalComputationValueType>::epsilon())
    {
    this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
    }
  else
    {
    this->m_LearningRate = this->m_MaximumNewtonStepSizeInPhysicalUnits / stepScale;
    if (this->m_LearningRate > NumericTraits<InternalComputationValueType>::One)
      {
      // learning rate is at most 1 for a newton step
      this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
      }
    }

  if (vcl_abs(this->m_LearningRate - NumericTraits<InternalComputationValueType>::One)
      > 0.01)
    {
    this->m_NewtonStep *= this->m_LearningRate;
    }
}

void QuasiNewtonOptimizerv4
::ResetNewtonStep(IndexValueType loc)
{
  const SizeValueType numLocalPara = this->m_Metric->GetAggregateNumberOfLocalParameters();

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

void
QuasiNewtonOptimizerv4
::EstimateNewtonStep()
{
  IndexRangeType fullrange;
  fullrange[0] = 0;
  fullrange[1] = this->m_Gradient.GetSize()-1; //range is inclusive
  /* Perform the modification either with or without threading */

  if( this->m_Gradient.GetSize() > 10000 )
    {
    /* This ends up calling EstimateNewtonStepOverSubRange from each thread */
    this->m_EstimateNewtonStepThreader->Execute( this, fullrange );
    }
  else
    {
    this->EstimateNewtonStepOverSubRange( fullrange );
    }
}

void QuasiNewtonOptimizerv4
::EstimateNewtonStepOverSubRange( const IndexRangeType& subrange )
{
  const SizeValueType numLocalPara = this->m_Metric->GetAggregateNumberOfLocalParameters();

  IndexValueType low = subrange[0] / numLocalPara;
  IndexValueType high = subrange[1] / numLocalPara;

  // let us denote the i-th thread's sub range by subrange_i
  // we assume subrange_i[1] + 1 = subrange_(i+1)[0] .
  // if the subrange_i doesn't start with the multiple of numLocalPara,
  // we assign this starting block of local parameters to thread_i
  // if the subrange_i doesn't end with the multiple of numLocalPara,
  // we assign this ending block of local parameters to thread_(i+1) .
  if( (subrange[1]+1) % numLocalPara != 0 )
    {
    high--;
    }

  for (IndexValueType loc = low; loc <= high; loc++)
    {
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

bool QuasiNewtonOptimizerv4
::ComputeHessianAndStepWithBFGS(IndexValueType loc)
{
  if (this->GetCurrentIteration() == 0)
    {
    return false;
    }

  const SizeValueType numLocalPara = this->m_Metric->GetAggregateNumberOfLocalParameters();
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
