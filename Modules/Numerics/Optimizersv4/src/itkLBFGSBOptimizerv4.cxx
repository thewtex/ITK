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
#include "itkLBFGSBOptimizerv4.h"

namespace itk
{
LBFGSBOptimizerv4
  ::LBFGSBOptimizerv4():
  //m_Trace(false), --> the same
  //m_OptimizerInitialized(false), --> the same
  m_CostFunctionConvergenceFactor(1e+7),
  //m_ProjectedGradientTolerance(1e-5), --> m_GradientConvergenceTolerance
  m_MaximumNumberOfIterations(500),
  //m_MaximumNumberOfEvaluations(500), --> m_MaximumNumberOfFunctionEvaluations
  m_MaximumNumberOfCorrections(5),
  m_CurrentIteration(0),
  m_InfinityNormOfProjectedGradient(0.0),
  m_InitialPosition(0),
  m_VnlOptimizer(0)
{
  m_LowerBound       = InternalBoundValueType(0);
  m_UpperBound       = InternalBoundValueType(0);
  m_BoundSelection   = InternalBoundSelectionType(0);
}

/**
 * Destructor
 */
LBFGSBOptimizerv4
::~LBFGSBOptimizerv4()
{
  delete m_VnlOptimizer;
}

void
LBFGSBOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  os << indent << "Trace: ";
  if ( m_Trace )
    {
    os << "On";
    }
  else
    {
    os << "Off";
    }
  os << std::endl;
  os << indent << "InitialPosition: " << m_InitialPosition << std::endl;
  os << indent << "CurrentPosition: " << this->GetCurrentPosition() << std::endl;

  os << indent << "MaximumNumberOfFunctionEvaluations: "
  << m_MaximumNumberOfFunctionEvaluations << std::endl;
  os << indent << "GradientConvergenceTolerance: "
  << m_GradientConvergenceTolerance << std::endl;

  os << indent << "LowerBound: " << m_LowerBound << std::endl;
  os << indent << "UpperBound: " << m_UpperBound << std::endl;
  os << indent << "BoundSelection: " << m_BoundSelection << std::endl;

  os << indent << "CostFunctionConvergenceFactor: "
  << m_CostFunctionConvergenceFactor << std::endl;

  os << indent << "MaximumNumberOfIterations: "
  << m_MaximumNumberOfIterations << std::endl;

  os << indent << "MaximumNumberOfEvaluations: "
  << m_MaximumNumberOfFunctionEvaluations << std::endl;

  os << indent << "MaximumNumberOfCorrections: "
  << m_MaximumNumberOfCorrections << std::endl;

  os << indent << "CurrentIteration: "
  << m_CurrentIteration << std::endl;

  os << indent << "Value: "
  << this->GetValue() << std::endl;

  os << indent << "InfinityNormOfProjectedGradient: "
  << m_InfinityNormOfProjectedGradient << std::endl;

  if ( this->m_VnlOptimizer )
    {
    os << indent << "Vnl LBFGSB Failure Code: "
    << this->m_VnlOptimizer->get_failure_code() << std::endl;
    }
}

void
LBFGSBOptimizerv4
::SetInitialPosition(const ParametersType & param)
{
  m_InitialPosition = param;
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetLowerBound(
  const BoundValueType & value)
{
  this->m_LowerBound = value;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_lower_bound(m_LowerBound);
    }
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetUpperBound(
  const BoundValueType & value)
{
  this->m_UpperBound = value;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_upper_bound(m_UpperBound);
    }
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetBoundSelection(
  const BoundSelectionType & value)
{
  m_BoundSelection = value;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_bound_selection(m_BoundSelection);
    }
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetCostFunctionConvergenceFactor(double value)
{
  if ( value < 0.0 )
    {
    itkExceptionMacro("Value " << value
                      << " is too small for SetCostFunctionConvergenceFactor()"
                      << "a typical range would be from 0.0 to 1e+12");
    }
  m_CostFunctionConvergenceFactor = value;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_cost_function_convergence_factor( m_CostFunctionConvergenceFactor );
    }
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetMaximumNumberOfIterations(unsigned int value)
{
  m_MaximumNumberOfIterations = value;
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetMaximumNumberOfCorrections(unsigned int value)
{
  m_MaximumNumberOfCorrections = value;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_max_variable_metric_corrections( m_MaximumNumberOfCorrections );
    }
  this->Modified();
}

void
LBFGSBOptimizerv4
::SetMetric(MetricType *metric)
{
  this->m_Metric = metric;

  const unsigned int numberOfParameters = metric->GetNumberOfParameters();

  CostFunctionAdaptorType *adaptor = new CostFunctionAdaptorType( numberOfParameters );

  adaptor->SetCostFunction( metric );

  if ( m_OptimizerInitialized )
    {
    delete m_VnlOptimizer;
    }

  this->SetCostFunctionAdaptor( adaptor );

  m_VnlOptimizer = new vnl_lbfgsb( *adaptor );

  // set the optimizer parameters
  m_VnlOptimizer->set_trace( m_Trace );
  m_VnlOptimizer->set_lower_bound( m_LowerBound );
  m_VnlOptimizer->set_upper_bound( m_UpperBound );
  m_VnlOptimizer->set_bound_selection( m_BoundSelection );
  m_VnlOptimizer->set_cost_function_convergence_factor( m_CostFunctionConvergenceFactor );
  m_VnlOptimizer->set_projected_gradient_tolerance( m_GradientConvergenceTolerance );
  m_VnlOptimizer->set_max_function_evals( static_cast< int >( m_MaximumNumberOfFunctionEvaluations ) );
  m_VnlOptimizer->set_max_variable_metric_corrections( m_MaximumNumberOfCorrections );

  m_OptimizerInitialized = true;

  this->Modified();
}

void
LBFGSBOptimizerv4
::StartOptimization(bool /*doOnlyInitialization*/ )
{
  // Check if all the bounds parameters are the same size as the initial
  // parameters.
  unsigned int numberOfParameters = m_Metric->GetNumberOfParameters();

  if ( this->GetInitialPosition().Size() < numberOfParameters )
    {
    itkExceptionMacro(<< "InitialPosition array does not have sufficient number of elements");
    }

  if ( m_LowerBound.size() < numberOfParameters )
    {
    itkExceptionMacro(<< "LowerBound array does not have sufficient number of elements");
    }

  if ( m_UpperBound.size() < numberOfParameters )
    {
    itkExceptionMacro(<< "UppperBound array does not have sufficient number of elements");
    }

  if ( m_BoundSelection.size() < numberOfParameters )
    {
    itkExceptionMacro(<< "BoundSelection array does not have sufficient number of elements");
    }

  if(this->m_CostFunctionConvergenceFactor == 0.0 && this->m_GradientConvergenceTolerance == 0.0)
    {
    itkExceptionMacro("LBFGSB Optimizer cannot function if both CostFunctionConvergenceFactor"
                      " and ProjectedGradienctTolerance are zero.");
    }

  ParametersType parameters( this->GetInitialPosition() );

  this->InvokeEvent( StartEvent() );

    // vnl optimizers return the solution by reference
    // in the variable provided as initial position
  m_VnlOptimizer->minimize(parameters);

  if ( parameters.size() != this->GetInitialPosition().Size() )
    {
      // set current position to initial position and throw an exception
    this->m_Metric->SetParameters( this->GetInitialPosition() );
    itkExceptionMacro(<< "Error occurred in optimization");
    }

  this->m_Metric->SetParameters( parameters );

  this->InvokeEvent( EndEvent() );
}
} // end namespace itk
