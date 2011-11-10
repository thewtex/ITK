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
#ifndef __itkIterationUpdateCommand_h
#define __itkIterationUpdateCommand_h

#include "itkCommand.h"
#include "itkWeakPointer.h"

namespace itk
{

/** \class IterationUpdateCommand
 * \brief Call update on one filter when another filter iterates.
 *
 * \tparam TIteratingFilter Filter that invokes iteration events.
 * \tparam TFilterToUpdate  Filter to call update on when the iteration event
 * occurs.
 *
 * \ingroup ITKLevelSetsv4Visualization
 */
template< class TIteratingFilter, class TFilterToUpdate >
class IterationUpdateCommand : public Command
{
public:
  typedef IterationUpdateCommand      Self;
  typedef Command                     Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  typedef TIteratingFilter  IteratingFilterType;
  typedef TFilterToUpdate   FilterToUpdateType;

  /** Run-time type information (and related methods). */
  itkTypeMacro( IterationUpdateCommand, Command );

  itkNewMacro( Self );

  virtual void Execute( const Object* caller, const EventObject& event );

  virtual void Execute( Object* caller, const EventObject& event );

  /** Set/Get the filter to call Update() on. */
  itkSetObjectMacro( FilterToUpdate, FilterToUpdateType );
  itkGetConstObjectMacro( FilterToUpdate, FilterToUpdateType );

protected:
  IterationUpdateCommand() {}
  virtual ~IterationUpdateCommand() {}

private:
  IterationUpdateCommand( const Self& ); // purposely not implemented
  void operator= ( const Self& ); // purposely not implemented

  WeakPointer< FilterToUpdateType >  m_FilterToUpdate;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIterationUpdateCommand.hxx"
#endif

#endif
