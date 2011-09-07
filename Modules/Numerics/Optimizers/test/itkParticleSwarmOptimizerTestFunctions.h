#ifndef __itkParticleSwarmOptimizerFunctions_h
#define __itkParticleSwarmOptimizerFunctions_h

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "itkCommand.h"
#include "itkParticleSwarmOptimizerBase.h"

/**
 * Function we want to optimize, comprised of two parabolas with C0 continuity
 * at 0:
 * f(x) = if(x<0) x^2+4x; else 2x^2-8x
 *
 * Minima are at -2 and 2 with function values of -4 and -8 respectivly.
 */
class ParticleSwarmTestF1 : public itk::SingleValuedCostFunction
{
public:

  typedef ParticleSwarmTestF1               Self;
  typedef itk::SingleValuedCostFunction     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );
  itkTypeMacro( ParticleSwarmTestF1, SingleValuedCostFunction );

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::MeasureType                 MeasureType;

  ParticleSwarmTestF1()
   {
   }

  double GetValue( const ParametersType & parameters ) const
    {
    double val;
    if(parameters[0]<0)
      val = parameters[0]*parameters[0]+4*parameters[0];
    else
      val = 2*parameters[0]*parameters[0]-8*parameters[0];
    return val;
    }

  void GetDerivative( const ParametersType & itkNotUsed(parameters),
                            DerivativeType & itkNotUsed(derivative) ) const
    {
      throw itk::ExceptionObject( __FILE__, __LINE__,
                                  "no derivative available" );
    }

  unsigned int GetNumberOfParameters(void) const
    {
      return 1;
    }
};


/**
 *  Function we want to optimize, quadratic:
 *
 *  1/2 x^T A x - b^T x
 *
 * where A = [ 3 2 ]  and b = [ 2 ]
 *           [ 2 6 ]          [-8 ]
 *
 * solution is [ 2 ] with a function value of 10.0
 *             [-2 ]
 *
 */
class ParticleSwarmTestF2 : public itk::SingleValuedCostFunction
{
public:

  typedef ParticleSwarmTestF2               Self;
  typedef itk::SingleValuedCostFunction     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );
  itkTypeMacro( ParticleSwarmTestF2, SingleValuedCostFunction );

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::DerivativeType              DerivativeType;
  typedef Superclass::MeasureType                 MeasureType;

  typedef vnl_vector<double>                      VectorType;
  typedef vnl_matrix<double>                      MatrixType;


  ParticleSwarmTestF2():m_A( 2, 2 ), m_b(2)
   {
    m_A[0][0] =  3;
    m_A[0][1] =  2;
    m_A[1][0] =  2;
    m_A[1][1] =  6;

    m_b[0]    =  2;
    m_b[1]    = -8;
    }

  double GetValue( const ParametersType & parameters ) const
    {
      return 0.5*( m_A(0,0)*parameters[0]*parameters[0] +
                   m_A(0,1)*parameters[0]*parameters[1] +
                   m_A(1,0)*parameters[0]*parameters[1] +
                   m_A(1,1)*parameters[1]*parameters[1] ) -
             m_b[0]*parameters[0] - m_b[1]*parameters[1];
    }

  void GetDerivative( const ParametersType & itkNotUsed(parameters),
                            DerivativeType & itkNotUsed(derivative) ) const
    {
      throw itk::ExceptionObject( __FILE__, __LINE__,
                                  "no derivative available" );
    }

  unsigned int GetNumberOfParameters(void) const
    {
    return 2;
    }

private:
  MatrixType        m_A;
  VectorType        m_b;
};


/**
 * The Rosenbrock function f(x,y) = (1-x)^2 + 100(y-x^2)^2
 * minimum is at (1,1) with f(x,y) = 0.
 */
class ParticleSwarmTestF3 : public itk::SingleValuedCostFunction
{
public:

  typedef ParticleSwarmTestF3               Self;
  typedef itk::SingleValuedCostFunction     Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;
  itkNewMacro( Self );
  itkTypeMacro( ParticleSwarmTestF3, SingleValuedCostFunction );

  typedef Superclass::ParametersType              ParametersType;
  typedef Superclass::MeasureType                 MeasureType;

  ParticleSwarmTestF3()
   {
   }

  double GetValue( const ParametersType & parameters ) const
    {
     return (1-parameters[0])*(1-parameters[0]) +
            100*(parameters[1]-parameters[0]*parameters[0])*
            (parameters[1]-parameters[0]*parameters[0]);
  }

  void GetDerivative( const ParametersType & itkNotUsed(parameters),
                            DerivativeType & itkNotUsed(derivative)) const
    {
      throw itk::ExceptionObject( __FILE__, __LINE__,
                                  "no derivative available" );
    }

  unsigned int GetNumberOfParameters(void) const
    {
      return 2;
    }
};


class CommandIterationUpdateParticleSwarm : public itk::Command
{
public:
  typedef  CommandIterationUpdateParticleSwarm   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

  void Reset() { m_IterationNumber = 0; }

  itkSetMacro( PrintOptimizer, bool );

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    const itk::ParticleSwarmOptimizerBase *optimizer =
      dynamic_cast< const itk::ParticleSwarmOptimizerBase * >( object );
    if( optimizer &&
        ( dynamic_cast< const itk::IterationEvent * >( &event ) ||
          dynamic_cast< const itk::StartEvent * >( &event ) ) )
      {
      std::cout << m_IterationNumber++ << ":  ";
      std::cout << "x: "<< optimizer->GetCurrentPosition() <<"  ";
      std::cout << "f(x): " << optimizer->GetValue() <<std::endl;
      if( m_PrintOptimizer )
        {
        itk::ParticleSwarmOptimizerBase::Pointer optimizerPtr =
          const_cast<itk::ParticleSwarmOptimizerBase *>(optimizer);
        std::cout<< optimizerPtr;
        }
      }
    }
protected:
  CommandIterationUpdateParticleSwarm()
  {
    m_IterationNumber=0;
    m_PrintOptimizer = false;
  }
private:
  unsigned long m_IterationNumber;
  bool m_PrintOptimizer;
};

#endif
