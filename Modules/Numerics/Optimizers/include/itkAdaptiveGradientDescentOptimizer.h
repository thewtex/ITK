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
#ifndef __itkAdaptiveGradientDescentOptimizer_h
#define __itkAdaptiveGradientDescentOptimizer_h

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/algo/vnl_matrix_inverse.h"

#include "itkIntTypes.h"
#include "itkGradientDescentOptimizer.h"
#include "itkOptimizerHelper.h"
#include <string>
namespace itk
{
/** \class AdaptiveGradientDescentOptimizer
 * \brief Implement a gradient descent optimizer
 *
 * AdaptiveGradientDescentOptimizer implements a simple gradient descent optimizer.
 * At each iteration the current position is updated according to
 *
 * \f[
 *        p_{n+1} = p_n
 *                + \mbox{learningRate}
                  \, \frac{\partial f(p_n) }{\partial p_n}
 * \f]
 *
 * The learning rate is a fixed scalar defined via SetLearningRate().
 * The optimizer steps through a user defined number of iterations;
 * no convergence checking is done.
 *
 * Additionally, user can scale each component of the df / dp
 * but setting a scaling vector using method SetScale().
 *
 * \sa RegularStepAdaptiveGradientDescentOptimizer
 *
 * \ingroup Numerics Optimizers
 * \ingroup ITK-Optimizers
 */
class ITK_EXPORT AdaptiveGradientDescentOptimizer:
  public GradientDescentOptimizer
{
public:
  /** Standard class typedefs. */
  typedef AdaptiveGradientDescentOptimizer  Self;
  typedef GradientDescentOptimizer          Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AdaptiveGradientDescentOptimizer, GradientDescentOptimizer);

  /** Advance one step following the gradient direction. */
  void AdvanceOneStep(void);

  /** Pointer of OptimizerHelper. */
  typedef OptimizerHelper::Pointer   OptimizerHelperPointer;
  /** Connect the OptimizerHelper.  */
  itkSetObjectMacro(OptimizerHelper, OptimizerHelper);

protected:
  AdaptiveGradientDescentOptimizer();
  virtual ~AdaptiveGradientDescentOptimizer() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  AdaptiveGradientDescentOptimizer(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

  OptimizerHelperPointer  m_OptimizerHelper;
};
} // end namespace itk

#endif
