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

#include "itkDOMNodeXMLWriter.h"

#include <fstream>

namespace itk
{

void DOMNodeXMLWriter::Update()
{
  if ( (const DOMNode*)this->m_InputObject == 0 )
  {
    itkExceptionMacro( "input object is null" );
  }
  //
  std::ofstream os( m_Filename.c_str() );
  if ( !os.is_open() )
  {
    itkExceptionMacro( "failed openning the output XML file" );
  }
  WriteToStream( os, this->m_InputObject );
  os.close();
}

void DOMNodeXMLWriter::WriteToStream( std::ostream& os, const DOMNode* object, std::string indent )
{
  if ( object == 0 ) return;

  // if it is a text node
  const DOMTextNode* tnode = dynamic_cast<const DOMTextNode*>( object );
  if ( tnode )
  {
    os << indent << tnode->GetText() << std::endl; return;
  }

  // write the start tag name
  os << indent << "<" << object->GetName();

  // write the attributes
  typedef DOMNode::AttributesListType AttributesListType;
  AttributesListType attributes;
  object->GetAttributes( attributes );
  for ( AttributesListType::iterator i = attributes.begin(); i != attributes.end(); ++i )
  {
    os << " " << i->first << "=\"" << i->second << "\"";
  }

  // write the ending of the start tag, and all children if applicable
  typedef DOMNode::ConstChildrenListType ConstChildrenListType;
  ConstChildrenListType children;
  object->GetChildren( children );
  if ( children.size() )
  {
    // write the closing bracket for the start tag
    os << ">" << std::endl;
    // write the children
    for ( unsigned int i=0; i<children.size(); i++ )
    {
      WriteToStream( os, children[i], indent+m_IndentStep );
    }
    // write the end tag
    os << indent << "</" << object->GetName() << ">" << std::endl;
  }
  else
  {
    // write the special closing bracket for the start tag if it has no children
    os << "/>" << std::endl;
  }
}

} // namespace itk
