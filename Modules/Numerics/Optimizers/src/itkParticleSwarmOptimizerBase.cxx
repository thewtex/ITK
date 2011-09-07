#include <algorithm>
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkParticleSwarmOptimizerBase.h"

namespace itk
{


ParticleSwarmOptimizerBase
::ParticleSwarmOptimizerBase(void)
{
  m_PrintSwarm = false;
  m_InitializeNormalDistribution = false;
  m_NumberOfParticles = 35;
  m_MaximalNumberOfIterations = 200;
  m_NumberOfGenerationsWithMinimalImprovement = 1;
  m_ParametersConvergenceTolerance.Fill( 1e-8 );
  m_PercentageParticlesConverged = 0.6;
  m_FunctionConvergenceTolerance = 1e-4;
}


void
ParticleSwarmOptimizerBase
::SetNumberOfParticles( unsigned int n )
{
  if( !m_Particles.empty() && n!=m_Particles.size() )
    {
    itkExceptionMacro(<<"swarm already set with different size, clear swarm and then set")
    }
  if( m_NumberOfParticles != n )
    {
    m_NumberOfParticles = n;
    Modified();
    }
}


void
ParticleSwarmOptimizerBase
::SetInitialSwarm( std::vector<ParticleData> &initialSwarm )
{
  std::vector<ParticleData>::iterator it, end;
  end = initialSwarm.end();
  unsigned int n;
  if( !initialSwarm.empty() )
    {
    n = initialSwarm[0].m_CurrentParameters.GetSize();
    }
            //check that the dimensions of the swarm data are consistent
  for( it=initialSwarm.begin(); it!=end; it++ )
    {
      if( (*it).m_CurrentParameters.GetSize() != n ||
          (*it).m_CurrentVelocity.GetSize() != n ||
          (*it).m_BestParameters.GetSize() != n )
        itkExceptionMacro(<<"inconsistent dimensions in swarm data")
    }
  m_Particles.clear();
  m_Particles.insert( m_Particles.begin(),
                      initialSwarm.begin(), initialSwarm.end() );
  if( !m_Particles.empty() )
    {
    m_NumberOfParticles = m_Particles.size();
    }
  Modified();
}


void
ParticleSwarmOptimizerBase
::ClearSwarm()
{
  if( !m_Particles.empty() )
    {
    m_Particles.clear();
    Modified();
    }
}


void
ParticleSwarmOptimizerBase
::SetParameterBounds( ParameterBoundsType & bounds )
{
  m_ParameterBounds.clear();
  m_ParameterBounds.insert( m_ParameterBounds.begin(),
                            bounds.begin(), bounds.end() );
  Modified();
}


void
ParticleSwarmOptimizerBase
::SetParameterBounds( std::pair<ParametersType::ValueType,
                      ParametersType::ValueType> &bounds,
                      unsigned int n )
{
  m_ParameterBounds.clear();
  m_ParameterBounds.insert( m_ParameterBounds.begin(), n, bounds );
  Modified();
}


ParticleSwarmOptimizerBase::ParameterBoundsType
ParticleSwarmOptimizerBase
::GetParameterBounds()
{
  return m_ParameterBounds;
}


void
ParticleSwarmOptimizerBase
::SetParametersConvergenceTolerance( ParametersType::ValueType
                                     convergenceTolerance, unsigned int sz )
{
  m_ParametersConvergenceTolerance.SetSize( sz );
  m_ParametersConvergenceTolerance.Fill( convergenceTolerance );
}


const ParticleSwarmOptimizerBase::CostFunctionType::MeasureType
ParticleSwarmOptimizerBase
::GetValue() const
{
  return m_FunctionBestValue;
}


const std::string
ParticleSwarmOptimizerBase
::GetStopConditionDescription() const
{
  return m_StopConditionDescription.str();
}


void
ParticleSwarmOptimizerBase
::PrintSelf( std::ostream& os, Indent indent ) const
{

  Superclass::PrintSelf( os, indent );
  os<<indent<<"Create swarm using [normal, uniform] distribution: ";
  os<<"["<<m_InitializeNormalDistribution<<", ";
  os<<!m_InitializeNormalDistribution<<"]\n";
  os<<indent<<"Number of particles in swarm: "<<m_NumberOfParticles<<"\n";
  os<<indent<<"Maximal number of iterations: "<<m_MaximalNumberOfIterations<<"\n";
  os<<indent<<"Number of generations with minimal improvement: ";
  os<<m_NumberOfGenerationsWithMinimalImprovement<<"\n";
  ParameterBoundsType::const_iterator it, end;
  end = m_ParameterBounds.end();
  os<<indent<<"Parameter bounds: [";
  for( it=m_ParameterBounds.begin(); it!=end; it++ )
    os<<" ["<<(*it).first<<", "<<(*it).second<<"]";
  os<<" ]\n";
  os<<indent<<"Parameters' convergence tolerance: "<<m_ParametersConvergenceTolerance;
  os<<"\n";
  os<<indent<<"Function convergence tolerance: "<<m_FunctionConvergenceTolerance;
  os<<"\n";
          //printing the swarm, usually should be avoided (too much information)
  if( m_PrintSwarm && !m_Particles.empty() )
    {
    os<<indent<<"swarm data [current_parameters current_velocity current_value ";
    os<<"best_parameters best_value]:\n";
    PrintSwarm( os, indent );
    }
}


void
ParticleSwarmOptimizerBase
::PrintSwarm( std::ostream& os, Indent indent ) const
{
  std::vector<ParticleData>::const_iterator it, end;
  end = m_Particles.end();
  os<<indent<<"[\n";
  for( it = m_Particles.begin(); it!=end; it++ )
    {
    const ParticleData & p = *it;
    os<<indent;
    PrintParamtersType( p.m_CurrentParameters, os );
    os<<" ";
    PrintParamtersType( p.m_CurrentVelocity, os );
    os<<" "<<p.m_CurrentValue<<" ";
    PrintParamtersType( p.m_BestParameters, os );
    os<<" "<<p.m_BestValue<<"\n";
    }
  os<<indent<<"]\n";
}


void
ParticleSwarmOptimizerBase
::PrintParamtersType( const ParametersType & x, std::ostream & os ) const
{
  unsigned int sz = x.size();
  for ( unsigned int i=0; i<sz; i++ )
    {
    os << x[i] << " ";
    }
}


void
ParticleSwarmOptimizerBase
::StartOptimization( void )
{
  unsigned int j, k, n, index, prevIndex;
  bool converged = false;
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    randomGenerator = Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();
  unsigned int bestValueMemorySize =
    m_NumberOfGenerationsWithMinimalImprovement+1;
  unsigned int percentileIndex =
    static_cast<unsigned int>( m_PercentageParticlesConverged*
                               (m_NumberOfParticles-1) + 0.5 );

  ValidateSettings();
          //initialize particle locations, velocities, etc.
  Initialize();

  InvokeEvent( StartEvent() );

          //run the simulation
  n = static_cast<unsigned int>( ( GetCostFunction() )->GetNumberOfParameters() );
  for( m_IterationIndex=1; m_IterationIndex<m_MaximalNumberOfIterations && !converged; m_IterationIndex++ )
    {

    UpdateSwarm();

              //update the best function value/parameters
    for( j=0; j<m_NumberOfParticles; j++ )
      {
      if( m_Particles[j].m_BestValue < m_FunctionBestValue )
        {
        m_FunctionBestValue = m_Particles[j].m_BestValue;
        m_ParametersBestValue = m_Particles[j].m_BestParameters;
        }
      }

    SetCurrentPosition( m_ParametersBestValue );

                //update the best value memory
    index = m_IterationIndex%bestValueMemorySize;
    m_FunctionBestValueMemory[index] = m_FunctionBestValue;
              //check for convergence. the m_FunctionBestValueMemory is a ring
              //buffer with m_NumberOfGenerationsWithMinimalImprovement+1
              //elements. the optimizer has converged if: (a) the difference
              //between the first and last elements currently in the ring buffer
              //is less than the user specificed threshold. and (b) the particles
             //are close enough to the best particle.
    if( m_IterationIndex>=m_NumberOfGenerationsWithMinimalImprovement )
      {
      if( index ==  m_NumberOfGenerationsWithMinimalImprovement )
        prevIndex = 0;
      else
        prevIndex = index+1;
                  //function value hasn't improved for a while, check the
                  //parameter space to see if the "best" swarm has collapsed
                  //around the swarm's best parameters, indicating convergence
      if( ( m_FunctionBestValueMemory[prevIndex] -
            m_FunctionBestValueMemory[index] ) <
            m_FunctionConvergenceTolerance )
        {
        converged = true;
        std::vector<ParametersType::ValueType> parameterDiffs( m_NumberOfParticles );
        for( k=0; k<n && converged; k++ )
          {
          for( j=0; j<m_NumberOfParticles; j++ )
            {
              parameterDiffs[j] =
                fabs( m_Particles[j].m_BestParameters[k] -
                      m_ParametersBestValue[k] );
            }
          std::nth_element( parameterDiffs.begin(),
                            parameterDiffs.begin()+percentileIndex,
                            parameterDiffs.end() );
          converged = converged &&
                      parameterDiffs[percentileIndex] < m_ParametersConvergenceTolerance[k];
          }
        }
      }
    InvokeEvent( IterationEvent() );
    }

    m_StopConditionDescription << GetNameOfClass() << ": ";
    if( converged )
      m_StopConditionDescription << "successfuly converged after "<< m_IterationIndex <<" iterations";
    else
      m_StopConditionDescription << "terminated after "<< m_IterationIndex <<" iterations";
    InvokeEvent( EndEvent() );
}


void
ParticleSwarmOptimizerBase
::ValidateSettings()
{
  unsigned int i,n;

               //we have to have a cost function
  if( GetCostFunction() == NULL )
    {
    itkExceptionMacro(<<"NULL cost function")
    }
        //if we got here it is safe to get the number of parameters the cost
        //function expects
  n =
    static_cast<unsigned int>( ( GetCostFunction() )->GetNumberOfParameters() );

        //check that the number of parameters match
  ParametersType initialPosition = GetInitialPosition();
  if( initialPosition.Size() != n )
    {
    itkExceptionMacro(<<"cost function and initial position dimensions mismatch")
    }
              //at least one particle
  if( m_NumberOfParticles == 0 )
    {
    itkExceptionMacro(<<"number of particles is zero")
    }
               //at least one iteration (the initialization phase)
  if( m_MaximalNumberOfIterations == 0 )
    {
    itkExceptionMacro(<<"number of iterations is zero")
    }
                    //we need at least one generation difference to
                    //compare to the previous one
  if( m_NumberOfGenerationsWithMinimalImprovement == 0 )
    {
    itkExceptionMacro(<<"number of generations with minimal improvement is zero")
    }

  if( m_ParameterBounds.size() != n )
    {
    itkExceptionMacro(<<"cost function and parameter bounds dimensions mismatch")
    }
  for( i=0; i<n; i++ )
    {
      if( initialPosition[i] < m_ParameterBounds[i].first ||
          initialPosition[i] > m_ParameterBounds[i].second )
        {
        itkExceptionMacro(<<"initial position is outside specified parameter bounds")
        }
    }
        //if the user set an initial swarm, check that the number of parameters
        //match and that they are inside the feasable region
  if( !m_Particles.empty() )
    {
    if(m_Particles[0].m_CurrentParameters.GetSize() != n )
      {
      itkExceptionMacro(<<"cost function and particle data dimensions mismatch")
      }
    std::vector<ParticleData>::iterator it, end = m_Particles.end();
    for( it=m_Particles.begin(); it!=end; it++ )
      {
      ParticleData &p = (*it);
      for( i=0; i<n; i++ )
        {
          if( p.m_CurrentParameters[i] < m_ParameterBounds[i].first ||
              p.m_CurrentParameters[i] > m_ParameterBounds[i].second ||
              p.m_BestParameters[i] < m_ParameterBounds[i].first ||
              p.m_BestParameters[i] > m_ParameterBounds[i].second )
          {
          itkExceptionMacro(<<"initial position is outside specified parameter bounds")
          }
        }
      }
    }
             //parameters' convergence tolerance has to be positive
  for( i=0; i<n; i++ )
    {
    if ( m_ParametersConvergenceTolerance[i] < 0 )
      {
      itkExceptionMacro(<<"negative parameters convergence tolerance")
      }
    }
           //function convergence tolerance has to be positive
  if ( m_FunctionConvergenceTolerance < 0 )
    {
      itkExceptionMacro(<<"negative function convergence tolerance")
    }
}

void
ParticleSwarmOptimizerBase
::Initialize()
{
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    randomGenerator = Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();
  randomGenerator->SetSeed();

  m_StopConditionDescription.str("");

  SetCurrentPosition( GetInitialPosition() );

  m_IterationIndex = 0;

  m_FunctionBestValueMemory.resize( m_NumberOfGenerationsWithMinimalImprovement
                                    + 1 );
              //user did not supply initial swarm
  if( m_Particles.empty() )
    {
    RandomInitialization();
    }
            //initialize best function value
  m_FunctionBestValue =
    itk::NumericTraits<CostFunctionType::MeasureType>::max();
  for(unsigned int i=0; i<m_Particles.size(); i++ )
    {
    if( m_FunctionBestValue > m_Particles[i].m_BestValue )
      {
      m_FunctionBestValue =  m_Particles[i].m_BestValue;
      m_ParametersBestValue = m_Particles[i].m_BestParameters;
      }
    }
  m_FunctionBestValueMemory[0] = m_FunctionBestValue;
}


void
ParticleSwarmOptimizerBase
::RandomInitialization()
{
  unsigned int i, j, n;
  n = GetInitialPosition().Size();
  ParameterBoundsType parameterBounds( m_ParameterBounds );
  ParametersType mean = GetInitialPosition();
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    randomGenerator = Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();

               //create swarm
  m_Particles.resize( m_NumberOfParticles );
  for( i=0; i<m_NumberOfParticles; i++ )
    {
      m_Particles[i].m_BestParameters.SetSize( n );
      m_Particles[i].m_CurrentParameters.SetSize( n );
      m_Particles[i].m_CurrentVelocity.SetSize( n );
    }

    //user supplied initial position is always one of the particles
  m_Particles[0].m_CurrentParameters = mean;

           //create particles with a normal distribution around the initial
           //parameter values, inside the feasible region
  if( m_InitializeNormalDistribution )
    {
    ParametersType variance( mean.GetSize() );
           //variance is set so that 3sigma == bounds
    for( i=0; i<n; i++ )
      {
        variance[i] = ( parameterBounds[i].second - parameterBounds[i].first )/3.0;
        variance[i]*= variance[i];
      }

    for( i=1; i<m_NumberOfParticles; i++ )
      {
      for( j=0; j<n; j++ )
        {
        m_Particles[i].m_CurrentParameters[j] =
          randomGenerator->GetNormalVariate( mean[j], variance[j] );
             //ensure that the particle is inside the feasible region
        if( m_Particles[i].m_CurrentParameters[j] < parameterBounds[j].first ||
            m_Particles[i].m_CurrentParameters[j] > parameterBounds[j].second )
          {
            j--;
          }
        }
      }
    }
     //create particles with uniform distribution inside the feasible region
  else
    {
    for( i=1; i<m_NumberOfParticles; i++ )
      {
      for( j=0; j<n; j++ )
        {
        m_Particles[i].m_CurrentParameters[j] =
          randomGenerator->GetUniformVariate( parameterBounds[j].first,
                                              parameterBounds[j].second );
        }
      }
    }

         //initialize the particles' velocity, so that x_i+v_i is inside the
         //feasible region, and set the best parameters to the current ones
  for( i=0; i<m_NumberOfParticles; i++ )
    {
    for( j=0; j<n; j++ )
      {
      m_Particles[i].m_CurrentVelocity[j] =
          ( randomGenerator->GetUniformVariate( parameterBounds[j].first,
                                                parameterBounds[j].second ) -
           m_Particles[i].m_CurrentParameters[j] );
      m_Particles[i].m_BestParameters[j] =
        m_Particles[i].m_CurrentParameters[j];
      }
    }
            //initial function evaluations
  for( i=0; i<m_NumberOfParticles; i++ )
    {
    m_Particles[i].m_CurrentValue =
      m_CostFunction->GetValue( m_Particles[i].m_CurrentParameters );
    m_Particles[i].m_BestValue = m_Particles[i].m_CurrentValue;
    }
}

}
