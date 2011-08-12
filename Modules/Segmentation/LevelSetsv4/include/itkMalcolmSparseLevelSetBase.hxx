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

#ifndef __itkMalcolmSparseLevelSetBase_hxx
#define __itkMalcolmSparseLevelSetBase_hxx

#include "itkMalcolmSparseLevelSetBase.h"

namespace itk
{
// ----------------------------------------------------------------------------
template< unsigned int VDimension >
MalcolmSparseLevelSetBase< VDimension >
::MalcolmSparseLevelSetBase() : Superclass(), m_LabelMap( 0 )
{
  InitializeLayers();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
char
MalcolmSparseLevelSetBase< VDimension >
::Status( const InputType& iP ) const
{
  return m_LabelMap->GetPixel( iP );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
typename MalcolmSparseLevelSetBase< VDimension >::OutputType
MalcolmSparseLevelSetBase< VDimension >::Evaluate( const InputType& iP ) const
{
  LayerMapConstIterator layerIt = m_Layers.begin();

  while( layerIt != m_Layers.end() )
    {
    LayerConstIterator it = ( layerIt->second ).find( iP );
    if( it != ( layerIt->second ).end() )
      {
      return it->second;
      }

    ++layerIt;
    }

  if( m_LabelMap->GetLabelObject( -1 )->HasIndex( iP ) )
    {
    return -1;
    }
  else
    {
    char status = m_LabelMap->GetPixel( iP );
    if( status == 1 )
      {
      return 1;
      }
    else
      {
      itkGenericExceptionMacro( <<"status "
                                << static_cast< int >( status )
                                << " should be 1 or -1" );
      return 1;
      }
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
typename MalcolmSparseLevelSetBase< VDimension >::GradientType
MalcolmSparseLevelSetBase< VDimension >::EvaluateGradient( const InputType& iP ) const
{
  itkWarningMacro( <<"to be implemented" );
  return GradientType();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
typename MalcolmSparseLevelSetBase< VDimension >::HessianType
MalcolmSparseLevelSetBase< VDimension >::EvaluateHessian( const InputType& iP ) const
{
  itkWarningMacro( <<"to be implemented" );
  return HessianType();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
void
MalcolmSparseLevelSetBase< VDimension >::Initialize()
{
  Superclass::Initialize();

  m_LabelMap = 0;
  this->InitializeLayers();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
void
MalcolmSparseLevelSetBase< VDimension >
::CopyInformation( const DataObject* data )
{
  Superclass::CopyInformation( data );

  const Self *LevelSet = NULL;
  try
    {
    LevelSet = dynamic_cast< const Self* >( data );
    }
  catch( ... )
    {
    // LevelSet could not be cast back down
    itkExceptionMacro( << "itk::MalcolmSparseLevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::MalcolmSparseLevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
void
MalcolmSparseLevelSetBase< VDimension >
::Graft( const DataObject* data )
{
  Superclass::Graft( data );
  const Self *LevelSet = 0;

  try
  {
  LevelSet = dynamic_cast< const Self* >( data );
  }
  catch( ... )
  {
  // mesh could not be cast back down
  itkExceptionMacro( << "itk::MalcolmSparseLevelSetBase::CopyInformation() cannot cast "
                     << typeid( data ).name() << " to "
                     << typeid( Self * ).name() );
  }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::MalcolmSparseLevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
  this->m_LabelMap->Graft( LevelSet->m_LabelMap );
  this->m_Layers = LevelSet->m_Layers;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
const typename MalcolmSparseLevelSetBase< VDimension >::LayerType&
MalcolmSparseLevelSetBase< VDimension >::GetLayer( char iVal ) const
{
  LayerMapConstIterator it = m_Layers.find( iVal );
  if( it == m_Layers.end() )
    {
    itkGenericExceptionMacro( <<"This layer does not exist" );
    }
  return it->second;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
typename MalcolmSparseLevelSetBase< VDimension >::LayerType&
MalcolmSparseLevelSetBase< VDimension >::GetLayer( char iVal )
{
  LayerMapIterator it = m_Layers.find( iVal );
  if( it == m_Layers.end() )
    {
    itkGenericExceptionMacro( <<"This layer does not exist" );
    }
  return it->second;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
void
MalcolmSparseLevelSetBase< VDimension >
::SetLayer( char iVal, const LayerType& iLayer )
{
  LayerMapIterator it = m_Layers.find( iVal );
  if( it != m_Layers.end() )
    {
      it->second = iLayer;
    }
    else
    {
      itkGenericExceptionMacro( <<iVal << "is out of bounds" );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< unsigned int VDimension >
void
MalcolmSparseLevelSetBase< VDimension >
::InitializeLayers()
{
  this->m_Layers.clear();
  this->m_Layers[ 0 ] = LayerType();
}
// ----------------------------------------------------------------------------

}
#endif // __itkMalcolmSparseLevelSetBase_hxx
