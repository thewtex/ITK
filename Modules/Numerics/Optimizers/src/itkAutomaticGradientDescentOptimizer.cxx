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
#ifndef _itkAutomaticGradientDescentOptimizer_txx
#define _itkAutomaticGradientDescentOptimizer_txx

#include "itkAutomaticGradientDescentOptimizer.h"
#include "itkCommand.h"
#include "itkEventObject.h"
#include "itkMacro.h"

namespace itk
{
/**
 * Constructor
 */
AutomaticGradientDescentOptimizer
::AutomaticGradientDescentOptimizer()
{
  itkDebugMacro("Constructor");

  m_MaximumVoxelShift = 1.0;
}

void
AutomaticGradientDescentOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "m_MaximumVoxelShift: "
     << m_MaximumVoxelShift << std::endl;
  os << indent << "OptimizerHelper: "
     << m_OptimizerHelper << std::endl;
}

/**
 * Start the optimization
 */
void
AutomaticGradientDescentOptimizer
::StartOptimization(void)
{
  itkDebugMacro("StartOptimization");

  // initialize scales
  ScalesType scales(this->GetInitialPosition().Size());
  m_OptimizerHelper->EstimateScales(this->GetInitialPosition(), scales);
  this->SetScales(scales);

  this->Superclass::StartOptimization();

}

/**
 * Advance one Step following the gradient direction
 */
void
AutomaticGradientDescentOptimizer
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");

  // initialize learningRate
  if (this->GetCurrentIteration() == 0)
    {
    double learningRate;
    learningRate = this->EstimateLearningRate();
    this->SetLearningRate( learningRate );
    }

  this->Superclass::AdvanceOneStep();

}

/** Compute learning late from voxel shift*/
double AutomaticGradientDescentOptimizer
::EstimateLearningRate() const
{
  ParametersType parameters = this->GetCurrentPosition();
  int            numPara    = parameters.size();

  DerivativeType step   = this->GetGradient();
  ScalesType     scales = this->GetScales();

  ParametersType deltaParameters(numPara);
  for (int i=0; i<numPara; i++)
    {
      deltaParameters[i] = step[i] / scales[i];
    }

  double shift, learningRate;

  shift = m_OptimizerHelper->ComputeMaximumVoxelShift(parameters, deltaParameters);
  learningRate = m_MaximumVoxelShift / shift;

  return learningRate;
}

} // end namespace itk

#endif
