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

#ifndef __itkLevelSetEquationCurvatureTerm_h
#define __itkLevelSetEquationCurvatureTerm_h

#include "itkLevelSetEquationTermBase.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkVector.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk
{
/**
 *  \class LevelSetEquationCurvatureTerm
 *  \brief Derived class to represents a curvature term in the level-set evolution PDE
 *
 *  \tparam TInput Input Image Type
 *  \tparam TLevelSetContainer Level set function container type
 *  \ingroup ITKLevelSetsv4
 */
template< class TInput, // Input image
          class TLevelSetContainer >
class LevelSetEquationCurvatureTerm :
    public LevelSetEquationTermBase< TInput, TLevelSetContainer >
{
public:
  typedef LevelSetEquationCurvatureTerm         Self;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;
  typedef LevelSetEquationTermBase< TInput, TLevelSetContainer >
                                                Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationCurvatureTerm,
                LevelSetEquationTermBase );

  typedef typename Superclass::InputImageType     InputImageType;
  typedef typename Superclass::InputImagePointer  InputImagePointer;
  typedef typename Superclass::InputPixelType     InputPixelType;
  typedef typename Superclass::InputPixelRealType InputPixelRealType;

  typedef typename Superclass::LevelSetContainerType      LevelSetContainerType;
  typedef typename Superclass::LevelSetContainerPointer   LevelSetContainerPointer;
  typedef typename Superclass::LevelSetType               LevelSetType;
  typedef typename Superclass::LevelSetPointer            LevelSetPointer;
  typedef typename Superclass::LevelSetOutputPixelType    LevelSetOutputPixelType;
  typedef typename Superclass::LevelSetOutputRealType     LevelSetOutputRealType;
  typedef typename Superclass::LevelSetInputIndexType     LevelSetInputIndexType;
  typedef typename Superclass::LevelSetGradientType       LevelSetGradientType;
  typedef typename Superclass::LevelSetHessianType        LevelSetHessianType;
  typedef typename Superclass::LevelSetIdentifierType     LevelSetIdentifierType;

  typedef typename Superclass::HeavisideType    HeavisideType;
  typedef typename Superclass::HeavisidePointer HeavisidePointer;

  itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

  /** Neighborhood radius type */
  typedef ZeroFluxNeumannBoundaryCondition< InputImageType > DefaultBoundaryConditionType;
  typedef typename ConstNeighborhoodIterator< InputImageType >::RadiusType RadiusType;
  typedef ConstNeighborhoodIterator< InputImageType, DefaultBoundaryConditionType > NeighborhoodType;

  typedef Vector< LevelSetOutputRealType, itkGetStaticConstMacro(ImageDimension) > NeighborhoodScalesType;

  /** Update the term parameter values at end of iteration */
  virtual void Update()
  {}

  /** Initialize the parameters in the terms prior to an iteration */
  virtual void InitializeParameters()
  {
    this->SetUp();
  }

  /** Initialize term parameters in the dense case by computing for each pixel location */
  virtual void Initialize( const LevelSetInputIndexType& ) {}

  /** Supply updates at pixels to keep the term parameters always updated */
  virtual void UpdatePixel( const LevelSetInputIndexType& iP,
                            const LevelSetOutputRealType & oldValue,
                            const LevelSetOutputRealType & newValue )
  {
    itkWarningMacro( << "this method is not implemented" );
  }

protected:
  LevelSetEquationCurvatureTerm() : Superclass()
  {
    for( unsigned int i = 0; i < ImageDimension; i++ )
      {
      m_NeighborhoodScales[i] = 1.0;
      }
  }

  virtual ~LevelSetEquationCurvatureTerm() {}

  /** Set the term name */
  virtual void SetDefaultTermName()
    {
    this->m_TermName = "Curvature term";
    }


  /** Returns the term contribution for a given location iP, i.e.
   *  \f$ \omega_i( p ) \f$. */
  virtual LevelSetOutputRealType Value( const LevelSetInputIndexType& iP )
    {
    if( this->m_Heaviside.IsNotNull() )
      {
      LevelSetOutputRealType center_value =
          static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( iP ) );
      LevelSetInputIndexType pA, pB;
      LevelSetInputIndexType pAa, pBa, pCa, pDa;
      LevelSetOutputRealType valueA, valueB;
      LevelSetOutputRealType valueAa, valueBa, valueCa, valueDa;
      LevelSetOutputRealType oValue = NumericTraits< LevelSetOutputRealType >::Zero;

      vnl_matrix_fixed< LevelSetOutputRealType,
                      itkGetStaticConstMacro(ImageDimension),
                      itkGetStaticConstMacro(ImageDimension) > m_dxy;

      /** Array of first derivatives */
      LevelSetOutputRealType m_dx[itkGetStaticConstMacro(ImageDimension)];
      LevelSetOutputRealType m_dx_forward[itkGetStaticConstMacro(ImageDimension)];
      LevelSetOutputRealType m_dx_backward[itkGetStaticConstMacro(ImageDimension)];
      LevelSetOutputRealType m_GradMagSqr = vnl_math::eps;

      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        pA = pB = iP;
        pA[i] += 1;
        pB[i] -= 1;

        valueA = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pA ) );
        valueB = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pB ) );

        m_dx[i] = 0.5 * ( valueA - valueB ) * m_NeighborhoodScales[i];
        m_dxy[i][i] = ( valueA + valueB - 2.0 * center_value ) * vnl_math_sqr(m_NeighborhoodScales[i]);
        m_dx_forward[i]  = ( valueA - center_value ) * m_NeighborhoodScales[i];
        m_dx_backward[i] = ( center_value - valueB ) * m_NeighborhoodScales[i];
        m_GradMagSqr += m_dx[i] * m_dx[i];

        for ( unsigned int j = i + 1; j < ImageDimension; j++ )
          {
          pAa = pB;
          pAa[j] -= 1;

          pBa = pB;
          pBa[j] += 1;

          pCa = pA;
          pCa[j] -= 1;

          pDa = pA;
          pDa[j] += 1;

          valueAa = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pAa ) );
          valueBa = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pBa ) );
          valueCa = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pCa ) );
          valueDa = static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pDa ) );

          m_dxy[i][j] = m_dxy[j][i] = 0.25 * ( valueAa - valueBa - valueCa + valueDa )
                                              * m_NeighborhoodScales[i] * m_NeighborhoodScales[j];
          }
        }
      LevelSetOutputRealType m_GradMag = vcl_sqrt( m_GradMagSqr );

      // Compute curvature
      for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
        for ( unsigned int j = 0; j < ImageDimension; j++ )
          {
          if ( j != i )
            {
            oValue -= m_dx[i] * m_dx[j] * m_dxy[i][j];
            oValue += m_dxy[j][j] * m_dx[i] * m_dx[i];
            }
          }
        }

      if ( m_GradMag > vnl_math::eps )
        {
        oValue /= m_GradMag * m_GradMag * m_GradMag;
        }
      else
        {
        oValue /= 1 + m_GradMagSqr;
        }

//       std::cout << iP << ' ' << oValue << std::endl;
      return oValue;
      }
    else
      {
      itkWarningMacro( << "m_Heaviside is NULL" );
      }

    return NumericTraits< LevelSetOutputPixelType >::Zero;
    }


  LevelSetOutputRealType  m_NeighborhoodScales[ImageDimension];

private:
  LevelSetEquationCurvatureTerm( const Self& );
  void operator = ( const Self& );
};

}
#endif
