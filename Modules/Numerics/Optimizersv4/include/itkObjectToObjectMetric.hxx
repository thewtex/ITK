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
#ifndef __itkObjectToObjectMetric_hxx
#define __itkObjectToObjectMetric_hxx

#include "itkObjectToObjectMetric.h"
#include "itkTransform.h"
#include "itkIdentityTransform.h"
#include "itkCompositeTransform.h"

namespace itk
{

/*
 * constructor
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::ObjectToObjectMetric()
{
  /* Both transforms default to an identity transform */
  typedef IdentityTransform<TInternalComputationValueType,
                            itkGetStaticConstMacro( MovingDimension ) > MovingIdentityTransformType;
  typedef IdentityTransform<TInternalComputationValueType,
                            itkGetStaticConstMacro( FixedDimension ) >  FixedIdentityTransformType;
  this->m_FixedTransform  = FixedIdentityTransformType::New();
  this->m_MovingTransform = MovingIdentityTransformType::New();

  this->m_UserHasSetVirtualDomain = false;
}

/*
 * destructor
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::~ObjectToObjectMetric()
{
}

/*
 * Initialize
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::Initialize() throw ( ExceptionObject )
{
  if ( !this->m_FixedTransform )
    {
    itkExceptionMacro( "Fixed transform is not present" );
    }

  if ( !this->m_MovingTransform )
    {
    itkExceptionMacro( "Moving transform is not present" );
    }

  /* Special checks for when the moving transform is dense/high-dimensional */
  if( this->HasLocalSupport() )
    {
    /* Verify that virtual domain and displacement field are the same size
     * and in the same physical space. Handles CompositeTransform by checking
     * if first applied transform is DisplacementFieldTransform */
    this->VerifyDisplacementFieldSizeAndPhysicalSpace();

    /* Verify virtual image pixel type is scalar. This effects the calcualtion
     * of offsets in ComputeParameterOffsetFromVirtualIndex().
     * NOTE:  Can this be checked at compile time? ConceptChecking has a
     * HasPixelTraits class, but looks like it just verifies that type T
     * has PixelTraits associated with it, and not a particular value.
     */
    if( PixelTraits< VirtualPixelType >::Dimension != 1 )
      {
      itkExceptionMacro("VirtualPixelType must be scalar for use "
                        "with high-dimensional transform. "
                        "Dimensionality is " <<
                        PixelTraits< VirtualPixelType >::Dimension );
      }
    }
}

/*
 * SetTransform
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::SetTransform( MovingTransformType* transform )
{
  this->SetMovingTransform( transform );
}

/*
 * GetTransform
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                                    TInternalComputationValueType>::MovingTransformType *
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetTransform()
  {
  return this->GetMovingTransform();
  }

/*
 * UpdateTransformParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::UpdateTransformParameters( const DerivativeType & derivative, TInternalComputationValueType factor )
{
  /* Rely on transform::UpdateTransformParameters to verify proper
   * size of derivative */
  this->m_MovingTransform->UpdateTransformParameters( derivative, factor );
}

/*
 * GetNumberOfParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                              TInternalComputationValueType>::NumberOfParametersType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetNumberOfParameters() const
{
  return this->m_MovingTransform->GetNumberOfParameters();
}

/*
 * GetParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                                    TInternalComputationValueType>::ParametersType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetParameters() const
  {
  return this->m_MovingTransform->GetParameters();
  }

/*
 * SetParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::SetParameters( ParametersType & params)
{
  this->m_MovingTransform->SetParametersByValue( params );
}

/*
 * GetNumberOfLocalParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                              TInternalComputationValueType>::NumberOfParametersType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetNumberOfLocalParameters() const
{
  return this->m_MovingTransform->GetNumberOfLocalParameters();
}

/*
 * HasLocalSupport
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::HasLocalSupport() const
{
  return ( this->m_MovingTransform->GetTransformCategory() == MovingTransformType::DisplacementField );
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::TransformPhysicalPointToVirtualIndex( const VirtualPointType & point, VirtualIndexType & index) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->TransformPhysicalPointToIndex( point,index );
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot transform.");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::TransformVirtualIndexToPhysicalPoint( const VirtualIndexType & index, VirtualPointType & point) const
{
  if( this->m_VirtualImage )
    {
    this->m_VirtualImage->TransformIndexToPhysicalPoint( index, point );
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot transform.");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::SetVirtualDomain( const VirtualSpacingType & spacing, const VirtualOriginType & origin,
                    const VirtualDirectionType & direction, const VirtualRegionType & region )
{
  this->m_VirtualImage = VirtualImageType::New();
  this->m_VirtualImage->SetSpacing( spacing );
  this->m_VirtualImage->SetOrigin( origin );
  this->m_VirtualImage->SetDirection( direction );
  this->m_VirtualImage->SetRegions( region );
  this->m_UserHasSetVirtualDomain = true;
  this->Modified();
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::SetVirtualDomainFromImage( VirtualImageType * virtualImage )
{
  itkDebugMacro("setting VirtualDomainImage to " << virtualImage);
  if ( this->m_VirtualImage != virtualImage )
    {
    this->m_VirtualImage = virtualImage;
    this->Modified();
    this->m_UserHasSetVirtualDomain = virtualImage != NULL;
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::SetVirtualDomainFromImage( const VirtualImageType * virtualImage )
{
  this->SetVirtualDomainFromImage( const_cast<VirtualImageType*>( virtualImage ) );
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
const TimeStamp&
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetVirtualDomainTimeStamp( void ) const
{
  if( !this->GetVirtualImage() )
    {
    return this->GetTimeStamp();
    }

  if( this->GetTimeStamp() > this->GetVirtualImage()->GetTimeStamp() )
    {
    return this->GetTimeStamp();
    }
  else
    {
    return this->GetVirtualImage()->GetTimeStamp();
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::IsInsideVirtualDomain( const VirtualPointType & point ) const
{
  if( !this->m_VirtualImage.IsNull() )
    {
    VirtualIndexType index;
    this->m_VirtualImage->TransformPhysicalPointToIndex( point, index );
    return this->GetVirtualRegion().IsInside( index );
    }

  // Otherwise always return true since a virtual domain hasn't been defined,
  // and
  // we assume the user is working in an unconstrained domain.
  return true;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::IsInsideVirtualDomain( const VirtualIndexType & index ) const
{
  if( !this->m_VirtualImage.IsNull() )
    {
    return this->GetVirtualRegion().IsInside( index );
    }

  // Otherwise always return true since a virtual domain hasn't been defined,
  // and
  // we assume the user is working in an unconstrained domain.
  return true;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
OffsetValueType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::ComputeParameterOffsetFromVirtualPoint( const VirtualPointType & point,
                                          const NumberOfParametersType & numberOfLocalParameters ) const
{
  if( !this->m_VirtualImage.IsNull() )
    {
    VirtualIndexType index;
    if( !this->m_VirtualImage->TransformPhysicalPointToIndex( point, index ) )
      {
      itkExceptionMacro(" point is not inside virtual domain. Cannot compute offset. ");
      }
    return this->ComputeParameterOffsetFromVirtualIndex( index, numberOfLocalParameters );
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot calculate offset.");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
OffsetValueType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::ComputeParameterOffsetFromVirtualIndex( const VirtualIndexType & index,
                                          const NumberOfParametersType & numberOfLocalParameters ) const
{
  if( m_VirtualImage )
    {
    OffsetValueType offset = this->m_VirtualImage->ComputeOffset(index) * numberOfLocalParameters;
    return offset;
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot calculate offset.");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                              TInternalComputationValueType>::VirtualSpacingType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetVirtualSpacing( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetSpacing();
    }
  else
    {
    VirtualSpacingType spacing;
    spacing.Fill( NumericTraits<typename VirtualSpacingType::ValueType>::One );
    return spacing;
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                              TInternalComputationValueType>::VirtualDirectionType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetVirtualDirection( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetDirection();
    }
  else
    {
    VirtualDirectionType direction;
    direction.Fill( NumericTraits<typename VirtualDirectionType::ValueType>::One );
    return direction;
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                              TInternalComputationValueType>::VirtualOriginType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetVirtualOrigin( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetOrigin();
    }
  else
    {
    VirtualOriginType origin;
    origin.Fill( NumericTraits<typename VirtualOriginType::ValueType>::Zero );
    return origin;
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                                    TInternalComputationValueType>::VirtualRegionType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetVirtualRegion( void ) const
  {
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetBufferedRegion();
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot return region. ");
    }
  }

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage,
                                    TInternalComputationValueType>::MovingDisplacementFieldTransformType *
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::GetMovingDisplacementFieldTransform() const
  {
  // If it's a composite transform and the displacement field is the first
  // to be applied (i.e. the most recently added), then return that.
  typedef CompositeTransform<CoordinateRepresentationType,
                             itkGetStaticConstMacro( MovingDimension ) > MovingCompositeTransformType;
  const MovingTransformType* transform = this->m_MovingTransform.GetPointer();
  // If it's a CompositeTransform, get the last transform (1st applied).
  const MovingCompositeTransformType* comptx = dynamic_cast< const MovingCompositeTransformType * > ( transform );
  if( comptx != NULL )
    {
    transform = comptx->GetBackTransform();
    }
  // Cast to a DisplacementField type.
  const MovingDisplacementFieldTransformType* deftx =
    dynamic_cast< const MovingDisplacementFieldTransformType * >( transform );
  return deftx;
  }

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::VerifyDisplacementFieldSizeAndPhysicalSpace()
{
  // TODO: replace with a common external method to check this,
  // possibly something in Transform.

  /* Verify that virtual domain and displacement field are the same size
   * and in the same physical space.
   * Effects transformation, and calculation of offset in StoreDerivativeResult.
   * If it's a composite transform and the displacement field is the first
   * to be applied (i.e. the most recently added), then it has to be
   * of the same size, otherwise not.
   * Eventually we'll want a method in Transform something like a
   * GetInputDomainSize to check this cleanly. */
  const MovingDisplacementFieldTransformType * displacementTransform = this->GetMovingDisplacementFieldTransform();

  if( displacementTransform == NULL )
    {
    itkExceptionMacro("Expected the moving transform to be of type DisplacementFieldTransform or derived, "
                      "or a CompositeTransform with DisplacementFieldTransform as the last to have been added." );
    }
  typedef typename MovingDisplacementFieldTransformType::DisplacementFieldType FieldType;
  typename FieldType::ConstPointer field = displacementTransform->GetDisplacementField();
  typename FieldType::RegionType fieldRegion = field->GetBufferedRegion();
  VirtualRegionType virtualRegion = this->GetVirtualRegion();
  if( virtualRegion.GetSize() != fieldRegion.GetSize() || virtualRegion.GetIndex() != fieldRegion.GetIndex() )
    {
    itkExceptionMacro("Virtual domain and moving transform displacement field"
                      " must have the same size and index for BufferedRegion."
                      << std::endl << "Virtual size/index: "
                      << virtualRegion.GetSize() << " / " << virtualRegion.GetIndex() << std::endl
                      << "Displacement field size/index: "
                      << fieldRegion.GetSize() << " / " << fieldRegion.GetIndex() << std::endl );
    }

  /* check that the image occupy the same physical space, and that
   * each index is at the same physical location.
   * this code is from ImageToImageFilter */

  /* tolerance for origin and spacing depends on the size of pixel
   * tolerance for directions a fraction of the unit cube. */
  const double coordinateTol = 1.0e-6 * this->GetVirtualSpacing()[0];
  const double directionTol  = 1.0e-6;

  if ( !this->GetVirtualOrigin().GetVnlVector().is_equal( field->GetOrigin().GetVnlVector(), coordinateTol ) ||
       !this->GetVirtualSpacing().GetVnlVector().is_equal( field->GetSpacing().GetVnlVector(), coordinateTol ) ||
       !this->GetVirtualDirection().GetVnlMatrix().as_ref().is_equal( field->GetDirection().GetVnlMatrix(),
                                                                      directionTol ) )
    {
    std::ostringstream originString, spacingString, directionString;
    originString << "Virtual Origin: " << this->GetVirtualOrigin()
                 << ", DisplacementField Origin: " << field->GetOrigin() << std::endl;
    spacingString << "Virtual Spacing: " << this->GetVirtualSpacing()
                  << ", DisplacementField Spacing: " << field->GetSpacing() << std::endl;
    directionString << "Virtual Direction: " << this->GetVirtualDirection()
                    << ", DisplacementField Direction: " << field->GetDirection() << std::endl;
    itkExceptionMacro(<< "Virtual Domain and DisplacementField do not "
                      << "occupy the same physical space! You may be able to "
                      << "simply call displacementField->CopyInformation( "
                      << "metric->GetVirtualImage() ) to align them. " << std::endl
                      << originString.str() << spacingString.str() << directionString.str() );
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::VerifyNumberOfValidPoints( MeasureType & value, DerivativeType & derivative) const
{
  if( this->m_NumberOfValidPoints == 0 )
    {
    value = NumericTraits<MeasureType>::max();
    derivative.Fill( NumericTraits<DerivativeValueType>::Zero );
    itkWarningMacro("No valid points were found during metric evaluation. "
                    "For image metrics, verify that the images overlap appropriately. "
                    "For instance, you can align the image centers by translation. "
                    "For point-set metrics, verify that the fixed points, once transformed "
                    "into the virtual domain space, actually lie within the virtual domain.");
    return false;
    }
  return true;
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, typename TVirtualImage,
         typename TInternalComputationValueType>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage, TInternalComputationValueType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ObjectToObjectMetric: " << std::endl
     << indent << "Fixed Transform: " << std::endl
     << indent << this->m_FixedTransform << std::endl
     << indent << "Moving Transform: " << std::endl
     << indent << this->m_MovingTransform << std::endl;
  if( this->m_VirtualImage.IsNull() )
    {
    os << indent << "Virtual Image: NULL " << std::endl;
    }
  else
    {
    os << indent << "Virtual Image: " << std::endl
       << indent << this->m_VirtualImage << std::endl;
    }
  os << indent << "m_UserHasSetVirtualDomain: " << this->m_UserHasSetVirtualDomain << std::endl
     << indent << "m_NumberOfValidPoints: " << this->m_NumberOfValidPoints << std::endl;
}

} //namespace itk

#endif
