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
#ifndef __itkCoxDeBoorBSplineKernelFunction_hxx
#define __itkCoxDeBoorBSplineKernelFunction_hxx

#include "itkCoxDeBoorBSplineKernelFunction.h"

namespace itk
{
template<unsigned int VSplineOrder, typename TValueType>
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::CoxDeBoorBSplineKernelFunction()
{
  this->m_SplineOrder = VSplineOrder;
  this->GenerateBSplineShapeFunctions( this->m_SplineOrder + 1 );
}

template<unsigned int VSplineOrder, typename TValueType>
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::~CoxDeBoorBSplineKernelFunction()
{}

template<unsigned int VSplineOrder, typename TValueType>
void
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::SetSplineOrder( const unsigned int order )
{
  if ( order != this->m_SplineOrder )
    {
    this->m_SplineOrder = order;
    this->GenerateBSplineShapeFunctions( this->m_SplineOrder + 1 );
    this->Modified();
    }
}

template<unsigned int VSplineOrder, typename TValueType>
void
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::GenerateBSplineShapeFunctions( const unsigned int order )
{
  unsigned int numberOfPieces =
    static_cast<unsigned int>( static_cast< TValueType >(0.5) * ( order + 1 ) );

  this->m_BSplineShapeFunctions.set_size( numberOfPieces, order );

  VectorType knots( order + 1 );
  for( unsigned int i = 0; i < knots.size(); i++ )
    {
    knots[i] = static_cast< TValueType >(-0.5) * static_cast< TValueType >( order ) +
      static_cast< TValueType >( i );
    }

  for( unsigned int i = 0; i < numberOfPieces; i++ )
    {
    PolynomialType poly = this->CoxDeBoor( order, knots,
      0, static_cast< unsigned int >( static_cast< TValueType >(0.5) * ( order ) ) + i );
    this->m_BSplineShapeFunctions.set_row( i, poly.coefficients() );
    }
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::PolynomialType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::CoxDeBoor( const unsigned short order, const VectorType knots,
  const unsigned int whichBasisFunction, const unsigned int whichPiece )
{
  VectorType tmp( 2 );
  PolynomialType poly1( NumericTraits< TValueType >::Zero );
  PolynomialType poly2( NumericTraits< TValueType >::Zero );
  unsigned short p = order - 1;
  unsigned short i = whichBasisFunction;

  if ( p == 0 && whichBasisFunction == whichPiece )
    {
    PolynomialType poly( NumericTraits< TValueType >::One );
    return poly;
    }

  // Term 1
  TValueType den = knots(i + p) - knots(i);
  if ( den == NumericTraits< TValueType >::Zero )
    {
    PolynomialType poly( NumericTraits< TValueType >::Zero );
    poly1 = poly;
    }
  else
    {
    tmp(0) = NumericTraits< TValueType >::One;
    tmp(1) = -knots(i);
    tmp /= den;
    poly1 = PolynomialType( tmp ) *
      this->CoxDeBoor( order - 1, knots, i, whichPiece );
    }

  // Term 2
  den = knots(i + p + 1) - knots(i + 1);
  if ( den == NumericTraits< TValueType >::Zero )
    {
    PolynomialType poly( NumericTraits< TValueType >::Zero );
    poly2 = poly;
    }
  else
    {
    tmp(0) = -NumericTraits< TValueType >::One;
    tmp(1) = knots(i + p + 1);
    tmp /= den;
    poly2 = PolynomialType( tmp ) *
      this->CoxDeBoor( order - 1, knots, i + 1, whichPiece );
    }
  return ( poly1 + poly2 );
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::MatrixType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::GetShapeFunctionsInZeroToOneInterval()
{
  int order = this->m_SplineOrder + 1;
  unsigned int numberOfPieces = static_cast<unsigned int>( order );
  MatrixType shapeFunctions( numberOfPieces, order );

  VectorType knots( 2 * order );

  for( unsigned int i = 0; i < knots.size(); i++ )
    {
    knots[i] = -static_cast<TValueType>( this->m_SplineOrder ) +
      static_cast<TValueType>( i );
    }

  for( unsigned int i = 0; i < numberOfPieces; i++ )
    {
    PolynomialType poly = this->CoxDeBoor( order, knots, i, order - 1 );
    shapeFunctions.set_row( i, poly.coefficients() );
    }
  return shapeFunctions;
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::MatrixType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::GetShapeFunctions()
{
  return this->m_BSplineShapeFunctions;
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::TValueType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::Evaluate( const TValueType &u ) const
{
  TValueType absValue = vnl_math_abs( u );

  unsigned int which;
  if( this->m_SplineOrder % 2 == 0 )
    {
    which = static_cast< unsigned int >( absValue + static_cast< TValueType >(0.5) );
    }
  else
    {
    which = static_cast< unsigned int >( absValue );
    }

  if( which < this->m_BSplineShapeFunctions.rows() )
    {
    return PolynomialType(
      this->m_BSplineShapeFunctions.get_row( which ) ).evaluate( absValue );
    }
  else
    {
    return NumericTraits< TValueType >::Zero;
    }
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::TValueType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::EvaluateDerivative( const TValueType &u ) const
{
  return this->EvaluateNthDerivative( u, 1 );
}

template<unsigned int VSplineOrder, typename TValueType>
typename CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>::TValueType
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::EvaluateNthDerivative( const TValueType & u, const unsigned int n ) const
{
  TValueType absValue = vnl_math_abs( u );

  unsigned int which;
  if( this->m_SplineOrder % 2 == 0 )
    {
    which = static_cast<unsigned int>( absValue + static_cast< TValueType >(0.5) );
    }
  else
    {
    which = static_cast<unsigned int>( absValue );
    }

  if( which < this->m_BSplineShapeFunctions.rows() )
    {
    PolynomialType polynomial(
      this->m_BSplineShapeFunctions.get_row( which ) );
    for( unsigned int i = 0; i < n; i++ )
      {
      polynomial = polynomial.derivative();
      }
    TValueType der = polynomial.evaluate( absValue );
    if( u < NumericTraits< TValueType >::Zero && n % 2 != 0 )
      {
      return -der;
      }
    else
      {
      return der;
      }
    }
  else
    {
    return NumericTraits< TValueType >::Zero;
    }
}

template<unsigned int VSplineOrder, typename TValueType>
void
CoxDeBoorBSplineKernelFunction<VSplineOrder,TValueType>
::PrintSelf( std::ostream &os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent  << "Spline Order: " << this->m_SplineOrder << std::endl;
  os << indent  << "Piecewise Polynomial Pieces: " << std::endl;

  TValueType a = NumericTraits< TValueType >::Zero;
  TValueType b = NumericTraits< TValueType >::Zero;

  for( unsigned int i = 0; i < this->m_BSplineShapeFunctions.rows(); i++ )
    {
    os << indent << indent;

    PolynomialType( this->m_BSplineShapeFunctions.get_row(  i) ).print( os );

    if ( i == 0 )
      {
      if ( this->m_SplineOrder % 2 == 0 )
        {
        b = static_cast< TValueType >(0.5);
        }
      else
        {
        b = NumericTraits< TValueType >::One;
        }
      }
    else
      {
      a = b;
      b += NumericTraits< TValueType >::One;
      }

    os << ",  X \\in [" << a << ", " << b << "]" << std::endl;
    }
}
} // namespace itk

#endif
