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

#ifndef __itkLevelSetEvolutionStoppingCriterionBase_h
#define __itkLevelSetEvolutionStoppingCriterionBase_h

#include "itkStoppingCriterionBase.h"
#include "itkNumericTraits.h"
#include "itkIntTypes.h"

namespace itk
{
 /** \class LevelSetEvolutionStoppingCriterionBase
  *  \ingroup ITKLevelSetsv4
  */
 template< class TLevelSetContainer >
 class LevelSetEvolutionStoppingCriterionBase : public StoppingCriterionBase
 {
 public:
   typedef LevelSetEvolutionStoppingCriterionBase       Self;
   typedef StoppingCriterionBase                        Superclass;
   typedef SmartPointer< Self >                         Pointer;
   typedef SmartPointer< const Self >                   ConstPointer;

   /** Run-time type information (and related methods). */
   itkTypeMacro(LevelSetEvolutionStoppingCriterionBase, StoppingCriterionBase);

   typedef TLevelSetContainer                               LevelSetContainerType;
   typedef typename LevelSetContainerType::Pointer          LevelSetContainerPointer;

   typedef typename LevelSetContainerType::LevelSetIdentifierType
                                                            LevelSetIdentifierType;

   typedef typename LevelSetContainerType::LevelSetType     LevelSetType;
   typedef typename LevelSetContainerType::LevelSetPointer  LevelSetPointer;

   typedef typename LevelSetContainerType::InputIndexType   InputIndexType;
   typedef typename LevelSetContainerType::OutputType       OutputType;
   typedef typename LevelSetContainerType::OutputRealType   OutputRealType;
   typedef typename LevelSetContainerType::GradientType     GradientType;
   typedef typename LevelSetContainerType::HessianType      HessianType;

   typedef typename LevelSetContainerType::HeavisideType    HeavisideType;
   typedef typename LevelSetContainerType::HeavisideType    HeavisidePointer;

   typedef IdentifierType IterationIdType;

   itkSetObjectMacro( LevelSetContainer, LevelSetContainerType );
   itkGetObjectMacro( LevelSetContainer, LevelSetContainerType );

   itkSetMacro( NumberOfIterations, IterationIdType );
   itkGetMacro( NumberOfIterations, IterationIdType );

   itkSetMacro( CurrentIteration, IterationIdType );
   itkGetMacro( CurrentIteration, IterationIdType );

   itkSetMacro( RMSChangeAccumulator, OutputRealType );
   itkGetMacro( RMSChangeAccumulator, OutputRealType );

 protected:
   /** Constructor */
   LevelSetEvolutionStoppingCriterionBase() : Superclass(),
     m_LevelSetContainer( NULL ),
     m_RMSChangeAccumulator( NumericTraits< OutputRealType >::Zero ),
     m_NumberOfIterations( NumericTraits< IterationIdType >::Zero ),
     m_CurrentIteration( NumericTraits< IterationIdType >::Zero )
    {}

   /** Destructor */
   ~LevelSetEvolutionStoppingCriterionBase() {}

   LevelSetContainerPointer m_LevelSetContainer;
   OutputRealType           m_RMSChangeAccumulator;
   IterationIdType          m_NumberOfIterations;
   IterationIdType          m_CurrentIteration;

 private:
   LevelSetEvolutionStoppingCriterionBase( const Self& );
    void operator = ( const Self& );
 };
 }
 #endif
