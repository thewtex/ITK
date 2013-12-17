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
#ifndef __itkLBFGSOptimizerv4_hxx
#define __itkLBFGSOptimizerv4_hxx
#include "itkLBFGSOptimizerv4.h"

namespace itk
{
/**
 * Constructor
 */
LBFGSOptimizerv4
::LBFGSOptimizerv4()
{
  m_OptimizerInitialized    = false;
  m_VnlOptimizer            = 0;
  m_Trace                              = false;
  m_MaximumNumberOfFunctionEvaluations = 2000;
  m_GradientConvergenceTolerance       = 1e-5;
  m_LineSearchAccuracy                 = 0.9;
  m_DefaultStepLength                  = 1.0;
}

/**
 * Destructor
 */
LBFGSOptimizerv4
::~LBFGSOptimizerv4()
{
  delete m_VnlOptimizer;
}

/**
 * PrintSelf
 */
void
LBFGSOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
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
  os << indent << "MaximumNumberOfFunctionEvaluations: "
     << m_MaximumNumberOfFunctionEvaluations << std::endl;
  os << indent << "GradientConvergenceTolerance: "
     << m_GradientConvergenceTolerance << std::endl;
  os << indent << "LineSearchAccuracy: "
     << m_LineSearchAccuracy << std::endl;
  os << indent << "DefaultStepLength: "
     << m_DefaultStepLength << std::endl;
}

/**
 * Set the optimizer trace flag
 */
void
LBFGSOptimizerv4
::SetTrace(bool flag)
{
  if ( flag == m_Trace )
    {
    return;
    }

  m_Trace = flag;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_trace(m_Trace);
    }

  this->Modified();
}

/**
 * Set the maximum number of function evalutions
 */
void
LBFGSOptimizerv4
::SetMaximumNumberOfFunctionEvaluations(unsigned int n)
{
  if ( n == m_MaximumNumberOfFunctionEvaluations )
    {
    return;
    }

  m_MaximumNumberOfFunctionEvaluations = n;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_max_function_evals(
      static_cast< int >( m_MaximumNumberOfFunctionEvaluations ) );
    }

  this->Modified();
}

/**
 * Set the gradient convergence tolerance
 */
void
LBFGSOptimizerv4
::SetGradientConvergenceTolerance(double f)
{
  if ( f == m_GradientConvergenceTolerance )
    {
    return;
    }

  m_GradientConvergenceTolerance = f;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_g_tolerance(m_GradientConvergenceTolerance);
    }

  this->Modified();
}

/**
 * Set the line search accuracy
 */
void
LBFGSOptimizerv4
::SetLineSearchAccuracy(double f)
{
  if ( f == m_LineSearchAccuracy )
    {
    return;
    }

  m_LineSearchAccuracy = f;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->line_search_accuracy = m_LineSearchAccuracy;
    }

  this->Modified();
}

/**
 * Set the default step length
 */
void
LBFGSOptimizerv4
::SetDefaultStepLength(double f)
{
  if ( f == m_DefaultStepLength )
    {
    return;
    }

  m_DefaultStepLength = f;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->default_step_length = m_DefaultStepLength;
    }

  this->Modified();
}

/**
 * Connect a Cost Function
 */
void
LBFGSOptimizerv4
::SetMetric(MetricType *metric)
{
  // assign to base class
  this->m_Metric = metric;

  // assign to vnl cost-function adaptor
  const unsigned int numberOfParameters = metric->GetNumberOfParameters();

  CostFunctionAdaptorType *adaptor = new CostFunctionAdaptorType( numberOfParameters );

  adaptor->SetCostFunction( metric );

  if ( m_OptimizerInitialized )
    {
    delete m_VnlOptimizer;
    }

  this->SetCostFunctionAdaptor( adaptor );

  m_VnlOptimizer = new vnl_lbfgs( *adaptor );

  // set the optimizer parameters
  m_VnlOptimizer->set_trace( m_Trace );
  m_VnlOptimizer->set_max_function_evals( static_cast< int >( m_MaximumNumberOfFunctionEvaluations ) );
  m_VnlOptimizer->set_g_tolerance( m_GradientConvergenceTolerance );
  m_VnlOptimizer->line_search_accuracy = m_LineSearchAccuracy;
  m_VnlOptimizer->default_step_length  = m_DefaultStepLength;

  m_OptimizerInitialized = true;

  this->Modified();
}

/**
 * Start the optimization
 */
void
LBFGSOptimizerv4
::StartOptimization(bool /* doOnliInitialization */)
{
  // Check for a local-support transform.
  // These aren't currently supported, see main class documentation.
  if( this->GetMetric()->HasLocalSupport() )
    {
    itkExceptionMacro("The assigned transform has local-support. This is not supported for this optimizer. See the optimizer documentation.");
    }

  // Perform some verification, check scales,
  // pass settings to cost-function adaptor.
  Superclass::StartOptimization();

  this->InvokeEvent( StartEvent() );

  // Note that it's tempting to use a reference to the parameters stored
  // in the metric for efficiency, but this causes trouble with non-identity
  // optimizer scales. Might be resolvable if deemed worth while for better
  // efficicency when using a high-dimensional transform in the metric.
  ParametersType parameters = this->m_Metric->GetParameters();

  // Scale the initial parameters up if scales are defined.
  // This compensates for later scaling them down in the cost function adaptor
  // and at the end of this function.
  // (NOTE: This is done per original code in v3 version of this optimizer.
  //  I can't figure out why this is done.)
  if ( ! this->GetScalesAreIdentity() )
    {
    ScalesType scales = this->GetScales();
    for ( unsigned int i = 0; i < parameters.size(); i++ )
      {
//x      parameters[i] *= scales[i];
      }
    }

  // vnl optimizers return the solution by reference
  // in the variable provided as initial position.
  // v4 registration always minimizes
  m_VnlOptimizer->minimize( parameters );

  MetricType::NumberOfParametersType initialParametersSize = parameters.size();
  if ( parameters.size() != initialParametersSize )
    {
    itkExceptionMacro(<< "Error occurred in optimization. parameters and initialParametersSize are not the same size: "
                      << parameters.size() << " vs. " << initialParametersSize );
    }

  // we scale the parameters down if scales are defined
  if ( ! this->GetScalesAreIdentity() )
    {
    ScalesType scales = this->GetScales();
    for ( unsigned int i = 0; i < parameters.size(); i++ )
      {
//x      parameters[i] /= scales[i];
      }
    }

  this->m_Metric->SetParameters( parameters );

  this->InvokeEvent( EndEvent() );
}

/**
 * Get the Optimizer
 */
vnl_lbfgs *
LBFGSOptimizerv4
::GetOptimizer()
{
  return m_VnlOptimizer;
}

const std::string
LBFGSOptimizerv4
::GetStopConditionDescription() const
{
  m_StopConditionDescription.str("");
  m_StopConditionDescription << this->GetNameOfClass() << ": ";
  if ( m_VnlOptimizer )
    {
    switch ( m_VnlOptimizer->get_failure_code() )
      {
      case vnl_nonlinear_minimizer::ERROR_FAILURE:
        m_StopConditionDescription << "Failure";
        break;
      case vnl_nonlinear_minimizer::ERROR_DODGY_INPUT:
        m_StopConditionDescription << "Dodgy input";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_FTOL:
        m_StopConditionDescription << "Function tolerance reached";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_XTOL:
        m_StopConditionDescription << "Solution tolerance reached";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_XFTOL:
        m_StopConditionDescription << "Solution and Function tolerance both reached";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_GTOL:
        m_StopConditionDescription << "Gradient tolerance reached";
        break;
      case vnl_nonlinear_minimizer::FAILED_TOO_MANY_ITERATIONS:
        m_StopConditionDescription << "Too many function evaluations. Function evaluations  = "
                                   << m_MaximumNumberOfFunctionEvaluations;
        break;
      case vnl_nonlinear_minimizer::FAILED_FTOL_TOO_SMALL:
        m_StopConditionDescription << "Function tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_XTOL_TOO_SMALL:
        m_StopConditionDescription << "Solution tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_GTOL_TOO_SMALL:
        m_StopConditionDescription << "Gradient tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_USER_REQUEST:
        m_StopConditionDescription << "User requested";
        break;
      }
    return m_StopConditionDescription.str();
    }
  else
    {
    return std::string("");
    }
}
} // end namespace itk
#endif // __itkLBFGSOptimizerv4_hxx
