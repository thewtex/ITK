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

 // implementation of DOM-based reader for TestObject

#include "itkTestObjectDOMReader.h"

namespace itk
{

void TestObjectDOMReader::GenerateOutput( const DOMNode* domnode, const void* userdata )
{
  std::string s;

  // the attribute value is a text string
  s = domnode->GetAttribute("notes");
  std::string notes = s;
  this->m_OutputObject->GetNotes() = notes;

  // the attribute value is a boolean value
  // exception of type std::ios_base::failure will be thrown if failed in reading,
  // e.g. incompatible format
  s = domnode->GetAttribute("active");
  int active = 0;
  s >> active;
  this->m_OutputObject->GetActive() = (bool)active;

  // the attribute value is an integer value
  // exception of type std::ios_base::failure will be thrown if failed in reading,
  // e.g. incompatible format
  s = domnode->GetAttribute("count");
  int count = 0;
  s >> count;
  this->m_OutputObject->GetCount() = count;

  // fparams is stored as an attribute instead of a text child in the XML file
  // the attribute value is a std::vector of float
  // exception of type std::ios_base::failure will be thrown if failed in reading,
  // e.g. incompatible format or missing of data elements
  s = domnode->GetAttribute("fparams");
  std::vector<float> fparams;
  //s >> fparams; // read number of elements given by the input data
  FromString( s, fparams ); // similar to the above function, but the number
                              // of elements to read is automatically calculated
  this->m_OutputObject->GetFparams() = fparams;

  // data1 is stored as a text child node instead of an attribute in the XML file
  // the attribute value is an itk::Array of double
  // exception of type std::ios_base::failure will be thrown if failed in reading,
  // e.g. incompatible format or missing of data elements
  s = domnode->GetChildByID("data1")->GetTextChild()->GetText();
  Array<double> data1;
  //s >> data1; // read number of elements given by the input data
  FromString( s, data1 ); // similar to the above function, but the number
                            // of elements to read is automatically calculated
  this->m_OutputObject->GetData1() = data1;

  // data2 is stored as a text child node instead of an attribute in the XML file
  // the attribute value is an itk::Array of double
  // exception of type std::ios_base::failure will be thrown if failed in reading,
  // e.g. incompatible format or missing of data elements
  s = domnode->GetChildByID("data2")->GetTextChild()->GetText();
  Array<double> data2;
  //s >> data2; // read number of elements given by the input data
  FromString( s, data2 ); // similar to the above function, but the number
                            // of elements to read is automatically calculated
  this->m_OutputObject->GetData2() = data2;
}

} // namespace itk
