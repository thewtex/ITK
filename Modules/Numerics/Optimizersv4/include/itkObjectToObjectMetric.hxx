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

#include "itkObjectToObjectMetricBase.h"
#include "itkTransform.h"
#include "itkIdentityTransform.h"
#include "itkDisplacementFieldTransform.h"
#include "itkCompositeTransform.h"

namespace itk
{

/*
 * constructor
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::ObjectToObjectMetric()
{
  /* Both transforms default to an identity transform */
  typedef IdentityTransform<ParametersValueType, itkGetStaticConstMacro( MovingDimension ) > MovingIdentityTransformType;
  typedef IdentityTransform<ParametersValueType, itkGetStaticConstMacro( FixedDimension ) > FixedIdentityTransformType;
  this->m_FixedTransform  = FixedIdentityTransformType::New();
  this->m_MovingTransform = MovingIdentityTransformType::New();

  this->m_UserHasSetVirtualDomain = false;
}

/*
 * destructor
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::~ObjectToObjectMetric()
{
}

/*
 * Initialize
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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
}

/*
 * SetTransform
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::SetTransform( MovingTransformType* transform )
{
  this->SetMovingTransform( transform );
}

/*
 * GetTransform
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::MovingTransformType *
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetTransform()
{
  return this->GetMovingTransform();
}

/*
 * UpdateTransformParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::UpdateTransformParameters( DerivativeType & derivative,
                             ParametersValueType factor )
{
  /* Rely on transform::UpdateTransformParameters to verify proper
   * size of derivative */
  this->m_MovingTransform->UpdateTransformParameters( derivative, factor );
}

/*
 * GetNumberOfParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::NumberOfParametersType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetNumberOfParameters() const
{
  return this->m_MovingTransform->GetNumberOfParameters();
}

/*
 * GetParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::ParametersType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetParameters() const
{
  return this->m_MovingTransform->GetParameters();
}

/*
 * SetParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::SetParameters( ParametersType & params)
{
  this->m_MovingTransform->SetParametersByValue( params );
}

/*
 * GetNumberOfLocalParameters
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::NumberOfParametersType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetNumberOfLocalParameters() const
{
  return this->m_MovingTransform->GetNumberOfLocalParameters();
}

/*
 * HasLocalSupport
 */
template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::HasLocalSupport() const
{
  return this->m_MovingTransform->HasLocalSupport();
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
bool
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::SetVirtualDomain( const VirtualSpacingType & spacing, const VirtualOriginType & origin,
                    const VirtualDirectionType & direction, const VirtualRegionType & region )
{
  this->m_VirtualImage = VirtualImageType::New();
  this->m_VirtualImage->SetSpacing( spacing );
  this->m_VirtualImage->SetOrigin( origin );
  this->m_VirtualImage->SetDirection( direction );
  this->m_VirtualImage->SetRegions( region );
  this->m_VirtualImage->Allocate();
  this->m_VirtualImage->FillBuffer( 0 );
  this->m_UserHasSetVirtualDomain = true;
  this->Modified();
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
OffsetValueType
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::ComputeParameterOffsetFromVirtualIndex( const VirtualIndexType & index, NumberOfParametersType numberOfLocalParameters ) const
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::VirtualSpacingType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetVirtualSpacing( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetSpacing();
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot return spacing. ");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::VirtualDirectionType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetVirtualDirection( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetDirection();
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot return direction. ");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::VirtualOriginType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::GetVirtualOrigin( void ) const
{
  if( this->m_VirtualImage )
    {
    return this->m_VirtualImage->GetOrigin();
    }
  else
    {
    itkExceptionMacro("m_VirtualImage is undefined. Cannot return origin. ");
    }
}

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
const typename ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>::VirtualRegionType &
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
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
  typedef DisplacementFieldTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingDimension ) >  MovingDisplacementFieldTransformType;
  typedef CompositeTransform<CoordinateRepresentationType,
    itkGetStaticConstMacro( MovingDimension ) >  MovingCompositeTransformType;
  MovingTransformType* transform;
  transform = this->m_MovingTransform.GetPointer();
  /* If it's a CompositeTransform, get the last transform (1st applied). */
  MovingCompositeTransformType* comptx = dynamic_cast< MovingCompositeTransformType * > ( transform );
  if( comptx != NULL )
    {
    transform = comptx->GetBackTransform().GetPointer();
    }
  /* Check that it's a DisplacementField type, or a derived type,
   * the only type we expect at this point. */
  MovingDisplacementFieldTransformType* deftx = dynamic_cast< MovingDisplacementFieldTransformType * >( transform );
  if( deftx == NULL )
    {
    itkExceptionMacro("Expected m_MovingTransform to be of type DisplacementFieldTransform or derived." );
    }
  typedef typename MovingDisplacementFieldTransformType::DisplacementFieldType FieldType;
  typename FieldType::Pointer field = deftx->GetDisplacementField();
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

    if ( !this->GetVirtualOrigin().GetVnlVector().
               is_equal( field->GetOrigin().GetVnlVector(), coordinateTol ) ||
         !this->GetVirtualSpacing().GetVnlVector().
               is_equal( field->GetSpacing().GetVnlVector(), coordinateTol ) ||
         !this->GetVirtualDirection().GetVnlMatrix().as_ref().
               is_equal( field->GetDirection().GetVnlMatrix(), directionTol ) )
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

template<unsigned int TFixedDimension, unsigned int TMovingDimension, class TVirtualImage>
void
ObjectToObjectMetric<TFixedDimension, TMovingDimension, TVirtualImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ObjectToObjectMetric: " << std::endl
     << indent << "Fixed Transform: " << std::endl
     << indent << this->m_FixedTransform << std::endl
     << indent << "Moving Transform: " << std::endl
     << indent << this->m_MovingTransform << std::endl;

}
}//namespace itk

#endif
