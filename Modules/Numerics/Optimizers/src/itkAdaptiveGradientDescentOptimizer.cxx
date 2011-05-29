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
#ifndef _itkAdaptiveGradientDescentOptimizer_txx
#define _itkAdaptiveGradientDescentOptimizer_txx

#include "itkAdaptiveGradientDescentOptimizer.h"
#include "itkCommand.h"
#include "itkEventObject.h"
#include "itkMacro.h"

namespace itk
{
/**
 * Constructor
 */
AdaptiveGradientDescentOptimizer
::AdaptiveGradientDescentOptimizer()
{
  itkDebugMacro("Constructor");

}

void
AdaptiveGradientDescentOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "OptimizerHelper: "
     << m_OptimizerHelper << std::endl;
}

/**
 * Advance one Step following the gradient direction
 */
void
AdaptiveGradientDescentOptimizer
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");

  ScalesType scales(this->GetScales().size());
  if (this->GetCurrentIteration() == 0)
    {
    m_OptimizerHelper->EstimateScales(this->GetCurrentPosition(),
                                      scales);
    this->SetScales(scales);
    }

  double learningRate;
  m_OptimizerHelper->EstimateLearningRate(this->GetCurrentPosition(),
                                          this->GetGradient(),
                                          this->GetScales(),
                                          learningRate);
  this->SetLearningRate(learningRate);

  Superclass::AdvanceOneStep();
}

} // end namespace itk

#endif
