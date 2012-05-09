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

#ifndef __itkDOMNodeXMLReader_h
#define __itkDOMNodeXMLReader_h

#include "itkDOMNode.h"
#include "itkLightProcessObject.h"

#include <istream>

namespace itk
{

/**
 * \class DOMNodeXMLReader
 * \brief Class to read a DOM object from an XML file or input stream.
 *
 * This class reads a DOM object from an XML file or input stream. The obtained DOM object can be subsequently
 * used to create user objects.
 *
 * Here is an example to use this class:
 *     itk::DOMNode::Pointer output_dom_object = ...
 *     const char* input_xml_file_name = ...
 *     itk::DOMNodeXMLReader::Pointer reader = itk::DOMNodeXMLReader::New();
 *     reader->SetOutputObject( output_dom_object );
 *     reader->SetFilename( input_xml_file_name );
 *     reader->Update();
 *
 * To read a user object from an XML file/stream, additional steps are needed after getting the DOM object.
 * To facilitate this process, it is recommended for the user to derive an object reader from class DOMReader,
 * which is an abstract class that hides the process of DOM object reading, and let the user to concentrate
 * only on object creation from the DOM object.
 *
 * \sa DOMReader
 * \sa DOMNode
 */
class DOMNodeXMLReader : public LightProcessObject
{
public:
  /** Standard class typedefs. */
  typedef DOMNodeXMLReader            Self;
  typedef LightProcessObject          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMNodeXMLReader, LightProcessObject);

  /** Set the XML filename to read. */
  itkSetStringMacro(Filename);
  /** Get the XML filename to read. */
  itkGetStringMacro(Filename);

  /** Set the output DOM object to read. */
  void SetOutputObject(DOMNode* obj) { m_OutputObject = obj; }
  /** Get the output DOM object to read. */
  DOMNode* GetOutputObject(void) { return m_OutputObject; }

public:
  /** Callback function -- called from XML parser with start-of-element
   * information.
   */
  virtual void StartElement( const char* name, const char** atts );

  /** Callback function -- called from XML parser when ending tag
   * encountered.
   */
  virtual void EndElement( const char* name );

  /** Callback function -- called from XML parser with the character data
   * for an XML element.
   */
  virtual void CharacterDataHandler( const char* text, int len );

public:
  /** Function called by the user to start XML parsing from the input file. */
  virtual void Update();

  /** Function to perform real XML parsing, using an input stream (file or string) as the source. */
  void ReadFromStream( std::istream& is, DOMNode* object );

protected:
  DOMNodeXMLReader() : m_OutputObject(0), m_Context(0) {}

  /** Variable to hold the DOM object to be read, which is a user input. */
  DOMNode::Pointer m_OutputObject;

  /** Variable to hold the input XML file name. */
  std::string m_Filename;

private:
  DOMNodeXMLReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** Variable to keep the current context during XML parsing. */
  DOMNode* m_Context;
};

} // namespace itk

/** The operator ">>" is overloaded such that a DOM object can be conveniently read from an input stream. */
inline std::istream& operator>>( std::istream& is, itk::DOMNode& object )
{
  itk::DOMNodeXMLReader::Pointer reader = itk::DOMNodeXMLReader::New();
  reader->ReadFromStream( is, &object );
  return is;
}

#endif // __itkDOMNodeXMLReader_h
