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

#ifndef __itkLevelSetEquationChanAndVeseInternalTerm_h
#define __itkLevelSetEquationChanAndVeseInternalTerm_h

#include "itkLevelSetEquationTermBase.h"

namespace itk
{
/**
 *  \class LevelSetEquationChanAndVeseInternalTerm
 *  \brief Derived class to represent the internal energy Chan And Vese term
 *
 *  \tparam TInput Input Image Type
 *  \tparam TLevelSetContainer Level set function container type
 *  \ingroup ITKLevelSetsv4
 */
template< class TInput, // Input image
          class TLevelSetContainer >
class LevelSetEquationChanAndVeseInternalTerm :
    public LevelSetEquationTermBase< TInput, TLevelSetContainer >
{
public:
  typedef LevelSetEquationChanAndVeseInternalTerm         Self;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;
  typedef LevelSetEquationTermBase< TInput,
                                    TLevelSetContainer >  Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationChanAndVeseInternalTerm,
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

  itkSetMacro( Mean, InputPixelRealType );
  itkGetMacro( Mean, InputPixelRealType );

  /** Update the term parameter values at end of iteration */
  virtual void Update()
  {
    if( m_TotalH > NumericTraits< LevelSetOutputRealType >::epsilon() )
      {
      LevelSetOutputRealType inv_total_h = 1. / m_TotalH;

      // depending on the pixel type, it may be more efficient to do
      // a multiplication than to do a division
      m_Mean = m_TotalValue * inv_total_h;
      }
    else
      {
      m_Mean = NumericTraits< InputPixelRealType >::Zero;
      }

    std::cout << m_TotalValue << '/' << m_TotalH << '=' << m_Mean << std::endl;
  }

  /** Initialize parameters in the terms prior to an iteration */
  virtual void InitializeParameters()
  {
    m_TotalValue = NumericTraits< InputPixelRealType >::Zero;
    m_TotalH = NumericTraits< LevelSetOutputRealType >::Zero;
    this->SetUp();
  }


  /** Initialize term parameters in the dense case by computing for each pixel location */
  virtual void Initialize( const LevelSetInputIndexType& iP )
  {
    if( this->m_Heaviside.IsNotNull() )
      {
      InputPixelType pixel = this->m_Input->GetPixel( iP );

      LevelSetOutputRealType prod;
      this->ComputeProduct( iP, prod );
      this->Accumulate( pixel, prod );
      }
    else
      {
      itkWarningMacro( << "m_Heaviside is NULL" );
      }
  }

  /** Compute the product of Heaviside functions in the multi-levelset cases */
  virtual void ComputeProduct( const LevelSetInputIndexType& iP,
                              LevelSetOutputRealType& prod )
  {
    LevelSetOutputRealType value = this->m_CurrentLevelSetPointer->Evaluate( iP );
    prod = this->m_Heaviside->Evaluate( -value );
  }

  /** Compute the product of Heaviside functions in the multi-levelset cases
   *  except the current levelset */
  virtual void ComputeProductTerm( const LevelSetInputIndexType& iP,
                                  LevelSetOutputRealType& prod )
  {}

  /** Supply updates at pixels to keep the term parameters always updated */
  virtual void UpdatePixel( const LevelSetInputIndexType& iP,
                           const LevelSetOutputRealType & oldValue,
                           const LevelSetOutputRealType & newValue )
  {
    // For each affected h val: h val = new hval (this will dirty some cvals)
    InputPixelType input = this->m_Input->GetPixel( iP );

    LevelSetOutputRealType oldH = this->m_Heaviside->Evaluate( -oldValue );
    LevelSetOutputRealType newH = this->m_Heaviside->Evaluate( -newValue );
    LevelSetOutputRealType change = newH - oldH;

    // update the foreground constant for current level-set function
    this->m_TotalH += change;
    this->m_TotalValue += input * change;
  }


protected:
  LevelSetEquationChanAndVeseInternalTerm() : Superclass(),
    m_Mean( NumericTraits< InputPixelRealType >::Zero ),
    m_TotalValue( NumericTraits< InputPixelRealType >::Zero ),
    m_TotalH( NumericTraits< LevelSetOutputRealType >::Zero )
  {}

  virtual ~LevelSetEquationChanAndVeseInternalTerm() {}

  /** Set the term name */
  virtual void SetDefaultTermName()
    {
    this->m_TermName = "Internal Chan And Vese term";
    }


  /** Returns the term contribution for a given location iP, i.e.
   *  \f$ \omega_i( p ) \f$. */
  virtual LevelSetOutputRealType Value( const LevelSetInputIndexType& iP )
    {
    if( this->m_Heaviside.IsNotNull() )
      {
      LevelSetOutputRealType value =
          static_cast< LevelSetOutputRealType >( this->m_CurrentLevelSetPointer->Evaluate( iP ) );

      LevelSetOutputRealType d_val = this->m_Heaviside->EvaluateDerivative( -value );

      InputPixelType pixel = this->m_Input->GetPixel( iP );
      LevelSetOutputRealType prod = 1;

      ComputeProductTerm( iP, prod );
      LevelSetOutputRealType oValue = d_val * prod *
        static_cast< LevelSetOutputRealType >( ( pixel - m_Mean ) * ( pixel - m_Mean ) );

//       std::cout << value << ' ' << int(pixel) << ' ' << d_val << ' ' << prod << ' ' << oValue << std::endl;
      return oValue;
      }
    else
      {
      itkWarningMacro( << "m_Heaviside is NULL" );
      }
    return NumericTraits< LevelSetOutputPixelType >::Zero;
    }

  /** Accumulate contribution to term parameters from a given pixel */
  void Accumulate( const InputPixelType& iPix,
                   const LevelSetOutputRealType& iH )
    {
    m_TotalValue += static_cast< InputPixelRealType >( iPix ) *
        static_cast< LevelSetOutputRealType >( iH );
    m_TotalH += static_cast< LevelSetOutputRealType >( iH );
    }

  InputPixelRealType      m_Mean;
  InputPixelRealType      m_TotalValue;
  LevelSetOutputRealType  m_TotalH;

private:
  LevelSetEquationChanAndVeseInternalTerm( const Self& );
  void operator = ( const Self& );
};

}
#endif
