#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkParticleSwarmOptimizer.h"

namespace itk
{


ParticleSwarmOptimizer
::ParticleSwarmOptimizer(void) :
  DEFAULT_INERTIA_COEFFICIENT( 0.7298 ),
  DEFAULT_PERSONAL_COEFFICIENT( 1.49609 ),
  DEFAULT_GLOBAL_COEFFICIENT( 1.49609 )
{
  m_InertiaCoefficient = DEFAULT_INERTIA_COEFFICIENT;
  m_PersonalCoefficient = DEFAULT_PERSONAL_COEFFICIENT;
  m_GlobalCoefficient = DEFAULT_GLOBAL_COEFFICIENT;
}


void
ParticleSwarmOptimizer
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os<<"Acceleration coefficients [inertia, personal, global]: ";
  os<<"["<<m_InertiaCoefficient<<", "<<m_PersonalCoefficient<<", ";
  os<<m_GlobalCoefficient<<"]\n";
}


void
ParticleSwarmOptimizer
::UpdateSwarm( void )
{
  unsigned int j, k, n;
  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer
    randomGenerator = Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();

  n = static_cast<unsigned int>( ( GetCostFunction() )->GetNumberOfParameters() );

  for( j=0; j<m_NumberOfParticles; j++ )
    {
    ParticleData & p = m_Particles[j];
    ParametersType::ValueType phi1, phi2;
    phi1 = randomGenerator->GetVariateWithClosedRange() * m_PersonalCoefficient;
    phi2 = randomGenerator->GetVariateWithClosedRange() * m_GlobalCoefficient;
    for( k=0; k<n; k++ )
      {       //update velocity
      p.m_CurrentVelocity[k] =
        m_InertiaCoefficient*p.m_CurrentVelocity[k] +
        phi1*(p.m_BestParameters[k]-p.m_CurrentParameters[k]) +
        phi2*(m_ParametersBestValue[k]-p.m_CurrentParameters[k]);
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
