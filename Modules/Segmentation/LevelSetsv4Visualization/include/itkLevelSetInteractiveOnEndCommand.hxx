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
#ifndef __itkLevelSetInteractiveOnEndCommand_hxx
#define __itkLevelSetInteractiveOnEndCommand_hxx

#include "itkLevelSetInteractiveOnEndCommand.h"

#include "vtkRenderWindow.h"

namespace itk
{

template< class TVTKVisualizeLevelSetFilter >
LevelSetInteractiveOnEndCommand< TVTKVisualizeLevelSetFilter >
::LevelSetInteractiveOnEndCommand()
{
  this->m_RenderWindowInteractor = vtkSmartPointer< vtkRenderWindowInteractor >::New();
}

template< class TVTKVisualizeLevelSetFilter >
void
LevelSetInteractiveOnEndCommand< TVTKVisualizeLevelSetFilter >
::Execute( const Object* caller, const EventObject& event )
{
  this->Execute( const_cast< Object* >( caller ), event );
}

template< class TVTKVisualizeLevelSetFilter >
void
LevelSetInteractiveOnEndCommand< TVTKVisualizeLevelSetFilter >
::Execute( Object* itkNotUsed(caller), const EventObject& event )
{
  if( EndEvent().CheckEvent( &event ) )
    {
    itkAssertInDebugAndIgnoreInReleaseMacro( this->m_VTKVisualizeLevelSetFilter );
    vtkRenderWindow * renWin = this->m_VTKVisualizeLevelSetFilter->GetRenderWindow();
    renWin->Finalize();
    vtkRenderWindowInteractor * iren = renWin->MakeRenderWindowInteractor();
    iren->Start();
    std::cout << "renWin is; " << renWin << std::endl;
    this->m_RenderWindowInteractor->SetRenderWindow( renWin );
    std::cout << "about to initialize.." << std::endl;
    this->m_RenderWindowInteractor->Initialize();
    renWin->Render();
    std::cout << "About to Start" << std::endl;
    this->m_RenderWindowInteractor->Start();
    }
}

} // end namespace itk

#endif
