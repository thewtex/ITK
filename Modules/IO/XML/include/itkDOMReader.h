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
#include "itkProcessObject.h"
#include "itkLogger.h"

namespace itk
{

/**
 * \class DOMReader
 * \brief Class to read an ITK object from an XML file or a DOM object, using the DOM APIs.
 *
 * End-users need to derive from this class to implement readers for user objects. In subclasses, users need
 * to provide implementations for two virtual functions: MakeOutput() and GenerateData(-,-).
 *
 * This class performs similar functions as the XMLReader - both provide the base for handling object reading from
 * an XML source. The difference is that, readers derived from this class perform object reading using the
 * easy-to-use DOM APIs, while XMLReader-based readers use the more error-prone SAX (Simple API for XML) APIs.
 *
 * Internally, this class first implicitly creates an intermediate DOM object from the input XML file using the
 * DOMNodeXMLReader, then performs output object generation by pulling information from the DOM object.
 *
 * Warning: Although this class is derived from ProcessObject, it does not exactly behave like a ProcessObject,
 *          i.e., the output of this class cannot be connected to an input of another ProcessObject except for DOMWriter,
 *          because the output of this class is not a DataObject. This design decision was made to handle XML reading for not
 *          only data objects, but also any ITK objects that can be serialized on disk, e.g., transformations, optimizers,
 *          registrations, user objects, and so on.
 *
 * The following code snippet demontrates how to use a DOM-based reader that is derived from this class:
 *
 *     itk::MyObjectType::Pointer output_object;
 *     const char* input_xml_file_name = ...
 *     itk::MyObjectDOMReader::Pointer reader = itk::MyObjectDOMReader::New();
 *     reader->SetFileName( input_xml_file_name );
 *     reader->Update();
 *     output_object = reader->GetOutput();
 *
 * \sa XMLReader
 * \sa DOMNodeXMLReader
 * \sa DOMNode
 *
 * \ingroup ITKIOXML
 */
template< class T >
class DOMReader : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef DOMReader Self;

  itkTypeMacro(DOMReader, ProcessObject);

  typedef T                             OutputType;
  typedef typename OutputType::Pointer  OutputPointer;

  typedef DOMNode                       DOMNodeType;
  typedef typename DOMNodeType::Pointer DOMNodePointer;

  typedef Logger                        LoggerType;
  typedef typename LoggerType::Pointer  LoggerPointer;

  /** Set the input XML filename. */
  itkSetStringMacro(FileName);

  /** Get the input XML filename. */
  itkGetStringMacro(FileName);

  /**
   * The output object will be created automatically, but the user
   * can appoint a user object as the output by calling this function.
   */
  itkSetObjectMacro( Output, OutputType );

  /** Get the output object for full access. */
  itkGetObjectMacro( Output, OutputType );

  /** Get the output object for read-only access. */
  itkGetConstObjectMacro( Output, OutputType );

  /**
   * Return the internal logger so that users can change the
   * output format or add/remove logging destinations.
   */
  itkGetObjectMacro( Logger, LoggerType );

  /**
   * Function called by Update() or end-users to generate the output object from a DOM object.
   * Some derived readers may accept an incomplete DOM object during the reading process, in those cases
   * the optional argument 'userdata' can be used to provide the missed information.
   */
  void Update( const DOMNodeType* domnode, const void* userdata = 0 );

  /**
   * Function called by end-users to generate the output object from the input XML file.
   */
  virtual void Update();

protected:
  DOMReader();

  typedef Object                        ObjectType;
  typedef typename ObjectType::Pointer  ObjectPointer;

  /**
   * Function to be implemented in subclasses. It is called automatically
   * when update functions are performed.
   * The function should create an output object and return a pointer that points to a base ITK object.
   */
  virtual ObjectPointer MakeOutput() const = 0;

  /**
   * Function to be implemented in subclasses. It is called automatically
   * when update functions are performed. It should fill the contents of the output object by pulling
   * information from the intermediate DOM object.
   * Some derived readers may accept an incomplete DOM object during the reading process, in those cases
   * the optional argument 'userdata' can be used to provide the missed information.
   */
  virtual void GenerateData( const DOMNodeType* domnode, const void* userdata ) = 0;

private:
  DOMReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** Set the intermediate DOM object. */
  itkSetObjectMacro( IntermediateDOM, DOMNodeType );

  /** Get the intermediate DOM object. */
  itkGetObjectMacro( IntermediateDOM, DOMNodeType );

  /** Variable to hold the input XML file name. */
  std::string m_FileName;

  /** Variable to hold the output object, created internally or supplied by the user. */
  OutputPointer m_Output;

  /** Variable to hold the intermediate DOM object. */
  DOMNodePointer m_IntermediateDOM;

  /** Variable to log various messages during the reading process. */
  LoggerPointer m_Logger;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDOMReader.hxx"
#endif

#endif // __itkDOMReader_h
