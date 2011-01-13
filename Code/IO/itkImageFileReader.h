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
#ifndef __itkImageFileReader_h
#define __itkImageFileReader_h

#include "itkPlainImageFileReader.h"

namespace itk
{

/** \brief Data source that reads image data from a single file.
 *
 * This source object is a general filter to read data from
 * a variety of file formats. It works with a ImageIOBase subclass
 * to actually do the reading of the data. Object factory machinery
 * can be used to automatically create the ImageIOBase, or the
 * ImageIOBase can be manually created and set. Note that this
 * class reads data from a single file; if you wish to read data
 * from a series of files use ImageSeriesReader.
 *
 * TOutputImage is the type expected by the external users of the
 * filter. If data stored in the file is stored in a different format
 * then specified by TOutputImage, than this filter converts data
 * between the file type and the external expected type.  The
 * ConvertTraits template argument is used to do the conversion.
 *
 * A Pluggable factory pattern is used this allows different kinds of readers
 * to be registered (even at run time) without having to modify the
 * code in this class. Normally just setting the FileName with the
 * appropriate suffix is enough to get the reader to instantiate the
 * correct ImageIO and read the file properly. However, some files (like
 * raw binary format) have no accepted suffix, so you will have to
 * manually create the ImageIO instance of the write type.
 *
 * \sa ImageSeriesReader
 * \sa ImageIOBase
 *
 * \ingroup IOFilters
 *
 */
/** \class ImageFileReader
 */
template< class TOutputImage,
          class ConvertPixelTraits = DefaultConvertPixelTraits<
            ITK_TYPENAME TOutputImage::IOPixelType > >
class ITK_EXPORT ImageFileReader:public PlainImageFileReader< TOutputImage, ConvertPixelTraits >
{
public:
  /** Standard class typedefs. */
  typedef ImageFileReader                                           Self;
  typedef PlainImageFileReader< TOutputImage, ConvertPixelTraits >  Superclass;
  typedef SmartPointer< Self >                                      Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageFileReader, ImageSource);

  /** The size of the output image. */
  typedef typename Superclass::SizeType SizeType;

  /** The size of the output image. */
  typedef typename Superclass::IndexType IndexType;

  /** The region of the output image. */
  typedef typename Superclass::ImageRegionType ImageRegionType;

  /** The pixel type of the output image. */
  typedef typename Superclass::OutputImagePixelType OutputImagePixelType;

protected:
  ImageFileReader();
  ~ImageFileReader();
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Attempt to create an ImageIO through the factory mechanism,
   *  based on the current filename. This method will throw an
   *  Exception if no ImageIO can be found.  */
  virtual ImageIOBase::Pointer CreateImageIOThroughTheFactory();

private:
  ImageFileReader(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented

  std::string m_ExceptionMessage;
};
} //namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageFileReader.txx"
#endif

#endif // __itkImageFileReader_h
