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
#ifndef __itkTransformParametersAdaptorBase_h
#define __itkTransformParametersAdaptorBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk
{
/** \class TransformParametersAdaptorBase
 * \brief Base helper class intended for multi-resolution image registration.
 *
 * During multi-resolution image registration, it is often useful to expand
 * the number of parameters describing the transform when going from one level
 * to the next.  For example, in B-spline registration, one often wants to
 * increase the mesh size (or, equivalently, the control point grid size) for
 * increased flexibility in optimizing the transform.  This requires the
 * propagation of the current transform solution to the next level where the
 * solution is identical but with an increase in the number of parameters.
 * This base class and those derived classes are meant to handle these types of
 * situations.
 *
 * Basic usage will involve the user specifying the required fixed parameters, i.e.
 *
 *   \code
 *   transformAdaptor->SetTransform( transform );
 *   transformAdaptor->SetRequiredFixedParameters( fixedParameters );
 *   transformAdaptor->AdaptTransformParameters();
 *   \endcode
 *
 * which will adjust the transform based on the new fixed parameters.
 *
 * \author Nick Tustison
 * \author Marius Staring
 *
 * \ingroup ITKRegistrationCommon
 */
template<class TTransform>
class TransformParametersAdaptorBase
: public Object
{
public:

  /** Standard class typedefs. */
  typedef TransformParametersAdaptorBase               Self;
  typedef Object                                       Superclass;
  typedef SmartPointer<Self>                           Pointer;
  typedef SmartPointer<const Self>                     ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( TransformParametersAdaptorBase, Object );

  /** Typedefs associated with the transform */
  typedef TTransform                                     TransformType;
  typedef typename TransformType::Pointer                TransformPointer;
  typedef typename TransformType::ParametersType         ParametersType;
  typedef typename ParametersType::ValueType             ParametersValueType;

  /** Set the transform to be adapted */
  itkSetObjectMacro( Transform, TransformType );

  /** Set the fixed parameters */
  itkSetMacro( RequiredFixedParameters, ParametersType );

  /** Get the fixed parameters */
  itkGetConstReferenceMacro( RequiredFixedParameters, ParametersType );

  /** Initialize the transform using the specified fixed parameters */
  virtual void AdaptTransformParameters() = 0;

protected:
  TransformParametersAdaptorBase() {}
  ~TransformParametersAdaptorBase() {}

  void PrintSelf( std::ostream & os, Indent indent ) const
  {
    this->m_Transform->Print( os, indent );
    os << "Fixed parameters" << this->m_RequiredFixedParameters << std::endl;
  }

  TransformPointer                           m_Transform;
  ParametersType                             m_RequiredFixedParameters;

private:
  TransformParametersAdaptorBase( const Self & ); //purposely not implemented
  void operator=( const Self & );             //purposely not implemented

}; //class TransformParametersAdaptorBase
}  // namespace itk

#endif /* __itkTransformParametersAdaptorBase_h */
