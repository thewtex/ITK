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

#include "itkDOMNodeXMLReader.h"
#include "expat.h"

#include <fstream>

namespace itk
{

static void itkXMLParserStartElement( void* parser, const char* name, const char** atts )
{
  // Begin element handler that is registered with the XML_Parser.
  // This just casts the user data to a itkXMLParser and calls
  // StartElement.
  static_cast<DOMNodeXMLReader*>(parser)->StartElement( name, atts );
}

static void itkXMLParserEndElement( void* parser, const char* name )
{
  // End element handler that is registered with the XML_Parser.  This
  // just casts the user data to a itkXMLParser and calls EndElement.
  static_cast<DOMNodeXMLReader*>(parser)->EndElement( name );
}

static void itkXMLParserCharacterDataHandler( void* parser, const char* data, int length )
{
  // Character data handler that is registered with the XML_Parser.
  // This just casts the user data to a itkXMLParser and calls
  // CharacterDataHandler.
  static_cast<DOMNodeXMLReader*>(parser)->CharacterDataHandler( data, length );
}

void DOMNodeXMLReader::StartElement( const char* name, const char** atts )
{
  DOMNode* node = 0;
  if ( m_Context )
  {
    DOMNode::Pointer node1 = DOMNode::New();
    node = (DOMNode*)node1;
    m_Context->AddChild( node );
  }
  else node = m_OutputObject;
  //
  node->GetName() = name;
  //
  int i = 0;
  while ( atts[i] )
  {
    std::string key(atts[i++]);
    std::string value(atts[i++]);
    if ( key=="id" || key=="ID" || key=="Id" )
    {
      node->SetAttribute( "id", value );
    }
    else
    {
      node->SetAttribute( key, value );
    }
  }
  //
  m_Context = node;
}

void DOMNodeXMLReader::EndElement( const char* name )
{
  //*
  if ( m_Context->GetName() != name )
  {
    itkExceptionMacro( "start/end tag names mismatch" );
  }
  //*/
  m_Context = m_Context->GetParent();
}

void DOMNodeXMLReader::CharacterDataHandler( const char* text, int len )
{
  std::string s( text, len );
  //*
  StringTools::Trim( s );
  if ( s.size() == 0 ) return;
  //*/
  m_Context->AddTextChild( s );
}

void DOMNodeXMLReader::Update()
{
  if ( (DOMNode*)this->m_OutputObject == 0 )
  {
    itkExceptionMacro( "output object is null" );
  }
  //
  std::ifstream is( m_Filename.c_str() );
  if ( !is.is_open() )
  {
    itkExceptionMacro( "failed openning the input XML file" );
  }
  ReadFromStream( is, this->m_OutputObject );
  is.close();
}

void DOMNodeXMLReader::ReadFromStream( std::istream& is, DOMNode* object )
{
  if ( object == 0 ) return;
  SetOutputObject( object );

  m_OutputObject->Clear();
  m_Context = 0;

  is >> std::noskipws;
  std::string s;
  while ( 1 )
  {
    char c = 0;
    is >> c;
    if ( !is.good() ) break;
    s.append( 1, c );
  }
  //std::cout << "XML string:\n" << s << std::endl;

  XML_Parser parser = XML_ParserCreate( 0 );
  XML_SetElementHandler( parser, &itkXMLParserStartElement, &itkXMLParserEndElement );
  XML_SetCharacterDataHandler( parser, &itkXMLParserCharacterDataHandler );
  XML_SetUserData( parser, this );
  //
  bool ok = XML_Parse( parser, s.data(), s.size(), false );
  if ( !ok )
  {
    ExceptionObject e( __FILE__, __LINE__ );
    std::string message( XML_ErrorString(XML_GetErrorCode(parser)) );
    e.SetDescription( message.c_str() );
    throw e;
  }
  //
  XML_ParserFree( parser );
}

} // namespace itk
