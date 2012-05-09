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

 // implementation of DOM-based writer for TestObject

#include "itkTestObjectDOMWriter.h"

namespace itk
{

void TestObjectDOMWriter::GenerateDOM( DOMNode* domnode, const void* userdata ) const
{
  // use StringObject instead of std::string to use our overloaded "<<" functions
  StringObject s;

  domnode->SetName( "TestObject" );

  // write a text string
  s << this->m_InputObject->GetNotes();
  domnode->SetAttribute( "notes", s );

  // write a boolean value
  s << (int)this->m_InputObject->GetActive();
  domnode->SetAttribute( "active", s );

  // write an integer value
  s << this->m_InputObject->GetCount();
  domnode->SetAttribute( "count", s );

  // write a std::vector
  // store fparams in an attribute instead of a text child node in the output XML file
  s << this->m_InputObject->GetFparams();
  domnode->SetAttribute( "fparams", s );

  // write an itk::Array
  // store data1 in a text child node instead of an attribute in the output XML file
  DOMNode::Pointer data1 = DOMNode::New();
  data1->SetName( "array" );
  data1->SetAttribute( "id", "data1" );
  domnode->AddChild( data1 );
  //
  s << this->m_InputObject->GetData1();
  data1->AddTextChild( s );

  // write an itk::Array
  // store data2 in a text child node instead of an attribute in the output XML file
  DOMNode::Pointer data2 = DOMNode::New();
  data2->SetName( "array" );
  data2->SetAttribute( "id", "data2" );
  domnode->AddChild( data2 );
  //
  s << this->m_InputObject->GetData2();
  data2->AddTextChild( s );
}

} // namespace itk
