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
#ifndef __itkMultiScaleHessianBasedMeasureImageFilter_txx
#define __itkMultiScaleHessianBasedMeasureImageFilter_txx

#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{
/**
 * Constructor
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::MultiScaleHessianBasedMeasureImageFilter()
{
  m_NonNegativeHessianBasedMeasure = true;

  m_CurrentSigma = 0.0;
  m_SigmaMinimum = 0.2;
  m_SigmaMaximum = 2.0;

  m_NumberOfSigmaSteps = 10;
  m_SigmaStepMethod = Self::LogarithmicSigmaSteps;

  m_HessianFilter = NULL;
  m_HessianToMeasureFilter = NULL;

  //Instantiate Update buffer
  m_UpdateBuffer = UpdateBufferType::New();

  m_GenerateScalesOutput = false;
  m_GenerateHessianOutput = false;

  this->ProcessObject::SetNumberOfRequiredOutputs(3);
  this->ProcessObject::SetNthOutput( 1, this->MakeOutput( 1 ) );
  this->ProcessObject::SetNthOutput( 2, this->MakeOutput( 2 ) );
}

/**
 * EnlargeOutputRequestedRegion
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::EnlargeOutputRequestedRegion(DataObject *output)
{
  // While this filter requires all of it's input it can produce only
  // the requested subset
  Superclass::EnlargeOutputRequestedRegion( output );
}


/**
 * GenerateInputRequestedRegion
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::GenerateInputRequestedRegion()
  throw( InvalidRequestedRegionError )
{
  // call the superclass' implementation of this method. this should
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();

  // This filter needs all of the input
  typename InputImageType::Pointer image = const_cast< InputImageType * >( this->GetInput() );

  if ( image )
    {
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

/**
 * MakeOutput
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
typename MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >::DataObjectPointer
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::MakeOutput(unsigned int idx)
{
  // This methods is used by the constructor, and is needed whenever
  // an output image is disconnected from the pipeline

  if ( idx == 1 )
    {
    return static_cast< DataObject * >( ScalesImageType::New().GetPointer() );
    }
  if ( idx == 2 )
    {
    return static_cast< DataObject * >( HessianImageType::New().GetPointer() );
    }
  return Superclass::MakeOutput(idx);
}


/**
 *  AllocateOutputs
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::AllocateOutputs( void )
{
  // Allocate the outptu based on the requested region
  OutputImageType* outputPtr = this->GetOutput();
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  // if we are requested to generate an output image the allocate it,
  // otherwise leave it alone
  if ( m_GenerateScalesOutput )
    {
    ScalesImageType *scalesPtr = dynamic_cast< ScalesImageType * >( this->ProcessObject::GetOutput(1) );
    scalesPtr->SetBufferedRegion( scalesPtr->GetRequestedRegion() );
    scalesPtr->Allocate();
    scalesPtr->FillBuffer(0);
    }

  if ( m_GenerateHessianOutput )
    {
    HessianImageType *hessianPtr = dynamic_cast< HessianImageType * >( this->ProcessObject::GetOutput(2) );
    hessianPtr->SetBufferedRegion( hessianPtr->GetRequestedRegion() );
    hessianPtr->Allocate();

    // SymmetricSecondRankTensor is already filled with zero elements at
    // construction.
    // No strict need of filling the buffer, but we do it explicitly here to
    // make sure.
    typename HessianImageType::PixelType zeroTensor(0.0);
    hessianPtr->FillBuffer(zeroTensor);
    }

}

/**
 *  AllocateUpdateBuffer
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::AllocateUpdateBuffer()
{
  // The update buffer looks just like the output and holds the best response
  // in the  objectness measure with double precision

  typename TOutputImage::Pointer output = this->GetOutput();

  // this copies meta data describing the output such as origin,
  // spacing and the largest region
  m_UpdateBuffer->CopyInformation(output);

  m_UpdateBuffer->SetRequestedRegion( output->GetRequestedRegion() );
  m_UpdateBuffer->SetBufferedRegion( output->GetBufferedRegion() );
  m_UpdateBuffer->Allocate();

  // Update buffer is used for > comparisons so make it really really small,
  // just to be sure. Thanks to Hauke Heibel.
  if ( m_NonNegativeHessianBasedMeasure )
    {
    m_UpdateBuffer->FillBuffer(itk::NumericTraits< BufferValueType >::Zero);
    }
  else
    {
    m_UpdateBuffer->FillBuffer( itk::NumericTraits< BufferValueType >::NonpositiveMin() );
    }
}

/**
 * GenerateData
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::GenerateData()
{

  // verify inputs
  if ( m_HessianToMeasureFilter.IsNull() )
    {
    itkExceptionMacro(" HessianToMeasure filter is not set. Use SetHessianToMeasureFilter() ");
    }

  this->AllocateOutputs();

  // Allocate the update buffer
  this->AllocateUpdateBuffer();


  typename InputImageType::ConstPointer input = this->GetInput();

  // allocate filter here to minimize extra memory stored internally
  this->m_HessianFilter = HessianFilterType::New();
  this->m_HessianFilter->SetInput(input);
  this->m_HessianFilter->SetNormalizeAcrossScale(true);
  if ( !m_GenerateHessianOutput )
    {
    this->m_HessianFilter->ReleaseDataFlagOn();
    }


  // Create a process accumulator for tracking the progress of this
  // minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // prevent a divide by zero
  if ( m_NumberOfSigmaSteps > 0 )
    {
    progress->RegisterInternalFilter(this->m_HessianFilter, .5 / m_NumberOfSigmaSteps);
    progress->RegisterInternalFilter(this->m_HessianToMeasureFilter, .5 / m_NumberOfSigmaSteps);
    }

  m_CurrentSigma = m_SigmaMinimum;

  int scaleLevel = 1;

  while ( scaleLevel <= m_NumberOfSigmaSteps )
    {

    std::cout << "Computing measure for scale with sigma = " << m_CurrentSigma << std::endl;

    m_HessianFilter->SetSigma(m_CurrentSigma);

    m_HessianToMeasureFilter->SetInput ( m_HessianFilter->GetOutput() );
    m_HessianToMeasureFilter->GetOutput()->SetRequestedRegion( this->GetOutput()->GetRequestedRegion() );
    m_HessianToMeasureFilter->Update();


    ////////////////////////////////////
    // Set up the multithreaded processing of coping the buffers
    typename Superclass::ThreadStruct str;
    str.Filter = this;

    this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
    this->GetMultiThreader()->SetSingleMethod( Self::ThreaderCallback, &str );

    this->GetMultiThreader()->SingleMethodExecute();
    ////////////

    m_CurrentSigma  = this->ComputeSigmaValue(scaleLevel);

    scaleLevel++;

    // reset the progress accumulator after each pass to continue
    // addtion of progress for the next pass
    progress->ResetFilterProgressAndKeepAccumulatedProgress();

    }


  // Copy the best response to the output image
  // A cast filter is used to perform the operation multithreaded, and
  // it the buffer type is the same as the output type then filter
  // will run in-place

  typedef itk::CastImageFilter< UpdateBufferType, TOutputImage > CopyImageFilter;
  typename CopyImageFilter::Pointer copyFilter = CopyImageFilter::New();
  copyFilter->SetInput( this->m_UpdateBuffer );
  copyFilter->InPlaceOn();

  // Perform standard graft-update-graft
  //
  // This saves memory, and copies the requested region to the filter
  // so that copy filter updates the correct requested region
  copyFilter->GraftOutput( this->GetOutput() );
  copyFilter->Update();
  this->GraftOutput( copyFilter->GetOutput() );

  // Because the UpdateBuffer is not connected to a pipeline the
  // largest region will be modified, set it to the original
  this->GetOutput()->SetLargestPossibleRegion( input->GetLargestPossibleRegion() );

  // Release intermediate data, and filters
  this->m_UpdateBuffer->ReleaseData();
  this->m_HessianFilter->GetOutput()->ReleaseData();
  this->m_HessianFilter = NULL;
  this->m_HessianToMeasureFilter->GetOutput()->ReleaseData();
}


/**
 * ThreadedGenerateData
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::ThreadedGenerateData(const OutputImageRegionType &outputRegionForThread, int threadId )
{
  ImageRegionIterator< UpdateBufferType > oit(m_UpdateBuffer, outputRegionForThread);

  // construct optional output iterator for scale
  typename ScalesImageType::Pointer scalesImage = static_cast< ScalesImageType * >( this->ProcessObject::GetOutput(1) );
  ImageRegionIterator< ScalesImageType > osit;

  // construct optional output iterator for hessian
  typename HessianImageType::Pointer hessianImage = static_cast< HessianImageType * >( this->ProcessObject::GetOutput(2) );
  ImageRegionIterator< HessianImageType > ohit;

  oit.GoToBegin();
  if ( m_GenerateScalesOutput )
    {
    osit = ImageRegionIterator< ScalesImageType >(scalesImage, outputRegionForThread);
    osit.GoToBegin();
    }
  if ( m_GenerateHessianOutput )
    {
    ohit = ImageRegionIterator< HessianImageType >(hessianImage, outputRegionForThread);
    ohit.GoToBegin();
    }

  typedef typename HessianToMeasureFilterType::OutputImageType HessianToMeasureOutputImageType;

  // intialize input iterators
  ImageRegionIterator< HessianToMeasureOutputImageType > it(m_HessianToMeasureFilter->GetOutput(), outputRegionForThread);
  ImageRegionIterator< HessianImageType >                hit;

  if ( m_GenerateHessianOutput )
    {
    hit = ImageRegionIterator< HessianImageType >(m_HessianFilter->GetOutput(), outputRegionForThread);
    }

  it.GoToBegin();
  hit.GoToBegin();

  // Copy measure to output if only it's greater then existing.
  // Also conditinally copy data to other outputs.
  while ( !oit.IsAtEnd() )
    {
    if ( oit.Value() < it.Value() )
      {
      oit.Value() = it.Value();
      if ( m_GenerateScalesOutput )
        {
        osit.Value() = static_cast< ScalesPixelType >( m_CurrentSigma );
        ++osit;
        }
      if ( m_GenerateHessianOutput )
        {
        ohit.Value() = hit.Value();
      ++ohit;
      ++hit;
        }
      }
    ++oit;
    ++it;
    }
}

/**
 * ComputeSigmaValue
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
double
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::ComputeSigmaValue(int scaleLevel)
{
  double sigmaValue;

  if ( m_NumberOfSigmaSteps < 2 )
    {
    return m_SigmaMinimum;
    }

  switch ( m_SigmaStepMethod )
    {
    case Self::EquispacedSigmaSteps:
      {
      const double stepSize = vnl_math_max( 1e-10, ( m_SigmaMaximum - m_SigmaMinimum ) / ( m_NumberOfSigmaSteps - 1 ) );
      sigmaValue = m_SigmaMinimum + stepSize * scaleLevel;
      break;
      }
    case Self::LogarithmicSigmaSteps:
      {
      const double stepSize =
        vnl_math_max( 1e-10, ( vcl_log(m_SigmaMaximum) - vcl_log(m_SigmaMinimum) ) / ( m_NumberOfSigmaSteps - 1 ) );
      sigmaValue = vcl_exp(vcl_log (m_SigmaMinimum) + stepSize * scaleLevel);
      break;
      }
    default:
      itkExceptionMacro( "Invalid SigmaStepMethod." );
      break;
    }

  return sigmaValue;
}

/**
 * SetSigmaStepMethodToEquispaced
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::SetSigmaStepMethodToEquispaced()
{
  this->SetSigmaStepMethod(Self::EquispacedSigmaSteps);
}


/**
 * SetSigmaStepMethodToLogarithmic
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::SetSigmaStepMethodToLogarithmic()
{
  this->SetSigmaStepMethod(Self::LogarithmicSigmaSteps);
}

/** Get the image containing the Hessian at which each pixel gave the
 * best response
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
const
typename MultiScaleHessianBasedMeasureImageFilter< TInputImage, THessianImage, TOutputImage, THessianFilterType >::HessianImageType *
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::GetHessianOutput() const
{
  return static_cast< const HessianImageType * >( this->ProcessObject::GetOutput(2) );
}

/** Get the image containing the scales at which each pixel gave the
 * best response
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
const
typename MultiScaleHessianBasedMeasureImageFilter< TInputImage, THessianImage, TOutputImage, THessianFilterType >::ScalesImageType *
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::GetScalesOutput() const
{
  return static_cast< const ScalesImageType * >( this->ProcessObject::GetOutput(1) );
}

/**
 * PrintSelf
 */
template< typename TInputImage,
          typename THessianImage,
          typename TOutputImage,
          typename THessianFilterType >
void
MultiScaleHessianBasedMeasureImageFilter
< TInputImage, THessianImage, TOutputImage, THessianFilterType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "CurrentSigma: " << m_CurrentSigma << std::endl;
  os << indent << "SigmaMinimum:  " << m_SigmaMinimum << std::endl;
  os << indent << "SigmaMaximum:  " << m_SigmaMaximum  << std::endl;
  os << indent << "NumberOfSigmaSteps:  " << m_NumberOfSigmaSteps  << std::endl;
  os << indent << "SigmaStepMethod:  " << m_SigmaStepMethod  << std::endl;
  os << indent << "HessianToMeasureFilter: " << m_HessianToMeasureFilter << std::endl;
  os << indent << "NonNegativeHessianBasedMeasure:  " << m_NonNegativeHessianBasedMeasure << std::endl;
  os << indent << "GenerateScalesOutput: " << m_GenerateScalesOutput << std::endl;
  os << indent << "GenerateHessianOutput: " << m_GenerateHessianOutput << std::endl;
}
} // end namespace itk

#endif
