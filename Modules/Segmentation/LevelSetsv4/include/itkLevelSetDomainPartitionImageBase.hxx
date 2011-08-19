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
#ifndef __itkLevelSetDomainPartitionImageBase_hxx
#define __itkLevelSetDomainPartitionImageBase_hxx

#include "itkLevelSetDomainPartitionImageBase.h"

namespace itk
{
template< class TImage >
void LevelSetDomainPartitionImageBase< TImage >
::PopulateListDomain()
  {
    ListSpacingType spacing = this->m_ListDomain->GetSpacing();

    ListRegionType region = this->m_ListDomain->GetLargestPossibleRegion();

    ListIteratorType lIt(this->m_ListDomain, region);

    for ( lIt.GoToBegin(); !lIt.IsAtEnd(); ++lIt )
      {
      ListIndexType ind = lIt.GetIndex();
      IdentifierListType L;
      for ( unsigned int i = 0; i < this->m_FunctionCount; i++ )
        {
        if ( this->m_LevelSetDataPointerVector[i]->VerifyInsideRegion(ind) )
          {
          L.push_back(i);
          }
        }
      lIt.Set(L);
      }
  }

template< class TImage >
void LevelSetDomainPartitionImageBase< TImage >
::AllocateListDomain()
  {
    if( m_Image.IsNull() )
      {
      itkGenericExceptionMacro( "m_Image is null" );
      }
    this->m_ListDomain = ListImageType::New();
    this->m_ListDomain->CopyInformation( m_Image );
    this->m_ListDomain->SetRegions( m_Image->GetLargestPossibleRegion() );
    this->m_ListDomain->Allocate();
  }
} //end namespace itk

#endif
