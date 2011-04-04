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

#ifndef __itkImageFastMarchingTraits2_h
#define __itkImageFastMarchingTraits2_h

#include "itkFastMarchingTraits.h"

#include "itkConceptChecking.h"

#include "itkImage.h"
#include "itkImageToImageFilter.h"

namespace itk
{
/** \class ImageFastMarchingTraits2
  \brief possible traits to be used when using itk::FastMarchingBase
inherited class with itk::Image

  \tparam TInputImage Input Image type
  \tparam TOutputImage Output Image type

  \sa ImageFastMarchingTraits
  */
template< class TInputImage, class TOutputImage >
class ImageFastMarchingTraits2 :
    public FastMarchingTraits<
      TInputImage,
      typename TOutputImage::IndexType,
      TOutputImage,
      ImageToImageFilter< TInputImage, TOutputImage > >
{
public:
  typedef ImageFastMarchingTraits2 Self;

  itkStaticConstMacro(ImageDimension, unsigned int, TOutputImage::ImageDimension);

#ifdef ITK_USE_CONCEPT_CHECKING

  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  // Let's make sure TInputImage and TOutputImage have the same dimension
  itkConceptMacro( SameDimension,
                   ( Concept::SameDimension<
                      itkGetStaticConstMacro(InputImageDimension),
                      itkGetStaticConstMacro(ImageDimension) > ) );
#endif
};
}
#endif // __itkImageFastMarchingTraits2_h
