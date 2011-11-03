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

#ifndef __itkSparseLevelSetEvolutionCommandIterationUpdate_h
#define __itkSparseLevelSetEvolutionCommandIterationUpdate_h

#include "itkCommand.h"
#include "vtkVisualize2DSparseLevelSetLayers.h"

namespace itk
{
template< class TLevelSetFilter >
class SparseLevelSetEvolutionCommandIterationUpdate : public Command
{
public:
  typedef SparseLevelSetEvolutionCommandIterationUpdate Self;
  typedef Command                                       Superclass;
  typedef SmartPointer< Self >                          Pointer;
  typedef SmartPointer< const Self >                    ConstPointer;

  typedef TLevelSetFilter                       LevelSetFilterType;
  typedef typename LevelSetFilterType::Pointer  LevelSetFilterPointer;

  typedef typename LevelSetFilterType::LevelSetType     LevelSetType;
  typedef typename LevelSetFilterType::LevelSetPointer  LevelSetPointer;

  typedef vtkVisualize2DSparseLevelSetLayers< LevelSetType >  VisualizationType;
  typedef typename VisualizationType::Pointer                 VisualizationPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( SparseLevelSetEvolutionCommandIterationUpdate, Command );

  itkNewMacro( SparseLevelSetEvolutionCommandIterationUpdate );

  void Execute( const Object* caller, const EventObject& event )
    {
    this->Execute( const_cast< Object* >( caller ), event );
    }

  void Execute( Object* caller, const EventObject& event )
    {
    LevelSetFilterType* filter =
      dynamic_cast< LevelSetFilterType* >( caller );

    if( filter )
      {
      if( IterationEvent().CheckEvent( &event ) )
        {
        LevelSetContainerType* lsContainer = filter->GetLevelSetContainer();
        LevelSetContainerIterator lsIt = lsContainer->Begin();

        LevelSetType* levelSet = lsIt->GetLevelSet();

        EquationContainerType* equationContainer = filter->GetEquationContainer();
        const InputImageType* input = equationContainer->GetInput();

        m_Viewer->SetInputImage( input );
        m_Viewer->SetLevelSet( levelSet );
        m_Viewer->SetScreenCapture( true );
        m_Viewer->Update();
        }
      }
    }

protected:
  SparseLevelSetEvolutionCommandIterationUpdate()
  {
    m_Viewer = VisualizationType::New();
    m_Viewer->SetPeriod( 1 );
  }

  ~SparseLevelSetEvolutionCommandIterationUpdate() {}

private:
  SparseLevelSetEvolutionCommandIterationUpdate( const Self& );
  void operator = ( const Self& );

  typedef typename LevelSetFilterType::LevelSetContainerType  LevelSetContainerType;
  typedef typename LevelSetContainerType::Iterator            LevelSetContainerIterator;

  typedef typename LevelSetFilterType::EquationContainerType  EquationContainerType;
  typedef typename EquationContainerType::InputImageType      InputImageType;

  VisualizationPointer m_Viewer;
};

  }

#endif
