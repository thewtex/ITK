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
#include "itkGradientDescentObjectOptimizerBase.h"

namespace itk
{

//-------------------------------------------------------------------
GradientDescentObjectOptimizerBase
::GradientDescentObjectOptimizerBase()
{
  /** Threader for apply scales to gradient */
  this->m_ProcessGradientThreader = ProcessGradientThreaderType::New();
  this->m_ProcessGradientThreader->SetThreadedGenerateData(
                                                Self::ProcessGradientThreaded );
  this->m_ProcessGradientThreader->SetHolder( this );

  this->m_LearningRate = NumericTraits<InternalComputationValueType>::One;
  this->m_NumberOfIterations = static_cast<SizeValueType>(100);
  this->m_CurrentIteration = NumericTraits<SizeValueType>::Zero;
  this->m_StopCondition = MAXIMUM_NUMBER_OF_ITERATIONS;
  this->m_StopConditionDescription << this->GetNameOfClass() << ": ";
}

//-------------------------------------------------------------------
GradientDescentObjectOptimizerBase
::~GradientDescentObjectOptimizerBase()
{}

//-------------------------------------------------------------------
void
GradientDescentObjectOptimizerBase
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "ProcessGradientThreader: " << this->m_ProcessGradientThreader
               << std::endl;
  os << indent << "Learning rate:" << this->m_LearningRate << std::endl;
  os << indent << "Number of iterations: " << this->m_NumberOfIterations  << std::endl;
  os << indent << "Current iteration: " << this->m_CurrentIteration << std::endl;
  os << indent << "Stop condition:"<< this->m_StopCondition << std::endl;
  os << indent << "Stop condition description: " << this->m_StopConditionDescription.str()  << std::endl;
  if( this->m_ScalesEstimator.IsNotNull() )
    {
    os << indent << "ParameterScalesEstimator: " << this->m_ScalesEstimator << std::endl;
    }
  else
    {
    os << indent << "ParameterScalesEstimator: unset." << std::endl;
    }
}

//-------------------------------------------------------------------
const GradientDescentObjectOptimizerBase::StopConditionReturnStringType
GradientDescentObjectOptimizerBase
::GetStopConditionDescription() const
{
  return this->m_StopConditionDescription.str();
}

//-------------------------------------------------------------------
void
GradientDescentObjectOptimizerBase
::StopOptimization(void)
{
  itkDebugMacro("StopOptimization");
  this->m_Stop = true;
  this->InvokeEvent( EndEvent() );
}

void
GradientDescentObjectOptimizerBase
::StartOptimization(void)
{
  /* Estimate the parameter scales. Do this before calling
   * Superclass, which will validate scales. */
  if ( this->m_ScalesEstimator.IsNotNull() )
    {
    this->m_ScalesEstimator->EstimateScales(this->m_Scales);
    }

  Superclass::StartOptimization();

  this->m_CurrentIteration = NumericTraits<SizeValueType>::Zero;
  this->m_ProcessGradientThreader->SetNumberOfThreads(
    this->GetNumberOfThreads() );
}

//-------------------------------------------------------------------
void
GradientDescentObjectOptimizerBase
::ResumeOptimization()
{
  this->m_StopConditionDescription.str("");
  this->m_StopConditionDescription << this->GetNameOfClass() << ": ";
  this->InvokeEvent( StartEvent() );

  this->m_Stop = false;
  while( ! this->m_Stop )
    {
    /* Check iteration count */
    if ( this->m_CurrentIteration >= this->m_NumberOfIterations )
      {
      this->m_StopConditionDescription << "Maximum number of iterations ("
                                 << this->m_NumberOfIterations
                                 << ") exceeded.";
      this->m_StopCondition = MAXIMUM_NUMBER_OF_ITERATIONS;
      this->StopOptimization();
      break;
      }

    /* Compute metric value/derivative. */
    try
      {
      /* m_Gradient will be sized as needed by metric. If it's already
       * proper size, no new allocation is done. */
      this->m_Metric->GetValueAndDerivative( this->m_Value, this->m_Gradient );
      }
    catch ( ExceptionObject & err )
      {
      this->m_StopCondition = COSTFUNCTION_ERROR;
      this->m_StopConditionDescription << "Metric error during optimization";
      this->StopOptimization();

      // Pass exception to caller
      throw err;
      }

    /* Check if optimization has been stopped externally.
     * (Presumably this could happen from a multi-threaded client app?) */
    if( this->m_Stop )
      {
      this->m_StopConditionDescription << "StopOptimization() called";
      break;
      }

    /* Advance one step along the gradient.
     * This will process the gradient and update learning rate
     * when appropriate. */
    this->AdvanceOneStep();

    /* Check if optimization was stopped during the call to AdvanceOneStep */
    if( this->m_Stop )
      {
      break;
      }

    /* Set the learning rate to one if it's within tolerance. This
     * avoid unncessary computation in the case of automatically calculated
     * learning rate. */
    InternalComputationValueType one =
      NumericTraits<InternalComputationValueType>::One;
    InternalComputationValueType tolerance =
      static_cast<InternalComputationValueType>(0.01);
    if( vcl_fabs( this->m_LearningRate - one ) < tolerance )
      {
      this->m_LearningRate = one;
      }

    /* Pass gradient to transform and let it do its own updating.
     * m_Gradient is modified in-place by derived classes for memory
     * efficiency in the case of large transforms with local-support. */
    try
      {
      this->m_Metric->UpdateTransformParameters(
        this->m_Gradient, this->m_LearningRate );
      }
    catch ( ExceptionObject & err )
      {
      this->m_StopCondition = UPDATE_PARAMETERS_ERROR;
      this->m_StopConditionDescription << "UpdateTransformParameters error";
      this->StopOptimization();

      // Pass exception to caller
      throw err;
      }

    /* Update iteration count */
    this->m_CurrentIteration++;

    this->InvokeEvent( IterationEvent() );

    } //while (!m_Stop)
}

//-------------------------------------------------------------------
void
GradientDescentObjectOptimizerBase
::ProcessGradient()
{
  IndexRangeType fullrange;
  fullrange[0] = 0;
  fullrange[1] = this->m_Gradient.GetSize()-1; //range is inclusive
  /* Perform the modification either with or without threading */
  if( this->m_Metric->HasLocalSupport() )
    {
    this->m_ProcessGradientThreader->SetOverallIndexRange( fullrange );
    /* This ends up calling ProcessGradientThreaded from each thread */
    this->m_ProcessGradientThreader->StartThreadedExecution();
    }
  else
    {
    /* Global transforms are small, so update without threading. */
      this->ProcessGradientOverSubRange( fullrange, NumericTraits<ThreadIdType>::Zero );
    }
}

//-------------------------------------------------------------------
void
GradientDescentObjectOptimizerBase
::ProcessGradientThreaded( const IndexRangeType& rangeForThread,
                          ThreadIdType threadId,
                          Self *holder )
{
  holder->ProcessGradientOverSubRange( rangeForThread, threadId );
}

}//namespace itk
