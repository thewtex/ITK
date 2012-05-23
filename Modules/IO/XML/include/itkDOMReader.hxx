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

#ifndef __itkDOMReader_hxx
#define __itkDOMReader_hxx

#include "itkDOMReader.h"
#include "itksys/SystemTools.hxx"
#include "itkStdStreamLogOutput.h"

namespace itk
{

template< class T >
DOMReader<T>::DOMReader()
{
  // Create the logger.
  this->m_Logger = LoggerType::New();
  // by default logged messages go to the console
  typename StdStreamLogOutput::Pointer defout = StdStreamLogOutput::New();
  defout->SetStream( std::cout );
  this->m_Logger->AddLogOutput( defout );
  // settings that may be important
  this->m_Logger->SetName( this->GetNameOfClass() );
  this->m_Logger->SetPriorityLevel( Logger::NOTSET ); // log everything
  this->m_Logger->SetLevelForFlushing( Logger::MUSTFLUSH ); // never flush (MUSTFLUSH actually leads to no flush, a bug in Logger)
  // some other settings
  this->m_Logger->SetTimeStampFormat( Logger::HUMANREADABLE );
  this->m_Logger->SetHumanReadableFormat( "%Y-%b-%d %H:%M:%S" ); // time stamp format
}

/**
 * Function called by Update() or end-users to generate the output object from a DOM object.
 * Some derived readers may accept an incomplete DOM object during the reading process, in those cases
 * the optional argument 'userdata' can be used to provide the missed information.
 */
template< class T >
void
DOMReader<T>::Update( const DOMNodeType* domnode, const void* userdata )
{
  if ( domnode == NULL )
    {
    itkExceptionMacro( "read from an invalid DOM object" );
    }

  if ( this->GetOutput() == NULL )
    {
    ObjectPointer object = this->MakeOutput();
    OutputType* output = dynamic_cast<OutputType*>( (ObjectType*)object );
    if ( output == NULL )
      {
      itkExceptionMacro( "an invalid output object was made" );
      }
    this->SetOutput( output );
    }

  this->GenerateData( domnode, userdata );
}

/**
 * Function called by end-users to generate the output object from the input XML file.
 */
template< class T >
void
DOMReader<T>::Update()
{
  // create the intermediate DOM object if it is not set
  DOMNodeType* domobj = this->GetIntermediateDOM();
  if ( domobj == NULL )
    {
    DOMNodePointer node = DOMNodeType::New();
    domobj = (DOMNodeType*)node;
    this->SetIntermediateDOM( domobj );
    }

  // remove previous data from the DOM object
  domobj->RemoveAllAttributesAndChildren();

  // read the input XML file and update the DOM object
  typename DOMNodeXMLReader::Pointer reader = DOMNodeXMLReader::New();
  reader->SetOutput( domobj );
  reader->SetFileName( this->m_FileName );
  reader->Update();

  // save the current working directory (WD), and change the WD to where the XML file is located
  std::string sOldWorkingDir = itksys::SystemTools::GetCurrentWorkingDirectory();
  std::string sNewWorkingDir = itksys::SystemTools::GetFilenamePath( this->m_FileName.c_str() );
  itksys::SystemTools::ChangeDirectory( sNewWorkingDir.c_str() );

  this->Update( domobj );

  // change the WD back to the previously saved
  itksys::SystemTools::ChangeDirectory( sOldWorkingDir.c_str() );
}

} // namespace itk

#endif // __itkDOMReader_hxx
