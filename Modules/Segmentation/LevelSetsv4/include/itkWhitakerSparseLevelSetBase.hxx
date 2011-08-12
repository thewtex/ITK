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

#ifndef __itkWhitakerSparseLevelSetBase_hxx
#define __itkWhitakerSparseLevelSetBase_hxx

#include "itkWhitakerSparseLevelSetBase.h"

namespace itk
{
// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
WhitakerSparseLevelSetBase< TOutput, VDimension >
::WhitakerSparseLevelSetBase() : Superclass(), m_LabelMap( 0 )
{
  InitializeLayers();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
char
WhitakerSparseLevelSetBase< TOutput, VDimension >
::Status( const InputType& iP ) const
{
  if( m_LabelMap.IsNotNull() )
    {
    return m_LabelMap->GetPixel( iP );
    }
  else
    {
    LayerMapConstIterator layerIt = m_Layers.begin();
    while( layerIt != m_Layers.end() )
      {
      LayerConstIterator it = ( layerIt->second ).find( iP );
      if( it != ( layerIt->second ).end() )
        {
        return layerIt->first;
        }

      ++layerIt;
      }

    return 3;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::OutputType
WhitakerSparseLevelSetBase< TOutput, VDimension >
::Evaluate( const InputType& iP ) const
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

  if( m_LabelMap.IsNotNull() )
    {
    if( m_LabelMap->GetLabelObject( -3 )->HasIndex( iP ) )
      {
      return -3;
      }
    else
      {
      char status = m_LabelMap->GetPixel( iP );
      if( status == 3 )
        {
        return 3.;
        }
      else
        {
        itkGenericExceptionMacro( <<"status "
                                  << static_cast< int >( status )
                                  << " should be 3 or -3" );
        return 3.;
        }
      }
    }
  else
    {
    std::cout << "*** Note: m_LabelMap is NULL ***" << std::endl;
    return 3;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::GradientType
WhitakerSparseLevelSetBase< TOutput, VDimension >::EvaluateGradient( const InputType& iP ) const
{
  itkWarningMacro( <<"to be implemented" );
  return GradientType();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::HessianType
WhitakerSparseLevelSetBase< TOutput, VDimension >::EvaluateHessian( const InputType& iP ) const
{
  itkWarningMacro( <<"to be implemented" );
  return HessianType();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >::
Initialize()
{
  Superclass::Initialize();

  m_LabelMap = NULL;
  this->InitializeLayers();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >::
CopyInformation( const DataObject* data )
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
  itkExceptionMacro( << "itk::WhitakerSparseLevelSetBase::CopyInformation() cannot cast "
                     << typeid( data ).name() << " to "
                     << typeid( Self * ).name() );
  }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::WhitakerSparseLevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >::
Graft( const DataObject* data )
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
  itkExceptionMacro( << "itk::WhitakerSparseLevelSetBase::CopyInformation() cannot cast "
                     << typeid( data ).name() << " to "
                     << typeid( Self * ).name() );
  }

  if ( !LevelSet )
    {
    // pointer could not be cast back down
    itkExceptionMacro( << "itk::WhitakerSparseLevelSetBase::CopyInformation() cannot cast "
                       << typeid( data ).name() << " to "
                       << typeid( Self * ).name() );
    }

  this->m_LabelMap->Graft( LevelSet->m_LabelMap );
  this->m_Layers = LevelSet->m_Layers;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
const typename WhitakerSparseLevelSetBase< TOutput, VDimension >::LayerType&
WhitakerSparseLevelSetBase< TOutput, VDimension >::GetLayer( char iVal ) const
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
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::LayerType&
WhitakerSparseLevelSetBase< TOutput, VDimension >::GetLayer( char iVal )
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
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >::
SetLayer( char iVal, const LayerType& iLayer )
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
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::InitializeLayers()
{
  this->m_Layers.clear();
  this->m_Layers[ -2 ] = LayerType();
  this->m_Layers[ -1 ] = LayerType();
  this->m_Layers[  0 ] = LayerType();
  this->m_Layers[  1 ] = LayerType();
  this->m_Layers[  2 ] = LayerType();
}
// ----------------------------------------------------------------------------

}

#endif // __itkWhitakerSparseLevelSetBase_hxx
