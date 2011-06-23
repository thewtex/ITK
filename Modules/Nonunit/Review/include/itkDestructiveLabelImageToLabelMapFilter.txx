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
#ifndef __itkDestructiveLabelImageToLabelMapFilter_txx
#define __itkDestructiveLabelImageToLabelMapFilter_txx

#include "itkDestructiveLabelImageToLabelMapFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkImageLinearConstIteratorWithIndex.h"

namespace itk
{
template< class TInputImage, class TOutputImage, class TFunctor >
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::DestructiveLabelImageToLabelMapFilter()
{
  m_OutputBackgroundValue = NumericTraits< OutputImagePixelType >::NonpositiveMin();
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  InputImagePointer input = const_cast< InputImageType * >( this->GetInput() );
  if ( !input )
    {
    return;
    }
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::EnlargeOutputRequestedRegion(DataObject *)
{
  OutputImagePointer output = this->GetOutput();
  output->SetRequestedRegion( output->GetLargestPossibleRegion() );
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::BeforeThreadedGenerateData()
{
  // init the temp images - one per thread
  m_TemporaryLabelObjectMap.clear();
  m_TemporaryLabelObjectMap.resize( this->GetNumberOfThreads() );
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::ThreadedGenerateData(const OutputImageRegionType & regionForThread, ThreadIdType threadId)
{
  ProgressReporter progress( this, threadId, regionForThread.GetNumberOfPixels() );

  LabelObjectMapType & lom = m_TemporaryLabelObjectMap[threadId];

  typedef ImageLinearConstIteratorWithIndex< InputImageType > InputLineIteratorType;
  InputLineIteratorType it(this->GetInput(), regionForThread);
  it.SetDirection(0);

  for ( it.GoToBegin(); !it.IsAtEnd(); it.NextLine() )
    {
    it.GoToBeginOfLine();

    while ( !it.IsAtEndOfLine() )
      {
      const InputImagePixelType & v = it.Get();

      if ( v != m_InputBackgroundValue )
        {
        // We've hit the start of a run
        IndexType idx = it.GetIndex();
        LengthType      length = 1;
        ++it;
        progress.CompletedPixel();

        while ( !it.IsAtEndOfLine() && it.Get() == v )
          {
          ++length;
          ++it;
          progress.CompletedPixel();
          }

        // create the run length object to go in the vector
        LabelObjectMapIterator mit = lom.find(v);
        LabelObjectPointer lo;
        if( mit == lom.end() )
          {
          lo = LabelObjectType::New();
          lom[v] = lo;
          }
        else
          {
          lo = mit->second;
          }
        lo->AddLine(idx, length);
        }
      else
        {
        // go the the next pixel
        ++it;
        progress.CompletedPixel();
        }
      }
    }
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::AfterThreadedGenerateData()
{
  ThreadIdType numberOfThreads = this->GetNumberOfThreads();

  LabelObjectMapType & mainLom = m_TemporaryLabelObjectMap[0];

  // merge the lines from the temporary images in the output image
  // don't use the first image - that's the output image
  for ( ThreadIdType i = 1; i < numberOfThreads; i++ )
    {
    LabelObjectMapType & lom = m_TemporaryLabelObjectMap[i];

    for ( LabelObjectMapIterator it = lom.begin();
         it != lom.end();
         ++it )
      {
      const InputImagePixelType & label = it->first;
      LabelObjectPointer labelObject = it->second;

      LabelObjectMapIterator mainIt = mainLom.find( label );
      if ( mainIt != mainLom.end() )
        {
        if ( mainIt->second.IsNotNull() )
          {
          // merge the lines in the output's object
          LineContainerType & src = labelObject->GetLineContainer();
          LineContainerType & dest = mainIt->second->GetLineContainer();

          dest.insert( dest.end(), src.begin(), src.end() );
          }
        else
          {
          // simply take the object
          mainIt->second = labelObject;
          }
        }
      else
        {
        // simply take the object
        mainLom[label] = labelObject;
        }
      }
    }

  OutputImagePointer output = this->GetOutput();

  for ( LabelObjectMapIterator it = mainLom.begin();
       it != mainLom.end();
       ++it )
    {
    const InputImagePixelType & ilabel = it->first;
    LabelObjectPointer labelObject = it->second;

    output->PushLabelObject(labelObject);
    this->CustomizeLabelObject( ilabel, labelObject );
    }

  // release the data in the temp images
  m_TemporaryLabelObjectMap.clear();
}

// -----------------------------------------------------------------------------
template< class TInputImage, class TOutputImage, class TFunctor >
void
DestructiveLabelImageToLabelMapFilter< TInputImage, TOutputImage, TFunctor >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  typedef typename NumericTraits< OutputImagePixelType >::PrintType OutputPrintType;

  os << indent << "OutputBackgroundValue: "
     << static_cast< OutputPrintType >( m_OutputBackgroundValue )
     << std::endl;
}
} // end namespace itk
#endif
