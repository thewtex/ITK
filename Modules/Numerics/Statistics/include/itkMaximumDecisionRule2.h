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
#ifndef __itkMaximumDecisionRule2_h
#define __itkMaximumDecisionRule2_h

/**
 * The class MaximumDecisionRule2 has been removed from ITK. It has
 * been replaced by MaximumDecisionRule. This file allows for backward
 * compatibility.
 */

#ifndef __itkMaximumDecisionRule_h
#warning "itk::Statistics::MaximumDecisionRule2 has been deprecated. itk::Statistics::MaximumDecisionRule has been substituted in its place. Please use itk::Statistics::MaximumDecisionRule directly."
#endif

#include "itkMaximumDecisionRule.h"

namespace itk
{
namespace Statistics
{
typedef MaximumDecisionRule MaximumDecisionRule2;
}
}

#endif
