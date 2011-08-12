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

#ifndef __itkLevelSetDomainMapImageFilter_hxx
#define __itkLevelSetDomainMapImageFilter_hxx

#include "itkLevelSetDomainMapImageFilter.h"

namespace itk
{
template < class TInputImage, class TOutputImage >
LevelSetDomainMapImageFilter< TInputImage, TOutputImage >
::LevelSetDomainMapImageFilter()
{
  this->Superclass::SetNumberOfRequiredInputs ( 1 );
  this->Superclass::SetNumberOfRequiredOutputs ( 1 );

  this->Superclass::SetNthOutput ( 0, OutputImageType::New() );
}


template < class TInputImage, class TOutputImage >
void
LevelSetDomainMapImageFilter< TInputImage, TOutputImage >::
ConsistencyCheck( bool& subRegionConsistent, InputImageRegionType& subRegion )
{
  InputImageConstPointer input = this->GetInput();

  InputConstIteratorType iIt( input, subRegion );
  iIt.GoToBegin();

  OutputIndexIteratorType oIt( this->GetOutput(), subRegion );
  oIt.GoToBegin();

  InputImagePixelType inputPixel = iIt.Get();
  InputImagePixelType nextPixel;
  InputImageIndexType startIdx = subRegion.GetIndex();
  InputImageIndexType stopIdx;
  InputImageSizeType sizeOfRegion;
  OutputImagePixelType segmentPixel;

  while( !iIt.IsAtEnd() )
    {
    segmentPixel = oIt.Get();
    nextPixel = iIt.Get();

    if ( ( nextPixel != inputPixel ) ||
         ( segmentPixel != NumericTraits< OutputImagePixelType >::Zero ) )
      {
      for( unsigned int i = 0; i < ImageDimension; i++ )
        {
        sizeOfRegion[i] = stopIdx[i] - startIdx[i] + 1;
        }
      subRegion.SetSize(sizeOfRegion);
      return;
      }

    stopIdx = iIt.GetIndex();
    ++iIt;
    ++oIt;
    }

  subRegionConsistent = true;
}


template < class TInputImage, class TOutputImage >
void
LevelSetDomainMapImageFilter< TInputImage, TOutputImage >::
GenerateData()
{
  InputImageConstPointer input = this->GetInput();
  InputImageSizeType size = input->GetLargestPossibleRegion().GetSize();

  OutputImagePointer output = this->GetOutput();
  output->CopyInformation( input );
  output->SetRegions( input->GetLargestPossibleRegion() );
  output->Allocate();
  output->FillBuffer( NumericTraits< OutputImagePixelType >::Zero );

  InputImagePixelType inputPixel, nextPixel;
  OutputImagePixelType outputPixel, currentOutputPixel;
  InputImageIndexType startIdx, stopIdx;
  InputImageIndexType end;
  InputImageSizeType sizeOfRegion;
  InputImageRegionType subRegion;

  for( unsigned int i = 0; i < ImageDimension; i++ )
    {
    end[i] = size[i] - 1;
    }

  IdentifierType segmentId = 1;

  InputConstIteratorType iIt( input, input->GetLargestPossibleRegion() );
  OutputIndexIteratorType oIt( output, output->GetLargestPossibleRegion() );

  iIt.GoToBegin();
  oIt.GoToBegin();
  while( !iIt.IsAtEnd() )
    {
    startIdx = iIt.GetIndex();
    stopIdx = startIdx;
    inputPixel = iIt.Get();
    outputPixel = oIt.Get();

    // outputPixel is null when it has not been processed yet,
    // or there is nothing to be processed
    if ( ( !inputPixel.empty() ) && ( outputPixel == 0 ) )
      {
      for( unsigned int i = 0; i < ImageDimension; i++ )
        {
        bool flag = true;
        stopIdx = startIdx;
        while ( ( flag ) && ( stopIdx[i] <= end[i] ) )
          {
          nextPixel = input->GetPixel( stopIdx );
          currentOutputPixel = output->GetPixel( stopIdx );

          // Check if the input list pixels are different, or
          // the output image already has been assigned to another region
          if ( ( nextPixel != inputPixel ) ||
               ( currentOutputPixel != NumericTraits< OutputImagePixelType >::Zero ) )
            {
            flag = false;
            }
          else
            {
            ++stopIdx[i];
            }
          }
        sizeOfRegion[i] = stopIdx[i] - startIdx[i];
        }

      subRegion.SetSize( sizeOfRegion );
      subRegion.SetIndex( startIdx );

      // Check that this subregion is consistent,
      // else partition it even further
      bool subRegionInputConsistent = false;

      while( !subRegionInputConsistent )
        {
        ConsistencyCheck( subRegionInputConsistent, subRegion );
        }

      m_LevelSetMap[segmentId] = NounToBeDefined( subRegion, inputPixel );

      OutputIndexIteratorType ooIt( output, subRegion );
      ooIt.GoToBegin();

      while( !ooIt.IsAtEnd() )
        {
        ooIt.Set( segmentId );
        ++ooIt;
        }
      ++segmentId;
      }
    ++iIt;
    ++oIt;
    }

  this->GraftOutput ( output );
}

template < class TInputImage, class TOutputImage >
void
LevelSetDomainMapImageFilter< TInputImage, TOutputImage >::
PrintSelf ( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf ( os,indent );
  os << indent << "Class Name:        " << GetNameOfClass() << std::endl;
}

} /* end namespace itk */

#endif
