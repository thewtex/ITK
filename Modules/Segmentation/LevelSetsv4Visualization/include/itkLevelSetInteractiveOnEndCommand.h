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
#ifndef __itkLevelSetInteractiveOnEndCommand_h
#define __itkLevelSetInteractiveOnEndCommand_h

#include "itkCommand.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"

namespace itk
{

/** \class LevelSetInteractiveOnEndCommand
 * \brief Starts up a vtkRenderWindowInteractor at the end of LevelSets
 * evolution to interact with the visualization.
 *
 * \tparam TVTKVisualizeLevelSetFilter VTK LevelSet visualization class.
 *
 * \ingroup ITKLevelSetsv4Visualization
 */
template< class TVTKVisualizeLevelSetFilter >
class LevelSetInteractiveOnEndCommand : public Command
{
public:
  typedef LevelSetInteractiveOnEndCommand Self;
  typedef Command                         Superclass;
  typedef SmartPointer< Self >            Pointer;
  typedef SmartPointer< const Self >      ConstPointer;

  typedef TVTKVisualizeLevelSetFilter VTKVisualizeLevelSetFilterType;

  /** Run-time type information (and related methods). */
  itkTypeMacro( LevelSetInteractiveOnEndCommand, Command );

  itkNewMacro( Self );

  virtual void Execute( const Object* caller, const EventObject& event );

  virtual void Execute( Object* caller, const EventObject& event );

  /** Set/Get the filter that does the visualization. */
  itkSetObjectMacro( VTKVisualizeLevelSetFilter, VTKVisualizeLevelSetFilterType );
  itkGetConstObjectMacro( VTKVisualizeLevelSetFilter, VTKVisualizeLevelSetFilterType );

protected:
  LevelSetInteractiveOnEndCommand();

private:
  LevelSetInteractiveOnEndCommand( const Self & ); // purposely not implemented
  void operator= ( const Self& ); // purposely not implemented;

  typename VTKVisualizeLevelSetFilterType::ConstPointer m_VTKVisualizeLevelSetFilter;

  vtkSmartPointer< vtkRenderWindowInteractor > m_RenderWindowInteractor;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetInteractiveOnEndCommand.hxx"
#endif

#endif
