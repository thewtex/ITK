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

#ifndef __itkDOMNodeXMLWriter_h
#define __itkDOMNodeXMLWriter_h

#include "itkDOMNode.h"
#include "itkLightProcessObject.h"

#include <ostream>

namespace itk
{

/**
 * \class DOMNodeXMLWriter
 * \brief Class to write a DOM object to an XML file or output stream.
 *
 * This class writes a DOM object to an XML file or output stream.
 *
 * Here is an example to use this class:
 *     itk::DOMNode::Pointer input_dom_object = ...
 *     const char* output_xml_file_name = ...
 *     itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
 *     writer->SetInputObject( input_dom_object );
 *     writer->SetFilename( output_xml_file_name );
 *     writer->Update();
 *
 * Before using this writer to write a user object, the corresponding DOM object must have been generated from
 * the user object. To facilitate this process, it is recommended for the user to derive an object writer
 * from the class DOMWriter, which is an abstract class that hides the process of DOM object writing, and lets
 * the user to focus only on DOM object generation from the user object.
 *
 * \sa DOMWriter
 * \sa DOMNode
 */
class DOMNodeXMLWriter : public LightProcessObject
{
public:
  /** Standard class typedefs. */
  typedef DOMNodeXMLWriter            Self;
  typedef LightProcessObject          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMNodeXMLWriter, LightProcessObject);

  /** Set the XML filename to write. */
  itkSetStringMacro(Filename);
  /** Get the XML filename to write. */
  itkGetStringMacro(Filename);

  /** Set the input DOM object to write. */
  void SetInputObject(const DOMNode* obj) { m_InputObject = obj; }
  /** Get the input DOM object to write. */
  const DOMNode* GetInputObject(void) const { return m_InputObject; }

public:
  /** Function called by the user to start writing the input DOM object to the output XML file. */
  virtual void Update();

  /** Function to perform real writing, using an output stream (file, string, console, etc) as the destination. */
  void WriteToStream( std::ostream& os, const DOMNode* object, std::string indent="" );

protected:
  DOMNodeXMLWriter() : m_InputObject(0), m_IndentStep("  ") {}

  /** Variable to hold the DOM object to be written, which is a user input. */
  DOMNode::ConstPointer m_InputObject;

  /** Variable to hold the output XML file name. */
  std::string m_Filename;

private:
  DOMNodeXMLWriter(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** Variable to hold the indentation (i.e. number of white spaces) for a child node w.r.t. its parent. */
  std::string m_IndentStep;
};

} // namespace itk

/** The operator "<<" is overloaded such that a DOM object can be conveniently write to an output stream. */
inline std::ostream& operator<<( std::ostream& os, const itk::DOMNode& object )
{
  itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
  writer->WriteToStream( os, &object );
  return os;
}

#endif // __itkDOMNodeXMLWriter_h
