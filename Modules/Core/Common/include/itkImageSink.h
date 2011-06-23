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

#ifndef __itkImageSink_h
#define __itkImageSink_h

#include "itkStreamingProcessObject.h"
#include "itkImage.h"
#include "itkImageRegionSplitter.h"

namespace itk
{

/** \class ImageSink
*
* \todo Write somethng useful here :)
*
* \ingroup ITKSystemObjects
* \ingroup DataProcessing
* \ingroup Streamed
* \ingroup ITK-Common
**/
template <class TInputImage >
class ITK_EXPORT ImageSink
  : public StreamingProcessObject
{
public:
  /** Standard class typedefs. */
  typedef ImageSink                                      Self;
  typedef ProcessObject                                  Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( ImageSink, StreamingProcessObject );

  /** Smart Pointer type to a DataObject. */
  typedef DataObject::Pointer DataObjectPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::Pointer    InputImagePointer;
  typedef typename InputImageType::RegionType InputImageRegionType;
  typedef typename InputImageType::PixelType  InputImagePixelType;

  /** SmartPointer to a region splitting object */
  typedef ImageRegionSplitter< InputImageType::ImageDimension > SplitterType;
  typedef typename SplitterType::Pointer RegionSplitterPointer;

  /** Dimension of input images. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  /** Set/Get the image input of this process object.  */
  virtual void SetInput(const TInputImage *input);
  const InputImageType * GetInput(void) const;

  /** Set/Get the nth data input of this filter */
  virtual void SetInput(unsigned int, const TInputImage *image);
  const InputImageType *GetInput(unsigned int idx) const;


  virtual void Update( );

protected:
  ImageSink();
  // ~ImageSink() {} // use default implementation

  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  virtual unsigned int GetNumberOfInputRequestedRegions (void);

  virtual void GenerateNthInputRequestedRegion (unsigned int inputRequestedRegionNumber);

  virtual void StreamedGenerateData( unsigned int  inputRequestedRegionNumber);

  virtual void ThreadedStreamedGenerateData( const InputImageRegionType &inputRegionForChunk, ThreadIdType ) = 0;


  /** Set the number of pieces to divide the input.  The upstream pipeline
   * will be executed this many times. */
  itkSetMacro(NumberOfStreamDivisions, unsigned int);

  /** Get the number of pieces to divide the input. The upstream pipeline
    * will be executed this many times. */
  itkGetConstReferenceMacro(NumberOfStreamDivisions, unsigned int);

  /** Set/Get the helper class for dividing the input into chunks. */
  itkSetObjectMacro(RegionSplitter, SplitterType);
  itkGetObjectMacro(RegionSplitter, SplitterType);

  /** Static function used as a "callback" by the MultiThreader.  The threading
   * library will call this routine for each thread, which will delegate the
   * control to ThreadedGenerateData(). */
  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void *arg);

   /** Internal structure used for passing image data into the threading library
    */
   struct ThreadStruct {
     Pointer Filter;
     InputImageRegionType currentInputRegion;
  };

private:
  ImageSink(const ImageSink &); // purposely not implemented
  ImageSink &operator=(const ImageSink &); // purposely not implemented

  unsigned int          m_NumberOfStreamDivisions;
  RegionSplitterPointer m_RegionSplitter;
  InputImageRegionType  m_CurrentInputRegion;
};

} // end namespace itk


#if ITK_TEMPLATE_TXX
#include "itkImageSink.txx"
#endif


#endif // __itkImageSink_h
