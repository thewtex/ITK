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

#include "itkDOMNode.h"

#include <sstream>

namespace itk
{

// retrieve an attribute by key
std::string& DOMNode::GetAttribute( const std::string& key )
{
  AttributesContainer::iterator i = m_Attributes.find( key );
  if ( i == m_Attributes.end() )
  {
    //*
    static std::string s;
    s = "";
    return s;
    /*/
    itkExceptionMacro( "attribute does not exist" );
    //*/
  }
  return i->second;
}
const std::string& DOMNode::GetAttribute( const std::string& key ) const
{
  return const_cast<Self*>(this)->GetAttribute( key );
}
// check whether has an attribute
bool DOMNode::HasAttribute( const std::string& key ) const
{
  AttributesContainer::const_iterator i = m_Attributes.find( key );
  return ( i != m_Attributes.end() );
}
// add or replace an attribute
void DOMNode::SetAttribute( const std::string& key, const std::string& value )
{
  AttributesContainer::iterator i = m_Attributes.find( key );
  if ( i == m_Attributes.end() )
  {
    i = m_Attributes.insert( m_Attributes.begin(), AttributeItemType(key,value) );
  }
  else
  {
    i->second = value;
  }
}
// remove an attribute by key and return true if successful
bool DOMNode::RemoveAttribute( const std::string& key )
{
  AttributesContainer::iterator i = m_Attributes.find( key );
  if ( i == m_Attributes.end() )
  {
    return false;
  }
  else
  {
    m_Attributes.erase( i );
    return true;
  }
}

// return all attributes
DOMNode::AttributesListType& DOMNode::GetAttributes( AttributesListType& output ) const
{
  output.insert( m_Attributes.begin(), m_Attributes.end() );
  return output;
}
// remove all attributes
void DOMNode::ClearAttributes()
{
  m_Attributes.clear();
}

// get number of children
int DOMNode::GetNumberOfChildren() const
{
  return (int)m_Children.size();
}

// remove all children
void DOMNode::ClearChildren()
{
  m_Children.clear();
}

// add a child in front of another child (add at the end if i is out of range)
//   note: remove the node from its previous parent if applicable
bool DOMNode::AddChild( Pointer node, int i )
{
  if ( (DOMNode*)node == 0 || (DOMNode*)node->m_Parent == this ) return false;
  //
  if ( (DOMNode*)node->m_Parent )
  {
    node->m_Parent->RemoveChild( node );
  }
  //
  node->m_Parent = this;
  if ( i<0 || i>=static_cast<int>(m_Children.size()) )
  {
    m_Children.push_back( node );
  }
  else
  {
    m_Children.insert( m_Children.begin()+i, node );
  }
  return true;
}
// replace a child and return yes if successful
//   note: remove the node from its previous parent if applicable
bool DOMNode::SetChild( Pointer node, int i )
{
  if ( (DOMNode*)node == 0 || (DOMNode*)node->m_Parent == this ) return false;
  if ( i<0 || i>=static_cast<int>(m_Children.size()) ) return false;
  //
  if ( (DOMNode*)node->m_Parent )
  {
    node->m_Parent->RemoveChild( node );
  }
  //
  node->m_Parent = this;
  m_Children[i]->m_Parent = 0;
  m_Children[i] = node;
  return true;
}

// remove a child by index and return the removed one (return 0 if unsuccessful)
DOMNode::Pointer DOMNode::RemoveChild( int i )
{
  if ( i<0 || i>=static_cast<int>(m_Children.size()) ) return Pointer(0);
  //
  Pointer node = m_Children[i];
  node->m_Parent = 0;
  m_Children.erase( m_Children.begin()+i );
  return node;
}

// remove a child recursively by node/object pointer and return yes or no
bool DOMNode::RemoveChild( Pointer node )
{
  if ( (DOMNode*)node == 0 || (DOMNode*)node->m_Parent == 0 ) return false;
  //
  if ( (DOMNode*)node->m_Parent != this )
  {
    return node->m_Parent->RemoveChild( node );
  }
  //
  for ( int i=0; i<GetNumberOfChildren(); i++ )
  {
    if ( GetChild(i) == node )
    {
      RemoveChild( i ); return true;
    }
  }
  return false;
}

void DOMNode::Clear()
{
  ClearAttributes();
  ClearChildren();
}

// retrieve a child by index
DOMNode::Pointer DOMNode::GetChild( int i )
{
  return ( i>=0 && i<static_cast<int>(this->m_Children.size()) ? this->m_Children[i] : 0 );
}
DOMNode::ConstPointer DOMNode::GetChild( int i ) const
{
  Pointer node = const_cast<Self*>(this)->GetChild( i );
  return ConstPointer( (DOMNode*)node );
}

// return all children
DOMNode::ChildrenListType& DOMNode::GetChildren( ChildrenListType& output )
{
  //output.clear();
  for ( int i=0; i<GetNumberOfChildren(); i++ )
  {
    output.push_back( GetChild(i) );
  }
  return output;
}
DOMNode::ConstChildrenListType& DOMNode::GetChildren( ConstChildrenListType& output ) const
{
  //output.clear();
  for ( int i=0; i<GetNumberOfChildren(); i++ )
  {
    output.push_back( GetChild(i) );
  }
  return output;
}

// return all children of a same tag name
DOMNode::ChildrenListType& DOMNode::GetChildren( const std::string& tag, ChildrenListType& output )
{
  //output.clear();
  for ( int i=0; i<GetNumberOfChildren(); i++ )
  {
    Pointer node = GetChild(i);
    if ( tag == node->m_Name ) output.push_back( node );
  }
  return output;
}
DOMNode::ConstChildrenListType& DOMNode::GetChildren( const std::string& tag, ConstChildrenListType& output ) const
{
  //output.clear();
  for ( int i=0; i<GetNumberOfChildren(); i++ )
  {
    ConstPointer node = GetChild(i);
    if ( tag == node->m_Name ) output.push_back( node );
  }
  return output;
}

// retrieve a child by tag name and an index
// (as multiple children can have a same tag name)
DOMNode::Pointer DOMNode::GetChild( const std::string& tag, int i )
{
  int k = 0;
  for ( int j=0; j<GetNumberOfChildren(); j++ )
  {
    Pointer node = GetChild(j);
    if ( tag == node->m_Name )
    {
      if ( k++ == i ) return node;
    }
  }
  return Pointer(0);
}
DOMNode::ConstPointer DOMNode::GetChild( const std::string& tag, int i ) const
{
  Pointer node = const_cast<Self*>(this)->GetChild( tag, i );
  return ConstPointer( (DOMNode*)node );
}

// retrieve a child by id
DOMNode::Pointer DOMNode::GetChildByID( const std::string& value )
{
  for ( int j=0; j<GetNumberOfChildren(); j++ )
  {
    Pointer node = GetChild(j);
    if ( value == node->GetAttribute("id") ) return node;
  }
  return Pointer(0);
}
DOMNode::ConstPointer DOMNode::GetChildByID( const std::string& value ) const
{
  Pointer node = const_cast<Self*>(this)->GetChildByID( value );
  return ConstPointer( (DOMNode*)node );
}

// retrieve an older or younger sibling by distance
DOMNode::Pointer DOMNode::GetSibling( int i )
{
  if ( (DOMNode*)this->m_Parent == 0 ) return 0;
  //
  int j;
  for ( j=0; j<this->m_Parent->GetNumberOfChildren(); j++ )
  {
    if ( this->m_Parent->GetChild(j) == this ) break;
  }
  return this->m_Parent->GetChild( j + i );
}
DOMNode::ConstPointer DOMNode::GetSibling( int i ) const
{
  Pointer node = const_cast<Self*>(this)->GetSibling( i );
  return ConstPointer( (DOMNode*)node );
}

// return the root node
DOMNode::Pointer DOMNode::GetRoot()
{
  if ( (DOMNode*)this->m_Parent == 0 ) return this;
  //
  Pointer node = this->m_Parent;
  while ( (DOMNode*)node->m_Parent ) node = node->m_Parent;
  return node;
}
DOMNode::ConstPointer DOMNode::GetRoot() const
{
  Pointer node = const_cast<Self*>(this)->GetRoot();
  return ConstPointer( (DOMNode*)node );
}

// find a node recursively by node/object pointer
bool DOMNode::HasNode( ConstPointer node ) const
{
  return ( (const DOMNode*)node && node->GetRoot() == GetRoot() );
}

// find a child or sibling or relative by a query string (or path) that is separated by '/'
//   integer number : a child by index
//   -[n]           : an older sibling by distance 1 or n
//   +[n]           : a younger sibling by distance 1 or n
//   <tag>[:n]      : a child by tag name and index 0 or n (<tag>=! means a text node)
//   :<id>          : a child by id
//   .              : current node
//   ..             : parent node
//   /<rpath>       : absolute path (denote apath), search from the root
DOMNode::Pointer DOMNode::Find( const std::string& path )
{
  std::string s;
  std::string rpath;
  {
    std::size_t pos = path.find_first_of( '/' );
    if ( pos == std::string::npos )
    {
      s = path;
      rpath = "";
    }
    else
    {
      s = path.substr( 0, pos );
      rpath = path.substr( pos+1 );
    }
  }
  Pointer node = 0;
  // /<rpath>
  if ( s == "" )
  {
    node = GetRoot();
  }
  // if it is an integer number
  else if ( s[0] >= '0' && s[0] <= '9' )
  {
    std::istringstream iss( s );
    unsigned int i = 0;
    iss >> i;
    if ( !iss.fail() ) node = GetChild( i );
  }
  // +[n]
  else if ( s[0] == '+' )
  {
    if ( s.size() > 1 )
    {
      s = s.substr( 1 );
      std::istringstream iss( s );
      unsigned int i = 0;
      iss >> i;
      if ( !iss.fail() ) node = GetSibling( int(i) );
    }
    else node = GetSibling( 1 );
  }
  // -[n]
  else if ( s[0] == '-' )
  {
    if ( s.size() > 1 )
    {
      s = s.substr( 1 );
      std::istringstream iss( s );
      unsigned int i = 0;
      iss >> i;
      if ( !iss.fail() ) node = GetSibling( -int(i) );
    }
    else node = GetSibling( -1 );
  }
  // :<id>
  else if ( s[0] == ':' )
  {
    s = s.substr( 1 );
    node = GetChildByID( s );
  }
  // ..
  else if ( s == ".." )
  {
    node = this->m_Parent;
  }
  // .
  else if ( s == "." )
  {
    node = this;
  }
  // <tag>[:n]
  else
  {
    std::size_t pos = s.find_first_of( ':' );
    if ( pos != std::string::npos )
    {
      std::string s2 = s.substr( pos+1 );
      s = s.substr( 0, pos );
      std::istringstream iss( s2 );
      unsigned int i = 0;
      iss >> i;
      if ( !iss.fail() ) node = GetChild( s, i );
    }
    else node = GetChild( s );
  }
  //
  return ( rpath == "" || (DOMNode*)node == 0 ? node : node->Find(rpath) );
}
DOMNode::ConstPointer DOMNode::Find( const std::string& path ) const
{
  Pointer node = const_cast<Self*>(this)->Find( path );
  return ConstPointer( (DOMNode*)node );
}

DOMTextNode::Pointer DOMNode::GetTextChild( int i )
{
  Pointer node = GetChild( i );
  return DOMTextNode::Pointer( dynamic_cast<DOMTextNode*>(node.GetPointer()) );
}
DOMTextNode::ConstPointer DOMNode::GetTextChild( int i ) const
{
  DOMTextNode::Pointer node = const_cast<Self*>(this)->GetTextChild( i );
  return DOMTextNode::ConstPointer( (DOMTextNode*)node );
}

bool DOMNode::AddTextChild( const std::string& text, int i )
{
  DOMTextNode::Pointer tnode = DOMTextNode::New();
  tnode->GetText() = text;
  return AddChild( Pointer( (DOMTextNode*)tnode ), i );
}
bool DOMNode::SetTextChild( const std::string& text, int i )
{
  DOMTextNode::Pointer tnode = DOMTextNode::New();
  tnode->GetText() = text;
  return SetChild( Pointer( (DOMTextNode*)tnode ), i );
}

} // namespace itk
