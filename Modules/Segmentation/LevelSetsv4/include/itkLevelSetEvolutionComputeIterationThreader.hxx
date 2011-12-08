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
  typename InputImageType::ConstPointer inputImage = this->m_Associate->m_EquationContainer->GetInput();

  ImageRegionConstIteratorWithIndex< InputImageType > it( inputImage, imageSubRegion );
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    typename LevelSetType::Pointer levelSetUpdate = this->m_Associate->m_UpdateBuffer->GetLevelSet( 0 );

    LevelSetDataType characteristics;

    typename TermContainerType::Pointer termContainer = this->m_Associate->m_EquationContainer->GetEquation( 0 );
    termContainer->ComputeRequiredData( it.GetIndex(), characteristics );

    LevelSetOutputRealType temp_update = termContainer->Evaluate( it.GetIndex(), characteristics );

    LevelSetImageType* levelSetImage = levelSetUpdate->GetImage();
    levelSetImage->SetPixel( it.GetIndex(), temp_update );
    ++it;
    }
}

} // end namespace itk

#endif
