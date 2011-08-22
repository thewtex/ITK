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
  m_NeighborhoodScales.Fill( NumericTraits< OutputRealType >::One );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::SetLabelMap( LabelMapType* iLabelMap )
{
  m_LabelMap = iLabelMap;
  typename LabelMapType::SpacingType spacing = m_LabelMap->GetSpacing();

  for( unsigned int dim = 0; dim < Dimension; dim++ )
    {
    m_NeighborhoodScales[dim] =
        NumericTraits< OutputRealType >::One / static_cast< OutputRealType >( spacing[dim ] );
    }
  this->Modified();
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
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateGradient( const InputType& iP ) const
{
  OutputRealType center_value =
      static_cast< OutputRealType >( this->Evaluate( iP ) );

  InputType pA, pB;
  pA = pB = iP;

  GradientType dx_forward;
  GradientType dx_backward;

  for( unsigned int dim = 0; dim < Dimension; dim++ )
    {
    pA[dim] += 1;
    pB[dim] -= 1;

    OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
    OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );

    dx_forward[dim] = ( valueA - center_value ) * m_NeighborhoodScales[dim];
    dx_backward[dim] = ( center_value - valueB ) * m_NeighborhoodScales[dim];

    pA[dim] = pB[dim] = iP[dim];
    }

  //which one to be returned ?

  return dx_forward;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
typename WhitakerSparseLevelSetBase< TOutput, VDimension >::HessianType
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateHessian( const InputType& iP ) const
{
  HessianType oHessian;

  OutputRealType center_value =
      static_cast< OutputRealType >( this->Evaluate( iP ) );

  InputType pA, pB, pAa, pBa, pCa, pDa;
  pA = pB = iP;

  for( unsigned int dim1 = 0; dim1 < Dimension; dim1++ )
    {
    pA[dim1] += 1;
    pB[dim1] -= 1;

    OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
    OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );

    oHessian[dim1][dim1] = ( valueA + valueB - 2.0 * center_value )
        * vnl_math_sqr(m_NeighborhoodScales[dim1]);

    pAa = pB;
    pBa = pB;

    pCa = pA;
    pDa = pA;

    for( unsigned int dim2 = dim1 + 1; dim2 < Dimension; dim2++ )
      {
      pAa[dim2] -= 1;
      pBa[dim2] += 1;

      pCa[dim2] -= 1;
      pDa[dim2] += 1;

      OutputRealType valueAa = static_cast< OutputRealType >( this->Evaluate( pAa ) );
      OutputRealType valueBa = static_cast< OutputRealType >( this->Evaluate( pBa ) );
      OutputRealType valueCa = static_cast< OutputRealType >( this->Evaluate( pCa ) );
      OutputRealType valueDa = static_cast< OutputRealType >( this->Evaluate( pDa ) );

      oHessian[dim1][dim2] = oHessian[dim2][dim1] =
          0.25 * ( valueAa - valueBa - valueCa + valueDa )
          * m_NeighborhoodScales[dim1] * m_NeighborhoodScales[dim2];

      pAa[dim2] = pB[dim2];
      pBa[dim2] = pB[dim2];

      pCa[dim2] = pA[dim2];
      pDa[dim2] = pA[dim2];
      }
    pA[dim1] = iP[dim1];
    pB[dim1] = iP[dim1];
    }

  return oHessian;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::Evaluate( const InputType& iP, LevelSetDataType& ioData ) const
{
  // if it has not already been computed before
  if( !ioData.Value.m_Computed )
    {
    ioData.Value.m_Computed = true;
    ioData.Value.m_Value = this->Evaluate( iP );
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateGradient( const InputType& iP, LevelSetDataType& ioData ) const
{
  // if it has not already been computed before
  if( !ioData.Gradient.m_Computed )
    {
    ioData.Gradient.m_Computed = true;
    // compute the gradient

    if( !ioData.Value.m_Computed )
      {
      ioData.Value.m_Computed = true;
      ioData.Value.m_Value = this->Evaluate( iP );
      }

    OutputRealType center_value =
        static_cast< OutputRealType >( ioData.Value.m_Value );

    InputType pA, pB;
    pA = pB = iP;

    GradientType dx_forward;
    GradientType dx_backward;

    for( unsigned int dim = 0; dim < Dimension; dim++ )
      {
      pA[dim] += 1;
      pB[dim] -= 1;

      OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
      OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );
      OutputRealType scale = m_NeighborhoodScales[dim];

      dx_forward[dim] = ( valueA - center_value ) * scale;
      dx_backward[dim] = ( center_value - valueB ) * scale;

      pA[dim] = pB[dim] = iP[dim];
      }
    ioData.Gradient.m_Value = dx_forward;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateHessian( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.Hessian.m_Computed )
    {
    ioData.Hessian.m_Computed = true;

    if( !ioData.Value.m_Computed )
      {
      ioData.Value.m_Computed = true;
      ioData.Value.m_Value = this->Evaluate( iP );
      }

    // compute the hessian
    OutputRealType center_value =
        static_cast< OutputRealType >( ioData.Value.m_Value );

    InputType pA, pB, pAa, pBa, pCa, pDa;
    pA = pB = iP;

    for( unsigned int dim1 = 0; dim1 < Dimension; dim1++ )
      {
      pA[dim1] += 1;
      pB[dim1] -= 1;

      OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
      OutputRealType valueB = static_cast< OutputRealType >( this->Evaluate( pB ) );

      ioData.Hessian.m_Value[dim1][dim1] =
          ( valueA + valueB - 2.0 * center_value ) * vnl_math_sqr( m_NeighborhoodScales[dim1] );

      pAa = pB;
      pBa = pB;

      pCa = pA;
      pDa = pA;

      for( unsigned int dim2 = dim1 + 1; dim2 < Dimension; dim2++ )
        {
        pAa[dim2] -= 1;
        pBa[dim2] += 1;

        pCa[dim2] -= 1;
        pDa[dim2] += 1;

        OutputRealType valueAa = static_cast< OutputRealType >( this->Evaluate( pAa ) );
        OutputRealType valueBa = static_cast< OutputRealType >( this->Evaluate( pBa ) );
        OutputRealType valueCa = static_cast< OutputRealType >( this->Evaluate( pCa ) );
        OutputRealType valueDa = static_cast< OutputRealType >( this->Evaluate( pDa ) );

        ioData.Hessian.m_Value[dim1][dim2] =
            ioData.Hessian.m_Value[dim2][dim1] =
            0.25 * ( valueAa - valueBa - valueCa + valueDa )
            * m_NeighborhoodScales[dim1] * m_NeighborhoodScales[dim2];

        pAa[dim2] = pB[dim2];
        pBa[dim2] = pB[dim2];

        pCa[dim2] = pA[dim2];
        pDa[dim2] = pA[dim2];
        }
      pA[dim1] = iP[dim1];
      pB[dim1] = iP[dim1];
      }
    }
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

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateForwardGradient( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.ForwardGradient.m_Computed )
    {
    ioData.ForwardGradient.m_Computed = true;

    // compute the gradient
    if( !ioData.Value.m_Computed )
      {
      ioData.Value.m_Computed = true;
      ioData.Value.m_Value = this->Evaluate( iP );
      }

    OutputRealType center_value =
        static_cast< OutputRealType >( ioData.Value.m_Value );

    InputType pA;
    pA = iP;

    GradientType dx;

    for( unsigned int dim = 0; dim < Dimension; dim++ )
      {
      pA[dim] += 1;

      OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
      OutputRealType scale = m_NeighborhoodScales[dim];

      dx[dim] = ( valueA - center_value ) * scale;

      pA[dim] = iP[dim];
      }
    ioData.ForwardGradient.m_Value = dx;
    }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template< typename TOutput, unsigned int VDimension >
void
WhitakerSparseLevelSetBase< TOutput, VDimension >
::EvaluateBackwardGradient( const InputType& iP, LevelSetDataType& ioData ) const
{
  if( !ioData.BackwardGradient.m_Computed )
    {
    ioData.BackwardGradient.m_Computed = true;

    // compute the gradient
    if( !ioData.Value.m_Computed )
      {
      ioData.Value.m_Computed = true;
      ioData.Value.m_Value = this->Evaluate( iP );
      }

    OutputRealType center_value =
        static_cast< OutputRealType >( ioData.Value.m_Value );

    InputType pA;
    pA = iP;

    GradientType dx;

    for( unsigned int dim = 0; dim < Dimension; dim++ )
      {
      pA[dim] -= 1;

      OutputRealType valueA = static_cast< OutputRealType >( this->Evaluate( pA ) );
      OutputRealType scale = m_NeighborhoodScales[dim];

      dx[dim] = ( center_value - valueA ) * scale;

      pA[dim] = iP[dim];
      }
    ioData.BackwardGradient.m_Value = dx;
    }
}
}

#endif // __itkWhitakerSparseLevelSetBase_hxx
