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

#ifndef __itkLevelSetEquationLaplacianTerm_h
#define __itkLevelSetEquationLaplacianTerm_h

#include "itkLevelSetEquationTermBase.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkVector.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk
{
/**
 *  \class LevelSetEquationLaplacianTerm
 *  \brief Derived class to represents a propagation term in the level-set evolution PDE
 *
 *  \tparam TInput Input Image Type
 *  \tparam TLevelSetContainer Level set function container type
 */
template< class TInput, // Input image
          class TLevelSetContainer >
class LevelSetEquationLaplacianTerm :
    public LevelSetEquationTermBase< TInput, TLevelSetContainer >
{
public:
  typedef LevelSetEquationLaplacianTerm         Self;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;
  typedef LevelSetEquationTermBase< TInput, TLevelSetContainer >
                                                Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationLaplacianTerm,
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

  /** \todo to be documented. */
  virtual void Initialize( const LevelSetInputIndexType& ) {}

  /** Supply updates at pixels to keep the term parameters always updated */
  virtual void UpdatePixel( const LevelSetInputIndexType& iP,
                            const LevelSetOutputRealType & oldValue,
                            const LevelSetOutputRealType & newValue )
  {
    itkWarningMacro( <<"this method is not implemented" );
  }

protected:
  LevelSetEquationLaplacianTerm() : Superclass()
  {
    for( unsigned int i = 0; i < ImageDimension; i++ )
      {
      m_NeighborhoodScales[i] = 1.0;
      }
  }

  virtual ~LevelSetEquationLaplacianTerm() {}

  /** Set the term name */
  virtual void SetDefaultTermName()
    {
    this->m_TermName = "Laplacian term";
    }

  /** Return the spatial speed dependence a given pixel location
   * Usually, it is constant across the image domain */
  LevelSetOutputRealType LaplacianSpeed( const LevelSetInputIndexType& iP ) const
  {
    return NumericTraits< LevelSetOutputRealType >::One;
  }

  /** Returns the term contribution for a given location iP, i.e.
   *  \f$ \omega_i( p ) \f$. */
  virtual LevelSetOutputRealType Value( const LevelSetInputIndexType& iP )
  {
    LevelSetInputIndexType pA, pB;
    LevelSetInputIndexType pAa, pBa, pCa, pDa;
    LevelSetOutputRealType valueAa, valueBa, valueCa, valueDa;
    LevelSetOutputRealType ZERO = NumericTraits< LevelSetOutputRealType >::Zero;

    vnl_matrix_fixed< LevelSetOutputRealType,
                    itkGetStaticConstMacro(ImageDimension),
                    itkGetStaticConstMacro(ImageDimension) > m_dxy;

    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      pA = pB = iP;
      pA[i] += 1;
      pB[i] -= 1;

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

        valueAa =
            static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pAa ) );
        valueBa =
            static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pBa ) );
        valueCa =
            static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pCa ) );
        valueDa =
            static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( pDa ) );

        m_dxy[i][j] = m_dxy[j][i] = 0.25 * ( valueAa - valueBa - valueCa + valueDa )
                                         * m_NeighborhoodScales[i] * m_NeighborhoodScales[j];
        }
      }

    LevelSetOutputRealType laplacianSpeed = this->LaplacianSpeed( iP );
    LevelSetOutputRealType laplacian = ZERO;

    // Compute the laplacian using the existing second derivative values
    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      laplacian += m_dxy[i][i];
      }

    laplacian = laplacian * laplacianSpeed;
//     std::cout << iP << ' ' << laplacian << std::endl;

    return laplacian;
  }

  LevelSetOutputRealType  m_NeighborhoodScales[ImageDimension];

private:
  LevelSetEquationLaplacianTerm( const Self& );
  void operator = ( const Self& );
};

}
#endif
