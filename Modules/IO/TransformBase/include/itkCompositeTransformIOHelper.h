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
#ifndef __itkCompositeTransformIOHelper_h
#define __itkCompositeTransformIOHelper_h

#include "itkCompositeTransformIOHelperTemplate.h"

namespace itk
{

/** \class CompositeTransformIOHelper
 *  \brief An adapter that adapts CompositeTransform into the
 *  TransformReader/Writer
 *
 *  The stumbling block with fitting CompositeTransforms into
 *  the Transform hierarchy is that it encapsulates a list of
 *  itk::Transform, templated to match the dimension and scalar
 *  type of the CompositeTransform itself.  But the Transform
 *  Reader/Writer uses instances of the superclass itk::TransformBase
 *  which is not templated.
 *
 *  This class handles this by hiding the conversion between a list
 *  of TransformBase and the queue of Transform.  It handles the
 *  conversion in a cascade of template function invocations to
 *  match up the transform list with the particular instantiation
 *  of CompositeTransform.
 * \ingroup ITKIOTransformBase
 */

typedef itk::CompositeTransformIOHelperTemplate<double> CompositeTransformIOHelper;

}

#endif //  __itkCompositeTransformIOHelper_h
