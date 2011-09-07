#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkInitializationBiasedParticleSwarmOptimizer.h"

namespace itk
{

InitializationBiasedParticleSwarmOptimizer
::InitializationBiasedParticleSwarmOptimizer(void)
{
  m_InertiaCoefficient = 0.7298;
  m_PersonalCoefficient = 1.49609;
  m_GlobalCoefficient = 1.49609;
  m_InitializationCoefficient = 1.49609;
}


void
InitializationBiasedParticleSwarmOptimizer
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os<<"Acceleration coefficients [inertia, personal, global, initialization]: ";
  os<<"["<<m_InertiaCoefficient<<", "<<m_PersonalCoefficient<<", ";
  os<<m_GlobalCoefficient<<", "<<m_InitializationCoefficient<<"]\n";
}


void
InitializationBiasedParticleSwarmOptimizer
::UpdateSwarm( void )
{
  unsigned int j, k, n;
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    randomGenerator = Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();
  ParametersType initialPosition = GetInitialPosition();

  n = static_cast<unsigned int>( ( GetCostFunction() )->GetNumberOfParameters() );
          //linear decrease in the weight of the initial parameter values
  double initializationCoefficient = m_InitializationCoefficient*
    ( 1.0 - static_cast<double>(m_IterationIndex)/static_cast<double>(m_MaximalNumberOfIterations));

  for( j=0; j<m_NumberOfParticles; j++ )
    {
    ParticleData & p = m_Particles[j];
    ParametersType::ValueType phi1, phi2, phi3;
    phi1 = randomGenerator->GetVariateWithClosedRange() *
           m_PersonalCoefficient;
    phi2 = randomGenerator->GetVariateWithClosedRange() *
           m_GlobalCoefficient;
    phi3 = randomGenerator->GetVariateWithClosedRange() *
           initializationCoefficient;
    for( k=0; k<n; k++ )
      {       //update velocity
      p.m_CurrentVelocity[k] =
        m_InertiaCoefficient*p.m_CurrentVelocity[k] +
        phi1*(p.m_BestParameters[k]-p.m_CurrentParameters[k]) +
        phi2*(m_ParametersBestValue[k]-p.m_CurrentParameters[k]) +
        phi3*(initialPosition[k]-p.m_CurrentParameters[k]);
                       //update location and ensure that it stays
                       //inside the feasible region
      p.m_CurrentParameters[k] += p.m_CurrentVelocity[k];
      if( p.m_CurrentParameters[k] < m_ParameterBounds[k].first )
        p.m_CurrentParameters[k] = m_ParameterBounds[k].first;
      else if (p.m_CurrentParameters[k] > m_ParameterBounds[k].second )
        p.m_CurrentParameters[k] = m_ParameterBounds[k].second;
      }       //evaluate function at new position
    p.m_CurrentValue = m_CostFunction->GetValue( p.m_CurrentParameters );
    if( p.m_CurrentValue < p.m_BestValue )
      {
      p.m_BestValue = p.m_CurrentValue;
      p.m_BestParameters = p.m_CurrentParameters;
      }
    }
}

}
