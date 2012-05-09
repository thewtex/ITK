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

#ifndef __itkDOMWriter_h
#define __itkDOMWriter_h

#include "itkDOMNodeXMLWriter.h"
#include "itkLightProcessObject.h"

#include "itksys/SystemTools.hxx"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace itk
{

/**
 * \class DOMWriter
 * \brief Class to write an ITK object to an XML file or output stream, using the DOM API.
 *
 * This abstract class facilitates the process of writing an ITK object to an XML file or output stream. The user
 * needs to derive an object writer from this class, and implements only the function GenerateDOM(), which performs
 * DOM object creation from the input user object.
 *
 * This class performs similar functions as the XMLWriterBase: both handle object writing to an XML destination. The
 * difference is that, this class performs object writing using the easier-to-use DOM API, while the other directly generates
 * textual XML document, which is more error prone.
 *
 * Here is an example to use this class:
 *     itk::MyObjectType::Pointer input_object = ...
 *     const char* output_xml_file_name = ...
 *     itk::MyObjectDOMWriter::Pointer writer = itk::MyObjectDOMWriter::New();
 *     writer->SetInputObject( input_object );
 *     writer->SetFilename( output_xml_file_name );
 *     writer->Update();
 *
 * Internally, this class uses DOMNodeXMLWriter to write the generated DOM object to an XML destination, and the writing
 * processing is performed implicitly and automatically.
 *
 * \sa XMLWriterBase
 * \sa DOMNodeXMLWriter
 * \sa DOMNode
 */
template< class T >
class DOMWriter : public LightProcessObject
{
public:
  /**
   * Users must implement this function to generate the internal DOM object from the input object.
   * In rare cases, the user may need extra information to create the DOM object. In such cases, the
   * optional argument 'userdata' can be used for such purpose.
   */
  virtual void GenerateDOM( DOMNode* domnode, const void* userdata = 0 ) const = 0;

public:
  /** Standard class typedefs. */
  typedef DOMWriter Self;

  itkTypeMacro(DOMWriter, LightProcessObject);

  typedef T ObjectType;

  /** Set the XML filename to write. */
  itkSetStringMacro(Filename);

  /** Get the XML filename to write. */
  itkGetStringMacro(Filename);

  /** Perform object writing to the output XML file. */
  virtual void Update()
  {
    if ( (const T*)m_InputObject == 0 )
    {
      itkExceptionMacro( "input object is null" );
    }
    //
    m_DOMObject = DOMNode::New();
    //
    // save the current working directory (WD), and change the WD to where the XML file is located
    std::string sOldWorkingDir = itksys::SystemTools::GetCurrentWorkingDirectory();
    std::string sNewWorkingDir = itksys::SystemTools::GetFilenamePath( m_Filename.c_str() );
    itksys::SystemTools::ChangeDirectory( sNewWorkingDir.c_str() );
    //
    GenerateDOM( (DOMNode*)m_DOMObject );
    // change the WD back to the previously saved
    itksys::SystemTools::ChangeDirectory( sOldWorkingDir.c_str() );
    //
    typename DOMNodeXMLWriter::Pointer writer = DOMNodeXMLWriter::New();
    writer->SetFilename( m_Filename );
    writer->SetInputObject( (DOMNode*)m_DOMObject );
    writer->Update();
  }

  /** Set the input/user object to write. */
  void SetInputObject( const T* obj )
  {
    m_InputObject = obj;
    m_InputObjectHolder = dynamic_cast<const LightObject*>( obj );
  }
  /** Get the input/user object to write. */
  const T* GetInputObject() const { return m_InputObject; }

  /** Return the internal DOM object, after it is generated from the input object. */
  DOMNode* GetDOMObject() const { return m_DOMObject; }

  /** Return the internal logger so that users can change the output format, add outputs, etc. */
  Logger* GetLogger() const { return m_Logger; }

protected:
  DOMWriter() : m_InputObject(0)
  {
    m_Logger = Logger::New();
    // add/set a default output
    StdStreamLogOutput::Pointer defout = StdStreamLogOutput::New();
    defout->SetStream( std::cout );
    m_Logger->AddLogOutput( defout );
    //
    m_Logger->SetName( GetNameOfClass() );
    m_Logger->SetPriorityLevel( Logger::NOTSET ); // log everything
    m_Logger->SetLevelForFlushing( Logger::MUSTFLUSH ); // never flush
    //
    m_Logger->SetTimeStampFormat( Logger::HUMANREADABLE );
    m_Logger->SetHumanReadableFormat( "%Y-%b-%d %H:%M:%S" ); // time stamp format
  }

  /** Variable to hold the input/user object to write. */
  const T* m_InputObject;
  typename LightObject::ConstPointer m_InputObjectHolder;

  /** Variable to hold the output XML file name. */
  std::string m_Filename;

  /** Variable to hold the internal DOM object. */
  mutable typename DOMNode::Pointer m_DOMObject;

  /** Variable to log various messages during the writing process. */
  mutable Logger::Pointer m_Logger;

private:
  DOMWriter(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

} // namespace itk

#endif // __itkDOMWriter_h
