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
#ifndef __itkGradientDescentObjectOptimizerBase_h
#define __itkGradientDescentObjectOptimizerBase_h

#include "itkThreadedIndexedContainerPartitioner.h"
#include "itkDomainThreader.h"
#include "itkObjectToObjectOptimizerBase.h"

namespace itk
{
/** \class GradientDescentObjectOptimizerBase
 *  \brief Abstract base class for gradient descent-style optimizers.
 *
 * Gradient modification is threaded in \c ModifyGradient.
 *
 * Derived classes must override \c ModifyGradientOverSubRange
 * and \c ResumeOptimization.
 *
 * \ingroup ITKHighDimensionalOptimizers
 */

class ITK_EXPORT GradientDescentObjectOptimizerBase
  : public ObjectToObjectOptimizerBase
{
public:
  /** Standard class typedefs. */
  typedef GradientDescentObjectOptimizerBase     Self;
  typedef ObjectToObjectOptimizerBase            Superclass;
  typedef SmartPointer< Self >                   Pointer;
  typedef SmartPointer< const Self >             ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(GradientDescentObjectOptimizerBase, ObjectToObjectOptimizerBase);

  /** Codes of stopping conditions. */
  typedef enum {
    MAXIMUM_NUMBER_OF_ITERATIONS,
    COSTFUNCTION_ERROR,
    UPDATE_PARAMETERS_ERROR,
    STEP_TOO_SMALL,
    QUASI_NEWTON_STEP_ERROR,
    OTHER_ERROR
    } StopConditionType;

  /** Stop condition return string type */
  typedef std::string                            StopConditionReturnStringType;

  /** Stop condition internal string type */
  typedef std::ostringstream                     StopConditionDescriptionType;

  /** Metric type over which this class is templated */
  typedef Superclass::MetricType                    MetricType;
  typedef MetricType::Pointer                       MetricTypePointer;

  /** Derivative type */
  typedef MetricType::DerivativeType                DerivativeType;

  /** Measure type */
  typedef Superclass::MeasureType                   MeasureType;

  /** Internal computation type, for maintaining a desired precision */
  typedef Superclass::InternalComputationValueType InternalComputationValueType;

  /** Get the most recent gradient values. */
  itkGetConstReferenceMacro( Gradient, DerivativeType );

  /** Get stop condition enum */
  itkGetConstReferenceMacro(StopCondition, StopConditionType);

  /** Set the number of iterations. */
  itkSetMacro(NumberOfIterations, SizeValueType);

  /** Get the number of iterations. */
  itkGetConstReferenceMacro(NumberOfIterations, SizeValueType);

  /** Get the current iteration number. */
  itkGetConstMacro(CurrentIteration, SizeValueType);

  /** Resume optimization.
   * This runs the optimization loop, and allows continuation
   * of stopped optimization */
  virtual void ResumeOptimization() = 0;

  /** Stop optimization. The object is left in a state so the
   * optimization can be resumed by calling ResumeOptimization. */
  virtual void StopOptimization(void);

  /** Get the reason for termination */
  virtual const StopConditionReturnStringType GetStopConditionDescription() const;

protected:

  /** Default constructor */
  GradientDescentObjectOptimizerBase();
  virtual ~GradientDescentObjectOptimizerBase();

  /** Modify the gradient in place, to advance the optimization.
   * This call performs a threaded modification for transforms with
   * local support (assumed to be dense). Otherwise the modification
   * is performed w/out threading.
   * The work is done in ModifyGradientThreader::ThreadedExecution in both cases.
   * At completion, m_Gradient can be used to update the transform
   * parameters. Derived classes may hold additional results in
   * other member variables.
   */
  virtual void ModifyGradient();

  /** \class ModifyGradientThreaderBase
   * \brief Modify the gradient during the gradient update in \c ModifyGradient.
   * \ingroup ITKHighDimensionalOptimizers */
  class ModifyGradientThreaderBase: public DomainThreader< ThreadedIndexedContainerPartitioner, GradientDescentObjectOptimizerBase >
  {
  public:
    /** Standard class typedefs. */
    typedef ModifyGradientThreaderBase                                                     Self;
    typedef DomainThreader< ThreadedIndexedContainerPartitioner, GradientDescentObjectOptimizerBase > Superclass;
    typedef SmartPointer< Self >                                                           Pointer;
    typedef SmartPointer< const Self >                                                     ConstPointer;

    itkTypeMacro( GradientDescentObjectOptimizerBase::ModifyGradientThreaderBase, DomainThreader );

    typedef Superclass::DomainType         DomainType;
    typedef Superclass::EnclosingClassType EnclosingClassType;
    typedef DomainType                     IndexRangeType;

  protected:
    ModifyGradientThreaderBase(){}
    virtual ~ModifyGradientThreaderBase(){}

    virtual void ThreadedExecution( const DomainType& domain,
                                    const ThreadIdType threadId ) = 0;
  private:
    ModifyGradientThreaderBase( const Self & ); // purposely not implemented
    void operator=( const Self & ); // purposely not implemented
  };

  /* Common variables for optimization control and reporting */
  bool                          m_Stop;
  StopConditionType             m_StopCondition;
  StopConditionDescriptionType  m_StopConditionDescription;
  SizeValueType                 m_NumberOfIterations;
  SizeValueType                 m_CurrentIteration;

  /** Current gradient */
  DerivativeType     m_Gradient;
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  /** Multi-threaded for gradient modification operation. Sub-classes must
   * instantiate a chile of this class. */
  ModifyGradientThreaderBase::Pointer      m_ModifyGradientThreaderBase;
private:
  GradientDescentObjectOptimizerBase( const Self & ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

};

} // end namespace itk

#endif
