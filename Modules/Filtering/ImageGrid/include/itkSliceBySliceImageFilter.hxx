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
#ifndef __itkSliceBySliceImageFilter_hxx
#define __itkSliceBySliceImageFilter_hxx

#include "itkSliceBySliceImageFilter.h"
#include "itkProgressReporter.h"
#include "itkImageAlgorithm.h"

namespace itk
{
template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::SliceBySliceImageFilter()
{
  m_InputFilter = NULL;
  m_OutputFilter = NULL;
  this->m_Dimension = ImageDimension - 1;
  m_SliceIndex = 0;
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::VerifyInputInformation()
{

  Superclass::VerifyInputInformation();

  // verify sane parameter
  if ( this->m_Dimension >= RegionType::ImageDimension )
    {
    itkExceptionMacro("Dimension selected for slicing is greater than ImageDimension");
    }

  if ( !m_InputFilter )
    {
    itkExceptionMacro("InputFilter must be set.");
    }

  if ( !m_OutputFilter )
    {
    itkExceptionMacro("OutputFilter must be set.");
    }

}


template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::GenerateInputRequestedRegion()
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  InputImagePointer inputPtr  =
    const_cast< TInputImage * >( this->GetInput() );

  const RegionType &requestedInputRegion = inputPtr->GetRequestedRegion();

  // The requested region is the largest is all but the slice
  // dimension. In that dimension we can stream the requested
  // slices.
  RegionType inputRegion = inputPtr->GetLargestPossibleRegion();
  inputRegion.SetIndex( m_Dimension, requestedInputRegion.GetIndex(m_Dimension) );
  inputRegion.SetSize( m_Dimension, requestedInputRegion.GetSize(m_Dimension) );

  inputPtr->SetRequestedRegion( inputRegion );
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::SetFilter(InputFilterType *filter)
{
  OutputFilterType *outputFilter = dynamic_cast< OutputFilterType * >( filter );

  if ( outputFilter == NULL && filter != NULL )
    {
    // TODO: can it be replaced by a concept check ?
    itkExceptionMacro(
      "Wrong output filter type. Use SetOutputFilter() and SetInputFilter() instead of SetFilter() when input and output filter types are different.");
    }
  this->SetInputFilter(filter);
  this->SetOutputFilter(outputFilter);
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::SetInputFilter(InputFilterType *filter)
{
  if ( m_InputFilter.GetPointer() != filter )
    {
    this->Modified();
    m_InputFilter = filter;
    // adapt the number of inputs and outputs
    this->SetNumberOfRequiredInputs( filter->GetNumberOfValidRequiredInputs() );
    }
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::SetOutputFilter(OutputFilterType *filter)
{
  if ( m_OutputFilter.GetPointer() != filter )
    {
    this->Modified();
    m_OutputFilter = filter;
    // adapt the number of inputs and outputs
    this->SetNumberOfRequiredOutputs( filter->GetNumberOfIndexedOutputs() );
    }
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::GenerateData()
{

  const ProcessObject::DataObjectPointerArraySizeType numberOfIndexedInputs = this->GetNumberOfIndexedInputs();
  const ProcessObject::DataObjectPointerArraySizeType numberOfIndexedOutputs = this->GetNumberOfIndexedOutputs();

  const SizeType firstInputSize = this->GetInput( 0 )->GetRequestedRegion().GetSize();
  for ( unsigned int i = 1; i < numberOfIndexedInputs; i++ )
    {
    if ( firstInputSize != this->GetInput( i )->GetRequestedRegion().GetSize() )
      {
      itkExceptionMacro(<< "Inputs must have the same size.");
      }
    }

  this->AllocateOutputs();


  const RegionType requestedRegion = this->GetOutput( 0 )->GetRequestedRegion();
  const IndexType requestedIndex = requestedRegion.GetIndex();
  const SizeType requestedSize = requestedRegion.GetSize();

  InternalRegionType internalOutputRegion;
  InternalRegionType internalInputRegion;

  // copy the requested region to the internal slice region in
  // dimension order
  unsigned int internal_i = 0;
  for ( unsigned int i = 0; internal_i < InternalImageDimension; ++i, ++internal_i )
    {
    if ( i == this->m_Dimension )
      {
      ++i;
      }
    internalOutputRegion.SetSize( internal_i, requestedSize[i] );
    internalOutputRegion.SetIndex( internal_i, requestedIndex[i] );

    internalInputRegion.SetSize( internal_i, this->GetInput( 0 )->GetRequestedRegion().GetSize(i) );
    internalInputRegion.SetIndex( internal_i, this->GetInput( 0 )->GetRequestedRegion().GetIndex(i) );

    }

  ProgressReporter progress(this, 0, requestedSize[m_Dimension]);

  // allocate a vector to store internal image
  typedef typename InternalInputImageType::Pointer InternalInputImagePointer;
  std::vector< InternalInputImagePointer > internalInputs( this->GetNumberOfIndexedInputs() );

  // keep the internal input around each iteration, because if the
  // fitlers are not run inplace, we don't need to reallocate each iteration
  for ( unsigned int i = 0; i < numberOfIndexedInputs; i++ )
      {
      internalInputs[i] = InternalInputImageType::New();
      }

  const IndexValueType sliceRangeMax =
    static_cast<IndexValueType>(requestedSize[m_Dimension] + requestedIndex[m_Dimension]);

  for ( IndexValueType slice = requestedIndex[m_Dimension]; slice < sliceRangeMax; ++slice )
    {
    // say to the user that we are begining a new slice
    this->m_SliceIndex = slice;
    this->InvokeEvent( IterationEvent() );


    RegionType inputRegion = this->GetInput( 0 )->GetRequestedRegion();
    inputRegion.SetIndex( m_Dimension, slice );
    inputRegion.SetSize( m_Dimension, 1 );


    // this region is the current output region we
    // are iterating on
    RegionType outputRegion = this->GetOutput( 0 )->GetRequestedRegion();
    outputRegion.SetIndex( m_Dimension, slice );
    outputRegion.SetSize( m_Dimension, 1 );

    itkDebugMacro( "inputRegion: " << inputRegion );
    itkDebugMacro( "internalInputRegion: " << internalInputRegion );

    itkDebugMacro( "outputRegion: " << outputRegion );
    itkDebugMacro( "internalOutputRegion: " << internalOutputRegion );


    itkAssertOrThrowMacro( inputRegion.GetNumberOfPixels() == internalInputRegion.GetNumberOfPixels(), "inputRegion.GetNumberOfPixels() == internalInputRegion.GetNumberOfPixel()" );
    itkAssertOrThrowMacro( outputRegion.GetNumberOfPixels() == internalOutputRegion.GetNumberOfPixels(), "outputRegion.GetNumberOfPixels() == internalOutputRegion.GetNumberOfPixel()" );

    // reallocate the internal input at each slice, so the slice by slice filter can work
    // even if the pipeline is run in place
    for ( unsigned int i = 0; i < numberOfIndexedInputs; i++ )
      {
      internalInputs[i]->SetRegions( internalInputRegion );
      internalInputs[i]->Allocate();
      m_InputFilter->SetInput(i, internalInputs[i]);
      }


    for ( unsigned int i = 0; i < numberOfIndexedInputs; i++ )
      {
      ImageAlgorithm::Copy( this->GetInput( i ), internalInputs[i].GetPointer(), inputRegion, internalInputRegion );
      }

    // run the filter on the current slice
    this->m_InputFilter->Modified();
    this->m_OutputFilter->Modified(); // should not be needed, but may help in some
                                // cases=
    this->m_OutputFilter->GetOutput()->SetRequestedRegion(internalOutputRegion);
    this->m_OutputFilter->Update();

    progress.CompletedPixel();

    // and copy the output slice to the output image
    for ( unsigned int i = 0; i < numberOfIndexedOutputs; i++ )
      {
      ImageAlgorithm::Copy( this->m_OutputFilter->GetOutput( i ), this->GetOutput( i ), internalOutputRegion, outputRegion );
      }
    }
}

template< class TInputImage, class TOutputImage, class TInputFilter, class TOutputFilter, class TInternalInputImageType,
          class TInternalOutputImageType >
void
SliceBySliceImageFilter< TInputImage, TOutputImage, TInputFilter, TOutputFilter, TInternalInputImageType,
                         TInternalOutputImageType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Dimension: " << this->m_Dimension << std::endl;
  os << indent << "InputFilter: " << this->m_InputFilter->GetNameOfClass()
     << " " << this->m_InputFilter.GetPointer() << std::endl;
  os << indent << "OutputFilter: " << this->m_OutputFilter->GetNameOfClass()
     << " " << this->m_OutputFilter.GetPointer() << std::endl;
  os << indent << "SliceIndex: " << m_SliceIndex << std::endl;
}
}

#endif
