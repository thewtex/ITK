#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include <iostream>
#include <cstdlib>
#include "itkInitializationBiasedParticleSwarmOptimizer.h"
#include "itkParticleSwarmOptimizerTestFunctions.h"

typedef  itk::InitializationBiasedParticleSwarmOptimizer OptimizerType;

/**
 * Test using a 1D function with two minima, two parabolas. Check that the
 * optimizer converges to the global minimum if it is initialized inside the
 * domain of either parabolas (runs the optimizer once with initial guess in
 * each of the domains).
 */
int IBPSOTest1();


/**
 * Test using a 2D quadratic function (single minimum), check that converges
 * correctly.
 */
int IBPSOTest2();


/**
 * Test using the 2D Rosenbrock function.
 */
int IBPSOTest3();

#define VERBOSE 1

/**
* The particle swarm optimizer is a stochastic algorithm. Consequentially, we run
* the same test multiple times and deem it a success if the number of successful
* runs is above a threshold.
*/
int itkInitializationBiasedParticleSwarmOptimizerTest(int, char* [] )
{
  unsigned int i, allIterations=10;
  double threshold = 0.8;
  unsigned int success1, success2, success3;

  std::cout << "Initialization Biased Particle Swarm Optimizer Test \n \n";

 success1 = success2 = success3 = 0;
  for( i=0; i<allIterations; i++ )
    {
    if( EXIT_SUCCESS == IBPSOTest1() )
      {
      success1++;
      }
    if( EXIT_SUCCESS == IBPSOTest2() )
      {
      success2++;
      }
    if( EXIT_SUCCESS == IBPSOTest3() )
      {
      success3++;
      }
    }

  std::cout<< "All Tests Completed."<< std::endl;

  if( static_cast<double>(success1)/ static_cast<double>(allIterations) <= threshold ||
      static_cast<double>(success2)/ static_cast<double>(allIterations) <= threshold ||
      static_cast<double>(success3)/ static_cast<double>(allIterations) <= threshold )
    {
    std::cout<<"[FAILURE]\n";
    return EXIT_FAILURE;
    }
  std::cout << "[SUCCESS]" << std::endl;
  return EXIT_SUCCESS;
}


int IBPSOTest1()
{
#if VERBOSE
  std::cout << "Particle Swarm Optimizer Test 1 [f(x) = if(x<0) x^2+4x; else 2x^2-8x]\n";
  std::cout << "-------------------------------\n";
#endif
  double knownParameters = 2.0;

    //the function we want to optimize
  ParticleSwarmTestF1::Pointer costFunction =
    ParticleSwarmTestF1::New();

  OptimizerType::Pointer  itkOptimizer = OptimizerType::New();

         // set optimizer parameters
  OptimizerType::ParameterBoundsType bounds;
  bounds.push_back( std::make_pair( -10, 10 ) );
  unsigned int numberOfParticles = 10;
  unsigned int maxIterations = 100;
  double xTolerance = 0.1;
  double fTolerance = 0.001;
  OptimizerType::ParametersType initialParameters( 1 ), finalParameters;

  itkOptimizer->SetParameterBounds( bounds );
  itkOptimizer->SetNumberOfParticles( numberOfParticles );
  itkOptimizer->SetMaximalNumberOfIterations( maxIterations );
  itkOptimizer->SetParametersConvergenceTolerance( xTolerance,
                                                   costFunction->GetNumberOfParameters() );
  itkOptimizer->SetFunctionConvergenceTolerance( fTolerance );
  itkOptimizer->SetCostFunction( costFunction.GetPointer() );

#if VERBOSE
          //observe the iterations
  CommandIterationUpdateParticleSwarm::Pointer observer =
    CommandIterationUpdateParticleSwarm::New();
  itkOptimizer->AddObserver( itk::IterationEvent(), observer );
  itkOptimizer->AddObserver( itk::StartEvent(), observer );
#endif

  try
    {
    initialParameters[0] =  3;
    itkOptimizer->SetInitialPosition( initialParameters );
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters ) > xTolerance )
      {
      //std::cout << "[Test 1 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }
             //run optimization again with a different initial value
    initialParameters[0] =  2.5;
    itkOptimizer->ClearSwarm();
    itkOptimizer->SetInitialPosition( initialParameters );
#if VERBOSE
    observer->Reset();
#endif
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters ) > xTolerance )
      {
      //std::cout << "[Test 1 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "[Test 1 FAILURE]" << std::endl;
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during Optimization" << std::endl;
    std::cout << "Location    = " << e.GetLocation()    << std::endl;
    std::cout << "Description = " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }
//  std::cout << "[Test 1 SUCCESS]" << std::endl;
  return EXIT_SUCCESS;
}


int IBPSOTest2()
{
#if VERBOSE
  std::cout << "Particle Swarm Optimizer Test 2 [f(x) = 1/2 x^T A x - b^T x]\n";
  std::cout << "----------------------------------\n";
#endif

  itk::Array<double> knownParameters( 2 );
  knownParameters[0] = 2.0;
  knownParameters[1] = -2.0;

    //the function we want to optimize
  ParticleSwarmTestF2::Pointer costFunction =
    ParticleSwarmTestF2::New();

  OptimizerType::Pointer  itkOptimizer = OptimizerType::New();

         // set optimizer parameters
  OptimizerType::ParameterBoundsType bounds;
  bounds.push_back( std::make_pair( -10, 10 ) );
  bounds.push_back( std::make_pair( -10, 10 ) );
  unsigned int numberOfParticles = 10;
  unsigned int maxIterations = 100;
  double xTolerance = 0.1;
  double fTolerance = 0.001;
  OptimizerType::ParametersType initialParameters( 2 ), finalParameters;

  itkOptimizer->SetParameterBounds( bounds );
  itkOptimizer->SetNumberOfParticles( numberOfParticles );
  itkOptimizer->SetMaximalNumberOfIterations( maxIterations );
  itkOptimizer->SetParametersConvergenceTolerance( xTolerance,
                                                   costFunction->GetNumberOfParameters() );
  itkOptimizer->SetFunctionConvergenceTolerance( fTolerance );
  itkOptimizer->SetCostFunction( costFunction.GetPointer() );

#if VERBOSE
     //observe the iterations
  CommandIterationUpdateParticleSwarm::Pointer observer =
    CommandIterationUpdateParticleSwarm::New();
  itkOptimizer->AddObserver( itk::IterationEvent(), observer );
#endif

  try
    {
    initialParameters[0] =  9;
    initialParameters[1] = -9;
    itkOptimizer->SetInitialPosition( initialParameters );
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters[0] ) > xTolerance ||
        fabs( finalParameters[1] - knownParameters[1] ) > xTolerance )
      {
      //std::cout << "[Test 2 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "[Test 2 FAILURE]" << std::endl;
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during Optimization" << std::endl;
    std::cout << "Location    = " << e.GetLocation()    << std::endl;
    std::cout << "Description = " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }
//  std::cout << "[Test 2 SUCCESS]" << std::endl;
  return EXIT_SUCCESS;
}

int IBPSOTest3()
{
#if VERBOSE
  std::cout << "Particle Swarm Optimizer Test 3 [f(x,y) = (1-x)^2 + 100(y-x^2)^2]\n";
  std::cout << "----------------------------------\n";
#endif

  itk::Array<double> knownParameters( 2 );
  knownParameters[0] = 1.0;
  knownParameters[1] = 1.0;

    //the function we want to optimize
  ParticleSwarmTestF3::Pointer costFunction =
    ParticleSwarmTestF3::New();

  OptimizerType::Pointer  itkOptimizer = OptimizerType::New();

         // set optimizer parameters
  OptimizerType::ParameterBoundsType bounds;
  bounds.push_back( std::make_pair( -100, 100 ) );
  bounds.push_back( std::make_pair( -100, 100 ) );
  unsigned int numberOfParticles = 100;
  unsigned int maxIterations = 200;
  double xTolerance = 0.1;
  double fTolerance = 0.01;
  OptimizerType::ParametersType initialParameters( 2 ), finalParameters;

  itkOptimizer->SetParameterBounds( bounds );
  itkOptimizer->SetNumberOfParticles( numberOfParticles );
  itkOptimizer->SetMaximalNumberOfIterations( maxIterations );
  itkOptimizer->SetParametersConvergenceTolerance( xTolerance,
                                                   costFunction->GetNumberOfParameters() );
  itkOptimizer->SetFunctionConvergenceTolerance( fTolerance );
  itkOptimizer->SetCostFunction( costFunction.GetPointer() );

#if VERBOSE
          //observe the iterations
  CommandIterationUpdateParticleSwarm::Pointer observer =
    CommandIterationUpdateParticleSwarm::New();
  itkOptimizer->AddObserver( itk::IterationEvent(), observer );
  itkOptimizer->AddObserver( itk::StartEvent(), observer );
#endif

  try
    {
    initialParameters[0] = 3;
    initialParameters[1] = 3;
    itkOptimizer->SetInitialPosition( initialParameters );
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters[0] ) > xTolerance ||
        fabs( finalParameters[1] - knownParameters[1] ) > xTolerance )
      {
      //std::cout << "[Test 3 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }

                //initial position near known minimum (1,1) - for PSO this
                //makes no difference when the initial swarm is generated using
                //a uniform distribution
    initialParameters[0] =  10;
    initialParameters[1] = 10;
    itkOptimizer->SetInitialPosition( initialParameters );
#if VERBOSE
          //reset the iteration count done by the observer
    observer->Reset();
#endif
    itkOptimizer->ClearSwarm();
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters[0] ) > xTolerance ||
        fabs( finalParameters[1] - knownParameters[1] ) > xTolerance )
      {
      //std::cout << "[Test 3 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }

                //initial position near known minimum (1,1) - potentially reduce
                //the number of iterations by initializing particles using a normal
                //distribution centered on initial parameter values
    initialParameters[0] =  10;
    initialParameters[1] = 10;
    itkOptimizer->SetInitialPosition( initialParameters );
    itkOptimizer->InitializeNormalDistributionOn();
#if VERBOSE
          //reset the iteration count done by the observer
    observer->Reset();
#endif
    itkOptimizer->ClearSwarm();
    itkOptimizer->StartOptimization();
    finalParameters = itkOptimizer->GetCurrentPosition();

              //check why we stopped and see if the optimization succeeded
#if VERBOSE
    std::cout<<"Reason for stopping optimization:\n";
    std::cout<<"\t"<<itkOptimizer->GetStopConditionDescription()<<"\n";
#endif
    finalParameters = itkOptimizer->GetCurrentPosition();
#if VERBOSE
    std::cout << "Known parameters   = " << knownParameters<<"   ";
    std::cout << "Estimated parameters = " << finalParameters << std::endl;
#endif
    if( fabs( finalParameters[0] - knownParameters[0] ) > xTolerance ||
        fabs( finalParameters[1] - knownParameters[1] ) > xTolerance )
      {
      //std::cout << "[Test 3 FAILURE]" << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << "[Test 3 FAILURE]" << std::endl;
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error ocurred during Optimization" << std::endl;
    std::cout << "Location    = " << e.GetLocation()    << std::endl;
    std::cout << "Description = " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
    }
//  std::cout << "[Test 3 SUCCESS]" << std::endl;
  return EXIT_SUCCESS;
}
