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
#ifndef __itkLeafTreeIterator_h
#define __itkLeafTreeIterator_h

#include <itkPreOrderTreeIterator.h>

namespace itk
{
template< class TTreeType >
class LeafTreeIterator:public TreeIteratorBase< TTreeType >
{
public:

  /** Typedefs */
  typedef LeafTreeIterator              Self;
  typedef TreeIteratorBase< TTreeType > Superclass;
  typedef TTreeType                     TreeType;
  typedef typename TreeType::ValueType  ValueType;
  typedef TreeNode< ValueType >         TreeNodeType;

  /** Constructor */
  LeafTreeIterator(const TreeType *tree);

  /** Constructor */
  LeafTreeIterator(TreeType *tree);

  /** Destructor */
  virtual ~LeafTreeIterator();

  /** Return the type of iterator */
  int GetType() const;

  /** Clone function */
  TreeIteratorBase< TTreeType > * Clone();

protected:

  /** Return the next value */
  const ValueType & Next();

  /** Return true if the next value exists */
  bool HasNext() const;

private:

  /** Find the next node */
  const TreeNodeType * FindNextNode() const;
};

/** Constructor */
template< class TTreeType >
LeafTreeIterator< TTreeType >::LeafTreeIterator(const TTreeType *tree):
  TreeIteratorBase< TTreeType >(tree, NULL)
{
  this->m_Begin = const_cast< TreeNodeType * >( this->FindNextNode() ); //
                                                                        //
                                                                        // Position
                                                                        // the
                                                                        //
                                                                        // iterator
                                                                        // to
                                                                        // the
                                                                        // first
                                                                        // leaf;
}

/** Constructor */
template< class TTreeType >
LeafTreeIterator< TTreeType >::LeafTreeIterator(TTreeType *tree):
  TreeIteratorBase< TTreeType >(tree, NULL)
{
  this->m_Begin = const_cast< TreeNodeType * >( this->FindNextNode() ); //
                                                                        //
                                                                        // Position
                                                                        // the
                                                                        //
                                                                        // iterator
                                                                        // to
                                                                        // the
                                                                        // first
                                                                        // leaf;
}

/** Destructor */
template< class TTreeType >
LeafTreeIterator< TTreeType >::~LeafTreeIterator()
{}

/** Return the type of iterator */
template< class TTreeType >
int LeafTreeIterator< TTreeType >::GetType() const
{
  return TreeIteratorBase< TTreeType >::LEAF;
}

/** Return true if the next value exists */
template< class TTreeType >
bool LeafTreeIterator< TTreeType >::HasNext() const
{
  if ( this->m_Position == NULL )
    {
    return false;
    }
  if ( const_cast< TreeNodeType * >( FindNextNode() ) != NULL )
    {
    return true;
    }
  return false;
}

/** Return the next node */
template< class TTreeType >
const typename LeafTreeIterator< TTreeType >::ValueType &
LeafTreeIterator< TTreeType >::Next()
{
  this->m_Position = const_cast< TreeNodeType * >( FindNextNode() );
  return this->m_Position->Get();
}

/** Find the next node given the position */
template< class TTreeType >
const typename LeafTreeIterator< TTreeType >::TreeNodeType *
LeafTreeIterator< TTreeType >::FindNextNode() const
{
  PreOrderTreeIterator< TTreeType > it(this->m_Tree, this->m_Position);
  ++it; // go next
  if ( it.IsAtEnd() )
    {
    return NULL;
    }

  if ( !it.HasChild() )
    {
    return it.GetNode();
    }

  while ( !it.IsAtEnd() )
    {
    if ( !it.HasChild() )
      {
      return it.GetNode();
      }
    ++it;
    }

  return NULL;
}

/** Clone function */
template< class TTreeType >
TreeIteratorBase< TTreeType > *LeafTreeIterator< TTreeType >::Clone()
{
  LeafTreeIterator< TTreeType > *clone = new LeafTreeIterator< TTreeType >(this->m_Tree);
  *clone = *this;
  return clone;
}
} // end namespace itk

#endif
