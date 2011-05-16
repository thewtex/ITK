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
#ifndef __itkImageAlgorithm_txx
#define __itkImageAlgorithm_txx

namespace itk
{

template<typename InputImageType, typename OutputImageType >
void ImageCopy( const InputImageType *inImage, OutputImageType *outImage,
                const typename InputImageType::RegionType &inRegion,
                const typename OutputImageType::RegionType &outRegion )
{
  itk::ImageRegionConstIterator<TInputImage> it( input, inRegion );
  itk::ImageRegionIterator<TOutputImage> ot( output, outRegion );

  while( !it.IsAtEnd() )
    {
    ot.Set( static_cast< typename TInputImage::PixelType >( it.Get() ) );
    ++ot;
    ++it;
    }
}

} // end namespace itk

#endif __itkImageAlgorithm_txx
