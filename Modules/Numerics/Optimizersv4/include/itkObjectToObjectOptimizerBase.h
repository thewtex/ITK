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
#ifndef __itkObjectToObjectOptimizerBase_h
#define __itkObjectToObjectOptimizerBase_h

#include "itkObjectToObjectOptimizerBaseTemplate.h"
#include "itkObjectToObjectMetricBase.h"

namespace itk
{
/** \class ObjectToObjectOptimizerBase
 * \brief Abstract base for object-to-object optimizers.
 *
 * The goal of this optimizer hierarchy is to work with metrics
 * of any type, i.e. working with any kind of object, such as
 * image or point-set.
 *
 * Transform parameters are not manipulated directly. Instead,
 * the optimizer retrieves the metric derivative from the metric,
 * modifies the derivative as required, then passes it back to
 * the metric as an update. The metric then processes it as
 * appropriate, typically by passing it to its transform that is
 * being optimized.
 *
 * SetScales() allows setting of a per-local-parameter scaling array.
 * The gradient of each local parameter is divided by the corresponding
 * scale. If unset, the \c m_Scales array will be initialized to all 1's.
 * Note that when used with transforms with local support, these scales
 * correspond to each _local_ parameter, and not to each parameter. For
 * example, in a DisplacementFieldTransform of dimensionality N, the Scales
 * is size N, with each element corresponding to a dimension within the
 * transform's displacement field, and is applied to each vector in the
 * displacement field.
 *
 * SetWeights() allows setting of a per-local-parameter weighting array.
 * If unset, the weights are treated as identity. Weights are multiplied
 * by the gradient at the same time scaling is applied. Weights are
 * different than scales, and may be used, for example, to easily mask out a
 * particular parameter during optimzation to hold it constant. Or they
 * may be used to apply another kind of prior knowledge.
 *
 * Threading of some optimizer operations may be handled within
 * derived classes, for example in GradientDescentOptimizer.
 *
 * \note Derived classes must override StartOptimization, and then call
 * this base class version to perform common initializations.
 *
 * \ingroup ITKOptimizersv4
 */
typedef ObjectToObjectOptimizerBaseTemplate<double> ObjectToObjectOptimizerBase;

} // end namespace itk

#endif
