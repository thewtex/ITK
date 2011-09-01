#ifndef __itkParticleSwarmOptimizerBase_h
#define __itkParticleSwarmOptimizerBase_h

#include "itkSingleValuedNonLinearOptimizer.h"

namespace itk
{
/** \class ParticleSwarmOptimizerBase
 * \brief Abstract implementation of a Particle Swarm Optimization (PSO) algorithm.
 *
 * The PSO algorithm was originally presented in:<br>
 * J. Kennedy, R. Eberhart, "Particle Swarm Optimization",
 * Proc. IEEE Int. Neural Networks, 1995.<br>
 *
 * The algorithm is a stochastic global search optimization approach.
 * Optimization is performed by maintaining a swarm of particles that traverse
 * the parameter space, searching for the optimal function value. Associated
 * with each particle are its location and speed, in parameter space.
 *
 * Swarm initialization is performed within the user supplied parameter bounds
 * using either a uniform distribution or a normal distribution centered on
 * the initial parameter values supplied by the user. The search terminates when
 * the maximal number of iterations has been reached or when the change in the
 * best value in the past \f$g\f$ generations is below a threshold and the swarm
 * has collapsed (i.e. best personal particle locations are close to the
 * swarm's best location in parameter space).
 *
 * The actual optimization procedure, updating the swarm, is performed in the
 * subclasses, required to implement the UpdateSwarm() method.
 *
 * NOTE: This implementation only performs minimization.
 *
 * \ingroup Numerics Optimizers
 * \ingroup ITK-Optimizers
 */
class ITK_EXPORT ParticleSwarmOptimizerBase :
  public SingleValuedNonLinearOptimizer
{
public:
  /** Standard "Self" typedef. */
  typedef ParticleSwarmOptimizerBase          Self;
  typedef SingleValuedNonLinearOptimizer      Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( ParticleSwarmOptimizerBase, SingleValuedNonLinearOptimizer )

  typedef std::vector< std::pair<ParametersType::ValueType,
                                 ParametersType::ValueType> > ParameterBoundsType;

  struct ParticleData
  {
    ParametersType m_CurrentParameters;
    ParametersType m_CurrentVelocity;
    CostFunctionType::MeasureType m_CurrentValue;
    ParametersType m_BestParameters;
    CostFunctionType::MeasureType m_BestValue;
  };

  /** Specify whether to initialize the particles using a normal distribution
    * centered on the user supplied initial value or a uniform distribution.
    * If the optimum is expected to be near the initial value it is likely
    * that initializing with a normal distribution will result in faster
    * convergence.*/
  itkSetMacro( InitializeNormalDistribution, bool )
  itkGetMacro( InitializeNormalDistribution, bool )
  itkBooleanMacro( InitializeNormalDistribution )

  /**
   * Specify the initial swarm. Useful for evaluating PSO variants. If the
   * initial swarm is set it will be used. To revert to random initialization
   * (uniform or normal particle distributions) set using an empty swarm.
   */
  void SetInitialSwarm( std::vector<ParticleData> &initialSwarm );
  void ClearSwarm();

  /**
   * Inidicate whether or not to output the swarm information when printing an
   * object. By default this option is turned off as it generates too much
   * information.
   */
  itkSetMacro( PrintSwarm, bool )
  itkBooleanMacro( PrintSwarm )

  /** Start optimization. */
  void StartOptimization( void );


  /** Set/Get number of particles in the swarm - the maximal number of function
      evaluations is m_MaximalNumberOfIterations*m_NumberOfParticles */
  void SetNumberOfParticles( unsigned int n );
  itkGetMacro( NumberOfParticles, unsigned int )

  /** Set/Get maximal number of iterations - the maximal number of function
      evaluations is m_MaximalNumberOfIterations*m_NumberOfParticles */
  itkSetMacro( MaximalNumberOfIterations, unsigned int )
  itkGetMacro( MaximalNumberOfIterations, unsigned int )

  /** Set/Get the number of generations to continue with minimal improvement in
   *  the function value, |f_best(g_i) - f_best(g_k)|<threshold where
   *  k <= i+NumberOfGenerationsWithMinimalImprovement
   *  Minimal value is one.*/
  itkSetMacro( NumberOfGenerationsWithMinimalImprovement, unsigned int )
  itkGetMacro( NumberOfGenerationsWithMinimalImprovement, unsigned int )

  /**Set/Get the parameter bounds. Search for optimal value is inside these
     bounds. NOTE: It is assumed that the first entry is the minimal value,
     second is the maximal value. */
  virtual void SetParameterBounds( ParameterBoundsType & bounds );
  void SetParameterBounds( std::pair<ParametersType::ValueType,
                           ParametersType::ValueType> &bounds,
                           unsigned int n );

  ParameterBoundsType GetParameterBounds();

    /** The optimization algorithm will terminate when the function improvement
     *  in the last m_NumberOfGenerationsWithMinimalImprovement generations
     *  is less than m_FunctionConvergenceTolerance and the maximal distance
     *  between particles and the best particle in each dimension is less than
     *  m_ParametersConvergenceTolerance[i] for the specified percentage of the
     *  particles.
     *  That is, we haven't improved the best function value for a while and in
     *  the parameter space most (m%) of our particles attained their best value
     *  close to the swarm's best value.
     *  NOTE: The use of different tolerances for each dimension is desired when
     *         optimizing over non-commensurate parameters (e.g. rotation and
     *         translation). Alternatively, we could use ITK's parameter scaling
     *         approach. The current approach seems more intuitive.
     */
  itkSetMacro( FunctionConvergenceTolerance, CostFunctionType::MeasureType )
  itkGetMacro( FunctionConvergenceTolerance, CostFunctionType::MeasureType )
  /**Set parameters convergence tolerance using the same value for all, sz,
     parameters*/
  void SetParametersConvergenceTolerance( ParametersType::ValueType
                                          convergenceTolerance,
                                          unsigned int sz );
  itkSetMacro( ParametersConvergenceTolerance, ParametersType )
  itkGetMacro( ParametersConvergenceTolerance, ParametersType )
  itkGetMacro( PercentageParticlesConverged, double )
  itkSetMacro( PercentageParticlesConverged, double )

  /** Get the function value for the current position.
   *  NOTE: This value is only valid during and after the execution of the
   *        StartOptimization() method.*/
  const CostFunctionType::MeasureType GetValue() const;

  /** Get the reason for termination */
  virtual const std::string GetStopConditionDescription() const;

  /** Print the swarm information to the given output stream. Each line
   * (particle data) is of the form:
   * current_parameters current_velocity current_value best_parameters best_value
   */
  void PrintSwarm( std::ostream& os, Indent indent ) const;

  /**Expose the use of defualt values, possibly make class variables (static)?*/
  const unsigned int DEFAULT_NUMBER_OF_PARTICLES;
  const unsigned int DEFAULT_MAXIMAL_NUMBER_OF_ITERATIONS;
  const ParametersType::ValueType DEFAULT_PARAMETERS_CONVERGENCE_TOLERANCE;
  const double DEFAULT_PERCENTAGE_PARTICLES_CONVERGED;
  const CostFunctionType::MeasureType DEFAULT_FUNCTION_CONVERGENCE_TOLERANCE;
  const unsigned int DEFAULT_NUMBER_GENERATIONS_MIN_IMPROVEMENT;


protected:
  ParticleSwarmOptimizerBase();
  virtual ~ParticleSwarmOptimizerBase() {};
  void PrintSelf( std::ostream& os, Indent indent ) const;
  void PrintParamtersType(  const ParametersType& x, std::ostream& os ) const;

  /**
   * Implement your update rule in this function.*/
  virtual void UpdateSwarm() = 0;

  ParticleSwarmOptimizerBase( const Self& ); //purposely not implemented
  void operator=( const Self& );//purposely not implemented

  virtual void ValidateSettings();

  /**
   * Initialize the particle swarm, and seed the random number generator.
   */
  virtual void Initialize();

  void RandomInitialization();
  void FileInitialization();

  bool                                         m_PrintSwarm;
  std::ostringstream                           m_StopConditionDescription;
  bool                                         m_InitializeNormalDistribution;
  unsigned int                                 m_NumberOfParticles;
  unsigned int                                 m_MaximalNumberOfIterations;
  unsigned int                                 m_NumberOfGenerationsWithMinimalImprovement;
  ParameterBoundsType                          m_ParameterBounds;
  ParametersType                               m_ParametersConvergenceTolerance;
  double                                       m_PercentageParticlesConverged;
  CostFunctionType::MeasureType                m_FunctionConvergenceTolerance;
  std::vector<ParticleData>                    m_Particles;
  CostFunctionType::MeasureType                m_FunctionBestValue;
  std::vector<CostFunctionType::MeasureType>   m_FunctionBestValueMemory;
  ParametersType                               m_ParametersBestValue;
  unsigned int                                 m_IterationIndex;
  //std::string                                  m_InitialSwarmFileName;
};

} // end namespace itk

#endif
