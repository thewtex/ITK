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

#ifndef __itkLevelSetBase_hxx
#define __itkLevelSetBase_hxx

#include "itkLevelSetBase.h"

namespace itk
{
// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::LevelSetBase() : Superclass()
  {}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::IsInside( const InputType& iP ) const
{
  return ( this->Evaluate( iP ) <= NumericTraits< OutputType >::Zero );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::Initialize()
{
  Superclass::Initialize();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::UpdateOutputInformation()
{
  if( this->GetSource() )
    {
    this->GetSource()->UpdateOutputInformation();
    }

  // Now we should know what our largest possible region is. If our
  // requested region was not set yet, (or has been set to something
  // invalid - with no data in it ) then set it to the largest
  // possible region.
  if ( m_RequestedRegion == -1 && m_RequestedNumberOfRegions == 0 )
    {
    this->SetRequestedRegionToLargestPossibleRegion();
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegionToLargestPossibleRegion()
{
  m_RequestedNumberOfRegions     = 1;
  m_RequestedRegion           = 0;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::CopyInformation(const DataObject *data)
{
  const LevelSetBase *LevelSet = NULL;

  try
    {
    LevelSet = dynamic_cast< const LevelSetBase * >( data );
    }
  catch ( ... )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( LevelSetBase * ).name() );
    }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( LevelSetBase * ).name() );
    }

  m_MaximumNumberOfRegions = LevelSet->GetMaximumNumberOfRegions();

  m_NumberOfRegions = LevelSet->m_NumberOfRegions;
  m_RequestedNumberOfRegions = LevelSet->m_RequestedNumberOfRegions;
  m_BufferedRegion  = LevelSet->m_BufferedRegion;
  m_RequestedRegion = LevelSet->m_RequestedRegion;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::Graft(const DataObject *data)
{
  // Copy Meta Data
  this->CopyInformation(data);

  const Self *LevelSet = NULL;

  try
    {
    LevelSet = dynamic_cast< const Self * >( data );
    }
  catch ( ... )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::LevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::RequestedRegionIsOutsideOfTheBufferedRegion()
{
  if ( m_RequestedRegion != m_BufferedRegion
       || m_RequestedNumberOfRegions != m_NumberOfRegions )
    {
    return true;
    }

  return false;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
bool
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::VerifyRequestedRegion()
{
  bool retval = true;

  // Are we asking for more regions than we can get?
  if ( m_RequestedNumberOfRegions > m_MaximumNumberOfRegions )
    {
    itkExceptionMacro(<< "Cannot break object into "
                      << m_RequestedNumberOfRegions << ". The limit is "
                      << m_MaximumNumberOfRegions);
    }

  if ( m_RequestedRegion >= m_RequestedNumberOfRegions || m_RequestedRegion < 0 )
    {
    itkExceptionMacro(<< "Invalid update region " << m_RequestedRegion
                      << ". Must be between 0 and "
                      << m_RequestedNumberOfRegions - 1);
    }

  return retval;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegion(DataObject *data)
{
  Self *LevelSet = dynamic_cast< Self * >( data );

  if ( LevelSet )
    {
    // only copy the RequestedRegion if the parameter is another PointSet
    m_RequestedRegion = LevelSet->m_RequestedRegion;
    m_RequestedNumberOfRegions = LevelSet->m_RequestedNumberOfRegions;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetRequestedRegion(const RegionType & region)
{
  if ( m_RequestedRegion != region )
    {
    m_RequestedRegion = region;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< class TInput, unsigned VDimension, typename TOutput, class TDomain >
void
LevelSetBase< TInput, VDimension, TOutput, TDomain >
::SetBufferedRegion(const RegionType & region)
{
  if ( m_BufferedRegion != region )
    {
    m_BufferedRegion = region;
    this->Modified();
    }
  }
}
// ----------------------------------------------------------------------------

#endif // __itkLevelSetBase_hxx
