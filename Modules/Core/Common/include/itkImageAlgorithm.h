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
#ifndef __itkImageAlgorithm_h
#define __itkImageAlgorithm_h

namespace itk
{


/**
 * \brief This generic function copies a region from one image to
 * another. It may perform optimizations on the copy for efficiency.
 *
 * This method performs the equivalent to the following:
 * \code
 *     itk::ImageRegionConstIterator<TInputImage> it( inImage, inRegion );
 *     itk::ImageRegionIterator<TOutputImage> ot( outImage, outRegion );
 *
 *     while( !it.IsAtEnd() )
 *       {
 *       ot.Set( static_cast< typename TInputImage::PixelType >( it.Get() ) );
 *       ++ot;
 *       ++it;
 *       }
 * \endcode
 */
template<typename InputImageType, typename OutputImageType >
void ImageCopy( const InputImageType *inImage, OutputImageType *outImage,
                const typename InputImageType::RegionType &inRegion,
                const typename OutputImageType::RegionType &outRegion );


} // end namespace itk


#if ITK_TEMPLATE_TXX
#include "itkImageAlgorithm.txx"
#endif


#endif //__itkImageAlgorithm_h
