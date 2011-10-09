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
#ifndef __itkCommandScalesUpdate_h
#define __itkCommandScalesUpdate_h

#include "itkCommand.h"
#include "itkWeakPointer.h"

namespace itk {

/**
 *  Implementation of the Command Pattern to be invoked every iteration
 * \class CommandScalesUpdate
 * \ingroup ITKRegistrationCommon
 */
template < class TOptimizer, class TScalesEstimator >
class ITK_EXPORT CommandScalesUpdate : public Command
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef CommandScalesUpdate   Self;


  /**
   * Standard "Superclass" typedef.
   */
  typedef itk::Command  Superclass;


  /**
   * Smart pointer typedef support.
   */
  typedef itk::SmartPointer<Self>  Pointer;

  /**
   * ConstSmart pointer typedef support.
   */
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**
   * Types from TOptimizer
   */
  typedef typename TOptimizer::ParametersType  ParametersType;
  typedef typename TOptimizer::DerivativeType  DerivativeType;
  typedef typename TOptimizer::ScalesType      ScalesType;
  typedef typename ScalesType::ValueType       FloatType;

  void QuietOn() { m_Quiet = true; }
  void QuietOff() { m_Quiet = false; }

  /**
   * Methods for step size
   */
  itkSetMacro(StepSize, FloatType);
  itkGetConstMacro(StepSize, FloatType);

  /**
   * Execute method will print data at each iteration
   */
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object *, const itk::EventObject & event)
  {
    if( typeid( event ) == typeid( itk::ScalesEvent ) )
      {
      ScalesType scales;
      m_ScalesEstimator->EstimateScales(scales);
      m_Optimizer->SetScales(scales);
      if ( !m_Quiet )
        {
        std::cout << "scales = " << scales << std::endl;
        }
      }
    else if( typeid( event ) == typeid( itk::LearningRateEvent ) )
      {
      const ScalesType &scales = m_Optimizer->GetScales();
      const DerivativeType &gradient = m_Optimizer->GetGradient();

      ParametersType step(gradient.GetSize());
      for (SizeValueType i=0; i<gradient.GetSize(); i++)
        {
        step[i] = gradient[i] / scales[i];
        }

      FloatType stepScale = m_ScalesEstimator->EstimateStepScale(step);
      FloatType learningRate = NumericTraits<FloatType>::One;

      if (stepScale > NumericTraits<FloatType>::epsilon())
        {
        learningRate = m_StepSize / stepScale;
        }

      m_Optimizer->SetLearningRate(learningRate);
      if ( !m_Quiet )
        {
        std::cout << "learningRate = " << learningRate << std::endl;
        }
      }

  }


  /**
   * Run-time type information (and related methods).
   */
  itkTypeMacro( CommandScalesUpdate, ::itk::Command );


  /**
   * Method for creation through the object factory.
   */
  itkNewMacro( Self );


  /**
   * Type defining the optimizer
   */
  typedef    TOptimizer       OptimizerType;

  /**
   * Type defining the optimizer
   */
  typedef    TScalesEstimator ScalesEstimatorType;

  /**
   * Set Optimizer
   */
  void SetOptimizer( OptimizerType * optimizer )
    {
    m_Optimizer = optimizer;
    m_Optimizer->AddObserver( itk::IterationEvent(), this );
    }

  /**
   * Set Scales Estimator
   */
  void SetScalesEstimator( ScalesEstimatorType * scalesEstimator )
    {
    m_ScalesEstimator = scalesEstimator;
    }

protected:

  /**
   * Constructor
   */
  CommandScalesUpdate()
    {
    this->m_StepSize = 0.5;
    this->m_Quiet = true;
    }

private:

  /**
   *  WeakPointer to the Optimizer
   */
  WeakPointer<OptimizerType>          m_Optimizer;

  /**
   *  WeakPointer to the Optimizer
   */
  WeakPointer<ScalesEstimatorType>    m_ScalesEstimator;

  /**
   *  Step size for learning rate estimation
   */
  FloatType                           m_StepSize;

  /**
   *  Switch for suppressing the messages
   */
  bool                                m_Quiet;
};

} // end namespace itk

#endif
