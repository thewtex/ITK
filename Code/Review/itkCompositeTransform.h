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
 * \brief This class contains a list of transforms and concatenates them by composition.
 *
 * This class concatenates transforms by means of composition:
 *    T_1 o T_0 = T_1(T_0(x))
 * Transforms are stored in a container (queue).
 *
 * Transforms are added via a single method, AddTransform(). This adds the
 * transforms to the front of the queue, pushing existing transforms towards
 * the back.
 *
 * In TransformPoint, transforms are applied starting from the front of the
 * queue. That is, in reverse order of which they were added.
 *
 * Example
 * A user wants to apply an Affine transform followed by a Deformation Field
 * (DF) transform. He first adds the Affine, then the DF. Because the user
 * typically conceptualizes a transformation as being applied from the Moving
 * image to the Fixed image, this makes intuitive sense. But since the
 * ResampleFilter expects to transform from the Fixed image to the Moving
 * image, the transforms are applied in reverse order of addition, i.e. from
 * the front of the queue.
 *
 * Optimization Flags
 * The m_TransformsToOptimize flags hold one flag for each transform in the
 * queue, designating if each transform is to be used for optimization. Note
 * that all transforms in the queue are applied in TransformPoint, regardless
 * of these flags states'. The methods GetParameters, SetParameters,
 * GetJacobian, GetFixedParameters, and SetFixedParameters all query these
 * flags and include only those transforms whose corresponsing flag is set.
 * Their input or output is a concatenated array of all transforms set for use
 * in optimization. The goal is to be able to optimize multiple transforms at
 * once, while leaving other transforms fixed. For example, a user adds two
 * Affine transforms and optimizes them together, then adds a Deformation Field
 * transform and optmizes that separately, while still applying the affine
 * transforms during the optimization process.
 *
 * Setting Optimization Flags
 * A transform's optimization flag is set when it is added to the queue, and
 * remains set as other transforms are added. The methods
 * SetNthTransformToOptimize* and SetAllTransformToOptimize* are used to
 * set and clear flags arbitrarily. SetOnlyMostRecentTransformToOptimizeOn is
 * a convenience method for setting only the most recently added transform
 * for optimization, with the idea that this will be a common practice.
 *
 * Inverse
 * The inverse transform is created by retrieving the inverse from each
 * sub transform and adding them to a composite transform in reverse order.
 * The m_TransformsToOptimize is copied in reverse for the inverse.
 *
 * TODO
 *
 * Interface Issues/Comments
 * x The PushFrontTransform and PushBackTransform methods are protected to
 *   force the user to use the AddTransform method, forcing the order of
 *   transforms. Are there use cases where the user would *need* to insert
 *   transforms at the back of the queue? Or at arbitrary positions?
 * x SetNth* routines. These require the user to understand the actual
 *   ordering of the transforms in the queue. Since this is the reverse of
 *   what they intuitively expect, should we reverse the queue to allow
 *   the indexing in these methods to match the user's expectation that
 *   adding a transform puts it in the back/end of a list, at position
 *   N-1?
 *   One motivation for ordering the queue as it is, was to allow the user
 *   to say SetNthTransformToOptimize(0) to designate the most-recently
 *   added transform. But SetOnlyMostRecentTransformToOptimizeOn can take
 *   care of that transparently.
 * x GetNthTransformToOptimize - this returns the optimize flag for the Nth
 *   transforms, but is ambiguous. It could mean get the Nth transform itself,
 *   from all those that are flagged for optimization.
 *
 * GetParameters efficiency optimization
 *  Can we optimize this to only query the sub-transforms when the params
 *  in the sub transforms have changed since the previous call? Can't use
 *  Modified time b/c that will get updated in sub-transforms with every
 *  call to SetParameters. Is this worth worrying about? i.e. how much time
 *  will it take in the overall registration process? Probably very little.
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
  typedef CompositeTransform                           Self;
  typedef Transform<TScalar, NDimensions, NDimensions> Superclass;
  typedef SmartPointer<Self>                           Pointer;
  typedef SmartPointer<const Self>                     ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( CompositeTransform, Transform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer InverseTransformBasePointer;

  /** Scalar type. */
  typedef typename Superclass::ScalarType ScalarType;

  /** Parameters type. */
  typedef typename Superclass::ParametersType      ParametersType;
  typedef typename Superclass::ParametersValueType ParametersValueType;
  /** Jacobian type. */
  typedef typename Superclass::JacobianType JacobianType;

  /** Standard coordinate point type for this class. */
  typedef typename Superclass::InputPointType  InputPointType;
  typedef typename Superclass::OutputPointType OutputPointType;

  /** Standard vector type for this class. */
  typedef typename Superclass::InputVectorType  InputVectorType;
  typedef typename Superclass::OutputVectorType OutputVectorType;

  /** Standard covariant vector type for this class */
  typedef typename Superclass::InputCovariantVectorType
  InputCovariantVectorType;
  typedef typename Superclass::OutputCovariantVectorType
  OutputCovariantVectorType;

  /** Standard vnl_vector type for this class. */
  typedef typename Superclass::InputVnlVectorType  InputVnlVectorType;
  typedef typename Superclass::OutputVnlVectorType OutputVnlVectorType;

  /** Dimension of the domain spaces. */
  itkStaticConstMacro( InputDimension, unsigned int, NDimensions );
  itkStaticConstMacro( OutputDimension, unsigned int, NDimensions );

  /** Functionality for sub transforms */

  /** Add transforms to the queue, as stack. Only allow one method for simplicity.
   *  Most-recently added transform is always at front of queue, index 0.
   *  This is the user will optimize the most recently added transform, and
   *  transforms are applied in reverse order, i.e. from the back of queue.
   */
  void AddTransform( Superclass *t  )
  {
    this->PushFrontTransform( t ); /* Also adds to TransformsToOptimize list */
  }

  /** Remove transforms from the front and the back of the queue */

  // NOTE: Brian, I figure we won't want these, to avoid user having to know
  // about the queue ordering?
  // BUT, any of the "Set/Get Nth" routines require that knowledge anyway.

  //void PopBackTransform() { this->m_TransformQueue.pop_back(); }
  //void PopFrontTransform() { this->m_TransformQueue.pop_front(); }

  /** See transforms at the front and the back of the queue */
  //typename Superclass::Pointer GetFrontTransform()
  //  { return this->m_TransformQueue.front(); };
  //typename Superclass::Pointer GetBackTransform()
  //  { return this->m_TransformQueue.back(); };
  typename Superclass::Pointer GetNthTransform( size_t n )
  {
    return this->m_TransformQueue[n];
  };

  /** Active Transform state manipulation */

  //NOTE: User will need to know about queue order to use these
  // "SetNth" routines.
  void SetNthTransformToOptimize( size_t i, bool state )
  {
    this->m_TransformsToOptimizeFlags.at(i)=state;
    this->Modified();
  }

  void SetNthTransformToOptimizeOn( size_t i )
  {
    this->SetNthTransformToOptimize( i, true );
  }

  void SetNthTransformToOptimizeOff( size_t i )
  {
    this->SetNthTransformToOptimize( i, false );
  }

  void SetAllTransformsToOptimize( bool state )
  {
    this->m_TransformsToOptimizeFlags.assign(
      this->m_TransformsToOptimizeFlags.size(), state );
    this->Modified();
  }

  void SetAllTransformsToOptimizeOn()
  {
    this->SetAllTransformsToOptimize( true );
  }

  void SetAllTransformsToOptimizeOff()
  {
    this->SetAllTransformsToOptimize( false );
  }

  void SetOnlyMostRecentTransformToOptimizeOn()
  {
    this->SetAllTransformsToOptimize( false );
    this->SetNthTransformToOptimizeOn( 0 );
  }

  /* NOTE: ambiguous function name here - are we getting if the Nth transform
      is set to be optimized, or the Nth of the transforms that are set to be
      optimized? */
  bool GetNthTransformToOptimize( size_t i ) const
  {
    return this->m_TransformsToOptimizeFlags.at(i);
  }

  /** Misc. functionality */
  bool IsTransformQueueEmpty()
  {
    return this->m_TransformQueue.empty();
  }

  const
  unsigned long GetNumberOfSubTransforms()
  {
    return this->m_TransformQueue.size();
  }

  const
  void ClearTransformQueue()
  {
    this->m_TransformQueue.clear();
    this->m_TransformsToOptimizeFlags.clear();
  }

  /** Return an inverse of this transform. */

  // NOTE: Brian, should this return the reverse of the active transform list
  //  as well? I figure yes. Or it could return them all set to active.
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
  {
    itkExceptionMacro( "TransformVector unimplemented" );
  }

  /**  Method to transform a vnl_vector. */
  virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &) const
  {
    itkExceptionMacro( "TransformVector unimplemented" );
  }

  /**  Method to transform a CovariantVector. */
  virtual OutputCovariantVectorType
  TransformCovariantVector(const InputCovariantVectorType &) const
  {
    itkExceptionMacro( "TransformCovariantVector unimplemented" );
  }

  virtual bool IsLinear() const;

  /**
   * Compute the jacobian with respect to the parameters.
   */
  virtual const JacobianType & GetJacobian(const InputPointType  &) const;

  /** Get/Set Parameter functions work on the current list of transforms
      that are set to be optimized using the 'Set[Nth|All]TransformToOptimze'
      routines. The parameter data from each sub transform is
      concatenated into a single ParametersType object. */
  virtual const ParametersType & GetParameters(void) const;

  virtual void  SetParameters(const ParametersType & p);

  /* Fixed Parameters.It's not clear yet how to handle this. */
  virtual const ParametersType & GetFixedParameters(void) const;

  virtual void  SetFixedParameters(const ParametersType & fixedParameters);

  virtual unsigned int GetNumberOfParameters(void) const;

  virtual unsigned int GetNumberOfFixedParameters(void) const;

protected:
  CompositeTransform();
  virtual ~CompositeTransform();
  void PrintSelf( std::ostream& os, Indent indent ) const;

  void PushFrontTransform( Superclass *t  )
  {
    this->m_TransformQueue.push_front( t );
    /* Add element to list of flags, and set true by default */
    this->m_TransformsToOptimizeFlags.push_front( true );
    this->Modified();
  }

  void PushBackTransform( Superclass *t  )
  {
    this->m_TransformQueue.push_back( t );
    /* Add element to list of flags, and set true by default */
    this->m_TransformsToOptimizeFlags.push_back( true );
    this->Modified();
  }

  typedef std::deque<typename Superclass::Pointer> TransformQueueType;
  typedef std::deque<bool>                         TransformsToOptimizeFlagsType;

  /** Transform container object. */
  mutable TransformQueueType m_TransformQueue;

  /** Get a list of transforms to optimize. Helper function. */
  TransformQueueType& GetTransformsToOptimizeQueue() const;

  mutable TransformQueueType            m_TransformsToOptimizeQueue;
  mutable TransformsToOptimizeFlagsType m_TransformsToOptimizeFlags;
private:
  CompositeTransform( const Self & ); //purposely not implemented
  void operator=( const Self& );      //purposely not implemented

  mutable unsigned long m_PreviousTransformsToOptimizeUpdateTime;
};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkCompositeTransform+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkCompositeTransform.txx"
#endif

#endif // __itkCompositeTransform_h
