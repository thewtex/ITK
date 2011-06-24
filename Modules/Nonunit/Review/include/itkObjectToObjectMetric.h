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
#ifndef __itkObjectToObjectMetric_h
#define __itkObjectToObjectMetric_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "itkArray.h"

namespace itk
{

/** \class ObjectToObjectMetric
 * \brief Computes similarity between regions of two objects.
 *
 * This is the base class for a hierarchy of similarity metrics that may, in
 * derived classes, operate on meshes, images, etc.  This class computes a
 * value that measures the similarity between the two objects.
 *
 * \ingroup RegistrationMetrics
 *
 * \ingroup ITK-Review
 */

class ITK_EXPORT ObjectToObjectMetric:
  public Object
{
public:
  /** Standard class typedefs. */
  typedef ObjectToObjectMetric       Self;
  typedef Object                     Superclass;
  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( ObjectToObjectMetric, Object );

  /**  Type of the measure. */
  typedef double MeasureType;

  /**  Type of the derivative. */
  typedef double                              DerivativeValueType;
  typedef Array<DerivativeValueType>          DerivativeType;

  /** Type of coordinate system used to calculate values, derivatives */
  typedef enum { Fixed = 0, Moving, Both }    DerivativeSourceType;

  /**
   * Set source of derivative.  This variable allows the user to switch
   * between calculating the derivative with respect to the fixed
   * object or moving object.
   */
  itkSetMacro( DerivativeSource, DerivativeSourceType );

  /**
   * Get source of derivative.  This variable allows the user to switch
   * between calculating the derivative with respect to the fixed object
   * or moving object.
   */
  itkGetConstMacro( DerivativeSource, DerivativeSourceType );

  /**
   * Initialize the metric by making sure that all the components
   *  are present and plugged together correctly     .
   */
  virtual void Initialize( void ) throw ( ExceptionObject ) = 0;

  /** This method returns the value of the cost function */
  virtual MeasureType GetValue() const = 0;

  /** This method returns the derivative of the cost function */
  virtual void GetDerivative( DerivativeType & derivative ) const
  {
    MeasureType value;
    this->GetValueAndDerivative( value, derivative );
  }

  /** This method returns the value and derivative of the cost function */
  virtual void GetValueAndDerivative( MeasureType &,
    DerivativeType & ) const = 0;

protected:
  ObjectToObjectMetric() : m_DerivativeSource( Moving ) {};
  virtual ~ObjectToObjectMetric() {};

  void PrintSelf( std::ostream & os, Indent indent ) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Derivative source: ";
    switch( this->m_DerivativeSource )
      {
      case Fixed:
        {
        os << "Fixed" << std::endl;
        break;
        }
      case Moving:
        {
        os << "Moving" << std::endl;
        break;
        }
      case Both:
        {
        os << "Both" << std::endl;
        break;
        }
      default:
        {
        os << "Unknown" << std::endl;
        break;
        }
      }
  }

  DerivativeSourceType                      m_DerivativeSource;

private:
  ObjectToObjectMetric( const Self & ); //purposely not implemented
  void operator=( const Self & );     //purposely not implemented

};
} // end namespace itk

#endif
