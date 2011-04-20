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
#ifndef __itkCorrespondingList_hpp
#define __itkCorrespondingList_hpp

#include "itkCorrespondingList.h"

namespace itk
{
/**
 * Constructor.
 */
template< typename TItemType, int VCliqueSize >
CorrespondingList< TItemType, VCliqueSize >
::CorrespondingList()
{}

/**
 * Destructor.
 */
template< typename TItemType, int VCliqueSize >
CorrespondingList< TItemType, VCliqueSize >
::~CorrespondingList()
{}
} // end namespace itk

#endif
