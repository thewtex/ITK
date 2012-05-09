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

#ifndef __itkDOMReader_h
#define __itkDOMReader_h

#include "itkDOMNodeXMLReader.h"
#include "itkLightProcessObject.h"

#include "itksys/SystemTools.hxx"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace itk
{

/**
 * \class DOMReader
 * \brief Class to read an ITK object from an XML file or input stream, using the DOM API.
 *
 * This abstract class facilitates the process of reading an ITK object from an XML file or input stream. The user
 * needs to derive an object reader from this class, and implements only the function GenerateOutput(), which performs
 * object creation from a DOM object.
 *
 * This class performs similar functions as the XMLReader: both handle object reading from an XML source. The
 * difference is that, this class performs object reading using the easier-to-use DOM API, while the other uses
 * the SAX (Simple API for XML).
 *
 * Here is an example to use this class:
 *     itk::MyObjectType::Pointer output_object = ...
 *     const char* input_xml_file_name = ...
 *     itk::MyObjectDOMReader::Pointer reader = itk::MyObjectDOMReader::New();
 *     reader->SetOutputObject( output_object );
 *     reader->SetFilename( input_xml_file_name );
 *     reader->Update();
 *
 * Internally, this class uses DOMNodeXMLReader to read the DOM object from an XML source, and the reading processing is
 * performed implicitly and automatically.
 *
 * \sa XMLReader
 * \sa DOMNodeXMLReader
 * \sa DOMNode
 */
template< class T >
class DOMReader : public LightProcessObject
{
public:
  /**
   * Users must implement this function to generate the output object from the internal DOM object.
   * In rare cases, the user may need extra information to create the output object. In such cases, the
   * optional argument 'userdata' can be used for such purpose.
   */
  virtual void GenerateOutput( const DOMNode* domnode, const void* userdata = 0 ) = 0;

public:
  /** Standard class typedefs. */
  typedef DOMReader Self;

  itkTypeMacro(DOMReader, LightProcessObject);

  typedef T ObjectType;

  /** Set the XML filename to read. */
  itkSetStringMacro(Filename);
  /** Get the XML filename to read. */
  itkGetStringMacro(Filename);

  /** Perform object reading from the input XML file. */
  virtual void Update()
  {
    if ( (T*)m_OutputObject == 0 )
    {
      itkExceptionMacro( "output object is null" );
    }
    //
    m_DOMObject = DOMNode::New();
    //
    typename DOMNodeXMLReader::Pointer reader = DOMNodeXMLReader::New();
    reader->SetFilename( m_Filename );
    reader->SetOutputObject( (DOMNode*)m_DOMObject );
    reader->Update();
    //
    // save the current working directory (WD), and change the WD to where the XML file is located
    std::string sOldWorkingDir = itksys::SystemTools::GetCurrentWorkingDirectory();
    std::string sNewWorkingDir = itksys::SystemTools::GetFilenamePath( m_Filename.c_str() );
    itksys::SystemTools::ChangeDirectory( sNewWorkingDir.c_str() );
    //
    GenerateOutput( (DOMNode*)m_DOMObject );
    //
    // change the WD back to the previously saved
    itksys::SystemTools::ChangeDirectory( sOldWorkingDir.c_str() );
  }

  /** Set the output/user object to read. */
  void SetOutputObject( T* obj )
  {
    m_OutputObject = obj;
    m_OutputObjectHolder = dynamic_cast<LightObject*>( obj );
  }
  /** Get the output/user object to read. */
  T* GetOutputObject() { return m_OutputObject; }

  /** Return the internal DOM object, after the input XML file has been parsed. */
  DOMNode* GetDOMObject() const { return m_DOMObject; }

  /** Return the internal logger so that users can change the output format, add outputs, etc. */
  Logger* GetLogger() const { return m_Logger; }

protected:
  DOMReader() : m_OutputObject(0)
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

  /** Variable to hold the output/user object to read. */
  T* m_OutputObject;
  typename LightObject::Pointer m_OutputObjectHolder;

  /** Variable to hold the input XML file name. */
  std::string m_Filename;

  /** Variable to hold the internal DOM object. */
  mutable typename DOMNode::Pointer m_DOMObject;

  /** Variable to log various messages during the reading process. */
  mutable Logger::Pointer m_Logger;

private:
  DOMReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

} // namespace itk

#endif // __itkDOMReader_h
