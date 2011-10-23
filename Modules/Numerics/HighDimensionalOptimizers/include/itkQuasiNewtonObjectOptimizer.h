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
#ifndef __itkQuasiNewtonObjectOptimizer_h
#define __itkQuasiNewtonObjectOptimizer_h

#include "itkArray2D.h"
#include "itkGradientDescentObjectOptimizer.h"

#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/algo/vnl_determinant.h"

namespace itk
{
/** \class QuasiNewtonObjectOptimizer
 * \brief Implement a Quasi-Newton optimizer with BFGS Hessian estimation.
 *
 * Second order approximation of the cost function is usually more efficient
 * since it estimates the descent or ascent direction more precisely. However,
 * computation of Hessian is usually expensive or unavailable. Alternatively
 * Quasi-Newton methods can estimate a Hessian from the gradients of previous
 * steps. A commonly used Quasi-Newton method is BFGS.
 *
 * Here BFGS method is used to calcute a Quasi-Newton step. The line search
 * is implemented using Strong Wolfe's Conditions.
 *
 * If a helper object of OptimizerParameterScalesEstimator is set, it is used
 * to estimate the maximum step size in line search.
 *
 * The line search algorithm is from "Introduction to Nonlinear Optimization"
 * by Paul J Atzberger, on
 * Page 7 on http://www.math.ucsb.edu/~atzberg/finance/nonlinearOpt.pdf
 *
 * \ingroup ITKHighDimensionalOptimizers
 */
class ITK_EXPORT QuasiNewtonObjectOptimizer:
  public GradientDescentObjectOptimizer
{
public:
  /** Standard class typedefs. */
  typedef QuasiNewtonObjectOptimizer        Self;
  typedef GradientDescentObjectOptimizer    Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(QuasiNewtonObjectOptimizer, GradientDescentObjectOptimizer);

  typedef Superclass::InternalComputationValueType    InternalComputationValueType;

  /** Type for Hessian matrix in the Quasi-Newton method */
  typedef itk::Array2D<InternalComputationValueType>  HessianType;

  /** Start and run the optimization */
  virtual void StartOptimization();

  /** Set the maximum tolerable number of iteration without any progress */
  itkSetMacro(MaximumIterationsWithoutProgress, SizeValueType);

  /** Get the most recent Newton step. */
  itkGetConstReferenceMacro( NewtonStep, DerivativeType );

protected:

  /** The maximum tolerable number of iteration without any progress */
  SizeValueType   m_MaximumIterationsWithoutProgress;

  /** The information about the current step */
  ParametersType  m_CurrentPosition;

  /** The information about the previous step */
  MeasureType     m_PreviousValue;
  ParametersType  m_PreviousPosition;
  DerivativeType  m_PreviousGradient;

  /** The best value so far and relevant information */
  MeasureType     m_BestValue;
  ParametersType  m_BestPosition;
  SizeValueType   m_BestIteration;

  /** The Quasi-Newton step */
  DerivativeType  m_NewtonStep;

  /** The Hessian and its inverse estimated by the Quasi-Newton method */
  HessianType     m_Hessian;
  HessianType     m_HessianInverse;

  /** Estimate a Newton step */
  virtual void EstimateNewtonStep();

  /** Estimate a Hessian with BFGS method */
  virtual void EstimateHessian();

  /** Reset Hessian to Identity */
  virtual void ResetHessian();

  /** Modify the gradient over a given index range. */
  virtual void ModifyNewtonStep(void);

  /** Advance one step using the Quasi-Newton step or the gradient step. */
  virtual void AdvanceOneStep(void);

  QuasiNewtonObjectOptimizer();
  virtual ~QuasiNewtonObjectOptimizer();

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  QuasiNewtonObjectOptimizer(const Self &);     //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

};
} // end namespace itk

#endif
