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
#ifndef __itkRegularStepGradientDescentObjectOptimizer_h
#define __itkRegularStepGradientDescentObjectOptimizer_h

#include "itkGradientDescentObjectOptimizerBase.h"

namespace itk
{
/** \class RegularStepGradientDescentObjectOptimizer
 *  \brief Gradient descent optimizer.
 *
 * FIXME - update for RegularStep:
 *
 * GradientDescentOptimizer implements a simple gradient descent optimizer.
 * At each iteration the current position is updated according to
 *
 * \f[
 *        p_{n+1} = p_n
 *                + \mbox{learningRate}
                  \, \frac{\partial f(p_n) }{\partial p_n}
 * \f]
 *
 * The user can scale each component of the df / dp
 * but setting a scaling vector using method SetScales().
 *
 * The user may set a member m_ScalesEstimator by calling SetScalesEstimator()
 * before optimization to estimate scales automatically during StartOptimization.
 *
 * \note The algorithm differes from the previous version of
 * RegularStepGradientDescentOptimizer, in that ... (div by scales).
 *
 * \note Unlike the previous version of RegularStepGradientDescentOptimizer,
 * this version does not have a "maximize/minimize" option
 * to modify the effect of the metric derivative.
 * The assigned metric is assumed to return a parameter derivative
 * result that "improves" the optimization when *added* to the current
 * parameters via the metric::UpateTransformParameters method, after the
 * optimizer applies scales and a learning rate.
 *
 * \ingroup ITKHighDimensionalOptimizers
 */
class ITK_EXPORT RegularStepGradientDescentObjectOptimizer
  : public GradientDescentObjectOptimizerBase
{
public:
  /** Standard class typedefs. */
  typedef RegularStepGradientDescentObjectOptimizer   Self;
  typedef GradientDescentObjectOptimizerBase          Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(RegularStepGradientDescentObjectOptimizer, RegularStepGradientDescentObjectOptimizerBase);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  /** Derivative type */
  typedef Superclass::DerivativeType      DerivativeType;

  /** Metric type over which this class is templated */
  typedef Superclass::MeasureType                  MeasureType;
  typedef Superclass::InternalComputationValueType InternalComputationValueType;

  /** Start and run the optimization */
  virtual void StartOptimization();

  /** Set/Get parameters to control the optimization process. */
  itkSetMacro(MaximumStepLength, InternalComputationValueType);
  itkSetMacro(MinimumStepLength, InternalComputationValueType);
  itkSetMacro(RelaxationFactor, InternalComputationValueType);
  itkSetMacro(GradientMagnitudeTolerance, InternalComputationValueType);
  itkGetConstReferenceMacro(MaximumStepLength, InternalComputationValueType);
  itkGetConstReferenceMacro(MinimumStepLength, InternalComputationValueType);
  itkGetConstReferenceMacro(RelaxationFactor, InternalComputationValueType);
  itkGetConstReferenceMacro(GradientMagnitudeTolerance, InternalComputationValueType);
  itkGetConstReferenceMacro(CurrentStepLength, InternalComputationValueType);

protected:

  /** Advance one Step following the gradient direction.
   * Includes transform update. */
  virtual void AdvanceOneStep(void);

  /** Process the gradient over a given index range. */
  virtual void ProcessGradientOverSubRange( const IndexRangeType& subrange,
    const ThreadIdType threadId );

  /** Default constructor */
  RegularStepGradientDescentObjectOptimizer();

  /** Destructor */
  virtual ~RegularStepGradientDescentObjectOptimizer();

  virtual void PrintSelf( std::ostream & os, Indent indent ) const;

private:

  DerivativeType     m_PreviousGradientScaled;

  InternalComputationValueType             m_GradientMagnitudeTolerance;
  InternalComputationValueType             m_MaximumStepLength;
  InternalComputationValueType             m_MinimumStepLength;
  InternalComputationValueType             m_CurrentStepLength;
  InternalComputationValueType             m_RelaxationFactor;

  std::vector<InternalComputationValueType> m_MagnitudeSquaredPerThread;
  std::vector<InternalComputationValueType> m_ScalarProductPerThread;

  //purposely not implemented
  RegularStepGradientDescentObjectOptimizer( const Self & );
  //purposely not implemented
  void operator=( const Self& );

};

} // end namespace itk

#endif
