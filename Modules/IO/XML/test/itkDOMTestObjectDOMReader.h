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

#ifndef __itkDOMTestObjectDOMReader_h
#define __itkDOMTestObjectDOMReader_h

#include "itkDOMReader.h"
#include "itkDOMTestObject.h"

namespace itk
{

class DOMTestObjectDOMReader : public DOMReader<DOMTestObject>
{
public:
  /** Standard class typedefs. */
  typedef DOMTestObjectDOMReader      Self;
  typedef DOMReader<DOMTestObject>    Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMTestObjectDOMReader, DOMReader);

protected:
  DOMTestObjectDOMReader() {}

  /**
   * This function is called automatically when update functions are performed.
   * It should create a DOMTestObject instance and return a pointer that points to a base ITK object.
   */
  virtual ObjectPointer MakeOutput() const;

  /**
   * This function is called automatically when update functions are performed.
   * It should fill the contents of the output object by pulling information from the intermediate DOM object.
   */
  virtual void GenerateData( const DOMNodeType* domnode, const void* );

private:
  DOMTestObjectDOMReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

inline DOMTestObjectDOMReader::ObjectPointer
DOMTestObjectDOMReader::MakeOutput() const
{
  OutputPointer output = OutputType::New();
  ObjectPointer object = (OutputType*)output;
  return object;
}

inline void
DOMTestObjectDOMReader::GenerateData( const DOMNodeType* domnode, const void* )
{
  OutputType* output = this->GetOutput();

  FancyString s;
  std::ifstream ifs;

  if ( domnode->GetName() != "DOMTestObject" )
    {
    itkExceptionMacro( "tag name DOMTestObject is expected" );
    }

  // read child foo
  const DOMNodeType* foo = domnode->GetChild( "foo" );
  if ( foo == NULL )
    {
    itkExceptionMacro( "child foo not found" );
    }
  s = foo->GetAttribute("fname");
  output->SetFooFileName( s );
  // read the foo value from file
  ifs.open( s.c_str() );
  if ( !ifs.is_open() )
    {
    itkExceptionMacro( "cannot read foo file" );
    }
  std::string fooValue;
  ifs >> fooValue;
  output->SetFooValue( fooValue );
  ifs.close();

  // add child bar
  const DOMNodeType* bar = domnode->GetChild( "bar" );
  if ( bar == NULL )
    {
    itkExceptionMacro( "child bar not found" );
    }
  s = bar->GetAttribute("fname");
  output->SetBarFileName( s );
  // read the foo value from file
  ifs.open( s.c_str() );
  if ( !ifs.is_open() )
    {
    itkExceptionMacro( "cannot read bar file" );
    }
  int barValue = 0;
  ifs >> barValue;
  output->SetBarValue( barValue );
  ifs.close();
}

} // namespace itk

#endif // __itkDOMTestObjectDOMReader_h
