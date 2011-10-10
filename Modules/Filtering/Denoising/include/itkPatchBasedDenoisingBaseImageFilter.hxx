#ifndef __itkPatchBasedDenoisingBaseImageFilter_hxx
#define __itkPatchBasedDenoisingBaseImageFilter_hxx
#include "itkPatchBasedDenoisingBaseImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkExceptionObject.h"
#include "itkEventObject.h"

namespace itk {

template <class TInputImage, class TOutputImage>
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::PatchBasedDenoisingBaseImageFilter()
{
  m_ElapsedIterations      = 0;
  m_NumberOfIterations     = 1;
  m_DoSigmaEstimation      = true;
  m_ManualReinitialization = false;
  m_State                  = UNINITIALIZED;
}

template <class TInputImage, class TOutputImage>
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::~PatchBasedDenoisingBaseImageFilter()
{
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::SetStateToInitialized()
{
  this->SetState(INITIALIZED);
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::SetStateToUninitialized()
{
  this->SetState(UNINITIALIZED);
}

/**
 *
 */
template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingBaseImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  // copy the output requested region to the input requested region
  Superclass::GenerateInputRequestedRegion();
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  if (this->GetState() == UNINITIALIZED)
  {
    // Allocate the output image
    this->AllocateOutputs();

    // Copy the input image to the output image.  Algorithms will operate
    // directly on the output image and the update buffer.
    this->CopyInputToOutput();

    // Perform any other necessary pre-iteration initialization.
    try
    {
      this->Initialize();
    }
    catch (DataObjectError& err)
    {
      throw err;
    }

    // Allocate the internal update buffer.  This takes place entirely within
    // the subclass, since this class cannot define an update buffer type.
    this->AllocateUpdateBuffer();

    this->SetStateToInitialized();
    m_ElapsedIterations = 0;
  }

  // Iterative algorithm

  this->PreProcessInput();

  while ( ! this->Halt() )
  {
    this->InitializeIteration(); // An optional method for precalculating
                                 // global values, or otherwise setting up
                                 // for the next iteration
    if ((m_DoSigmaEstimation) &&
        (m_ElapsedIterations % m_SigmaUpdateFrequency == 0))
    {
      this->ComputeSigmaUpdate();
    }

    {
      std::ostringstream msg;
      msg << "Computing Image Update iteration " << m_ElapsedIterations
          << " of " << m_NumberOfIterations << std::endl;
      m_Logger->Write(itk::LoggerBase::INFO, msg.str());
    }
    this->ComputeImageUpdate();
    this->ApplyUpdate();
    ++m_ElapsedIterations;

    // Invoke the iteration event.
    this->InvokeEvent( IterationEvent() );
    if( this->GetAbortGenerateData() )
    {
      this->InvokeEvent( IterationEvent() );
      this->ResetPipeline();
      throw ProcessAborted(__FILE__,__LINE__);
    }
  }

  if (m_ManualReinitialization == false)
  {
    this->SetStateToUninitialized(); // Reset the state once execution is
                                     // completed
  }

  // Any further processing of the solution can be done here.
  this->PostProcessOutput();
}

template <class TInputImage, class TOutputImage>
bool
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::Halt()
{
  if (m_NumberOfIterations != 0)
    {
    this->UpdateProgress( static_cast<float>( this->GetElapsedIterations() ) /
                          static_cast<float>( m_NumberOfIterations ) );
    }

  if (this->GetElapsedIterations() >= m_NumberOfIterations)
    {
    return true;
    }
  else if ( this->GetElapsedIterations() == 0)
    {
    return false;
    }
  else
    {
    return false;
    }
}

template <class TInputImage, class TOutputImage>
void
PatchBasedDenoisingBaseImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ElapsedIterations: " << m_ElapsedIterations << std::endl;
  os << indent << "NumberOfIterations: " << m_NumberOfIterations << std::endl;
  os << indent << "SigmaUpdateFrequency: " << m_SigmaUpdateFrequency << std::endl;
  os << indent << "State: " << m_State << std::endl;
  os << indent << "ManualReinitialization: " << m_ManualReinitialization << std::endl;
  os << std::endl;
}


}// end namespace itk

#endif
