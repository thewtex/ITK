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


#ifndef __itkLevelSetEvolutionBase_h
#define __itkLevelSetEvolutionBase_h

#include <list>

#include "itkImage.h"
#include "itkDiscreteLevelSetImageBase.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkNumericTraits.h"
#include "itkLevelSetEvolutionStoppingCriterionBase.h"

namespace itk
{
/**
 *  \class LevelSetEvolutionBase
 *  \brief Class for iterating and evolving the dense level-set function
 *
 *  \tparam TEquationContainer Container holding the system of level set of equations
 *   \ingroup ITKLevelSetsv4
 */
template< class TEquationContainer, class TLevelSet >
class LevelSetEvolutionBase : public Object
{
public:
  typedef LevelSetEvolutionBase      Self;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef Object                     Superclass;

  /** Run-time type information */
  itkTypeMacro( LevelSetEvolutionBase, Object );

  typedef TEquationContainer                      EquationContainerType;
  typedef typename EquationContainerType::Pointer EquationContainerPointer;
  typedef typename EquationContainerType::TermContainerType
                                                  TermContainerType;
  typedef typename TermContainerType::Pointer     TermContainerPointer;

  typedef typename TermContainerType::TermType TermType;
  typedef typename TermType::Pointer           TermPointer;

  typedef typename TermContainerType::InputImageType InputImageType;
  typedef typename InputImageType::PixelType         InputImagePixelType;
  typedef typename InputImageType::ConstPointer      InputImageConstPointer;
  typedef typename InputImageType::RegionType        InputImageRegionType;
  typedef typename NumericTraits< InputImagePixelType >::RealType
                                                     InputPixelRealType;

  itkStaticConstMacro ( ImageDimension, unsigned int, InputImageType::ImageDimension );

  typedef typename TermContainerType::LevelSetContainerType       LevelSetContainerType;
  typedef typename LevelSetContainerType::Pointer                 LevelSetContainerPointer;

  typedef typename LevelSetContainerType::LevelSetIdentifierType  LevelSetIdentifierType;
  typedef typename LevelSetContainerType::LevelSetContainerConstIteratorType
                                                                  LevelSetContainerConstIteratorType;
  typedef typename LevelSetContainerType::LevelSetContainerIteratorType
                                                                  LevelSetContainerIteratorType;

  typedef TLevelSet                               LevelSetType;
  typedef typename LevelSetType::Pointer          LevelSetPointer;
  typedef typename LevelSetType::InputType        LevelSetInputType;
  typedef typename LevelSetType::OutputType       LevelSetOutputType;
  typedef typename LevelSetType::OutputRealType   LevelSetOutputRealType;
  typedef typename LevelSetType::LevelSetDataType LevelSetDataType;


  typedef std::list< IdentifierType >                    IdListType;
  typedef typename IdListType::iterator                  IdListIterator;
  typedef Image< IdListType, ImageDimension >            IdListImageType;
  typedef Image< short, ImageDimension >                 CacheImageType;
  typedef typename LevelSetContainerType::DomainMapImageFilterType
                                                         DomainMapImageFilterType;
  typedef typename DomainMapImageFilterType::Pointer     DomainMapImageFilterPointer;

  typedef LevelSetEvolutionStoppingCriterionBase< LevelSetContainerType >
                                                  StoppingCriterionType;
  typedef typename StoppingCriterionType::Pointer StoppingCriterionPointer;

  itkSetObjectMacro( LevelSetContainer, LevelSetContainerType );
  itkGetObjectMacro( LevelSetContainer, LevelSetContainerType );

  /** Set/Get the value of alpha for computing the time-step using CFL conditions */
  itkSetMacro( Alpha, LevelSetOutputRealType );
  itkGetMacro( Alpha, LevelSetOutputRealType );

  /** Set a user-specified value of the time-step */
  void SetTimeStep( const LevelSetOutputRealType& iDt );

  /** Set/Get the equation container for updating all the level sets */
  itkSetObjectMacro( EquationContainer, EquationContainerType );
  itkGetObjectMacro( EquationContainer, EquationContainerType );

  /** Set/Get the Stopping Criterion */
  itkGetObjectMacro( StoppingCriterion, StoppingCriterionType );
  itkSetObjectMacro( StoppingCriterion, StoppingCriterionType );

  /** Set/Get the number of iterations that have occured. */
  itkSetMacro( NumberOfIterations, IdentifierType );
  itkGetConstMacro( NumberOfIterations, IdentifierType );

  /** Set/Get the number of level sets in a multi-level set analysis before the
   * multi-threading approach changes.  If the number of level sets evolving
   * meets or exceeds this number, threading occurs by processing each level set in a
   * thread instead of dividing the analysis of an individual level set into
   * multiple threads. Defaults to the GlobalDefaultNumberOfThreads return by
   * the MultiThreader class. */
  itkSetMacro( NumberOfLevelSetsForThreadingMethodChange, IdentifierType );
  itkGetConstMacro( NumberOfLevelSetsForThreadingMethodChange, IdentifierType );

  /** Update the filter by computing the output level function
   * by calling Evolve() once the instantiation of necessary variables
   * is verified */
  void Update();

protected:
  LevelSetEvolutionBase();

  virtual ~LevelSetEvolutionBase();

  void CheckSetUp();

  /** Initialize the iteration by computing parameters in the terms of the level set equation */
  void InitializeIteration();

  /** Run the iterative loops of calculating levelset function updates until
   *  the stopping criterion is satisfied.  Calls AllocateUpdateBuffer,
   *  ComputeIteration, ComputeTimeStepForNextIteration, UpdateLevelSets,
   *  UpdateEquations.  */
  void Evolve();

  /** Initialize the update buffers for all level sets to hold the updates of
   *  equations in each iteration. No-op by default. */
  virtual void AllocateUpdateBuffer();

  /** Computer the update at each pixel and store in the update buffer. No-op by
   * default. */
  virtual void ComputeIteration();

  /** Compute the time-step for the next iteration. No-op by default. */
  virtual void ComputeTimeStepForNextIteration();

  virtual void UpdateLevelSets() = 0;

  virtual void UpdateEquations() = 0;

  /** Should we do parallel processing by processing a level set in each thread?
   * Yes, if this method returns true; otherwise, split level set across
   * threads. */
  bool MultiThreadWithLevelSetPerThread() const;

  StoppingCriterionPointer    m_StoppingCriterion;

  EquationContainerPointer    m_EquationContainer;
  LevelSetContainerPointer    m_LevelSetContainer;

  LevelSetOutputRealType      m_Alpha;
  LevelSetOutputRealType      m_Dt;
  LevelSetOutputRealType      m_RMSChangeAccumulator;
  bool                        m_UserGloballyDefinedTimeStep;
  IdentifierType              m_NumberOfIterations;
  IdentifierType              m_NumberOfLevelSetsForThreadingMethodChange;

  /** Helper variables to be used when parallel processing by processing parts
   * of the level set in each thread. */
  typename LevelSetContainerType::Iterator m_LevelSetToProcessWhenThreading;
  typename LevelSetContainerType::Iterator m_LevelSetUpdateToProcessWhenThreading;
  typename EquationContainerType::Iterator m_EquationContainerToProcessWhenThreading;

private:
  LevelSetEvolutionBase( const Self& ); // purposely not implemented
  void operator = ( const Self& );  // purposely not implemented
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetEvolutionBase.hxx"
#endif

#endif // __itkLevelSetEvolutionBase_h
