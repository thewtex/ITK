/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkCompositeTransform.txx,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCompositeTransform_txx
#define __itkCompositeTransform_txx

#include "itkCompositeTransform.h"

namespace itk
{

/**
 * Constructor
 */
template
<class TScalar, unsigned int NDimensions>
CompositeTransform<TScalar, NDimensions>::
CompositeTransform() : Superclass( NDimensions, 0 )
{
  this->m_ActiveTransform=0;
  this->m_TransformQueue.clear();
}

/**
 * Destructor
 */
template
<class TScalar, unsigned int NDimensions>
CompositeTransform<TScalar, NDimensions>::
~CompositeTransform()
{
}

template
<class TScalar, unsigned int NDimensions>
bool CompositeTransform<TScalar, NDimensions>
::IsLinear() const
{
  typename TransformQueueType::const_iterator it;

  for( it = this->m_TransformQueue.begin(); it != this->m_TransformQueue.end(); ++it )
    {
    if ( !(*it)->IsLinear() )
      {
      return false;
      }
    }
  return true;
}

/**
 * Transform point
 */
template
<class TScalar, unsigned int NDimensions>
typename CompositeTransform<TScalar, NDimensions>
::OutputPointType
CompositeTransform<TScalar, NDimensions>
::TransformPoint( const InputPointType& inputPoint,
  bool &isInsideTransformRegion ) const
{
  OutputPointType outputPoint( inputPoint );
  typename TransformQueueType::const_iterator it;

  for( it = this->m_TransformQueue.begin(); it != this->m_TransformQueue.end(); ++it )
    {
    outputPoint = (*it)->TransformPoint( outputPoint );
    }
  return outputPoint;
}

/**
 * return an inverse transformation
 */
template
<class TScalar, unsigned int NDimensions>
bool
CompositeTransform<TScalar, NDimensions>
::GetInverse( Self *inverse ) const
{
  typename TransformQueueType::const_iterator it;

  /* clear queue! */
  inverse->ClearTransformQueue();

  for( it = this->m_TransformQueue.begin(); it != this->m_TransformQueue.end(); ++it )
    {
    typename Superclass::Pointer inverseTransform = dynamic_cast<Superclass *>(
      ( ( *it )->GetInverseTransform() ).GetPointer() );
    if ( !inverseTransform )
      {
      /* Empty the queue in case any were added before this fail.
       * Originally this was calling 'inverse=NULL' but that doesn't
       * work to set the pointer to NULL outside of this scope, and
       * doesn't really seen to be proper behavior anyway. */
      inverse->ClearTransformQueue();
      return false;
      }
    else
      {
      inverse->PushFrontTransform( inverseTransform );
      }
    }
  return true;
}

/**
 * Return an inverse of this transform
 */
template
<class TScalar, unsigned int NDimensions>
typename CompositeTransform<TScalar, NDimensions>
::InverseTransformBasePointer
CompositeTransform<TScalar, NDimensions>
::GetInverseTransform() const
{
  Pointer inverseTransform = New();
  if( this->GetInverse( inverseTransform ) )
    {
    return inverseTransform.GetPointer();
    }
  else
    {
    return NULL;
    }
}

template
<class TScalar, unsigned int NDimensions>
const typename CompositeTransform< TScalar, NDimensions >::JacobianType &
CompositeTransform<TScalar, NDimensions>
::GetJacobian( const InputPointType & p ) const
{
  typename TransformType::Pointer t=const_cast< Self * >( this )->GetNthTransform(this->m_ActiveTransform);
  this->m_Jacobian=t->GetJacobian(p);
  return this->m_Jacobian;
}

template
<class TScalar, unsigned int NDimensions>
const typename CompositeTransform< TScalar, NDimensions >::ParametersType &
CompositeTransform<TScalar, NDimensions>
::GetParameters( ) const
{
  typename TransformType::Pointer t=const_cast< Self * >( this )->GetNthTransform(this->m_ActiveTransform);
  this->m_Parameters=t->GetParameters();
  return this->m_Parameters;
}


template <class TScalarType, unsigned int NDimensions>
void
CompositeTransform<TScalarType, NDimensions>::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  if( this->m_TransformQueue.empty() )
    {
    os << indent << "Transform queue is empty." << std::endl;
    return;
    }

  os << indent <<  "Transforms in queue, from begin to end:" << std::endl;
  typename TransformQueueType::const_iterator it;
  for( it = this->m_TransformQueue.begin();
       it != this->m_TransformQueue.end(); ++it )
    {
    os << indent << ">>>>>>>>>" << std::endl;
    (*it)->Print( os, indent );
    }
  os << indent <<  "End of Transforms." << std::endl << "<<<<<<<<<<" << std::endl;
}

} // namespace itk

#endif
