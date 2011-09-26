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

#ifndef __itkLevelSetEquationChanAndVese_h
#define __itkLevelSetEquationChanAndVese_h

#include "itkLevelSetEquationTermContainerBase.h"

#include "itkLevelSetEquationChanAndVeseInternalTerm.h"
#include "itkLevelSetEquationChanAndVeseExternalTerm.h"
#include "itkLevelSetEquationCurvatureTerm.h"

namespace itk
{
/**
 *  \class LevelSetEquationChanAndVese
 *  \brief Class for container holding the terms of a given level set update equation
 *
 *  \tparam TInputImage Input image or speed image or feature image for segmentation
 *  \tparam TLevelSetContainer Container holding the all the level set functions
 *  \ingroup ITKLevelSetsv4
 */
template< class TInputImage,
          class TLevelSetContainer >
class LevelSetEquationChanAndVese :
public LevelSetEquationTermContainerBase< TInputImage, TLevelSetContainer >
{
public:
  typedef LevelSetEquationChanAndVese                                           Self;
  typedef SmartPointer< Self >                                                  Pointer;
  typedef SmartPointer< const Self >                                            ConstPointer;
  typedef LevelSetEquationTermContainerBase< TInputImage, TLevelSetContainer >  Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationChanAndVese,
                LevelSetEquationTermContainerBase );

  typedef typename Superclass::TermIdType TermIdType;

  typedef typename Superclass::InputImageType     InputImageType;
  typedef typename Superclass::InputImagePointer  InputImagePointer;

  typedef typename Superclass::LevelSetContainerType    LevelSetContainerType;
  typedef typename Superclass::LevelSetContainerPointer LevelSetContainerPointer;

  typedef typename Superclass::LevelSetType     LevelSetType;
  typedef typename Superclass::LevelSetPointer  LevelSetPointer;

  typedef typename Superclass::LevelSetIdentifierType   LevelSetIdentifierType;
  typedef typename Superclass::LevelSetOutputPixelType  LevelSetOutputPixelType;
  typedef typename Superclass::LevelSetOutputRealType   LevelSetOutputRealType;
  typedef typename Superclass::LevelSetDataType         LevelSetDataType;
  typedef typename Superclass::LevelSetInputIndexType   LevelSetInputIndexType;
  typedef typename Superclass::LevelSetGradientType     LevelSetGradientType;
  typedef typename Superclass::LevelSetHessianType      LevelSetHessianType;

  typedef typename Superclass::TermType     TermType;
  typedef typename Superclass::TermPointer  TermPointer;

  typedef LevelSetEquationChanAndVeseInternalTerm< TInput, TLevelSetContainer > InternalTermType;
  typedef typename InternalType::Pointer                                        InternalTermPointer;

  typedef LevelSetEquationChanAndVeseExternalTerm< TInput, TLevelSetContainer > ExternalTermType;
  typedef typename ExternalTermType::Pointer                                    ExternalTermPointer;

  typedef LevelSetEquationCurvatureTerm< TInput, TLevelSetContainer > CurvatureTermType;
  typedef typename CurvatureTermType::Pointer                         CurvatureTermPointer;

  void SetInternalTermCoefficient( LevelSetOutputRealType Coeff )
    {
    this->GetTerm( 0 )->SetCoefficient( Coeff );
    }

  LevelSetOutputRealType GetInternalTermCoefficient( )
    {
    return this->GetTerm( 0 )->GetCoefficient();
    }

  void SetExternalTermCoefficient( LevelSetOutputRealType Coeff )
    {
    this->GetTerm( 1 )->SetCoefficient( Coeff );
    }

  LevelSetOutputRealType GetExternalTermCoefficient()
    {
    return this->GetTerm( 1 )->GetCoefficient();
    }

  void SetCurvatureTermCoefficient( LevelSetOutputRealType Coeff )
    {
    this->GetTerm( 2 )->SetCoefficient( Coeff );
    }

  LevelSetOutputRealType GetCurvatureTermCoefficient()
    {
    return this->GetTerm( 2 )->GetCoefficient();
    }

protected:
  LevelSetEquationChanAndVese()
  {
    this->AddTerm( 0, InternalTermType::New() );
    this->SetInternalTermCoefficient( 1. );

    this->AddTerm( 1, ExternalTermType::New() );
    this->SetExternalTermCoefficient( 1. );

    this->AddTerm( 2, CurvatureTermType::New() );
    this->SetCurvatureTermCoefficient( 0 );
  }

  ~LevelSetEquationChanAndVese() {}

  typedef typename Superclass::HashMapStringTermContainerType;

  typedef typename Superclass::RequiredDataType;

  typedef typename Superclass::MapTermContainerType;
  typedef typename Superclass::MapTermContainerIteratorType;
  typedef typename Superclass::MapTermContainerConstIteratorType;

  typedef typename Superclass::MapCFLContainerType;
  typedef typename Superclass::MapCFLContainerIterator;
  typedef typename Superclass::MapCFLContainerConstIterator;

private:
  LevelSetEquationChanAndVese( const Self& );
  void operator = ( const Self& );
};

}
//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkLevelSetEquationChanAndVese.hxx"
//#endif

#endif // __itkLevelSetEquationChanAndVese_h
