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
#ifndef __itkLBFGSOptimizerv4_h
#define __itkLBFGSOptimizerv4_h

#include "itkSingleValuedNonLinearVnlOptimizerv4.h"
#include "vnl/algo/vnl_lbfgs.h"

namespace itk
{
/** \class LBFGSOptimizerv4
 * \brief Wrap of the vnl_lbfgs algorithm for use in ITKv4 registration framework.
 *
 * \note The StopConditionDescription returned by this class is directly from the vnl
 * optimizer by calling <tt> m_VnlOptimizer->get_failure_code() </tt>. This seems to
 * return "Failure" even when no error has occured. The same behavior is observed
 * in the ITKv3 version of this optimizer.
 *
 * \ingroup ITKOptimizersv4
 */
class ITK_EXPORT LBFGSOptimizerv4:
  public SingleValuedNonLinearVnlOptimizerv4
{
public:
  /** Standard "Self" typedef. */
  typedef LBFGSOptimizerv4                    Self;
  typedef SingleValuedNonLinearVnlOptimizerv4 Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LBFGSOptimizerv4, SingleValuedNonLinearVnlOptimizerv4);

  /** InternalParameters typedef. */
  typedef   vnl_vector< double > InternalParametersType;

  /** Internal optimizer type. */
  typedef   vnl_lbfgs InternalOptimizerType;

  typedef   Superclass::MetricType          MetricType;

  /** Method for getting access to the internal optimizer. */
  vnl_lbfgs * GetOptimizer(void);

  /** Start optimization with an initial value. */
  void StartOptimization(void);

  /** Plug in a Cost Function into the optimizer  */
  virtual void SetMetric(MetricType *metric);

  /** Set/Get the optimizer trace flag. If set to true, the optimizer
   * prints out information every iteration.
   */
  virtual void SetTrace(bool flag);

  itkGetMacro(Trace, bool);
  itkBooleanMacro(Trace);

  /** Set/Get the maximum number of function evaluations allowed. */
  virtual void SetMaximumNumberOfFunctionEvaluations(unsigned int n);

  itkGetMacro(MaximumNumberOfFunctionEvaluations, unsigned int);

  /** Set/Get the gradient convergence tolerance. This is a positive
   * real number that determines the accuracy with which the solution is to
   * be found. The optimization terminates when:
   * ||G|| < gtol max(1,||X||) where ||.|| denotes the Euclidean norm.
   */
  virtual void SetGradientConvergenceTolerance(double gtol);

  itkGetMacro(GradientConvergenceTolerance, double);

  /** Set/Get the line search accuracy. This is a positive real number
   * with a default value of 0.9, which controls the accuracy of the line
   * search. If the function and gradient evalutions are inexpensive with
   * respect to the cost of the iterations it may be advantageous to set
   * the value to a small value (say 0.1).
   */
  virtual void SetLineSearchAccuracy(double tol);

  itkGetMacro(LineSearchAccuracy, double);

  /** Set/Get the default step size. This is a positive real number
   * with a default value of 1.0 which determines the stpe size in the line
   * search.
   */
  virtual void SetDefaultStepLength(double stp);

  itkGetMacro(DefaultStepLength, double);

  /** Get the reason for termination */
  const std::string GetStopConditionDescription() const;

protected:
  LBFGSOptimizerv4();
  virtual ~LBFGSOptimizerv4();
  void PrintSelf(std::ostream & os, Indent indent) const;

  typedef Superclass::CostFunctionAdaptorType CostFunctionAdaptorType;
private:
  LBFGSOptimizerv4(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  bool                       m_OptimizerInitialized;
  InternalOptimizerType *    m_VnlOptimizer;
  mutable std::ostringstream m_StopConditionDescription;

  bool         m_Trace;
  unsigned int m_MaximumNumberOfFunctionEvaluations;
  double       m_GradientConvergenceTolerance;
  double       m_LineSearchAccuracy;
  double       m_DefaultStepLength;
};
} // end namespace itk

#endif
