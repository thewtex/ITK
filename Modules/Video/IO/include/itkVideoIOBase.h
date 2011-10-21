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
#ifndef __itkVideoIOBase_h
#define __itkVideoIOBase_h

#include "itkImageIOBase.h"
#include "itkExceptionObject.h"
#include "vnl/vnl_vector.h"

#include <string>

namespace itk
{
/** \class VideoIOBase
 * \brief Abstract superclass defines video IO interface.
 *
 * VideoIOBase is a class that reads and/or writes video data
 * using a particular external technique or external library (OpenCV, vxl). The
 * VideoIOBase encapsulates both the reading and writing of data. The
 * VideoIOBase is used by the VideoReader class (to read data)
 * and the VideoWriter (to write data). Normally the user does not directly
 * manipulate this class directly.
 *
 * A Pluggable factory pattern is used. This allows different kinds of
 * readers to be registered (even at run time) without having to
 * modify the code in this class.
 *
 * Note that when instantiable children override CanReadFile() and/or
 * CanWriteFile(), the children must handle the possible occurance of some
 * device ID used in lieu of a file name.  (The only current form of valid
 * device ID is a camera URL, e.g. camera://opencv/0 for the first OpenCV
 * camera.)  VideoIOBase provides helper functions to assist with this, named
 * FileNameIsDeviceID() and FileNameIsCamera(), which return true, respectively,
 * if the given file name is actually a device ID or (more restrictively) a
 * camera identifier.
 *
 * \sa VideoWriter
 * \sa VideoReader
 *
 * \ingroup ITKVideoIO
 */
class ITK_EXPORT VideoIOBase : public ImageIOBase
{

public:
  /** Standard class typedefs. */
  typedef VideoIOBase          Self;
  typedef ImageIOBase          Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef ::itk::SizeValueType SizeValueType;

  /** Frame offset typedefs */
  typedef double               TemporalOffsetType;
  typedef SizeValueType        FrameOffsetType;
  typedef double               TemporalRatioType;

  /** Video-specific typedefs */
  typedef unsigned long        CameraIDType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VideoIOBase, Superclass);

  /** Close the reader and writer and reset members */
  virtual void FinishReadingOrWriting() = 0;

  /*-------- This part of the interface deals with reading data. ------ */

  /** Test whether or not a given file name is actually some sort of a device
   * ID, such as a camera identifer or some other (future potential) sort of
   * streaming device.  Currently this just calls this->FileNameIsCamera(). */
  inline bool FileNameIsDeviceID(const std::string strFileName)
    {
    return this->FileNameIsCamera(strFileName);
    }
  inline bool FileNameIsDeviceID(const char* FileName)
    {
    std::string strFileName = FileName;
    return this->FileNameIsDeviceID(strFileName);
    }

  /** Test whether or not a given file name is actually a camera identifer.
   * Currently, this just tests whether the first 9 characters of the name are
   * camera:// as specified for camera URLs:
   * camera://[interface name]/[camera identifer]?parm1=val1&...&parmn=valn */
  inline bool FileNameIsCamera(const std::string strFileName)
    {
    return 0 == strFileName.compare(0, 9, "camera://");
    }
  inline bool FileNameIsCamera(const char* FileName)
    {
    std::string strFileName = FileName;
    return this->FileNameIsCamera(strFileName);
    }

  /** Set the next frame that should be read. Return true if your operation
   * was succesful */
  virtual bool SetNextFrameToRead( FrameOffsetType frameNumber ) = 0;

  /** Virtual accessor functions to be implemented in each derived class */
  virtual TemporalOffsetType GetPositionInMSec() const = 0;
  virtual TemporalRatioType GetRatio() const = 0;
  virtual FrameOffsetType GetFrameTotal() const = 0;
  virtual TemporalRatioType GetFramesPerSecond() const = 0;
  virtual FrameOffsetType GetCurrentFrame() const = 0;
  virtual FrameOffsetType GetLastIFrame() const = 0;

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Set Writer Parameters */
  virtual void SetWriterParameters( TemporalRatioType framesPerSecond,
                                    const std::vector<SizeValueType>& dim,
                                    const char* fourCC,
                                    unsigned int nChannels,
                                    IOComponentType componentType) = 0;

protected:

  VideoIOBase();
  virtual ~VideoIOBase();

  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Member Variables */
  TemporalRatioType  m_FramesPerSecond;
  FrameOffsetType    m_FrameTotal;
  FrameOffsetType    m_CurrentFrame;
  FrameOffsetType    m_IFrameInterval;
  FrameOffsetType    m_LastIFrame;
  TemporalRatioType  m_Ratio;
  TemporalOffsetType m_PositionInMSec;
  bool               m_WriterOpen;
  bool               m_ReaderOpen;

private:
  VideoIOBase(const Self &);    //purposely not implemented
  void operator=(const Self &); //purposely not implemented

};

} // end namespace itk

#endif // __itkVideoIOBase_h
