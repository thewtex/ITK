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
#include "itkLBFGSOptimizerv4.h"

namespace itk
{
LBFGSOptimizerv4
::LBFGSOptimizerv4():
m_OptimizerInitialized(false),
m_VnlOptimizer(0),
m_Trace(false),
m_Verbose(false),
m_MaximumNumberOfFunctionEvaluations(2000),
m_GradientConvergenceTolerance(1e-5),
m_LineSearchAccuracy(0.9),
m_DefaultStepLength(1.0)
{
}

LBFGSOptimizerv4
::~LBFGSOptimizerv4()
{
  delete m_VnlOptimizer;
}

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

void
LBFGSOptimizerv4
::VerboseOn()
{
  if ( m_Verbose == true )
    {
    return;
    }

  m_Verbose = true;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_verbose(true);
    }

  this->Modified();
}

void
LBFGSOptimizerv4
::VerboseOff()
{
  if ( m_Verbose == false )
    {
    return;
    }

  m_Verbose = false;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_verbose(false);
    }

  this->Modified();
}

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
  m_VnlOptimizer->set_verbose( m_Verbose );
  m_VnlOptimizer->set_max_function_evals( static_cast< int >( m_MaximumNumberOfFunctionEvaluations ) );
  m_VnlOptimizer->set_g_tolerance( m_GradientConvergenceTolerance );
  m_VnlOptimizer->line_search_accuracy = m_LineSearchAccuracy;
  m_VnlOptimizer->default_step_length  = m_DefaultStepLength;
  // set for debugging
  //m_VnlOptimizer->set_check_derivatives( true );

  m_OptimizerInitialized = true;

  this->Modified();
}

void
LBFGSOptimizerv4
::StartOptimization(bool /* doOnlyInitialization */)
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
  // in the metric for efficiency.
  ParametersType parameters = this->m_Metric->GetParameters();

  if( parameters.size() == 0 )
    {
    itkExceptionMacro(<<"Optimizer parameters are not initialized.");
    }

  // Scale the initial parameters up if scales are defined.
  // This compensates for later scaling them down in "compute" function of
  // the cost function adaptor and at the end of this function.
  InternalParametersType vnlCompatibleParameters(parameters.size());
  const ScalesType & scales = this->GetScales();
  for ( unsigned int i = 0; i < parameters.size(); i++ )
    {
    vnlCompatibleParameters[i] = ( this->GetScalesAreIdentity() )
      ? parameters[i] : parameters[i] * scales[i];
    }

  // vnl optimizers return the solution by reference
  // in the variable provided as initial position.
  // Also note that v4 registration always minimizes because v4 metrics return the negate value
  // if the cost function should be maximized.
  m_VnlOptimizer->minimize( vnlCompatibleParameters );

  // Check if the output parameters are not null.
  if ( vnlCompatibleParameters.size() != parameters.size() )
    {
    itkExceptionMacro(<< "Error occurred in optimization. Optimized parameters and initial parameters are not the same size: "
                      << vnlCompatibleParameters.size() << " vs. " << parameters.size() );
    }

  // we scale the parameters down if scales are defined
  for ( unsigned int i = 0; i < vnlCompatibleParameters.size(); i++ )
    {
    parameters[i] = ( this->GetScalesAreIdentity() )
      ? vnlCompatibleParameters[i] : vnlCompatibleParameters[i] / scales[i];
    }

  this->m_Metric->SetParameters( parameters );

  this->InvokeEvent( EndEvent() );
}

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
