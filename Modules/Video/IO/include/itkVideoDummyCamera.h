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
#ifndef __itkVideoDummyCamera_h
#define __itkVideoDummyCamera_h

// Define support for FileListVideo
#ifndef ITK_VIDEO_USE_FILE_LIST
#define ITK_VIDEO_USE_FILE_LIST
#endif

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkVideoCaptureBase.h"

namespace itk
{
/** \class VideoDummyCamera
 *
 * \brief VideoIO object for grabbing synthetically generated frames from
 * a simulated video grabber.
 *
 * The camera URL "filename" for this device is:
 *
 *   camera://dummy/{solidgray,solidcolor,...}?[standard parameters]&...
 * ____________________________________________________________________________
 *
 * solidgray:  camera://dummy/solidgray?[standard parameters]&[gray=00].
 *
 * The solidgray dummy camera always returns the same blank image, which by
 * default is black but can be changed via the optional gray attribute, which
 * can be set to an arbitrary 8-bit hex value using 2-digit hex notation.
 * For example, camera://dummy/solidgray?gray=ff would return pure white
 * 320x240 images at 1 fps with a spacing of 1 in all directions.
 * ____________________________________________________________________________
 *
 * solidcolor:  camera://dummy/solidcolor?[standard parameters]&[color=000000].
 *
 * The solid dummy camera always returns the same blank image, which by default
 * is black but can be changed via the optional color attribute, which can be
 * set to an arbitrary 24-bit RGB color using 6-digit rrggbb hex notation.
 * For example, camera://dummy/solid?color=ff0000 would return pure, bright red
 * 320x240 images at 1 fps with a spacing of 1 in all directions.
 * ____________________________________________________________________________
 *
 * Note: Many of the above default values are coded as private constants in the
 *       parent classes.
 *
 * \sa itkVideoIOBase
 * \sa itkVideoCaptureBase
 *
 * \ingroup ITKVideoIO
 *
 */
class ITK_EXPORT VideoDummyCamera : public VideoCaptureBase
{
public:
  /** Standard class typedefs. */
  typedef VideoDummyCamera     Self;
  typedef VideoCaptureBase     Superclass;
  typedef SmartPointer< Self > Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VideoDummyCamera, Superclass);

  /** Close the reader and writer and reset members */  // required by VideoIOBase
  virtual void FinishReadingOrWriting()
  {
    this->ResetMembers();
  }

  /** Set Writer Parameters */  // required by VideoIOBase
  virtual void SetWriterParameters(double itkNotUsed(fps),
                                   const std::vector<SizeValueType>& itkNotUsed(dim),
                                   const char* itkNotUsed(fourCC),
                                   unsigned int itkNotUsed(nChannels),
                                   IOComponentType itkNotUsed(componentType) ){
    itkExceptionMacro(<< "You should never set writer parameters for a camera:// "
                      << "device.  Future API should remove this function from here.");
    return;
  }

  /** Set the next frame that should be read. Return true if you operation succesful */  // required by VideoIOBase
  virtual bool SetNextFrameToRead(FrameOffsetType frameNumber){
    itkExceptionMacro(<< "Video grabbers, such as camera:// devices, do not necessarily support skipping frames (especially backwards): "<<frameNumber);
    return false;
  }

  /*-------- This part of the interface deals with reading data. ------ */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified.  (Returns false for camera identifiers such as camera:// URLs.) */
  virtual bool CanReadFile(const char *);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void *buffer);

protected:
  VideoDummyCamera();
  ~VideoDummyCamera();

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  VideoDummyCamera(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented

  /** Member Variables */
  std::vector<unsigned char> m_BackgroundValue; // stored as raw bytes

};
} // end namespace itk

#endif // __itkVideoDummyCamera_h
