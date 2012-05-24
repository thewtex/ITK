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

#include "itkGradientDescentLineSearchOptimizerv4.h"

namespace itk
{

/**
 * Default constructor
 */
GradientDescentLineSearchOptimizerv4
::GradientDescentLineSearchOptimizerv4()
{
  this->m_LowerLimit = 0;
  this->m_UpperLimit = 2;
  this->m_Phi = (1 + vcl_sqrt( 5 ) ) / 2;
  this->m_Resphi = 2 - this->m_Phi;
  this->m_Epsilon = 1.e-6;
}

/**
 * Destructor
 */
GradientDescentLineSearchOptimizerv4
::~GradientDescentLineSearchOptimizerv4()
{}


/**
 *PrintSelf
 */
void
GradientDescentLineSearchOptimizerv4
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/**
 * Advance one Step following the gradient direction
 */
void
GradientDescentLineSearchOptimizerv4
::AdvanceOneStep()
{
  itkDebugMacro("AdvanceOneStep");
  InternalComputationValueType baselearningrate = this->m_LearningRate;
  this->m_LearningRate = this->GoldenSectionSearch( this->m_LearningRate * this->m_LowerLimit ,
    this->m_LearningRate , this->m_LearningRate * this->m_UpperLimit  );

  /* Begin threaded gradient modification. Work is done in
   * ModifyGradientOverSubRange */
  this->ModifyGradient();

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

  /** reset to base learning rate */
  this->m_LearningRate = baselearningrate;

  this->InvokeEvent( IterationEvent() );
}


// a and c are the current bounds; the minimum is between them.
// b is a center point
// f(x) is some mathematical function elsewhere defined
// a corresponds to x1; b corresponds to x2; c corresponds to x3
// x corresponds to x4
GradientDescentLineSearchOptimizerv4::InternalComputationValueType
GradientDescentLineSearchOptimizerv4
::GoldenSectionSearch( InternalComputationValueType a, InternalComputationValueType b, InternalComputationValueType c )
{
  InternalComputationValueType x;
  if ( c - b > b - a )
    x = b + this->m_Resphi * ( c - b );
  else
    x = b - this->m_Resphi * ( b - a );
  if ( vcl_abs( c - a ) < this->m_Epsilon * ( vcl_abs( b ) + vcl_abs( x ) ) )
    return ( c + a ) / 2;

  // hold : learning rate , parameters , gradient
  InternalComputationValueType baselearningrate = this->m_LearningRate;
  DerivativeType basegradient( this->m_Gradient );
  ParametersType baseparameters( this->GetCurrentPosition() );

  InternalComputationValueType metricx;
  this->m_LearningRate = x;
  this->ModifyGradient();
  this->m_Metric->UpdateTransformParameters( this->m_Gradient );
  this->GetMetric()->GetValueAndDerivative( this->m_Value ,  this->m_Gradient  );
  metricx = this->m_Value;

  /** reset position of transform */
  this->m_Metric->SetParameters( baseparameters );
  this->m_Gradient = basegradient;
  InternalComputationValueType metricb;
  this->m_LearningRate = b;
  this->ModifyGradient();
  this->m_Metric->UpdateTransformParameters( this->m_Gradient );
  this->GetMetric()->GetValueAndDerivative( this->m_Value ,  this->m_Gradient );
  metricb = this->m_Value;

  /** reset position of transform and learning rate */
  this->m_Metric->SetParameters( baseparameters );
  this->m_Gradient = basegradient;
  this->m_LearningRate = baselearningrate;

  /** golden section */
  if (  metricx < metricb )
    {
    if (c - b > b - a) return this->GoldenSectionSearch( b, x, c );
    else return this->GoldenSectionSearch( a, x, b );
    }
  else
    {
    if ( c - b > b - a ) return this->GoldenSectionSearch( a, b, x );
    else return this->GoldenSectionSearch( x, b, c  );
    }
}

}//namespace itk
