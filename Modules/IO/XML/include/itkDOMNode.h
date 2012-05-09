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

#ifndef __itkDOMNode_h
#define __itkDOMNode_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include <string>
#include <vector>
#include <set>
#include <map>
//#include <utility>

namespace itk
{

class DOMTextNode; // forward declaration

/**
 * \class DOMNode
 * \brief Class to represent a node in a Document Object Model (DOM) tree structure.
 *
 * A DOM is a structured document representation, in our case parsed from an XML stream (i.e. a file or a string).
 * It is a tree structure in which each node has links to its children nodes and
 * to its parent node (if it is not the root). This class serves as a base node corresponding to an
 * XML tag that only contains attributes (no content for the tag itself).
 *
 * NOTE: We reserve the use of the attribute "id" (all combinations of upper and lower
 *       case) for uniquely identifying an XML structure among its siblings. That is, we assume
 *       that this tag is unique among all siblings in an XML tree structure. If it is not unique,
 *       the user may not be able to correctly retrieve a node by function GetChildByID(), or to search for
 *       a node using Find() with a query string that is based on "id".
 */
class DOMNode : public Object
{
public:
  /** Standard class typedefs. */
  typedef DOMNode                     Self;
  typedef Object                      Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMNode, Object);

  /** Containers to return all or a subset of the children of a DOM node. */
  typedef std::vector<Pointer>          ChildrenListType;
  typedef std::vector<ConstPointer>     ConstChildrenListType;

  /** An attribute is a pair of <key,value>, both key and value are strings. */
  typedef std::string AttributeKeyType;
  typedef std::string AttributeValueType;
  //
  typedef std::pair<const AttributeKeyType,AttributeValueType> AttributeItemType;

  /** Container to return the attributes of a DOM node. */
  typedef std::set<AttributeItemType> AttributesListType;

protected:
  DOMNode() : m_Parent(0), m_Name("") {}

  /** The parent node that this node was placed into. */
  Pointer m_Parent;

  /** The XML tag of this node. */
  std::string m_Name;

  /** Internally the children are stored in a vector. */
  typedef std::vector<Pointer> ChildrenContainer;
  ChildrenContainer m_Children;

  /** Internally the attributes are stored in a map. */
  typedef std::map<AttributeKeyType,AttributeValueType> AttributesContainer;
  AttributesContainer m_Attributes;

private:
  DOMNode(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

public:
  /** Retrieve the parent node. */
  void SetParent( Pointer parent ) { m_Parent = parent; }
  Pointer GetParent() { return this->m_Parent; }
  ConstPointer GetParent() const { return ConstPointer( this->m_Parent.GetPointer() ); }

  /** Retrieve the tag name of this node. */
  void SetName( const std::string& name ) { this->m_Name = name; }
  std::string& GetName() { return this->m_Name; }
  const std::string& GetName() const { return this->m_Name; }

  /** Retrieve an attribute by key. */
  virtual std::string& GetAttribute( const std::string& key );
  virtual const std::string& GetAttribute( const std::string& key ) const;
  /** Check whether has an attribute. */
  virtual bool HasAttribute( const std::string& key ) const;
  /** Add or replace an attribute. */
  virtual void SetAttribute( const std::string& key, const std::string& value );
  /** Remove an attribute by key and return true if successful. */
  virtual bool RemoveAttribute( const std::string& key );

  /** Return all attributes. */
  virtual AttributesListType& GetAttributes( AttributesListType& output ) const;
  /** Remove all attributes. */
  virtual void ClearAttributes();

  /** Get number of children. */
  virtual int GetNumberOfChildren() const;

  /** Return all children. */
  virtual ChildrenListType& GetChildren( ChildrenListType& output );
  virtual ConstChildrenListType& GetChildren( ConstChildrenListType& output ) const;

  /** Return all children of a same tag name. */
  virtual ChildrenListType& GetChildren( const std::string& tag, ChildrenListType& output );
  virtual ConstChildrenListType& GetChildren( const std::string& tag, ConstChildrenListType& output ) const;

  /** Remove all children. */
  virtual void ClearChildren();

  /** Add a child in front of another child (add at the end if i is out of range). */
  virtual bool AddChild( Pointer node, int i=-1 );
  /** Replace a child and return yes if successful. */
  virtual bool SetChild( Pointer node, int i=0 );

  /** Remove a child by index and return the removed one (return 0 if unsuccessful). */
  virtual Pointer RemoveChild( int i=0 );
  /** Remove a child recursively by node/object pointer and return yes or no. */
  virtual bool RemoveChild( Pointer node );

  /** Clear attributes and children. */
  virtual void Clear();

  /** Retrieve a child by index. */
  virtual Pointer GetChild( int i=0 );
  virtual ConstPointer GetChild( int i=0 ) const;

  /** Retrieve a child by tag name and an index (multiple children can have a same tag name). */
  virtual Pointer GetChild( const std::string& tag, int i=0 );
  virtual ConstPointer GetChild( const std::string& tag, int i=0 ) const;

  /** Retrieve a child by its unique "id" attribute value. */
  virtual Pointer GetChildByID( const std::string& value );
  virtual ConstPointer GetChildByID( const std::string& value ) const;

  /** Retrieve an older or younger sibling by distance. */
  virtual Pointer GetSibling( int i );
  virtual ConstPointer GetSibling( int i ) const;

  /** Return the root node. */
  virtual Pointer GetRoot();
  virtual ConstPointer GetRoot() const;

  /** Find a node recursively by node/object pointer. */
  virtual bool HasNode( ConstPointer node ) const;

  /**
   * The following function finds a child or sibling or relative using a query string or path.
   * A path or QueryString consists of multiple following items that are separated by '/':
   *     -[n]           : an older sibling by distance 1 (when omitted) or n;
   *     +[n]           : a younger sibling by distance 1 (when omitted) or n;
   *     n              : a child at index n;
   *     <tag>[:n]      : a child at index 0 (when omitted) or n after filtering children with a tag name;
   *     ![:n]          : a child at index 0 (when omitted) or n within all text children;
   *     :<id>          : a child by id;
   *     .              : current node;
   *     ..             : parent node;
   *     /<rpath>       : absolute path (denote apath), search from the root.
   */
  virtual Pointer Find( const std::string& path );
  virtual ConstPointer Find( const std::string& path ) const;

  /** Get a child and cast it to a text node. */
  virtual SmartPointer<DOMTextNode> GetTextChild( int i=0 );
  virtual SmartPointer<const DOMTextNode> GetTextChild( int i=0 ) const;

  /** Generate a text node from a string and add/set it as a child. */
  virtual bool AddTextChild( const std::string& text, int i=-1 );
  virtual bool SetTextChild( const std::string& text, int i=0 );
};

/**
 * \class DOMTextNode
 * \brief Class to represent a special DOM node that holds a text string.
 *
 * A text node has no attributes and children. So a text node is always a leaf node,
 * and the special attribute "id" cannot be used.
 * In this implementation, a text node is internally represented using a special tag name of "!".
 * This is not a problem as "!" is not a valid tag name in any XML file.
 */
class DOMTextNode : public DOMNode
{
public:
  /** Standard class typedefs. */
  typedef DOMTextNode                 Self;
  typedef DOMNode                     Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DOMTextNode, DOMNode);

  /** Functions to set/get the enclosed text of this node. */
  void SetText( const std::string& text ) { this->m_Text = text; }
  std::string& GetText() { return this->m_Text; }
  const std::string& GetText() const { return this->m_Text; }

protected:
  DOMTextNode() : m_Text("") { this->m_Name = "!"; }

  /** Variable to hold the text string of this node. */
  std::string m_Text;

private:
  DOMTextNode(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

} // namespace itk

#include "itkDOMHelpers.h"

#endif // __itkDOMNode_h
