/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCompositeTransform.h,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCompositeTransform_h
#define __itkCompositeTransform_h

#include "itkTransform.h"

#include <deque>

namespace itk
{

/** \class CompositeTransform
 *
 * \ingroup Transforms
 *
 */
template
  <class TScalar = double, unsigned int NDimensions = 3>
class ITK_EXPORT CompositeTransform :
  public Transform<TScalar, NDimensions, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef CompositeTransform                                Self;
  typedef Transform<TScalar, NDimensions, NDimensions>      Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( CompositeTransform, Transform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  typedef Superclass TransformType;

  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer  InverseTransformBasePointer;

  /** Scalar type. */
  typedef typename Superclass::ScalarType  ScalarType;

   /** Parameters type. */
  typedef typename Superclass::ParametersType ParametersType;

  /** Jacobian type. */
  typedef typename Superclass::JacobianType  JacobianType;

  /** Standard coordinate point type for this class. */
  typedef typename Superclass::InputPointType   InputPointType;
  typedef typename Superclass::OutputPointType  OutputPointType;

  /** Standard vector type for this class. */
  typedef typename Superclass::InputVectorType   InputVectorType;
  typedef typename Superclass::OutputVectorType  OutputVectorType;

  /** Standard covariant vector type for this class */
  typedef typename Superclass::InputCovariantVectorType
    InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType
    OutputCovariantVectorType;

  /** Standard vnl_vector type for this class. */
  typedef typename Superclass::InputVnlVectorType   InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType  OutputVnlVectorType;

  /** Dimension of the domain spaces. */
  itkStaticConstMacro( InputDimension, unsigned int, NDimensions );
  itkStaticConstMacro( OutputDimension, unsigned int, NDimensions );

  /** Transform container object. */
  typedef std::deque<typename Superclass::Pointer> TransformQueueType;

  /** Functionality for composing transforms */

  /** Add transforms to the front and the back of the queue */
  void PushBackTransform( TransformType *t  )
    { this->m_TransformQueue.push_back( t ); }
  void PushFrontTransform( TransformType *t )
    { this->m_TransformQueue.push_front( t ); }

  /** Remove transforms from the front and the back of the queue */
  void PopBackTransform() { this->m_TransformQueue.pop_back(); }
  void PopFrontTransform() { this->m_TransformQueue.pop_front(); }
  void SetActiveTransform( unsigned int i ) { this->m_ActiveTransform=i; }


  /** Get the parameters that uniquely define the transform This is typically
   * used by optimizers during the process of image registration.  The parameters
   * are organized as {scale X, scale Y, scale Z } = { parameter[0],
   * parameter[1], parameter[2] } respectively */
//  const ParametersType & GetParameters(void) const;

  /** See transforms at the front and the back of the queue */
  typename Superclass::Pointer GetFrontTransform()
    { return this->m_TransformQueue.front(); };
  typename Superclass::Pointer GetBackTransform()
    { return this->m_TransformQueue.back(); };
  typename Superclass::Pointer GetNthTransform( unsigned int n )
    { return this->m_TransformQueue[n]; };

  /** Misc. functionality */
  bool IsTransformQueueEmpty() { return this->m_TransformQueue.empty(); }
  unsigned long GetNumberOfComposingTransforms()
    { return this->m_TransformQueue.size(); }
  void ClearTransformQueue() { this->m_TransformQueue.clear(); }

  /** Return an inverse of this transform. */
  bool GetInverse( Self *inverse ) const;
  virtual InverseTransformBasePointer GetInverseTransform() const;

  /** Compute the position of point in the new space */
  virtual OutputPointType TransformPoint( const InputPointType &inputPoint ) const
    {
    bool isInside = true;
    return this->TransformPoint( inputPoint, isInside );
    }
  virtual OutputPointType TransformPoint( const InputPointType& thisPoint,
    bool &isInsideTransformRegion ) const;

  /**  Method to transform a vector. */
  virtual OutputVectorType TransformVector(const InputVectorType &) const
    { itkExceptionMacro( "TransformVector unimplemented" ); }

  /**  Method to transform a vnl_vector. */
  virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &) const
    { itkExceptionMacro( "TransformVector unimplemented" ); }

  /**  Method to transform a CovariantVector. */
  virtual OutputCovariantVectorType
    TransformCovariantVector(const InputCovariantVectorType &) const
    { itkExceptionMacro( "TransformCovariantVector unimplemented" ); }

  virtual bool IsLinear() const;

  /**
   * Compute the jacobian with respect to the parameters.  Since there are
   * no parameters for this transform, the Jacobian shouldn't be requested.
   */
  virtual const JacobianType & GetJacobian(const InputPointType  &) const;

  virtual const ParametersType & GetParameters(void) const;

  virtual void SetParameters(const ParametersType & p)
  {
    typename TransformType::Pointer t=const_cast< Self * >( this )->GetNthTransform(this->m_ActiveTransform);
    t->SetParameters(p);
    return;
  }

  virtual const ParametersType & GetFixedParameters(void) const
  {
    typename TransformType::Pointer t=const_cast< Self * >( this )->GetNthTransform(this->m_ActiveTransform);
    return t->GetFixedParameters();
  }

  /** Set the fixed parameters and update internal transformation. */
  virtual void SetFixedParameters(const ParametersType &)
    { itkExceptionMacro("SetFixedParameters unimplemented."); }

  virtual unsigned int GetNumberOfParameters(void) const
  {
    typename TransformType::Pointer t=const_cast< Self * >( this )->GetNthTransform(this->m_ActiveTransform);
    return t->GetNumberOfParameters();
  }

protected:
  CompositeTransform();
  virtual ~CompositeTransform();
  void PrintSelf( std::ostream& os, Indent indent ) const;

  TransformQueueType  m_TransformQueue;
  unsigned int        m_ActiveTransform;

private:
  CompositeTransform( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkCompositeTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkCompositeTransform.txx"
#endif

#endif // __itkCompositeTransform_h
