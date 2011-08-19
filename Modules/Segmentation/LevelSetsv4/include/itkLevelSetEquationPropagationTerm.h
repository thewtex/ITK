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

#ifndef __itkLevelSetEquationPropagationTerm_h
#define __itkLevelSetEquationPropagationTerm_h

#include "itkLevelSetEquationTermBase.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkVector.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk
{
/**
 *  \class LevelSetEquationPropagationTerm
 *  \brief Derived class to represents a propagation term in the level-set evolution PDE
 *
 *  \tparam TInput Input Image Type
 *  \tparam TLevelSetContainer Level set function container type
 *  \ingroup ITKLevelSetsv4
 */
template< class TInput, // Input image
          class TLevelSetContainer >
class LevelSetEquationPropagationTerm :
    public LevelSetEquationTermBase< TInput, TLevelSetContainer >
{
public:
  typedef LevelSetEquationPropagationTerm       Self;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;
  typedef LevelSetEquationTermBase< TInput, TLevelSetContainer >
                                                Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationPropagationTerm,
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

  /** \todo to be documented. */
  virtual void Update()
  {}

  /** Initialize the parameters in the terms prior to an iteration */
  virtual void InitializeParameters()
  {
    this->SetUp();
  }

  /** \todo to be documented. */
  virtual void Initialize( const LevelSetInputIndexType& iP )
  {}

  /** Supply updates at pixels to keep the term parameters always updated */
  virtual void UpdatePixel( const LevelSetInputIndexType& iP,
                            const LevelSetOutputRealType & oldValue,
                            const LevelSetOutputRealType & newValue )
  {
    itkWarningMacro( <<"this method is not implemented" );
  }

protected:
  LevelSetEquationPropagationTerm() : Superclass()
  {
    for( unsigned int i = 0; i < ImageDimension; i++ )
      {
      m_NeighborhoodScales[i] = 1.0;
      }
  }

  virtual ~LevelSetEquationPropagationTerm() {}

  /** Set the term name */
  virtual void SetDefaultTermName()
    {
    this->m_TermName = "Propagation term";
    }

  virtual void SetRequiredData()
    {
    this->m_RequiredData.insert( "Gradient" );
    }

  /** Return the spatial speed dependence a given pixel location
   * Usually, it is constant across the image domain */
  LevelSetOutputRealType PropagationSpeed( const LevelSetInputIndexType& iP ) const
  {
    return ( static_cast< LevelSetOutputRealType >( this->m_Input->GetPixel(iP) ) );
  }

  /** Returns the term contribution for a given location iP, i.e.
   *  \f$ \omega_i( p ) \f$. */
  virtual LevelSetOutputRealType Value( const LevelSetInputIndexType& iP );

  LevelSetOutputRealType m_NeighborhoodScales[ImageDimension];

private:
  LevelSetEquationPropagationTerm( const Self& );
  void operator = ( const Self& );
};

}

#include "itkLevelSetEquationPropagationTerm.hxx"
#endif
