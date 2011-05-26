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
#ifndef __itkMaximumRatioDecisionRule2_h
#define __itkMaximumRatioDecisionRule2_h

/**
 * The class MaximumRatioDecisionRule2 has been removed from ITK. It has
 * been replaced by MaximumRatioDecisionRule. This file allows for backward
 * compatibility.
 */

#ifndef __itkMaximumRatioDecisionRule_h
#warning "itk::Statistics::MaximumRatioDecisionRule2 has been deprecated. itk::Statistics::MaximumRatioDecisionRule has been substituted in its place. Please use itk::Statistics::MaximumRatioDecisionRule directly."
#endif

#include "itkMaximumRatioDecisionRule.h"

namespace itk
{
namespace Statistics
{
typedef MaximumRatioDecisionRule MaximumRatioDecisionRule2;
}
}

#endif
