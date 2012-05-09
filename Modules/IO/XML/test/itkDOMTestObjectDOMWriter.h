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

#ifndef __itkDOMTestObjectDOMWriter_h
#define __itkDOMTestObjectDOMWriter_h

#include "itkDOMWriter.h"
#include "itkDOMTestObject.h"

namespace itk
{

class DOMTestObjectDOMWriter : public DOMWriter<DOMTestObject>
{
public:
  /** Standard class typedefs. */
  typedef DOMTestObjectDOMWriter      Self;
  typedef DOMWriter<DOMTestObject>    Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMTestObjectDOMWriter, DOMWriter);

protected:
  DOMTestObjectDOMWriter() {}

  /**
   * This function is called automatically when update functions are performed.
   * It should fill the contents of the intermediate DOM object by pulling information from the input object.
   */
  virtual void GenerateData( DOMNodeType* outputdom, const void* ) const;

private:
  DOMTestObjectDOMWriter(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

inline void
DOMTestObjectDOMWriter::GenerateData( DOMNodeType* outputdom, const void* ) const
{
  const InputType* input = this->GetInput();

  std::ofstream ofs;

  outputdom->SetName( "DOMTestObject" );

  // write child foo
  DOMNodePointer foo = DOMNodeType::New();
  foo->SetName( "foo" );
  foo->SetAttribute( "fname", input->GetFooFileName() );
  outputdom->AddChild( foo );
  // write the foo value to file
  ofs.open( input->GetFooFileName().c_str() );
  if ( !ofs.is_open() )
    {
    itkExceptionMacro( "cannot write foo file" );
    }
  ofs << input->GetFooValue();
  ofs.close();

  // write child bar
  DOMNodePointer bar = DOMNodeType::New();
  bar->SetName( "bar" );
  bar->SetAttribute( "fname", input->GetBarFileName() );
  outputdom->AddChildAtEnd( bar );
  // write the bar value to file
  ofs.open( input->GetBarFileName().c_str() );
  if ( !ofs.is_open() )
    {
    itkExceptionMacro( "cannot write bar file" );
    }
  ofs << input->GetBarValue();
  ofs.close();
}

} // namespace itk

#endif // __itkDOMTestObjectDOMWriter_h
