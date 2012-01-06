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
#ifndef __itkImageTraits_h
#define __itkImageTraits_h

#include "itkNumericTraits.h"

namespace itk
{

template< class, unsigned int> class VectorImage;
template< class > class VariableLengthVector;
template< class, unsigned int> class Image;
template< class > class LabelMap;

template <typename TImageType>
struct ImageTraits;

/**
* example usage:
* typedef typename ImageTypeTraits<TImageType>::template Rebind< float >::Type OutputImageType;
*
*/
template <typename TPixelType, unsigned int TImageDimension>
struct ImageTraits< itk::Image<TPixelType, TImageDimension> >
{
  typedef itk::Image<TPixelType, TImageDimension> Type;
  typedef TPixelType                              PixelType;
  static const unsigned int ImageDimension = TImageDimension;

  template <typename UPixelType, unsigned int UImageDimension = ImageDimension>
  struct Rebind
    {
      typedef itk::Image<UPixelType, UImageDimension>  Type;
    };

  // optional possibility to include in image traits.
  typedef itk::Image<typename itk::NumericTraits<PixelType>::AccumulateType, ImageDimension > AccumulateType;
  typedef itk::Image<typename itk::NumericTraits<PixelType>::AbsType, ImageDimension >        AbsType;
  typedef itk::Image<typename itk::NumericTraits<PixelType>::FloatType, ImageDimension >      FloatType;
  typedef itk::Image<typename itk::NumericTraits<PixelType>::RealType, ImageDimension >       RealType;

};

template <typename TPixelType, unsigned int TImageDimension>
struct ImageTraits< itk::VectorImage<TPixelType, TImageDimension> >
{
  typedef itk::VectorImage<TPixelType, TImageDimension> Type;
  typedef TPixelType                                    PixelType;

  static const unsigned int ImageDimension = TImageDimension;

  template <typename UPixelType, unsigned int UImageDimension = TImageDimension>
  struct Rebind
  {
    typedef itk::VectorImage<UPixelType, UImageDimension>  Type;
  };

  template <typename UElementType, unsigned int UImageDimension>
  struct Rebind< VariableLengthVector< UElementType >, UImageDimension>
  {
    typedef itk::VectorImage<UElementType, UImageDimension>  Type;
  };
};


template <typename TPixelType, unsigned int TImageDimension,
          template <typename SPixelType, unsigned int SImageDimension>  class TLabelObject >
struct ImageTraits< itk::LabelMap< TLabelObject<TPixelType, TImageDimension> > >
{
  typedef itk::LabelMap< TLabelObject<TPixelType, TImageDimension> > Type;
  typedef TPixelType                                                 PixelType;

  static const unsigned int ImageDimension = TImageDimension;

  template <typename UPixelType, unsigned int UImageDimension = TImageDimension>
  struct Rebind
  {
    typedef itk::LabelMap< TLabelObject<UPixelType, UImageDimension> > Type;
  };
};


} // end namespace itk

#endif // __sitkImageTypeTraits_h
