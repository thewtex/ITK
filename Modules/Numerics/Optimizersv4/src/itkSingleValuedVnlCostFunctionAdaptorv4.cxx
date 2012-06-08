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
#include "itkSingleValuedVnlCostFunctionAdaptorv4.h"

namespace itk
{
/**  Constructor.  */
SingleValuedVnlCostFunctionAdaptorv4
::SingleValuedVnlCostFunctionAdaptorv4(unsigned int spaceDimension):
  vnl_cost_function(spaceDimension)
{
  m_ScalesInitialized = false;
  m_Reporter = Object::New();
  m_CachedValue = NumericTraits< MeasureType >::Zero;
  m_CachedDerivative.Fill(0);
}

/** Set current parameters scaling. */
void
SingleValuedVnlCostFunctionAdaptorv4
::SetScales(const ScalesType & scales)
{
  m_Scales = scales;
  m_ScalesInitialized = true;
}

/**  Delegate computation of the value to the ObjectMetric. */
SingleValuedVnlCostFunctionAdaptorv4::InternalMeasureType
SingleValuedVnlCostFunctionAdaptorv4
::f(const InternalParametersType & inparameters)
{
  if ( !m_ObjectMetric )
    {
    itkGenericExceptionMacro(
      << "Attempt to use a SingleValuedVnlCostFunctionAdaptorv4 without any Metric plugged in");
    }

  // Use scales if they are provided
  ParametersType parameters( inparameters.size() );
  if ( m_ScalesInitialized )
    {
    for ( unsigned int i = 0; i < parameters.size(); i++ )
      {
      parameters[i] = inparameters[i] / m_Scales[i];
      }
    }
  else
    {
    parameters.SetData( const_cast< double * >( inparameters.data_block() ) );
    }

  this->m_ObjectMetric->SetParameters( parameters );
  InternalMeasureType value = static_cast< InternalMeasureType >( m_ObjectMetric->GetValue() );

  // Notify observers. This is used for overcoming the limitaion of VNL
  // optimizers of not providing callbacks per iteration.
  m_CachedValue = value;
  this->ReportIteration( FunctionEvaluationIterationEvent() );

  return value;
}

/**  Delegate computation of the gradient to the ObjectMetric.  */
void
SingleValuedVnlCostFunctionAdaptorv4
::gradf(const InternalParametersType & inparameters, InternalDerivativeType & gradient)
{
  if ( !m_ObjectMetric )
    {
    itkGenericExceptionMacro("Attempt to use a SingleValuedVnlCostFunctionAdaptorv4 without any Metric plugged in");
    }

  // Use scales if they are provided
  ParametersType parameters( inparameters.size() );
  if ( m_ScalesInitialized )
    {
    for ( unsigned int i = 0; i < parameters.size(); i++ )
      {
      parameters[i] = inparameters[i] / m_Scales[i];
      }
    }
  else
    {
    parameters.SetData( const_cast< double * >( inparameters.data_block() ) );
    }

  this->m_ObjectMetric->SetParameters( parameters );
  this->m_ObjectMetric->GetDerivative( m_CachedDerivative );
  this->ConvertExternalToInternalGradient( m_CachedDerivative, gradient );

  // Notify observers. This is used for overcoming the limitaion of VNL
  // optimizers of not providing callbacks per iteration.
  // Note that m_CachedDerivative is already loaded in the GetDerivative()
  // above.
  this->ReportIteration( GradientEvaluationIterationEvent() );
}

/**  Delegate computation of value and gradient to the metric.     */
void
SingleValuedVnlCostFunctionAdaptorv4
::compute(const InternalParametersType & x, InternalMeasureType *fun, InternalDerivativeType *g)
{
  // delegate the computation to the ObjectMetric
  ParametersType parameters( x.size() );
  double         measure;

  if ( m_ScalesInitialized )
    {
    for ( unsigned int i = 0; i < parameters.size(); i++ )
      {
      parameters[i] = x[i] / m_Scales[i];
      }
    }
  else
    {
    parameters.SetData( const_cast< double * >( x.data_block() ) );
    }

  this->m_ObjectMetric->SetParameters( parameters );
  this->m_ObjectMetric->GetValueAndDerivative( measure, m_CachedDerivative );
  if ( g ) // sometimes Vnl doesn't pass a valid pointer
    {
    this->ConvertExternalToInternalGradient(m_CachedDerivative, *g);
    }
  if ( fun ) // paranoids have longer lives...
    {
    *fun = static_cast< InternalMeasureType >( measure );
    // Notify observers. This is used for overcoming the limitaion of VNL
    // optimizers of not providing callbacks per iteration.
    // Note that m_CachedDerivative is already loaded in the GetDerivative()
    // above.
    m_CachedValue = *fun;
    }
  this->ReportIteration( FunctionAndGradientEvaluationIterationEvent() );
}

/**  Convert external derviative measures into internal type  */
void
SingleValuedVnlCostFunctionAdaptorv4
::ConvertExternalToInternalGradient(const DerivativeType & input, InternalDerivativeType & output) const
{
  const unsigned int size = input.size();

  output = InternalDerivativeType(size);
  for ( unsigned int i = 0; i < size; i++ )
    {
     output[i] = input[i];

    if ( m_ScalesInitialized )
      {
      output[i] /= m_Scales[i];
      }
    }
}

/**  This method reports iterations events. It is intended to
 *   help monitoring the progress of the optimization process. */
void
SingleValuedVnlCostFunctionAdaptorv4
::ReportIteration(const EventObject & event) const
{
  this->m_Reporter->InvokeEvent(event);
}

/**  Connects a Command/Observer to the internal reporter class.
 *   This is useful for reporting iteration event to potential observers. */
unsigned long
SingleValuedVnlCostFunctionAdaptorv4
::AddObserver(const EventObject & event, Command *command) const
{
  return m_Reporter->AddObserver(event, command);
}

/**  Return the cached value of the cost function */
const SingleValuedVnlCostFunctionAdaptorv4::MeasureType &
SingleValuedVnlCostFunctionAdaptorv4
::GetCachedValue() const
{
  return m_CachedValue;
}

/**  Return the cached value of the cost function derivative */
const SingleValuedVnlCostFunctionAdaptorv4::DerivativeType &
SingleValuedVnlCostFunctionAdaptorv4
::GetCachedDerivative() const
{
  return m_CachedDerivative;
}

/**  Return the cached value of the parameters used for computing the function
  */
const SingleValuedVnlCostFunctionAdaptorv4::ParametersType &
SingleValuedVnlCostFunctionAdaptorv4
::GetCachedCurrentParameters() const
{
  return this->m_ObjectMetric->GetParameters();
}

} // end namespace itk
