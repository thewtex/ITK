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
#include "itkGradientDescentObjectOptimizerBase.h"
#include "itkObjectToObjectMetric.h"
#include "itkImage.h"
#include "itkTestingMacros.h"
#include "itkIntTypes.h"

/* Create a simple metric to use for testing here. */
template< class TFixedObject,  class TMovingObject >
class ITK_EXPORT GradientDescentObjectOptimizerBaseTestMetric:
  public itk::ObjectToObjectMetric
{
public:
  /** Standard class typedefs. */
  typedef GradientDescentObjectOptimizerBaseTestMetric  Self;
  typedef itk::ObjectToObjectMetric                     Superclass;
  typedef itk::SmartPointer< Self >                     Pointer;
  typedef itk::SmartPointer< const Self >               ConstPointer;

  typedef typename Superclass::MeasureType          MeasureType;
  typedef typename Superclass::DerivativeType       DerivativeType;
  typedef typename Superclass::ParametersType       ParametersType;
  typedef typename Superclass::ParametersValueType  ParametersValueType;

  itkTypeMacro(GradientDescentObjectOptimizerBaseTestMetric, ObjectToObjectMetric);

  itkNewMacro(Self);

  // Pure virtual functions that all Metrics must provide
  unsigned int GetNumberOfParameters() const { return 5; }

  MeasureType GetValue() const
    {
    return itk::NumericTraits< MeasureType >::One;
    }

  void GetValueAndDerivative( MeasureType & value, DerivativeType & derivative ) const
    {
    std::cout << "GradientDescentObjectOptimizerBaseTestMetric::GetValueAndDerivative called." << std::endl;
    value = itk::NumericTraits< MeasureType >::One;
    derivative.SetSize( this->GetNumberOfParameters() );
    derivative.Fill( itk::NumericTraits< ParametersValueType >::Zero );
    }

  unsigned int GetNumberOfLocalParameters() const
  { return this->GetNumberOfParameters(); }

  bool HasLocalSupport() const
  { return false; }

  void UpdateTransformParameters( DerivativeType &, ParametersValueType ) {}

  const ParametersType & GetParameters() const
  { return m_Parameters; }

  void Initialize(void) throw ( itk::ExceptionObject ) {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const
  { Superclass::PrintSelf( os, indent ); }

protected:
  GradientDescentObjectOptimizerBaseTestMetric()
  {
    m_Parameters.SetSize( this->GetNumberOfParameters() );
    m_Parameters.Fill( itk::NumericTraits< ParametersValueType >::Zero );
  }
  ~GradientDescentObjectOptimizerBaseTestMetric() {}//purposely not implemented

private:
  GradientDescentObjectOptimizerBaseTestMetric( const Self& ); //purposely not implemented
  void operator = ( const Self& ); //purposely not implemented

  ParametersType m_Parameters;
};

/* Define a simple derived optimizer class.
 * \class GradientDescentObjectOptimizerBaseTestOptimizer */
class GradientDescentObjectOptimizerBaseTestOptimizer
  : public itk::GradientDescentObjectOptimizerBase
{
public:
  /** Standard "Self" typedef. */
  typedef GradientDescentObjectOptimizerBaseTestOptimizer Self;
  typedef GradientDescentObjectOptimizerBase              Superclass;
  typedef itk::SmartPointer< Self >                       Pointer;
  typedef itk::SmartPointer< const Self >                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( GradientDescentObjectOptimizerBaseTestOptimizer,
                GradientDescentObjectOptimizerBase);

  /* Provide an override for the pure virtual StartOptimization */
  void StartOptimization()
    {
    Superclass::StartOptimization();
    std::cout << "Test StartOptimization called." << std::endl;
    this->m_ProcessGradientOverSubRangeCalled = false;
    this->ResumeOptimization();
    }

  void ProcessGradientOverSubRange (const IndexRangeType& index,
       const itk::ThreadIdType threadId )
    {
    std::cout << "Test ProcessGradientOverSubRange called with index:"
              << index << " and threadId " << threadId << std::endl;
    this->m_ProcessGradientOverSubRangeCalled = true;
    }

  bool     m_ProcessGradientOverSubRangeCalled;

protected:

  void AdvanceOneStep(void)
    {
    std::cout << "Test AdvanceOneStep called" << std::endl;
    this->ProcessGradient();
    }

  GradientDescentObjectOptimizerBaseTestOptimizer()
    {
    this->m_ProcessGradientOverSubRangeCalled = false;
    }
  virtual ~GradientDescentObjectOptimizerBaseTestOptimizer(){}

private:
  GradientDescentObjectOptimizerBaseTestOptimizer(const Self& ); //purposely not implemented
  void operator = (const Self&); //purposely not implemented

};

/**
 */
int itkGradientDescentObjectOptimizerBaseTest(int , char* [])
{
  const int ImageDimension = 2;
  typedef itk::Image<double, ImageDimension>                    ImageType;

  typedef GradientDescentObjectOptimizerBaseTestMetric<ImageType,ImageType> MetricType;

  MetricType::Pointer metric = MetricType::New();
  GradientDescentObjectOptimizerBaseTestOptimizer::Pointer optimizer = GradientDescentObjectOptimizerBaseTestOptimizer::New();

  optimizer->SetMetric( metric );
  if( optimizer->GetMetric() != metric )
    {
    std::cerr << "Set/GetMetric failed." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "value: " << optimizer->GetValue() << std::endl;

  optimizer->SetNumberOfThreads( 2 );
  optimizer->SetNumberOfIterations( 2 );
  TRY_EXPECT_NO_EXCEPTION( optimizer->StartOptimization() );

  /* Check that control made it down to the gradient modification method */
  if( ! optimizer->m_ProcessGradientOverSubRangeCalled )
    {
    std::cerr << "Call to StartOptimization failed to reach expected code. "
              << "m_ProcessGradientOverSubRangeCalled is false" << std::endl;
    return EXIT_FAILURE;
    }
  /* exercise some methods */
  std::cout << "Number of iterations: " << optimizer->GetNumberOfIterations()
            << std::endl
            << "Current gradient: " << optimizer->GetGradient() << std::endl
            << "StopCondition: " << optimizer->GetStopCondition() << std::endl
            << "StopConditionDescription: " << optimizer->GetStopConditionDescription()
            << std::endl
            << "CurrentIteration: " << optimizer->GetCurrentIteration() << std::endl;
  std::cout << "Printing self.." << std::endl;
  std::cout << optimizer << std::endl;

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
