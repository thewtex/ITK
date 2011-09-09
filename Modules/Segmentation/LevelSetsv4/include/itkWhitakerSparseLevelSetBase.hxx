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
::WhitakerSparseLevelSetBase()
{
  this->InitializeLayers();
}

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
WhitakerSparseLevelSetBase< TOutput, VDimension >
::~WhitakerSparseLevelSetBase()
{
}

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::OutputType
WhitakerSparseLevelSetBase< TOutput, VDimension >
::Evaluate( const InputType& iP ) const
{
  LayerMapConstIterator layerIt = this->m_Layers.begin();

  while( layerIt != this->m_Layers.end() )
    {
    LayerConstIterator it = ( layerIt->second ).find( iP );
    if( it != ( layerIt->second ).end() )
      {
      return it->second;
      }

    ++layerIt;
    }

  if( this->m_LabelMap.IsNotNull() )
    {
    if( this->m_LabelMap->GetLabelObject( MinusThreeLayer() )->HasIndex( iP ) )
      {
      return static_cast<OutputType>( MinusThreeLayer() );
      }
    else
      {
      char status = this->m_LabelMap->GetPixel( iP );
      if( status == this->PlusThreeLayer() )
        {
        return static_cast<OutputType>( this->PlusThreeLayer() );
        }
      else
        {
        itkGenericExceptionMacro( <<"status "
                                  << static_cast< int >( status )
                                  << " should be 3 or -3" );
        return static_cast<OutputType>( this->PlusThreeLayer() );
        }
      }
    }
  else
    {
    itkGenericExceptionMacro( <<"Note: m_LabelMap is NULL"  );
    return this->PlusThreeLayer();
    }
}
// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::Evaluate(const InputType &iP, LevelSetDataType &ioData) const
{
  Superclass::Evaluate( iP, ioData );
}

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::InitializeLayers()
{
  this->m_Layers.clear();
  this->m_Layers[ MinusTwoLayer() ] = LayerType();
  this->m_Layers[ MinusOneLayer() ] = LayerType();
  this->m_Layers[ ZeroLayer()     ] = LayerType();
  this->m_Layers[ PlusOneLayer()  ] = LayerType();
  this->m_Layers[ PlusTwoLayer()  ] = LayerType();
}

}

#endif // __itkWhitakerSparseLevelSetBase_hxx
