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

#include "itkMultiGradientOptimizerv4.h"

namespace itk
{

//-------------------------------------------------------------------
MultiGradientOptimizerv4
::MultiGradientOptimizerv4()
{
  this->m_NumberOfIterations = static_cast<SizeValueType>(0);
  this->m_CurrentIteration   = static_cast<SizeValueType>(0);
  this->m_StopCondition      = MAXIMUM_NUMBER_OF_ITERATIONS;
  this->m_StopConditionDescription << this->GetNameOfClass() << ": ";

  this->m_MaximumMetricValue=NumericTraits<MeasureType>::max();
  this->m_MinimumMetricValue = this->m_MaximumMetricValue;
}

//-------------------------------------------------------------------
MultiGradientOptimizerv4
::~MultiGradientOptimizerv4()
{}

//-------------------------------------------------------------------
void
MultiGradientOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Number of iterations: " << this->m_NumberOfIterations  << std::endl;
  os << indent << "Current iteration: " << this->m_CurrentIteration << std::endl;
  os << indent << "Stop condition:"<< this->m_StopCondition << std::endl;
  os << indent << "Stop condition description: " << this->m_StopConditionDescription.str()  << std::endl;
}

//-------------------------------------------------------------------
MultiGradientOptimizerv4::OptimizersListType &
MultiGradientOptimizerv4
::GetOptimizersList()
{
  return this->m_OptimizersList;
}

/** Set the list of parameters over which to search */
void
MultiGradientOptimizerv4
::SetOptimizersList(MultiGradientOptimizerv4::OptimizersListType & p)
{
  if( p != this->m_OptimizersList )
    {
    this->m_OptimizersList = p;
    this->Modified();
    }
}

/** Get the list of metric values that we produced after the multi-start search.  */
const MultiGradientOptimizerv4::MetricValuesListType &
MultiGradientOptimizerv4
::GetMetricValuesList() const
{
  return this->m_MetricValuesList;
}

//-------------------------------------------------------------------
const MultiGradientOptimizerv4::StopConditionReturnStringType
MultiGradientOptimizerv4
::GetStopConditionDescription() const
{
  return this->m_StopConditionDescription.str();
}

//-------------------------------------------------------------------
void
MultiGradientOptimizerv4
::StopOptimization(void)
{
  itkDebugMacro("StopOptimization");
  std::cout << "StopOptimization called with a description - "
    << this->GetStopConditionDescription() << std::endl;
  this->m_Stop = true;

  // FIXME
  // this->m_Metric->SetParameters( this->m_OptimizersList[ this->m_BestParametersIndex ] );
  this->InvokeEvent( EndEvent() );
}

/**
 * Start and run the optimization
 */
void
MultiGradientOptimizerv4
::StartOptimization()
{
  itkDebugMacro("StartOptimization");
  unsigned int maxopt=this->m_OptimizersList.size();
  if ( maxopt == 0 )
    {
    std::cout <<" No optimizers are set."<<std::endl;
    exit(1);
    }
  if ( ! this->m_Metric ) this->m_Metric=this->m_OptimizersList[0]->GetMetric();
  unsigned long paramsize=this->GetCurrentPosition().GetSize();
  this->m_MetricValuesList.clear();
  this->m_MinimumMetricValue = this->m_MaximumMetricValue;
  for (unsigned int whichoptimizer=0; whichoptimizer<maxopt; whichoptimizer++)
    {
    this->m_MetricValuesList.push_back(this->m_MaximumMetricValue);
    unsigned long locsize=this->m_OptimizersList[whichoptimizer]->GetCurrentPosition().GetSize();
    if ( locsize != paramsize )
      {
      std::cout <<" Parameter sizes are not equivalent across all optimizers/metrics."<<std::endl;
      exit(1);
      }
    }

  /* Must call the superclass version for basic validation and setup */
  if ( this->m_NumberOfIterations > static_cast<SizeValueType>(0) )
    {
    Superclass::StartOptimization();
    }

  this->m_CurrentIteration = static_cast<SizeValueType>(0);

  if ( this->m_NumberOfIterations > static_cast<SizeValueType>(0) )
    {
    this->ResumeOptimization();
    }
}

/**
 * Resume optimization.
 */
void
MultiGradientOptimizerv4
::ResumeOptimization()
{
  bool debug=false;
  this->m_StopConditionDescription.str("");
  this->m_StopConditionDescription << this->GetNameOfClass() << ": ";
  this->InvokeEvent( StartEvent() );
  if (debug) std::cout << " start " << std::endl;
  this->m_Stop = false;
  while( ! this->m_Stop )
    {
    /* Compute metric value/derivative. */
    unsigned int maxopt=this->m_OptimizersList.size();
    /** we rely on learning rate or parameter scale estimator to do the weighting */
    double combinefunction=1.0/(double)maxopt;
    if (debug) std::cout << " nopt " << maxopt <<  std::endl;
    for (unsigned int whichoptimizer=0; whichoptimizer<maxopt; whichoptimizer++)
      {
      this->m_OptimizersList[whichoptimizer]->GetMetric()->GetValueAndDerivative(
        const_cast<double&>( this->m_OptimizersList[whichoptimizer]->GetValue()),
        const_cast<itk::Array<double>&>(this->m_OptimizersList[whichoptimizer]->GetGradient()) );
      if (debug) std::cout << " got-deriv " << whichoptimizer <<  std::endl;
      if ( this->m_Gradient.Size() != this->m_OptimizersList[whichoptimizer]->GetGradient().Size() )
        {
        this->m_Gradient.SetSize( this->m_OptimizersList[whichoptimizer]->GetGradient().Size() );
        if (debug) std::cout << " resized " << std::endl;
        }
      this->m_OptimizersList[whichoptimizer]->ModifyGradient();
      if (debug) std::cout << " mod-grad "  <<  std::endl;
      /** combine the gradients */
      if ( whichoptimizer == 0 ) this->m_Gradient.Fill(0);
      this->m_Gradient=this->m_Gradient+this->m_OptimizersList[whichoptimizer]->GetGradient()*combinefunction;
      if (debug) std::cout << " add-grad "  <<  std::endl;
      //this->m_MetricValuesList[whichoptimizer]=//FIXME --- need efficient implementation to get this going.
      }
      /* Check if optimization has been stopped externally.
      * (Presumably this could happen from a multi-threaded client app?) */
      if ( this->m_Stop )
      {
        this->m_StopConditionDescription << "StopOptimization() called";
        break;
      }
    try
      {
      /* Pass combined gradient to transforms and let them update */
      for (unsigned int whichoptimizer=0; whichoptimizer<maxopt; whichoptimizer++)
        {
        if (debug) std::cout << " combine-grad "  <<  std::endl;
        this->m_OptimizersList[whichoptimizer]->GetMetric()->UpdateTransformParameters( this->m_Gradient );
        }
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
    /* Update and check iteration count */
    this->m_CurrentIteration++;
    if ( this->m_CurrentIteration >= this->m_NumberOfIterations )
      {
      this->m_StopConditionDescription << "Maximum number of iterations ("
                                 << this->m_NumberOfIterations
                                 << ") exceeded.";
      this->m_StopCondition = MAXIMUM_NUMBER_OF_ITERATIONS;
      this->StopOptimization();
      break;
      }
    } //while (!m_Stop)

}

} //namespace itk
