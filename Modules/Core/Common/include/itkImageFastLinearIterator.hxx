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
#ifndef __itkImageFastLinearIterator_hxx
#define __itkImageFastLinearIterator_hxx

#include "itkImageFastLinearIterator.h"

namespace itk
{
template< typename TImage >
ImageFastLinearIterator< TImage >
::ImageFastLinearIterator():
  ImageFastLinearConstIterator< TImage >()
{}

template< typename TImage >
ImageFastLinearIterator< TImage >
::ImageFastLinearIterator(ImageType *ptr, const RegionType & region):
  ImageFastLinearConstIterator< TImage >(ptr, region)
{}

template< typename TImage >
ImageFastLinearIterator< TImage >
::ImageFastLinearIterator(const ImageIterator< TImage > & it):
  ImageFastLinearConstIterator< TImage >(it)
{}

template< typename TImage >
ImageFastLinearIterator< TImage >
::ImageFastLinearIterator(const ImageFastLinearConstIterator< TImage > & it):
  ImageFastLinearConstIterator< TImage >(it)
{}

template< typename TImage >
ImageFastLinearIterator< TImage > &
ImageFastLinearIterator< TImage >
::operator=(const ImageFastLinearConstIterator< TImage > & it)
{
  this->ImageFastLinearConstIterator< TImage >::operator=(it);
  return *this;
}

} // end namespace itk

#endif
