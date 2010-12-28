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
#include <algorithm>
#include "itkPriorityQueueContainer.h"

int itkPriorityQueueTest2( int, char * [] )
{
  typedef int ItemType;
  typedef double PriorityType;
  typedef int IdType;

  typedef itk::MinPriorityQueueElementWrapper< ItemType, PriorityType, IdType > MinPQElementType;
  typedef itk::MaxPriorityQueueElementWrapper< ItemType, PriorityType, IdType > MaxPQElementType;

  typedef itk::PriorityQueueContainer<
    MinPQElementType*,
    itk::ElementWrapperPointerInterface< MinPQElementType* >,
    PriorityType,
    IdType > MinPQType;
  MinPQType::Pointer min_priority_queue = MinPQType::New( );

  std::cout << min_priority_queue->GetNameOfClass() << std::endl;

  typedef itk::PriorityQueueContainer<
    MaxPQElementType*,
    itk::ElementWrapperPointerInterface< MaxPQElementType* >,
    PriorityType,
    IdType > MaxPQType;
  MaxPQType::Pointer max_priority_queue = MaxPQType::New( );

  std::list< double > sequence;
  sequence.push_back( -0.1 );
  sequence.push_back( 0.1 );
  sequence.push_back( 0.4 );
  sequence.push_back( -0.2 );
  sequence.push_back( -0.3 );
  sequence.push_back( 0.3 );
  sequence.push_back( 0.2 );
  sequence.push_back( 0.5 );
  sequence.push_back( -0.6 );
  sequence.push_back( -0.5 );
  sequence.push_back( 0.6 );
  sequence.push_back( 1. );
  sequence.push_back( -1. );

  std::map< ItemType, MinPQElementType* > list_min_elements;
  std::map< ItemType, MaxPQElementType* > list_max_elements;

  std::list< double >::const_iterator it = sequence.begin();
  ItemType i = 0; // int
  while( it != sequence.end() )
    {
    MinPQElementType* v_min = new MinPQElementType( i, *it );
    list_min_elements[ i ] = v_min;
    min_priority_queue->Push( v_min );

    MaxPQElementType* v_max = new MaxPQElementType( i, *it );
    list_max_elements[ i ] = v_max;
    max_priority_queue->Push( v_max );

    ++it;
    ++i;
    }

  std::replace( sequence.begin(), sequence.end(), 0.4, 2. );

  ( list_min_elements[2] )->m_Priority = 2.;
  min_priority_queue->Update( list_min_elements[2] );

  ( list_max_elements[2] )->m_Priority = 2.;
  max_priority_queue->Update( list_max_elements[2] );

  sequence.sort();
  it = sequence.begin();
  size_t s = sequence.size();

  std::cout <<"Min Priority Queue   ";
  while( !min_priority_queue->Empty() )
    {
    if( min_priority_queue->Peek()->m_Priority != *it )
      {
      std::cout <<min_priority_queue->Peek()->m_Priority <<" " <<*it <<std::endl;
      return EXIT_FAILURE;
      }
    if( min_priority_queue->Size() != s )
      {
      std::cout <<"Size " <<min_priority_queue->Size() <<" " <<i <<std::endl;
      return EXIT_FAILURE;
      }
    min_priority_queue->Pop();
    it++;
    s--;
    }
  std::cout <<"OK" <<std::endl;

  std::cout <<"Max Priority Queue   ";
  while( !max_priority_queue->Empty() )
    {
    if( max_priority_queue->Peek()->m_Priority != sequence.back() )
      {
      std::cout <<max_priority_queue->Peek()->m_Priority <<" " <<sequence.back() <<std::endl;
      return EXIT_FAILURE;
      }
    if( max_priority_queue->Size() != sequence.size() )
      {
      std::cout <<"Size " <<max_priority_queue->Size() <<" " <<sequence.size() <<std::endl;
      return EXIT_FAILURE;
      }
    max_priority_queue->Pop();
    if (max_priority_queue->Empty())
      {
      break;
      }
    sequence.pop_back();
    }
  std::cout <<"OK" <<std::endl;

  std::map< ItemType, MinPQElementType* >::iterator min_it = list_min_elements.begin();
  std::map< ItemType, MaxPQElementType* >::iterator max_it = list_max_elements.begin();

  while( min_it != list_min_elements.end() )
    {
    delete min_it->second;
    delete max_it->second;
    ++min_it;
    ++max_it;
    }

  return EXIT_SUCCESS;
}
