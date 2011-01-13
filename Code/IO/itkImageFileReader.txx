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
#ifndef __itkImageFileReader_txx
#define __itkImageFileReader_txx

#include "itkImageFileReader.h"

#include "itkObjectFactory.h"
#include "itkImageIOFactory.h"

namespace itk
{
template< class TOutputImage, class ConvertPixelTraits >
ImageFileReader< TOutputImage, ConvertPixelTraits >
::ImageFileReader()
{
}

template< class TOutputImage, class ConvertPixelTraits >
ImageFileReader< TOutputImage, ConvertPixelTraits >
::~ImageFileReader()
{}

template< class TOutputImage, class ConvertPixelTraits >
void ImageFileReader< TOutputImage, ConvertPixelTraits >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/* This method will throw an exception if no ImageIO can be created for the
 * current filename. */
template< class TOutputImage, class ConvertPixelTraits >
ImageIOBase::Pointer
ImageFileReader< TOutputImage, ConvertPixelTraits >
::CreateImageIOThroughTheFactory()
{
  std::string filename = this->GetFileName();

  ImageIOBase::Pointer imageIO = ImageIOFactory::CreateImageIO(filename.c_str(), ImageIOFactory::ReadMode);

  if ( imageIO.IsNull() )
    {
    std::ostringstream msg;
    msg << " Could not create IO object for file "
        << filename.c_str() << std::endl;
    if ( m_ExceptionMessage.size() )
      {
      msg << m_ExceptionMessage;
      }
    else
      {
      msg << "  Tried to create one of the following:" << std::endl;
      std::list< LightObject::Pointer > allobjects =
        ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
      for ( std::list< LightObject::Pointer >::iterator i = allobjects.begin();
            i != allobjects.end(); ++i )
        {
        ImageIOBase *io = dynamic_cast< ImageIOBase * >( i->GetPointer() );
        msg << "    " << io->GetNameOfClass() << std::endl;
        }
      msg << "  You probably failed to set a file suffix, or" << std::endl;
      msg << "    set the suffix to an unsupported type." << std::endl;
      }
    ImageFileReaderException e(__FILE__, __LINE__, msg.str().c_str(), ITK_LOCATION);
    throw e;
    return imageIO;
    }
  return imageIO;
}


} //namespace ITK

#endif
