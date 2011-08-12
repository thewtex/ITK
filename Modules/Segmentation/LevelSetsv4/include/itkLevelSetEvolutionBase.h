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

#include "itkImage.h"
#include "itkLevelSetImageBase.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include <list>
#include "itkObject.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkNumericTraits.h"
#include "itkLevelSetEvolutionStoppingCriterionBase.h"

namespace itk
{
/**
 *  \class LevelSetEvolutionBase
 *  \brief Class for iterating and evolving the dense level-set function
 *
 *  \tparam TEquationContainer Container holding the system of level set of equations
 */
template< class TEquationContainer >
class LevelSetEvolutionBase : public Object
{
public:
  typedef LevelSetEvolutionBase      Self;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef Object                     Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

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
  typedef typename InputImageType::Pointer           InputImagePointer;
  typedef typename InputImageType::RegionType        InputImageRegionType;
  typedef typename NumericTraits< InputImagePixelType >::RealType
                                                     InputPixelRealType;

  itkStaticConstMacro ( ImageDimension, unsigned int, InputImageType::ImageDimension );

  typedef typename TermContainerType::LevelSetContainerType LevelSetContainerType;
  typedef typename LevelSetContainerType::LevelSetIdentifierType
                                                            LevelSetIdentifierType;
  typedef typename LevelSetContainerType::Pointer           LevelSetContainerPointer;
  typedef typename LevelSetContainerType::LevelSetContainerConstIteratorType
                                                            LevelSetContainerConstIteratorType;
  typedef typename LevelSetContainerType::LevelSetContainerIteratorType
                                                            LevelSetContainerIteratorType;

  typedef typename LevelSetContainerType::LevelSetType LevelSetType;
  typedef typename LevelSetType::Pointer               LevelSetPointer;
  typedef typename LevelSetType::ImageType             LevelSetImageType;
  typedef typename LevelSetType::OutputRealType        LevelSetOutputRealType;
  typedef typename LevelSetImageType::Pointer          LevelSetImagePointer;


  typedef BinaryThresholdImageFilter< LevelSetImageType, LevelSetImageType >
                                                       ThresholdFilterType;
  typedef typename ThresholdFilterType::Pointer        ThresholdFilterPointer;
  typedef SignedMaurerDistanceMapImageFilter< LevelSetImageType, LevelSetImageType >
                                                       MaurerType;
  typedef typename MaurerType::Pointer                 MaurerPointer;

  typedef ImageRegionIteratorWithIndex< LevelSetImageType > LevelSetImageIteratorType;

  typedef ImageRegionConstIteratorWithIndex< LevelSetImageType > LevelSetImageConstIteratorType;

  typedef ImageRegionIteratorWithIndex< InputImageType > InputImageIteratorType;

  typedef ImageRegionConstIteratorWithIndex< InputImageType > InputImageConstIteratorType;

  typedef std::list< IdentifierType >                    IdListType;
  typedef typename IdListType::iterator                  IdListIterator;
  typedef Image< IdListType, ImageDimension >            IdListImageType;
  typedef Image< short, ImageDimension >                 CacheImageType;
  typedef typename LevelSetContainerType::DomainMapImageFilterType
                                                         DomainMapImageFilterType;
  typedef typename DomainMapImageFilterType::Pointer     DomainMapImageFilterPointer;
  typedef typename DomainMapImageFilterType::NounToBeDefined NounToBeDefined;

  //   typedef typename DomainMapImageFilterType::DomainIteratorType DomainIteratorType;
  typedef typename std::map< itk::IdentifierType, NounToBeDefined >::iterator DomainIteratorType;

  typedef LevelSetEvolutionStoppingCriterionBase< LevelSetContainerType >
                                                  StoppingCriterionType;
  typedef typename StoppingCriterionType::Pointer StoppingCriterionPointer;

  itkSetObjectMacro( LevelSetContainer, LevelSetContainerType );
  itkGetObjectMacro( LevelSetContainer, LevelSetContainerType );

  /** Update the filter by computing the output level function
   * by calling GenerateData() once the instantiation of necessary variables
   * is verified */
  void Update()
    {
    m_DomainMapFilter = m_LevelSetContainer->GetDomainMapFilter();
    //Run iteration
    this->GenerateData();
    }

  /** Set/Get the value of alpha for computing the time-step using CFL conditions */
  itkSetMacro( Alpha, LevelSetOutputRealType );
  itkGetMacro( Alpha, LevelSetOutputRealType );

  /** Set a user-specified value of the time-step */
  void SetTimeStep( const LevelSetOutputRealType& iDt )
    {
    if( iDt > NumericTraits< LevelSetOutputRealType >::epsilon() )
      {
      m_UserDefinedDt = true;
      m_Dt = iDt;
      this->Modified();
      }
    else
      {
      itkGenericExceptionMacro( <<"iDt should be > epsilon")
      }
    }

  /** Set/Get the equation container for updating all the level sets */
  itkSetObjectMacro( EquationContainer, EquationContainerType );
  itkGetObjectMacro( EquationContainer, EquationContainerType );

  /** Set/Get the Stopping Criterion */
  itkGetObjectMacro( StoppingCriterion, StoppingCriterionType );
  itkSetObjectMacro( StoppingCriterion, StoppingCriterionType );


protected:
  LevelSetEvolutionBase() : m_StoppingCriterion( NULL ), m_InputImage( NULL ),
    m_EquationContainer( NULL ), m_LevelSetContainer( NULL ),
    m_UpdateBuffer( NULL ), m_DomainMapFilter( NULL ), m_Alpha( 0.9 ),
    m_Dt( 1. ), m_RMSChangeAccumulator( -1. ), m_UserDefinedDt( false )
  {}

  ~LevelSetEvolutionBase() {}

  StoppingCriterionPointer  m_StoppingCriterion;

  InputImagePointer           m_InputImage;
  EquationContainerPointer    m_EquationContainer;
  LevelSetContainerPointer    m_LevelSetContainer;
  LevelSetContainerPointer    m_UpdateBuffer;
  DomainMapImageFilterPointer m_DomainMapFilter;

  LevelSetOutputRealType      m_Alpha;
  LevelSetOutputRealType      m_Dt;
  LevelSetOutputRealType      m_RMSChangeAccumulator;
  bool                        m_UserDefinedDt;

  /** Initialize the update buffers for all level sets to hold the updates of
   *  equations in each iteration */
  void AllocateUpdateBuffer()
    {
    this->m_UpdateBuffer = LevelSetContainerType::New();
    this->m_UpdateBuffer->CopyInformationAndAllocate( m_LevelSetContainer, true );
    }

  /** Run the iterative loops of calculating levelset function updates until
   *  the stopping criterion is satisfied */
  void GenerateData()
    {
    std::cout << "Generate data" << std::endl;
    m_InputImage = m_EquationContainer->GetInput();

    // Get the LevelSetContainer from the EquationContainer
    m_LevelSetContainer = m_EquationContainer->GetEquation( 0 )->GetTerm( 0 )->GetLevelSetContainer();
    AllocateUpdateBuffer();

    InitializeIteration();

    typename StoppingCriterionType::IterationIdType iter = 0;
    m_StoppingCriterion->SetCurrentIteration( iter );
    m_StoppingCriterion->SetLevelSetContainer( m_LevelSetContainer );

    while( !m_StoppingCriterion->IsSatisfied() )
      {
      m_RMSChangeAccumulator = 0;

      // one iteration over all container
      // update each level set based on the different equations provided
      ComputeIteration();

//       ComputeCFL();

      ComputeDtForNextIteration();

      UpdateLevelSets();
      Reinitialize();
      UpdateEquations();

      // DEBUGGING
/*      typedef Image< unsigned char, ImageDimension > WriterImageType;
      typedef BinaryThresholdImageFilter< LevelSetImageType, WriterImageType >  FilterType;
      typedef ImageFileWriter< WriterImageType > WriterType;
      typedef typename WriterType::Pointer       WriterPointer;

      typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();
      while( it != m_LevelSetContainer->End() )
      {
        std::ostringstream filename;
        filename << "/home/krm15/temp/" << iter << "_" <<  it->GetIdentifier() << ".png";

        LevelSetPointer levelSet = it->GetLevelSet();

        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput( levelSet->GetImage() );
        filter->SetOutsideValue( 0 );
        filter->SetInsideValue(  255 );
        filter->SetLowerThreshold( NumericTraits<typename LevelSetImageType::PixelType>::NonpositiveMin() );
        filter->SetUpperThreshold( 0 );
        filter->Update();

        WriterPointer writer2 = WriterType::New();
        writer2->SetInput( filter->GetOutput() );
        writer2->SetFileName( filename.str().c_str() );
        writer2->Update();
        ++it;
      }
*/
      ++iter;

      m_StoppingCriterion->SetRMSChangeAccumulator( m_RMSChangeAccumulator );
      m_StoppingCriterion->SetCurrentIteration( iter );

      this->InvokeEvent( IterationEvent() );
      }
    }

  /** Initialize the iteration by computing parameters in the terms of the level set equation */
  void InitializeIteration()
  {
    DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
    DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

    std::cout << "Initialize iteration" << std::endl;

    // Initialize parameters here
    m_EquationContainer->InitializeParameters();

    while( map_it != map_end )
    {
//       std::cout << map_it->second.m_Region << std::endl;

      InputImageIteratorType it( m_InputImage, map_it->second.m_Region );
      it.GoToBegin();

      while( !it.IsAtEnd() )
      {
//         std::cout << it.GetIndex() << std::endl;
        IdListType lout = map_it->second.m_List;

        if( lout.empty() )
        {
          itkGenericExceptionMacro( <<"No level set exists at voxel" );
        }

        for( IdListIterator lIt = lout.begin(); lIt != lout.end(); ++lIt )
        {
//           std::cout << *lIt -1 << " ";
          m_EquationContainer->GetEquation( *lIt - 1 )->Initialize( it.GetIndex() );
        }
//         std::cout << std::endl;
        ++it;
      }
      ++map_it;
    }
    m_EquationContainer->Update();
  }

  /** Computer the update at each pixel and store in the update buffer */
  void ComputeIteration()
    {
    DomainIteratorType map_it = m_DomainMapFilter->m_LevelSetMap.begin();
    DomainIteratorType map_end = m_DomainMapFilter->m_LevelSetMap.end();

    std::cout << "Begin iteration" << std::endl;

    while( map_it != map_end )
      {
//       std::cout << map_it->second.m_Region << std::endl;

      InputImageIteratorType it( m_InputImage, map_it->second.m_Region );
      it.GoToBegin();

      while( !it.IsAtEnd() )
        {
//         std::cout << it.GetIndex() << std::endl;
        IdListType lout = map_it->second.m_List;

        if( lout.empty() )
          {
          itkGenericExceptionMacro( <<"No level set exists at voxel" );
          }

        for( IdListIterator lIt = lout.begin(); lIt != lout.end(); ++lIt )
          {
//           std::cout << *lIt << " ";
          LevelSetPointer levelSet = m_LevelSetContainer->GetLevelSet( *lIt - 1);
//           std::cout << levelSet->Evaluate( it.GetIndex() ) << ' ';

          LevelSetPointer levelSetUpdate = m_UpdateBuffer->GetLevelSet( *lIt - 1);

          LevelSetOutputRealType temp_update =
              m_EquationContainer->GetEquation( *lIt - 1 )->Evaluate( it.GetIndex() );

          levelSetUpdate->GetImage()->SetPixel( it.GetIndex(), temp_update );
          }
//         std::cout << std::endl;
        ++it;
        }
      ++map_it;
      }
    }

  /** Compute the time-step for the next iteration */
  void ComputeDtForNextIteration()
    {
    // if the time step is not globally set
    if( !m_UserDefinedDt )
      {
      if( ( m_Alpha > NumericTraits< LevelSetOutputRealType >::Zero ) &&
          ( m_Alpha < NumericTraits< LevelSetOutputRealType >::One ) )
        {
        LevelSetOutputRealType contribution = m_EquationContainer->ComputeCFLContribution();

        if( contribution > NumericTraits< LevelSetOutputRealType >::epsilon() )
          {
          m_Dt = m_Alpha / contribution;
          }
        else
          {
          if( contribution == NumericTraits< LevelSetOutputRealType >::max() )
            {
            itkGenericExceptionMacro( << "contribution is " << contribution );
            }
          else
            {
            itkGenericExceptionMacro( << "contribution is too low" );
            }
          }
        }
      else
        {
        itkGenericExceptionMacro( <<"m_Alpha should be in [0,1]" );
        }
      }

      std::cout << "Dt = " << m_Dt << std::endl;
//       m_Dt = 0.08;
    }

  /** Update the levelset by 1 iteration from the computed updates */
  virtual void UpdateLevelSets()
    {
    typename LevelSetContainerType::Iterator it1 = m_LevelSetContainer->Begin();
    typename LevelSetContainerType::ConstIterator it2 = m_UpdateBuffer->Begin();

    LevelSetOutputRealType p;

    while( it1 != m_LevelSetContainer->End() )
      {
      LevelSetImagePointer image1 = it1->GetLevelSet()->GetImage();
      LevelSetImagePointer image2 = it2->GetLevelSet()->GetImage();

      LevelSetImageIteratorType It1( image1, image1->GetBufferedRegion() );
      LevelSetImageIteratorType It2( image2, image2->GetBufferedRegion() );
      It1.GoToBegin();
      It2.GoToBegin();

      while( !It1.IsAtEnd() )
        {
        p = m_Dt * It2.Get();
        It1.Set( It1.Get() + p );

        m_RMSChangeAccumulator += p*p;

        ++It1;
        ++It2;
        }

      ++it1;
      ++it2;
      }
  }

  /** Update the equations at the end of 1 iteration */
  void UpdateEquations()
    {
    InitializeIteration();
//     m_EquationContainer->Update();
    }

  /** Reinitialize the level set functions to a signed distance function */
  void Reinitialize()
  {
    typename LevelSetContainerType::Iterator it = m_LevelSetContainer->Begin();

    while( it != m_LevelSetContainer->End() )
      {
      LevelSetImagePointer image = it->GetLevelSet()->GetImage();

      ThresholdFilterPointer thresh = ThresholdFilterType::New();
      thresh->SetLowerThreshold(
            NumericTraits< LevelSetOutputRealType >::NonpositiveMin() );
      thresh->SetUpperThreshold( 0 );
      thresh->SetInsideValue( 1 );
      thresh->SetOutsideValue( 0 );
      thresh->SetInput( image );
      thresh->Update();

      MaurerPointer maurer = MaurerType::New();
      maurer->SetInput( thresh->GetOutput() );
      maurer->SetSquaredDistance( false );
      maurer->SetUseImageSpacing( true );
      maurer->SetInsideIsPositive( false );
      maurer->Update();

      LevelSetImageIteratorType It1( image, image->GetBufferedRegion() );
      LevelSetImageIteratorType It2( maurer->GetOutput(), image->GetBufferedRegion() );
      It1.GoToBegin();
      It2.GoToBegin();
      while( !It1.IsAtEnd() )
        {
        It1.Set( It2.Get() );
        ++It1;
        ++It2;
        }
      ++it;
      }
  }

private:
  LevelSetEvolutionBase( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkLevelSetEvolutionBase_h
