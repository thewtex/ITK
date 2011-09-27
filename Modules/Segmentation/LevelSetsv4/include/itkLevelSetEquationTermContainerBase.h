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

#ifndef __itkLevelSetEquationTermContainerBase_h
#define __itkLevelSetEquationTermContainerBase_h

#include "itkLevelSetEquationTermBase.h"
#include "itkObject.h"

#include "itksys/hash_map.hxx"

#include <map>
#include <string>

namespace itk
{
/**
 *  \class LevelSetEquationTermContainerBase
 *  \brief Class for container holding the terms of a given level set update equation
 *
 *  \tparam TInputImage Input image or speed image or feature image for segmentation
 *  \tparam TLevelSetContainer Container holding the all the level set functions
 *  \ingroup ITKLevelSetsv4
 */
template< class TInputImage,
          class TLevelSetContainer >
class LevelSetEquationTermContainerBase : public Object
{
public:
  typedef LevelSetEquationTermContainerBase Self;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;
  typedef Object                            Superclass;

  /** Method for creation through object factory */
  itkNewMacro( Self );

  /** Run-time type information */
  itkTypeMacro( LevelSetEquationTermContainerBase,
                Object );

  typedef unsigned int                      TermIdType;

  typedef TInputImage                       InputImageType;
  typedef typename InputImageType::Pointer  InputImagePointer;

  typedef TLevelSetContainer                                LevelSetContainerType;
  typedef typename LevelSetContainerType::Pointer           LevelSetContainerPointer;

  typedef typename LevelSetContainerType::LevelSetType      LevelSetType;
  typedef typename LevelSetContainerType::LevelSetPointer   LevelSetPointer;

  typedef typename LevelSetContainerType::LevelSetIdentifierType
                                                            LevelSetIdentifierType;
  typedef typename LevelSetContainerType::OutputType        LevelSetOutputPixelType;
  typedef typename LevelSetContainerType::OutputRealType    LevelSetOutputRealType;
  typedef typename LevelSetContainerType::LevelSetDataType  LevelSetDataType;
  typedef typename LevelSetContainerType::InputIndexType    LevelSetInputIndexType;
  typedef typename LevelSetContainerType::GradientType      LevelSetGradientType;
  typedef typename LevelSetContainerType::HessianType       LevelSetHessianType;

  typedef LevelSetEquationTermBase< InputImageType, LevelSetContainerType >
                                                                       TermType;
  typedef typename TermType::Pointer                                   TermPointer;

  /** Set/Get the input image to be segmented. */
  itkSetObjectMacro( Input, InputImageType );
  itkGetObjectMacro( Input, InputImageType );

  /** Add a term to the end of the container  */
  void PushTerm( TermType* iTerm );

  /** Replace the pointer to the term with the given id */
  void AddTerm( const TermIdType& iId, TermType* iTerm );

  /** Get the term with the given id */
  TermType* GetTerm( const TermIdType& iId );

  /** Get the term with the given name */
  TermType* GetTerm( const std::string& iName );

  /** \todo  */
  void Initialize( const LevelSetInputIndexType& iP );

  /** Supply the update at a given pixel location to update the term parameters */
  void UpdatePixel( const LevelSetInputIndexType& iP,
                    const LevelSetOutputRealType & oldValue,
                    const LevelSetOutputRealType & newValue );

  /** Initialize the term parameters prior to the start of an iteration */
  void InitializeParameters();

  /** Evaluate the term at a given pixel location */
  LevelSetOutputRealType Evaluate( const LevelSetInputIndexType& iP );

  LevelSetOutputRealType Evaluate( const LevelSetInputIndexType& iP,
                                   const LevelSetDataType& iData );

  /** Update the term parameters at end of iteration */
  void Update();

  /** Return the CFL contribution of the current term */
  LevelSetOutputRealType ComputeCFLContribution() const;

  void ComputeRequiredData( const LevelSetInputIndexType& iP,
                            LevelSetDataType& ioData );

protected:

  typedef std::map< TermIdType, TermPointer >           MapTermContainerType;
  typedef typename MapTermContainerType::iterator       MapTermContainerIteratorType;
  typedef typename MapTermContainerType::const_iterator MapTermContainerConstIteratorType;

public:
  class TermIterator;
  friend class TermIterator;

  class ConstTermIterator
  {
  public:
    ConstTermIterator() {}
    ConstTermIterator( const MapTermContainerConstIteratorType& it ) : m_Iterator( it ) {}
    ~ConstTermIterator() {}
    ConstTermIterator( const TermIterator& it ) : m_Iterator( it.m_Iterator ) {}
    ConstTermIterator & operator * () { return *this; }
    ConstTermIterator * operator->() { return this; }
    ConstTermIterator & operator++()
      {
      ++m_Iterator;
      return *this;
      }
    ConstTermIterator operator++(int)
      {
      ConstTermIterator tmp( *this );
      ++(*this);
      return tmp;
      }
    ConstTermIterator & operator--()
      {
      --m_Iterator;
      return *this;
      }
    ConstTermIterator operator--(int)
      {
      ConstTermIterator tmp( *this );
      --(*this);
      return tmp;
      }
    bool operator == (const TermIterator& it) const
      {
      return (m_Iterator == it.m_Iterator);
      }
    bool operator != (const TermIterator& it) const
      {
      return (m_Iterator != it.m_Iterator);
      }
    bool operator == (const ConstTermIterator& it) const
      {
      return (m_Iterator == it.m_Iterator);
      }
    bool operator != (const ConstTermIterator& it) const
      {
      return (m_Iterator != it.m_Iterator);
      }
    TermIdType GetIdentifier() const
      {
      return m_Iterator->first;
      }

    TermType * GetTerm() const
      {
      return m_Iterator->second;
      }
  private:
    MapTermContainerConstIteratorType m_Iterator;
    friend class TermIterator;
  };

  class TermIterator
  {
  public:
    TermIterator() {}
    TermIterator( const MapTermContainerIteratorType& it ) : m_Iterator( it ) {}
    TermIterator( const ConstTermIterator& it ) : m_Iterator( it.m_Iterator ) {}
    ~TermIterator() {}

    TermIterator & operator * () { return *this; }
    TermIterator * operator ->() { return this; }

    TermIterator & operator++()
    {
      ++m_Iterator;
      return *this;
    }
    TermIterator operator++(int)
    {
       TermIterator tmp( *this );
      ++(*this);
      return tmp;
    }
    TermIterator & operator--()
    {
      --m_Iterator;
      return *this;
    }
    TermIterator operator--(int)
    {
      TermIterator tmp( *this );
      --(*this);
      return tmp;
    }

    bool operator==(const TermIterator& it) const
    {
      return (m_Iterator==it.m_Iterator);
    }
    bool operator!=(const TermIterator& it) const
    {
      return (m_Iterator!=it.m_Iterator);
    }
    bool operator==(const ConstTermIterator& it)const
    {
      return (m_Iterator == it.m_Iterator);
    }
    bool operator!=(const ConstTermIterator& it)const
    {
      return (m_Iterator != it.m_Iterator);
    }
    TermIdType GetIdentifier() const
      {
      return m_Iterator->first;
      }

    TermType * GetTerm() const
      {
      return m_Iterator->second;
      }
  private:
    MapTermContainerIteratorType m_Iterator;
    friend class ConstTermIterator;
  };

   TermIterator TermBegin();
   TermIterator TermEnd();

  ConstTermIterator TermBegin() const;
  ConstTermIterator TermEnd() const;

protected:
  LevelSetEquationTermContainerBase();

  virtual ~LevelSetEquationTermContainerBase();

  InputImagePointer     m_Input;

  typedef itksys::hash_map< std::string, TermPointer > HashMapStringTermContainerType;

  HashMapStringTermContainerType m_NameContainer;

  typedef typename TermType::RequiredDataType RequiredDataType;
  RequiredDataType  m_RequiredData;

  MapTermContainerType  m_Container;

  typedef std::map< TermIdType, LevelSetOutputRealType >  MapCFLContainerType;
  typedef typename MapCFLContainerType::iterator          MapCFLContainerIterator;
  typedef typename MapCFLContainerType::const_iterator    MapCFLContainerConstIterator;

  MapCFLContainerType   m_TermContribution;

private:
  LevelSetEquationTermContainerBase( const Self& );
  void operator = ( const Self& );
};

}
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLevelSetEquationTermContainerBase.hxx"
#endif

#endif // __itkLevelSetEquationTermContainerBase_h
