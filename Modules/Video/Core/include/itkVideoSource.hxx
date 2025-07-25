/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkVideoSource_hxx
#define itkVideoSource_hxx

#include "itkMultiThreaderBase.h"

namespace itk
{

//-CONSTRUCTOR PRINT-----------------------------------------------------------

//
// Constructor
//
template <typename TOutputVideoStream>
VideoSource<TOutputVideoStream>::VideoSource()
{
  const typename OutputVideoStreamType::Pointer output =
    static_cast<OutputVideoStreamType *>(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
}

//
// PrintSelf
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

//-PUBLIC METHODS--------------------------------------------------------------

//
// GetOutput()
//
template <typename TOutputVideoStream>
auto
VideoSource<TOutputVideoStream>::GetOutput() -> OutputVideoStreamType *
{
  // Make sure there is at least 1 output
  if (this->GetNumberOfOutputs() < 1)
  {
    itkWarningMacro("No outputs set");
    return nullptr;
  }

  // Return the output
  return this->GetOutput(0);
}

//
// GetOutput(idx)
//
template <typename TOutputVideoStream>
TOutputVideoStream *
VideoSource<TOutputVideoStream>::GetOutput(unsigned int idx)
{
  auto * out = dynamic_cast<OutputVideoStreamType *>(this->TemporalProcessObject::GetOutput(idx));

  // Make sure there is at least 1 output
  if (out == nullptr)
  {
    itkWarningMacro("dynamic_cast to output type failed");
  }

  return out;
}

//
// GraftOutput
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::GraftOutput(TOutputVideoStream * graft)
{
  this->GraftNthOutput(0, graft);
}

//
// GraftNthOutput
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::GraftNthOutput(unsigned int idx, TOutputVideoStream * graft)
{
  if (idx >= this->GetNumberOfOutputs())
  {
    itkExceptionMacro("Requested to graft output " << idx << " but this VideoSource only has "
                                                   << this->GetNumberOfOutputs() << " Outputs.");
  }
  if (!graft)
  {
    itkExceptionMacro("Cannot graft from a nullptr pointer");
  }

  // we use the process object method since all our outputs may not be of the
  // same type
  DataObject * output = this->ProcessObject::GetOutput(idx);
  output->Graft(graft);
}

//
// MakeOutput
//
template <typename TOutputVideoStream>
DataObject::Pointer
VideoSource<TOutputVideoStream>::MakeOutput(DataObjectPointerArraySizeType itkNotUsed(idx))
{
  return OutputVideoStreamType::New().GetPointer();
}

//-PROTECTED METHODS-----------------------------------------------------------

//
// GenerateOutputRequestedTemporalRegion
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::GenerateOutputRequestedTemporalRegion(TemporalDataObject * output)
{
  // Check if requested temporal region unset
  bool                 resetNumFrames = false;
  const TemporalRegion outputRequest = output->GetRequestedTemporalRegion();

  if (!outputRequest.GetFrameDuration())
  {
    resetNumFrames = true;
  }

  // Call superclass's version - this will set the requested temporal region
  Superclass::GenerateOutputRequestedTemporalRegion(this->GetOutput());

  // Make sure the output has enough buffers available for the entire output
  // only if this request has just been matched to the largest possible spatial
  // region. This should only happen for filters at the end of the pipeline
  // since mid-pipeline filters will have their outputs' requested temporal
  // regions set automatically.
  const SizeValueType requestDuration = this->GetOutput()->GetRequestedTemporalRegion().GetFrameDuration();
  if (resetNumFrames && this->GetOutput()->GetNumberOfBuffers() < requestDuration)
  {
    this->GetOutput()->SetNumberOfBuffers(requestDuration);
  }

  // If requested temporal region was just set to largest possible, set the
  // spatial regions for every frame to the largest possible as well
  if (resetNumFrames)
  {
    const SizeValueType frameStart = this->GetOutput()->GetRequestedTemporalRegion().GetFrameStart();
    const SizeValueType numFrames = this->GetOutput()->GetRequestedTemporalRegion().GetFrameDuration();
    for (SizeValueType i = frameStart; i < frameStart + numFrames; ++i)
    {
      // this->GetOutput()->SetFrameRequestedSpatialRegion(i,
      //  this->GetOutput()->GetFrameLargestPossibleSpatialRegion(i));
      OutputVideoStreamType * out = this->GetOutput();
      out->GetFrameLargestPossibleSpatialRegion(i);
    }
  }
}

//
// AllocateOutputs
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::AllocateOutputs()
{
  // Get the output
  OutputVideoStreamType * output = this->GetOutput();

  // Get a list of unbuffered requested frames
  const TemporalRegion unbufferedRegion = output->GetUnbufferedRequestedTemporalRegion();

  // We don't touch the buffered temporal region here because that is handled
  // by the default implementation of GenerateData in TemporalProcessObject

  // If there are no unbuffered frames, return now
  const SizeValueType numFrames = unbufferedRegion.GetFrameDuration();

  if (numFrames == 0)
  {
    return;
  }

  // Initialize any empty frames (which will set region values from cache)
  output->InitializeEmptyFrames();

  // Loop through the unbuffered frames and set the buffered spatial region to
  // match the requested spatial region then allocate the data
  const SizeValueType startFrame = unbufferedRegion.GetFrameStart();
  for (SizeValueType i = startFrame; i < startFrame + numFrames; ++i)
  {
    output->SetFrameBufferedSpatialRegion(i, output->GetFrameRequestedSpatialRegion(i));
    const typename OutputFrameType::Pointer frame = output->GetFrame(i);
    frame->SetBufferedRegion(output->GetFrameRequestedSpatialRegion(i));
    frame->Allocate();
  }
}

//
// TemporalStreamingGenerateData
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::TemporalStreamingGenerateData()
{
  // Call a method that can be overridden by a subclass to allocate
  // memory for the filter's outputs
  this->AllocateOutputs();

  // Call a method that can be overridden by a subclass to perform
  // some calculations prior to splitting the main computations into
  // separate threads
  this->BeforeThreadedGenerateData();

  // Set up the multithreaded processing
  ThreadStruct str;
  str.Filter = this;

  this->GetMultiThreader()->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());
  this->GetMultiThreader()->SetSingleMethodAndExecute(this->ThreaderCallback, &str);

  // Call a method that can be overridden by a subclass to perform
  // some calculations after all the threads have completed
  this->AfterThreadedGenerateData();
}

//
// ThreadedGenerateData
//
template <typename TOutputVideoStream>
void
VideoSource<TOutputVideoStream>::ThreadedGenerateData(
  const typename TOutputVideoStream::SpatialRegionType & itkNotUsed(outputRegionForThread),
  int                                                    itkNotUsed(threadId))
{
  itkExceptionMacro("itk::ERROR: " << this->GetNameOfClass() << '(' << this << "): "
                                   << "Subclass should override this method!!!");
}

//
// SplitRequestedSpatialRegion -- Copied mostly from ImageSource
//
// Note: This implementation bases the spatial region split on the requested
// spatial region for the current Head frame of the output. This could
// potentially cause issues if frames are different sized.
//
template <typename TOutputVideoStream>
int
VideoSource<TOutputVideoStream>::SplitRequestedSpatialRegion(
  int                                              i,
  int                                              num,
  typename TOutputVideoStream::SpatialRegionType & splitRegion)
{
  // Get the output pointer and a pointer to the first output frame
  OutputVideoStreamType * outputPtr = this->GetOutput();
  const SizeValueType     currentFrame = outputPtr->GetRequestedTemporalRegion().GetFrameStart();
  OutputFrameType *       framePtr = outputPtr->GetFrame(currentFrame);

  const typename TOutputVideoStream::SizeType & requestedRegionSize = framePtr->GetRequestedRegion().GetSize();

  // Initialize the splitRegion to the output requested region
  splitRegion = framePtr->GetRequestedRegion();
  typename TOutputVideoStream::IndexType splitIndex = splitRegion.GetIndex();
  typename TOutputVideoStream::SizeType  splitSize = splitRegion.GetSize();

  // split on the outermost dimension available
  int splitAxis = framePtr->GetImageDimension() - 1;
  while (requestedRegionSize[splitAxis] == 1)
  {
    --splitAxis;
    if (splitAxis < 0)
    { // cannot split
      itkDebugMacro("  Cannot Split");
      return 1;
    }
  }

  // determine the actual number of pieces that will be generated
  const typename TOutputVideoStream::SizeType::SizeValueType range = requestedRegionSize[splitAxis];

  int valuesPerThread = 0;
  int maxThreadIdUsed = 0;
  if (range != 0)
  {
    valuesPerThread = Math::Ceil<int>(range / static_cast<double>(num));
    maxThreadIdUsed = Math::Ceil<int>(range / static_cast<double>(valuesPerThread)) - 1;
  }

  // Split the region
  if (i < maxThreadIdUsed)
  {
    splitIndex[splitAxis] += i * valuesPerThread;
    splitSize[splitAxis] = valuesPerThread;
  }
  if (i == maxThreadIdUsed)
  {
    splitIndex[splitAxis] += i * valuesPerThread;
    // last thread needs to process the "rest" dimension being split
    splitSize[splitAxis] = splitSize[splitAxis] - i * valuesPerThread;
  }

  // set the split region ivars
  splitRegion.SetIndex(splitIndex);
  splitRegion.SetSize(splitSize);

  itkDebugMacro("  Split Piece: " << splitRegion);

  return maxThreadIdUsed + 1;
}

//
// ThreaderCallback -- Copied from ImageSource
//
template <typename TOutputVideoStream>
ITK_THREAD_RETURN_FUNCTION_CALL_CONVENTION
VideoSource<TOutputVideoStream>::ThreaderCallback(void * arg)
{
  const int workUnitID = (static_cast<MultiThreaderBase::WorkUnitInfo *>(arg))->WorkUnitID;
  const int threadCount = (static_cast<MultiThreaderBase::WorkUnitInfo *>(arg))->NumberOfWorkUnits;

  auto * str = (ThreadStruct *)((static_cast<MultiThreaderBase::WorkUnitInfo *>(arg))->UserData);

  // execute the actual method with appropriate output region
  // first find out how many pieces extent can be split into.
  typename TOutputVideoStream::SpatialRegionType splitRegion;
  const int total = str->Filter->SplitRequestedSpatialRegion(workUnitID, threadCount, splitRegion);

  if (workUnitID < total)
  {
    str->Filter->ThreadedGenerateData(splitRegion, workUnitID);
  }
  // else
  //   {
  //   otherwise don't use this thread. Sometimes the threads don't
  //   break up very well and it is just as efficient to leave a
  //   few threads idle.
  //   }

  return ITK_THREAD_RETURN_DEFAULT_VALUE;
}

} // end namespace itk

#endif
