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
#ifndef __itkAutomaticGradientDescentOptimizer_h
#define __itkAutomaticGradientDescentOptimizer_h

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "itkArray.h"
#include "itkArray2D.h"

#include "itkIntTypes.h"
#include "itkGradientDescentOptimizer.h"
#include "itkOptimizerHelper.h"
#include <string>
namespace itk
{
/** \class AutomaticGradientDescentOptimizer
 * \brief Implement a gradient descent optimizer with basic parameter
 * estimation.
 *
 * The scales are estimated from an OptimizerHelper object.
 *
 * The learning rate is estimated by restricting the voxel shift at the
 * first iteration.
 *
 * \ingroup Numerics Optimizers
 * \ingroup ITK-Optimizers
 */
class ITK_EXPORT AutomaticGradientDescentOptimizer:
  public GradientDescentOptimizer
{
public:
  /** Standard class typedefs. */
  typedef AutomaticGradientDescentOptimizer   Self;
  typedef GradientDescentOptimizer            Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AutomaticGradientDescentOptimizer, GradientDescentOptimizer);

  /** Start optimization. */
  void StartOptimization(void);

  /** Advance one step following the gradient direction. */
  void AdvanceOneStep(void);

  /** Pointer of OptimizerHelper. */
  typedef OptimizerHelper::Pointer   OptimizerHelperPointer;

  /** Connect the OptimizerHelper.  */
  itkSetObjectMacro(OptimizerHelper, OptimizerHelper);
  itkGetObjectMacro(OptimizerHelper, OptimizerHelper);

protected:

  OptimizerHelperPointer        m_OptimizerHelper;
  double                        m_MaximumVoxelShift;

  /** Function to compute the learning rate. */
  virtual double EstimateLearningRate() const;

  AutomaticGradientDescentOptimizer();
  virtual ~AutomaticGradientDescentOptimizer() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  AutomaticGradientDescentOptimizer(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

};
} // end namespace itk

#endif
