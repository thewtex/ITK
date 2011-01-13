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
#ifndef __itkImageFileWriter_h
#define __itkImageFileWriter_h

#include "itkPlainImageFileWriter.h"

namespace itk
{

/** \class ImageFileWriter
 * \brief Writes image data to a single file.
 *
 * ImageFileWriter writes its input data to a single output file.
 * ImageFileWriter interfaces with an ImageIO class to write out the
 * data. If you wish to write data into a series of files (e.g., a
 * slice per file) use ImageSeriesWriter.
 *
 * A pluggable factory pattern is used that allows different kinds of writers
 * to be registered (even at run time) without having to modify the
 * code in this class. You can either manually instantiate the ImageIO
 * object and associate it with the ImageFileWriter, or let the class
 * figure it out from the extension. Normally just setting the filename
 * with a suitable suffix (".png", ".jpg", etc) and setting the input
 * to the writer is enough to get the writer to work properly.
 *
 * \sa ImageSeriesReader
 * \sa ImageIOBase
 *
 * \ingroup IOFilters
 */
template< class TInputImage >
class ITK_EXPORT ImageFileWriter : public PlainImageFileWriter< TInputImage >
{
public:
  /** Standard class typedefs. */
  typedef ImageFileWriter                       Self;
  typedef PlainImageFileWriter< TInputImage >   Superclass;
  typedef SmartPointer< Self >                  Pointer;
  typedef SmartPointer< const Self >            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageFileWriter, PlainImageFileWriter);

  /** Some convenient typedefs. */
  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::Pointer    InputImagePointer;
  typedef typename InputImageType::RegionType InputImageRegionType;
  typedef typename InputImageType::PixelType  InputImagePixelType;

protected:
  ImageFileWriter();
  ~ImageFileWriter();
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Attempt to create an ImageIO through the factory mechanism,
   *  based on the current filename. This method will throw an
   *  Exception if no ImageIO can be found.  */
  virtual ImageIOBase::Pointer CreateImageIOThroughTheFactory();

private:
  ImageFileWriter(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented

  std::string m_ExceptionMessage;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageFileWriter.txx"
#endif

#endif // __itkImageFileWriter_h
