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
#ifndef __itkRegularStepGradientDescentOptimizerv4_h
#define __itkRegularStepGradientDescentOptimizerv4_h

#include "itkGradientDescentOptimizerBasev4.h"

namespace itk
{
  /** \class RegularStepGradientDescentOptimizerv4Template
   *  \brief Regular Step Gradient descent optimizer.
   *
   *   This optimizer does not use the ScaleEstimator to estimate the learning rate.
   *
   * \ingroup ITKOptimizersv4
   */
template<typename TInternalComputationValueType>
class RegularStepGradientDescentOptimizerv4Template
: public GradientDescentOptimizerBasev4Template<TInternalComputationValueType>
{
public:
  /** Standard class typedefs. */
  typedef RegularStepGradientDescentOptimizerv4Template                         Self;
  typedef GradientDescentOptimizerBasev4Template<TInternalComputationValueType> Superclass;
  typedef SmartPointer< Self >                                                  Pointer;
  typedef SmartPointer< const Self >                                            ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(RegularStepGradientDescentOptimizerv4Template, Superclass);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);


  /** It should be possible to derive the internal computation type from the class object. */
  typedef TInternalComputationValueType                     InternalComputationValueType;

  /** Derivative type */
  typedef typename Superclass::DerivativeType               DerivativeType;

  /** Metric type over which this class is templated */
  typedef typename Superclass::MeasureType                  MeasureType;
  typedef typename Superclass::IndexRangeType               IndexRangeType;
  typedef typename Superclass::ScalesType                   ScalesType;
  typedef typename Superclass::ParametersType               ParametersType;
  typedef typename Superclass::StopConditionType            StopConditionType;

  /** Set the learning rate. */
  itkSetMacro(LearningRate, TInternalComputationValueType);

  /** Get the learning rate. */
  itkGetConstReferenceMacro(LearningRate, TInternalComputationValueType);

  /** Minimum step length (learning rate) value for convergence checking.
   *  The step length is decreased by relaxation factor if the step is too
   *  long, and the algorithm passes the local minimum.
   *  When the step length value reaches a small value, it would be treated
   *  as converged.
   *
   *  The default m_MinimumStepLength is set to 1e-4 to pass all
   *  tests.
   */
  itkSetMacro(MinimumStepLength, TInternalComputationValueType);
  itkGetConstReferenceMacro(MinimumStepLength, TInternalComputationValueType);

  /** Set/Get relaxation factor value */
  itkSetMacro(RelaxationFactor, TInternalComputationValueType);
  itkGetConstReferenceMacro(RelaxationFactor, TInternalComputationValueType);

  /** Flag. Set to have the optimizer track and return the best
   *  best metric value and corresponding best parameters that were
   *  calculated during the optimization. This captures the best
   *  solution when the optimizer oversteps or osciallates near the end
   *  of an optimization.
   *  Results are stored in m_CurrentMetricValue and in the assigned metric's
   *  parameters, retrievable via optimizer->GetCurrentPosition().
   *  This option requires additional memory to store the best
   *  parameters, which can be large when working with high-dimensional
   *  transforms such as DisplacementFieldTransform.
   */
  itkSetMacro(ReturnBestParametersAndValue, bool);
  itkGetConstReferenceMacro(ReturnBestParametersAndValue, bool);
  itkBooleanMacro(ReturnBestParametersAndValue);

  /** Start and run the optimization */
  virtual void StartOptimization( bool doOnlyInitialization = false );

  virtual void StopOptimization(void);

  virtual void ResumeOptimization();

protected:

  /** Advance one Step following the gradient direction.
   * Includes transform update. */
  virtual void AdvanceOneStep(void);

  /** Modify the input gradient over a given index range. */
  virtual void ModifyGradientByScalesOverSubRange( const IndexRangeType& subrange );
  virtual void ModifyGradientByLearningRateOverSubRange( const IndexRangeType& subrange );

  /** Manual learning rate to apply. It is overridden by
   * automatic learning rate estimation if enabled. See main documentation.
   */
  TInternalComputationValueType  m_LearningRate;
  TInternalComputationValueType  m_RelaxationFactor;

  /** Default constructor */
  RegularStepGradientDescentOptimizerv4Template();

  /** Destructor */
  virtual ~RegularStepGradientDescentOptimizerv4Template();

  virtual void PrintSelf( std::ostream & os, Indent indent ) const;

  /** Minimum gradient step value for convergence checking */
  TInternalComputationValueType  m_MinimumStepLength;

  /** Store the best value and related paramters */
  MeasureType                  m_CurrentBestValue;
  ParametersType               m_BestParameters;

  /** Flag to control returning of best value and parameters. */
  bool m_ReturnBestParametersAndValue;

  /** Store the previous gradient value at each iteration,
   * so we can detect the changes in geradient direction.
   */
  DerivativeType m_PreviousGradient;

private:
  RegularStepGradientDescentOptimizerv4Template( const Self & ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented
};

/** This helps to meet backward compatibility */
typedef RegularStepGradientDescentOptimizerv4Template<double> RegularStepGradientDescentOptimizerv4;

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegularStepGradientDescentOptimizerv4.hxx"
#endif

#endif
