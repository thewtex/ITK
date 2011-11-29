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

#ifndef __itkFastMarchingSizeStoppingCriterion_h
#define __itkFastMarchingSizeStoppingCriterion_h

#include "itkFastMarchingStoppingCriterionBase.h"
#include "itkObjectFactory.h"

namespace itk
{
  /**
    \class FastMarchingSizeStoppingCriterion
    \brief Stopping Criterion is verified when Current Size is equal to or
    greater than the provided size.

    \note For images the size can easily be linked to the physical size of the object,
    i.e. \f$ PhysicalSize = Size \cdot \prod_{i=1}{dim} Spacing_{i} \f$

    \ingroup ITKFastMarching
    */
  template< class TInput, class TOutput >
  class FastMarchingSizeStoppingCriterion :
      public FastMarchingStoppingCriterionBase< TInput, TOutput >
    {
  public:
    typedef FastMarchingSizeStoppingCriterion                     Self;
    typedef FastMarchingStoppingCriterionBase< TInput, TOutput >  Superclass;
    typedef SmartPointer< Self >                                  Pointer;
    typedef SmartPointer< const Self >                            ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(FastMarchingSizeStoppingCriterion,
                 FastMarchingStoppingCriterionBase );

    typedef typename Superclass::OutputPixelType  OutputPixelType;
    typedef typename Superclass::NodeType         NodeType;

    /** Get/set the threshold used by the stopping criteria. */
    itkSetMacro( Size, IdentifierType );
    itkGetMacro( Size, IdentifierType );

    bool IsSatisfied() const
      {
      return ( this->m_CurrentSize >= this->m_Size );
      }

    std::string GetDescription() const
      {
      return "Current Size >= Size";
      }

  protected:
    FastMarchingSizeStoppingCriterion() : Superclass(),
      m_CurrentSize( NumericTraits< IdentifierType >::Zero ),
      m_Size( NumericTraits< IdentifierType >::Zero )
    {}

    ~FastMarchingSizeStoppingCriterion() {}

    IdentifierType  m_CurrentSize;
    IdentifierType  m_Size;

    void SetCurrentNode( const NodeType& )
      {
      ++this->m_CurrentSize;
      }

    void Reset()
      {
      this->m_CurrentSize = NumericTraits< IdentifierType >::Zero;
      }

  private:
    FastMarchingSizeStoppingCriterion( const Self& );
    void operator = ( const Self& );
    };

}
#endif // __itkFastMarchingSizeStoppingCriterion_h
