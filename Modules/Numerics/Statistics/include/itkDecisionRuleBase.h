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
#ifndef __itkDecisionRuleBase_h
#define __itkDecisionRuleBase_h

/**
 * The original class DecisionRuleBase has been removed from ITK. It
 * has been replaced by DecisionRule. This file allows for backward
 * compatibility.
 */

#ifndef __itkDecisionRule_h
#warning "itk::DecisionRuleBase has been deprecated. itk::Statistics::DecisionRule has been substituted in its place. Please use itk::Statistics::DecisionRule directly."
#endif

#include "itkDecisionRule.h"

namespace itk
{
typedef Statistics::DecisionRule DecisionRuleBase;
}

#endif
