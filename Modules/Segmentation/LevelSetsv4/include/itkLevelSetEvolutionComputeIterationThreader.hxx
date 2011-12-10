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
#ifndef __itkLevelSetEvolutionComputeIterationThreader_hxx
#define __itkLevelSetEvolutionComputeIterationThreader_hxx

#include "itkLevelSetEvolutionComputeIterationThreader.h"

#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

template< class TImage, class TLevelSetEvolution >
LevelSetEvolutionComputeIterationThreader< LevelSetDenseImageBase< TImage >, ThreadedImageRegionPartitioner< TImage::ImageDimension >, TLevelSetEvolution >
::LevelSetEvolutionComputeIterationThreader()
{
}

template< class TImage, class TLevelSetEvolution >
void
LevelSetEvolutionComputeIterationThreader< LevelSetDenseImageBase< TImage >, ThreadedImageRegionPartitioner< TImage::ImageDimension >, TLevelSetEvolution >
::ThreadedExecution( const DomainType & imageSubRegion,
                     const ThreadIdType itkNotUsed(threadId) )
{
  typename LevelSetContainerType::Iterator levelSetContainerIt = this->m_Associate->m_LevelSetContainer->Begin();
  typename LevelSetContainerType::ConstIterator levelSetUpdateContainerIt = this->m_Associate->m_UpdateBuffer->Begin();
  typename EquationContainerType::Iterator equationContainerIt = this->m_Associate->m_EquationContainer->Begin();

  // This is for single level set analysis, so we only process the first level
  // set.
  typename LevelSetType::Pointer levelSet = levelSetContainerIt->GetLevelSet();
  typename LevelSetType::Pointer levelSetUpdate = levelSetUpdateContainerIt->GetLevelSet();

  typename LevelSetImageType::ConstPointer levelSetImage = levelSet->GetImage();
  typename LevelSetImageType::Pointer levelSetUpdateImage = levelSetUpdate->GetImage();

  typename TermContainerType::Pointer termContainer = equationContainerIt->GetEquation();

  ImageRegionConstIteratorWithIndex< LevelSetImageType > it( levelSetImage, imageSubRegion );
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    LevelSetDataType characteristics;

    termContainer->ComputeRequiredData( it.GetIndex(), characteristics );

    LevelSetOutputRealType temp_update = termContainer->Evaluate( it.GetIndex(), characteristics );

    levelSetUpdateImage->SetPixel( it.GetIndex(), temp_update );
    ++it;
    }
}

template< class TOutput, unsigned int VDimension, class TLevelSetEvolution >
LevelSetEvolutionComputeIterationThreader<
      WhitakerSparseLevelSetImage< TOutput, VDimension >,
      ThreadedIteratorRangePartitioner< typename WhitakerSparseLevelSetImage< TOutput, VDimension >::LayerConstIterator >,
      TLevelSetEvolution >
::LevelSetEvolutionComputeIterationThreader()
{
}

template< class TOutput, unsigned int VDimension, class TLevelSetEvolution >
void
LevelSetEvolutionComputeIterationThreader<
      WhitakerSparseLevelSetImage< TOutput, VDimension >,
      ThreadedIteratorRangePartitioner< typename WhitakerSparseLevelSetImage< TOutput, VDimension >::LayerConstIterator >,
      TLevelSetEvolution >
::BeforeThreadedExecution()
{
  this->m_NodePairsPerThread.resize( this->GetNumberOfThreadsUsed() );

  for( ThreadIdType ii = 0; ii < this->GetNumberOfThreadsUsed(); ++ii )
    {
    this->m_NodePairsPerThread[ii].clear();
    }
}

template< class TOutput, unsigned int VDimension, class TLevelSetEvolution >
void
LevelSetEvolutionComputeIterationThreader<
      WhitakerSparseLevelSetImage< TOutput, VDimension >,
      ThreadedIteratorRangePartitioner< typename WhitakerSparseLevelSetImage< TOutput, VDimension >::LayerConstIterator >,
      TLevelSetEvolution >
::ThreadedExecution( const DomainType & iteratorSubRange,
                     const ThreadIdType threadId )
{
  typename LevelSetContainerType::Iterator it = this->m_Associate->m_LevelSetContainer->Begin();
  typename LevelSetType::ConstPointer levelSet = it->GetLevelSet();

  LevelSetIdentifierType levelSetId = it->GetIdentifier();
  typename TermContainerType::Pointer termContainer = this->m_Associate->m_EquationContainer->GetEquation( levelSetId );

  typename LevelSetType::LayerConstIterator listIt = iteratorSubRange.Begin();

  while( listIt != iteratorSubRange.End() )
    {
    const LevelSetInputType idx = listIt->first;

    LevelSetDataType characteristics;

    termContainer->ComputeRequiredData( idx, characteristics );

    const LevelSetOutputType temp_update =
        static_cast< LevelSetOutputType >( termContainer->Evaluate( idx, characteristics ) );

    this->m_NodePairsPerThread[threadId].push_back( NodePairType( idx, temp_update ) );

    ++listIt;
    }
}

template< class TOutput, unsigned int VDimension, class TLevelSetEvolution >
void
LevelSetEvolutionComputeIterationThreader<
      WhitakerSparseLevelSetImage< TOutput, VDimension >,
      ThreadedIteratorRangePartitioner< typename WhitakerSparseLevelSetImage< TOutput, VDimension >::LayerConstIterator >,
      TLevelSetEvolution >
::AfterThreadedExecution()
{
  typename LevelSetContainerType::Iterator it = this->m_Associate->m_LevelSetContainer->Begin();
  LevelSetIdentifierType levelSetId = it->GetIdentifier();

  for( ThreadIdType ii = 0; ii < this->GetNumberOfThreadsUsed(); ++ii )
    {
    for( typename std::vector< NodePairType >::const_iterator pairIt = this->m_NodePairsPerThread[ii].begin(); pairIt != this->m_NodePairsPerThread[ii].end(); ++pairIt )
      {
      this->m_Associate->m_UpdateBuffer[ levelSetId ]->insert( *pairIt );
      }
    }
}

} // end namespace itk

#endif
