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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#include "itkProcessObject.h"
#include "itkCommand.h"

#include <functional>
#include <algorithm>

namespace itk
{
/**
 * Instantiate object with no start, end, or progress methods.
 */
ProcessObject
::ProcessObject() :
  m_CachedNamedInputReleaseDataFlags(),
  m_NamedInputs(),
  m_NamedOutputs()
{
  m_NumberOfRequiredInputs = 0;

  m_NumberOfRequiredOutputs = 0;

  m_AbortGenerateData = false;
  m_Progress = 0.0f;
  m_Updating = false;

  m_Threader = MultiThreader::New();
  m_NumberOfThreads = m_Threader->GetNumberOfThreads();

  m_ReleaseDataBeforeUpdateFlag = true;
}

/**
 * This is a default implementation to make sure we have something.
 * Once all the subclasses of ProcessObject provide an appopriate
 * MakeOutput(), then ProcessObject::MakeOutput() can be made pure
 * virtual.
 */
DataObject::Pointer
ProcessObject
::MakeOutput(unsigned int)
{
  return static_cast< DataObject * >( DataObject::New().GetPointer() );
}

/**
 * Destructor for the ProcessObject class. We've got to
 * UnRegister() the use of any input classes.
 */
ProcessObject
::~ProcessObject()
{
  // Tell each output that we are going away.  If other objects have a
  // reference to one of these outputs, the data object will not be deleted
  // when the process object is deleted.  However, the data object's source
  // will still point back to the now nonexistent process object if we do not
  // clean things up now.
  unsigned int idx;

  for ( idx = 0; idx < m_Outputs.size(); ++idx )
    {
    if ( m_Outputs[idx] )
      {
      // let the output know we no longer want to associate with the object
      m_Outputs[idx]->DisconnectSource(this, idx);
      // let go of our reference to the data object
      m_Outputs[idx] = 0;
      }
    }
  for ( DataObjectPointerMap::iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
    {
    if ( it->second )
      {
      // let the output know we no longer want to associate with the object
      it->second->DisconnectSource(this, idx);
      // let go of our reference to the data object
      it->second = 0;
      }
    }
}

ProcessObject::ConstDataObjectArray
ProcessObject
::GetAllInputs() const
{
  ConstDataObjectArray res;
  for ( unsigned int idx = 0; idx < m_Inputs.size(); ++idx )
    {
    res.push_back( m_Inputs[idx].GetPointer() );
    }
  for ( DataObjectPointerMap::const_iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::ConstDataObjectArray
ProcessObject
::GetNamedInputs() const
{
  ConstDataObjectArray res;
  for ( DataObjectPointerMap::const_iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::DataObjectArray
ProcessObject
::GetAllInputs()
{
  DataObjectArray res;
  for ( unsigned int idx = 0; idx < m_Inputs.size(); ++idx )
    {
    res.push_back( m_Inputs[idx].GetPointer() );
    }
  for ( DataObjectPointerMap::iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::DataObjectArray
ProcessObject
::GetNamedInputs()
{
  DataObjectArray res;
  for ( DataObjectPointerMap::iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::ConstDataObjectArray
ProcessObject
::GetAllOutputs() const
{
  ConstDataObjectArray res;
  for ( unsigned int idx = 0; idx < m_Outputs.size(); ++idx )
    {
    res.push_back( m_Outputs[idx].GetPointer() );
    }
  for ( DataObjectPointerMap::const_iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::ConstDataObjectArray
ProcessObject
::GetNamedOutputs() const
{
  ConstDataObjectArray res;
  for ( DataObjectPointerMap::const_iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::DataObjectArray
ProcessObject
::GetAllOutputs()
{
  DataObjectArray res;
  for ( unsigned int idx = 0; idx < m_Outputs.size(); ++idx )
    {
    res.push_back( m_Outputs[idx].GetPointer() );
    }
  for ( DataObjectPointerMap::iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

ProcessObject::DataObjectArray
ProcessObject
::GetNamedOutputs()
{
  DataObjectArray res;
  for ( DataObjectPointerMap::iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
    {
    res.push_back( it->second.GetPointer() );
    }

  return res;
}

//typedef DataObject *DataObjectPointer;

/**
 * Called by constructor to set up input array.
 */
void
ProcessObject
::SetNumberOfInputs(unsigned int num)
{
  // in case nothing has changed.
  if ( num == m_Inputs.size() )
    {
    return;
    }
  m_Inputs.resize(num);
  this->Modified();
}

/**
 * Get the number of specified inputs
 */
ProcessObject::DataObjectPointerArraySizeType
ProcessObject
::GetNumberOfValidRequiredInputs() const
{
  DataObjectPointerArraySizeType num;

  if ( m_NumberOfRequiredInputs < m_Inputs.size() )
    {
    num = m_NumberOfRequiredInputs;
    }
  else
    {
    num = m_Inputs.size();
    }
  // count the number of non-null inputs
  // this used to use std::count_if, but that function object
  // did not work correctly with SunPro CC 5.6.
  int count = 0;
  for ( std::vector< DataObjectPointer >::const_iterator i = m_Inputs.begin();
        i < ( m_Inputs.begin() + num ); ++i )
    {
    if ( ( *i ).IsNotNull() )
      {
      count++;
      }
    }
  return count;
}

/**
 * Adds an input to the first null position in the input list.
 * Expands the list memory if necessary
 */
void
ProcessObject
::AddInput(DataObject *input)
{
  DataObjectPointerArraySizeType idx;

  this->Modified();

  for ( idx = 0; idx < m_Inputs.size(); ++idx )
    {
    if ( !m_Inputs[idx] )
      {
      m_Inputs[idx] = input;
      return;
      }
    }

  this->SetNumberOfInputs( static_cast< int >( m_Inputs.size() + 1 ) );
  m_Inputs[static_cast< int >( m_Inputs.size() ) - 1] = input;
}

/**
 * Remove an input.
 *
 * Removes the first occurence of the given OutputObject from the
 * inputs to this ProcessObject.  If it's the last object on the
 * list, shortens the list.
 */
void
ProcessObject
::RemoveInput(DataObject *input)
{
  if ( !input )
    {
    return;
    }

  // find the input in the list of inputs
  DataObjectPointerArray::iterator pos =
    std::find(m_Inputs.begin(), m_Inputs.end(), input);

  if ( pos == m_Inputs.end() )
    {
    itkDebugMacro("tried to remove an input that was not in the list");
    return;
    }

  // Set the position in the m_Inputs containing input to 0
  *pos = 0;

  // if that was the last input, then shrink the list
  if ( pos == m_Inputs.end() - 1 )
    {
    this->SetNumberOfInputs(static_cast< int >( m_Inputs.size() ) - 1);
    }

  this->Modified();
}

/**
 * Set an Input of this filter. This method
 * does Register()/UnRegister() manually to
 * deal with the fact that smart pointers aren't
 * around to do the reference counting.
 */
void
ProcessObject
::SetNthInput(unsigned int idx, DataObject *input)
{
  // does this change anything?
  if ( idx < m_Inputs.size() && m_Inputs[idx] == input )
    {
    return;
    }

  // Expand array if necessary.
  if ( idx >= m_Inputs.size() )
    {
    this->SetNumberOfInputs(idx + 1);
    }

  m_Inputs[idx] = input;

  this->Modified();
}

/**
 * Model a queue on the input list by providing a push back
 */
void
ProcessObject
::PushBackInput(const DataObject *input)
{
  m_Inputs.push_back( const_cast< DataObject * >( input ) );
  this->Modified();
}

/**
 * Model a stack on the input list by providing a pop back
 */
void
ProcessObject
::PopBackInput()
{
  if ( !m_Inputs.empty() )
    {
    m_Inputs.pop_back();
    this->Modified();
    }
}

/**
 *
 */
void
ProcessObject
::PushFrontInput(const DataObject *input)
{
  // add an empty element to the end of the vector to make sure that
  // we have enough space for the copy
  m_Inputs.push_back(0);

  // shift the current inputs down by one place
  std::copy_backward( m_Inputs.begin(), m_Inputs.end() - 1,
                      m_Inputs.end() );

  // put in the new input in the front
  m_Inputs[0] = const_cast< DataObject * >( input );

  this->Modified();
}

/**
 *
 */
void
ProcessObject
::PopFrontInput()
{
  if ( !m_Inputs.empty() )
    {
    std::copy( m_Inputs.begin() + 1, m_Inputs.end(),
               m_Inputs.begin() );

    m_Inputs.pop_back();
    this->Modified();
    }
}

void
ProcessObject
::RemoveOutput(DataObject *output)
{
  if ( !output )
    {
    return;
    }

  // find the input in the list of inputs
  DataObjectPointerArray::iterator pos =
    std::find(m_Outputs.begin(), m_Outputs.end(), output);

  if ( pos == m_Outputs.end() )
    {
    itkDebugMacro("tried to remove an output that was not in the list");
    return;
    }

  // let the output know we no longer want to associate with the object
  ( *pos )->DisconnectSource( this, pos - m_Outputs.begin() );
  // let go of our reference to the data object
  *pos = 0;

  // if that was the last output, then shrink the list
  if ( pos == m_Outputs.end() - 1 )
    {
    this->SetNumberOfOutputs(static_cast< int >( m_Outputs.size() ) - 1);
    }

  this->Modified();
}

/**
 * Set an output of this filter. This method specifically
 * does not do a Register()/UnRegister() because of the
 * desire to break the reference counting loop.
 */
void
ProcessObject
::SetNthOutput(unsigned int idx, DataObject *output)
{
  // does this change anything?
  if ( idx < m_Outputs.size() && output == m_Outputs[idx] )
    {
    return;
    }

  // Expand array if necessary.
  if ( idx >= m_Outputs.size() )
    {
    this->SetNumberOfOutputs(idx + 1);
    }

  // Keep a handle to the original output and disconnect the old output from
  // the pipeline
  DataObjectPointer oldOutput;
  if ( m_Outputs[idx] )
    {
    oldOutput = m_Outputs[idx];
    m_Outputs[idx]->DisconnectSource(this, idx);
    }

  if ( output )
    {
    output->ConnectSource(this, idx);
    }
  // save the current reference (which releases the previous reference)
  m_Outputs[idx] = output;

  // if we are clearing an output, we need to create a new blank output
  // so we are prepared for the next Update(). this copies the requested
  // region ivar
  if ( !m_Outputs[idx] )
    {
    itkDebugMacro(" creating new output object.");
    DataObjectPointer newOutput = this->MakeOutput(idx);
    this->SetNthOutput(idx, newOutput);

    // If we had an output object before, copy the requested region
    // ivars and release data flag to the the new output
    if ( oldOutput )
      {
      newOutput->SetRequestedRegion(oldOutput);
      newOutput->SetReleaseDataFlag( oldOutput->GetReleaseDataFlag() );
      }
    }

  this->Modified();
}

/**
 * Adds an output to the first null position in the output list.
 * Expands the list memory if necessary
 */
void
ProcessObject
::AddOutput(DataObject *output)
{
  unsigned int idx;

  for ( idx = 0; idx < m_Outputs.size(); ++idx )
    {
    if ( m_Outputs[idx].IsNull() )
      {
      m_Outputs[idx] = output;

      if ( output )
        {
        output->ConnectSource(this, idx);
        }
      this->Modified();

      return;
      }
    }

  this->SetNumberOfOutputs(static_cast< int >( m_Outputs.size() ) + 1);
  m_Outputs[static_cast< int >( m_Outputs.size() ) - 1] = output;
  if ( output )
    {
    output->ConnectSource(this, static_cast< int >( m_Outputs.size() ) - 1);
    }
  this->Modified();
}

/**
 * Called by constructor to set up output array.
 */
void
ProcessObject
::SetNumberOfOutputs(unsigned int num)
{
  // in case nothing has changed.
  if ( num == m_Outputs.size() )
    {
    return;
    }
  m_Outputs.resize(num);
  this->Modified();
}

/**
 *
 */
DataObject *
ProcessObject
::GetOutput(unsigned int i)
{
  if ( m_Outputs.size() < i + 1 )
    {
    return NULL;
    }

  return m_Outputs[i].GetPointer();
}

const DataObject *
ProcessObject
::GetOutput(unsigned int i) const
{
  if ( m_Outputs.size() < i + 1 )
    {
    return NULL;
    }

  return m_Outputs[i].GetPointer();
}

/**
 *
 */
DataObject *
ProcessObject
::GetOutput(const std::string & key)
{
  DataObjectPointerMap::iterator it = m_NamedOutputs.find(key);
  if ( it == m_NamedOutputs.end() )
    {
    return NULL;
    }

  return it->second.GetPointer();
}

const DataObject *
ProcessObject
::GetOutput(const std::string & key) const
{
  DataObjectPointerMap::const_iterator it = m_NamedOutputs.find(key);
  if ( it == m_NamedOutputs.end() )
    {
    return NULL;
    }

  return it->second.GetPointer();
}

/**
 *
 */
void
ProcessObject
::SetOutput(const std::string & key, DataObject * output)
{
  if ( m_NamedOutputs[key] != output )
    {
    m_NamedOutputs[key] = output;
    this->Modified();
    }
}

/**
 *
 */
DataObject *
ProcessObject
::GetInput(unsigned int i)
{
  if ( m_Inputs.size() < i + 1 )
    {
    return NULL;
    }

  return m_Inputs[i].GetPointer();
}

const DataObject *
ProcessObject
::GetInput(unsigned int i) const
{
  if ( m_Inputs.size() < i + 1 )
    {
    return NULL;
    }

  return m_Inputs[i].GetPointer();
}

/**
 *
 */
DataObject *
ProcessObject
::GetInput(const std::string & key)
{
  DataObjectPointerMap::iterator it = m_NamedInputs.find(key);
  if ( it == m_NamedInputs.end() )
    {
    return NULL;
    }

  return it->second.GetPointer();
}

const DataObject *
ProcessObject
::GetInput(const std::string & key) const
{
  DataObjectPointerMap::const_iterator it = m_NamedInputs.find(key);
  if ( it == m_NamedInputs.end() )
    {
    return NULL;
    }

  return it->second.GetPointer();
}

/**
 *
 */
void
ProcessObject
::SetInput(const std::string & key, DataObject * input)
{
  if ( m_NamedInputs[key] != input )
    {
    m_NamedInputs[key] = input;
    this->Modified();
    }
}

/**
 * Update the progress of the process object. If a ProgressMethod exists,
 * execute it. Then set the Progress ivar to amount. The parameter amount
 * should range between (0,1).
 */
void
ProcessObject
::UpdateProgress(float amount)
{
  m_Progress = amount;
  this->InvokeEvent( ProgressEvent() );
}

/**
 *
 */
bool
ProcessObject
::GetReleaseDataFlag() const
{
  if ( this->GetOutput(0) )
    {
    return this->GetOutput(0)->GetReleaseDataFlag();
    }
  itkWarningMacro(<< "Output doesn't exist!");
  return false;
}

/**
 *
 */
void
ProcessObject
::SetReleaseDataFlag(bool val)
{
  DataObjectArray outputs = this->GetAllOutputs();
  for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
    {
    if ( *it )
      {
      (*it)->SetReleaseDataFlag(val);
      }
    }
}

/**
 *
 */
void
ProcessObject
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number Of Required Inputs: "
     << m_NumberOfRequiredInputs << std::endl;

  os << indent << "Number Of Required Outputs: "
     << m_NumberOfRequiredOutputs << std::endl;

  os << indent << "Number Of Threads: "
     << m_NumberOfThreads << std::endl;

  os << indent << "ReleaseDataFlag: "
     << ( this->GetReleaseDataFlag() ? "On" : "Off" ) << std::endl;

  os << indent << "ReleaseDataBeforeUpdateFlag: "
     << ( m_ReleaseDataBeforeUpdateFlag ? "On" : "Off" ) << std::endl;

  if ( m_Inputs.size() )
    {
    DataObjectPointerArraySizeType idx;
    for ( idx = 0; idx < m_Inputs.size(); ++idx )
      {
      os << indent << "Input " << static_cast< int >( idx );
      os << ": (" << m_Inputs[idx].GetPointer() << ")\n";
      }
    }
  else
    {
    os << indent << "No Inputs\n";
    }
  if ( !m_NamedInputs.empty() )
    {
    for ( DataObjectPointerMap::const_iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
      {
      if ( it->second )
        {
        os << indent << "Input " << it->first;
        os << ": (" << it->second.GetPointer() << ")\n";
        }
      }
    }
  else
    {
    os << indent << "No Named Inputs\n";
    }
  if ( m_Outputs.size() )
    {
    DataObjectPointerArraySizeType idx;
    for ( idx = 0; idx < m_Outputs.size(); ++idx )
      {
      os << indent << "Output " << static_cast< int >( idx );
      os << ": (" << m_Outputs[idx].GetPointer() << ")\n";
      }
    }
  else
    {
    os << indent << "No Output\n";
    }
  if ( !m_NamedOutputs.empty() )
    {
    for ( DataObjectPointerMap::const_iterator it=m_NamedOutputs.begin(); it!=m_NamedOutputs.end(); it++ )
      {
      if ( it->second )
        {
        os << indent << "Output " << it->first;
        os << ": (" << it->second.GetPointer() << ")\n";
        }
      }
    }
  else
    {
    os << indent << "No Named Outputs\n";
    }

  os << indent << "AbortGenerateData: " << ( m_AbortGenerateData ? "On\n" : "Off\n" );
  os << indent << "Progress: " << m_Progress << "\n";

  os << indent << "Multithreader: " << std::endl;
  m_Threader->PrintSelf( os, indent.GetNextIndent() );
}

/**
 * The following methods are used to coordinate the execution of the
 * data processing pipeline.
 */

/**
 *
 */
void
ProcessObject
::Update()
{
  if ( this->GetOutput(0) )
    {
    this->GetOutput(0)->Update();
    }
}

void
ProcessObject
::ResetPipeline()
{
  if ( this->GetOutput(0) )
    {
    this->GetOutput(0)->ResetPipeline();
    }
}

void
ProcessObject
::PropagateResetPipeline()
{
  //
  // Reset this object.
  //
  // Clear the updating flag.
  m_Updating = 0;

  //
  // Loop through the inputs
  //
  DataObjectArray inputs = this->GetAllInputs();
  for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
    {
    if ( *it )
      {
      (*it)->PropagateResetPipeline();
      }
    }
}

/**
 *
 */
void
ProcessObject
::UpdateOutputInformation()
{
  unsigned long                  t1, t2;
  DataObject *                   input;
  DataObject *                   output;

  /**
   * Watch out for loops in the pipeline
   */
  if ( m_Updating )
    {
    /**
     * Since we are in a loop, we will want to update. But if
     * we don't modify this filter, then we will not execute
     * because our OutputInformationMTime will be more recent than
     * the MTime of our output.
     */
    this->Modified();
    return;
    }

  /**
   * We now wish to set the PipelineMTime of each output DataObject to
   * the largest of this ProcessObject's MTime, all input DataObject's
   * PipelineMTime, and all input's MTime.  We begin with the MTime of
   * this ProcessObject.
   */
  t1 = this->GetMTime();

  /**
   * Loop through the inputs
   */
  DataObjectArray inputs = this->GetAllInputs();
  for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
    {
    if ( *it )
      {
      input = *it;

      /**
       * Propagate the UpdateOutputInformation call
       */
      m_Updating = true;
      input->UpdateOutputInformation();
      m_Updating = false;

      /**
       * What is the PipelineMTime of this input? Compare this against
       * our current computation to find the largest one.
       */
      t2 = input->GetPipelineMTime();

      if ( t2 > t1 )
        {
        t1 = t2;
        }

      /**
       * Pipeline MTime of the input does not include the MTime of the
       * data object itself. Factor these mtimes into the next PipelineMTime
       */
      t2 = input->GetMTime();
      if ( t2 > t1 )
        {
        t1 = t2;
        }
      }
    }

  /**
   * Call GenerateOutputInformation for subclass specific information.
   * Since UpdateOutputInformation propagates all the way up the pipeline,
   * we need to be careful here to call GenerateOutputInformation only if
   * necessary. Otherwise, we may cause this source to be modified which
   * will cause it to execute again on the next update.
   */
  if ( t1 > m_OutputInformationMTime.GetMTime() )
    {
    DataObjectArray outputs = this->GetAllOutputs();
    for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
      {
      output = *it;
      if ( output )
        {
        output->SetPipelineMTime(t1);
        }
      }

    this->GenerateOutputInformation();

    /**
     * Keep track of the last time GenerateOutputInformation() was called
     */
    m_OutputInformationMTime.Modified();
    }
}

/**
 *
 */
void
ProcessObject
::PropagateRequestedRegion(DataObject *output)
{
  /**
   * check flag to avoid executing forever if there is a loop
   */
  if ( m_Updating )
    {
    return;
    }

  /**
   * Give the subclass a chance to indicate that it will provide
   * more data then required for the output. This can happen, for
   * example, when a source can only produce the whole output.
   * Although this is being called for a specific output, the source
   * may need to enlarge all outputs.
   */
  this->EnlargeOutputRequestedRegion(output);

  /**
   * Give the subclass a chance to define how to set the requested
   * regions for each of its outputs, given this output's requested
   * region.  The default implementation is to make all the output
   * requested regions the same.  A subclass may need to override this
   * method if each output is a different resolution.
   */
  this->GenerateOutputRequestedRegion(output);

  /**
   * Give the subclass a chance to request a larger requested region on
   * the inputs. This is necessary when, for example, a filter
   * requires more data at the "internal" boundaries to
   * produce the boundary values - such as an image filter that
   * derives a new pixel value by applying some operation to a
   * neighborhood of surrounding original values.
   */
  this->GenerateInputRequestedRegion();

  /**
   * Now that we know the input requested region, propagate this
   * through all the inputs.
   */
  m_Updating = true;
  DataObjectArray inputs = this->GetAllInputs();
  for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
    {
    if ( *it )
      {
      (*it)->PropagateRequestedRegion();
      }
    }
  m_Updating = false;
}

/**
 * By default we require all the input to produce the output. This is
 * overridden in the subclasses since we can often produce the output with
 * just a portion of the input data.
 */
void
ProcessObject
::GenerateInputRequestedRegion()
{
  DataObjectArray inputs = this->GetAllInputs();
  for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
    {
    if ( *it )
      {
      (*it)->SetRequestedRegionToLargestPossibleRegion();
      }
    }
}

/**
 * By default we set all the output requested regions to be the same.
 */
void
ProcessObject
::GenerateOutputRequestedRegion(DataObject *output)
{
  DataObjectArray outputs = this->GetAllOutputs();
  for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
    {
    if ( *it && *it != output )
      {
      (*it)->SetRequestedRegion(output);
      }
    }
}

/**
 *
 */
void
ProcessObject
::PrepareOutputs()
{
  if ( this->GetReleaseDataBeforeUpdateFlag() )
    {
    DataObjectArray outputs = this->GetAllOutputs();
    for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
      {
      if ( *it )
        {
        (*it)->PrepareForNewData();
        }
      }
    }
}

/**
 *
 */
void
ProcessObject
::ReleaseInputs()
{
  DataObjectArray inputs = this->GetAllInputs();
  for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
    {
    if ( *it )
      {
      if ( (*it)->ShouldIReleaseData() )
        {
        (*it)->ReleaseData();
        }
      }
    }
}

/**
 *
 */
void
ProcessObject
::UpdateOutputData( DataObject *itkNotUsed(output) )
{
  /**
   * prevent chasing our tail
   */
  if ( m_Updating )
    {
    return;
    }

  /**
   * Prepare all the outputs. This may deallocate previous bulk data.
   */
  this->PrepareOutputs();

  /**
   * Propagate the update call - make sure everything we
   * might rely on is up-to-date
   * Must call PropagateRequestedRegion before UpdateOutputData if multiple
   * inputs since they may lead back to the same data object.
   */
  m_Updating = true;
  DataObjectArray inputs = this->GetAllInputs();
  if ( inputs.size() == 1 )
    {
    if ( inputs[0] )
      {
      inputs[0]->UpdateOutputData();
      }
    }
  else
    {
    for ( DataObjectArray::iterator it=inputs.begin(); it!=inputs.end(); it++ )
      {
      if ( *it )
        {
        (*it)->PropagateRequestedRegion();
        (*it)->UpdateOutputData();
        }
      }
    }

  /**
   * Cache the state of any ReleaseDataFlag's on the inputs. While the
   * filter is executing, we need to set the ReleaseDataFlag's on the
   * inputs to false in case the current filter is implemented using a
   * mini-pipeline (which will try to release the inputs).  After the
   * filter finishes, we restore the state of the ReleaseDataFlag's
   * before the call to ReleaseInputs().
   */
  this->CacheInputReleaseDataFlags();

  /**
   * Tell all Observers that the filter is starting
   */
  this->InvokeEvent( StartEvent() );

  /**
   * GenerateData this object - we have not aborted yet, and our progress
   * before we start to execute is 0.0.
   */
  m_AbortGenerateData = false;
  m_Progress = 0.0f;

  try
    {
    /**
    * Count the number of required inputs which have been assigned
    */
    DataObjectPointerArraySizeType ninputs = this->GetNumberOfValidRequiredInputs();
    if ( ninputs < m_NumberOfRequiredInputs )
      {
      itkExceptionMacro(<< "At least " << m_NumberOfRequiredInputs
                        << " inputs are required but only " << ninputs
                        << " are specified.");
      }

    this->GenerateData();
    }
  catch ( ProcessAborted & excp )
    {
    this->InvokeEvent( AbortEvent() );
    this->ResetPipeline();
    this->RestoreInputReleaseDataFlags();
    throw excp;
    }
  catch (...)
    {
    this->ResetPipeline();
    this->RestoreInputReleaseDataFlags();
    throw;
    }

  /**
   * If we ended due to aborting, push the progress up to 1.0 (since
   * it probably didn't end there)
   *
   */
  if ( m_AbortGenerateData )
    {
    this->UpdateProgress(1.0f);
    }

  /**
   * Notify end event observers
   */
  this->InvokeEvent( EndEvent() );

  /**
   * Now we have to mark the data as up to date.
   */
  DataObjectArray outputs = this->GetAllOutputs();
  for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
    {
    if ( *it )
      {
      (*it)->DataHasBeenGenerated();
      }
    }

  /**
   * Restore the state of any input ReleaseDataFlags
   */
  this->RestoreInputReleaseDataFlags();

  /**
   * Release any inputs if marked for release
   */
  this->ReleaseInputs();

  // Mark that we are no longer updating the data in this filter
  m_Updating = false;
}

/**
 *
 */
void
ProcessObject
::CacheInputReleaseDataFlags()
{
  unsigned int idx;

  m_CachedInputReleaseDataFlags.resize( m_Inputs.size() );
  for ( idx = 0; idx < m_Inputs.size(); ++idx )
    {
    if ( m_Inputs[idx] )
      {
      m_CachedInputReleaseDataFlags[idx] = m_Inputs[idx]->GetReleaseDataFlag();
      m_Inputs[idx]->ReleaseDataFlagOff();
      }
    else
      {
      m_CachedInputReleaseDataFlags[idx] = false;
      }
    }
  for ( DataObjectPointerMap::iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    if ( it->second )
      {
      m_CachedNamedInputReleaseDataFlags[it->first] = it->second->GetReleaseDataFlag();
      it->second->ReleaseDataFlagOff();
      }
    else
      {
      m_CachedNamedInputReleaseDataFlags[it->first] = false;
      }
    }
}

/**
 *
 */
void
ProcessObject
::RestoreInputReleaseDataFlags()
{
  unsigned int idx;

  for ( idx = 0;
        idx < m_Inputs.size() && idx < m_CachedInputReleaseDataFlags.size();
        ++idx )
    {
    if ( m_Inputs[idx] )
      {
      m_Inputs[idx]->SetReleaseDataFlag(m_CachedInputReleaseDataFlags[idx]);
      }
    }
  for ( DataObjectPointerMap::iterator it=m_NamedInputs.begin(); it!=m_NamedInputs.end(); it++ )
    {
    if ( it->second )
      {
      it->second->SetReleaseDataFlag(m_CachedNamedInputReleaseDataFlags[it->first]);
      }
    }
  m_CachedNamedInputReleaseDataFlags.clear();
}

/**
 * Default implementation - copy information from first input to all outputs
 */
void
ProcessObject
::GenerateOutputInformation()
{
  DataObjectPointer input, output;

  DataObjectArray inputs = this->GetAllInputs();
  if ( inputs.size() && inputs[0] )
    {
    input = m_Inputs[0];

    DataObjectArray outputs = this->GetAllOutputs();
    for ( DataObjectArray::iterator it=outputs.begin(); it!=outputs.end(); it++ )
      {
      output = *it;
      if ( output )
        {
        output->CopyInformation(input);
        }
      }
    }
}

/**
 *
 */
void
ProcessObject
::UpdateLargestPossibleRegion()
{
  this->UpdateOutputInformation();

  if ( this->GetOutput(0) )
    {
    this->GetOutput(0)->SetRequestedRegionToLargestPossibleRegion();
    this->GetOutput(0)->Update();
    }
}
} // end namespace itk
