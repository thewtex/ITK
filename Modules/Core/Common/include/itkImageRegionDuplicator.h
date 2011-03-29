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
#ifndef __itkImageRegionDuplicator_h
#define __itkImageRegionDuplicator_h

#include "itkObject.h"
#include "itkImage.h"

namespace itk
{
/** \class ImageRegionDuplicator
 * \brief This helper class which copies a region from one image to
 * another. It performs optimizations on the copy for efficiency.
 *
 * This class it to perform the equivalent to the following:
 * \code
 *     itk::ImageRegionConstIterator<TInputImage> it( input, inRegion );
 *     itk::ImageRegionIterator<TOutputImage> ot( output, outRegion );
 *
 *     while( !it.IsAtEnd() )
 *       {
 *       ot.Set( static_cast< typename TInputImage::PixelType >( it.Get() ) );
 *       ++ot;
 *       ++it;
 *       }
 * \endcode
 *
 */
template< class TInputImage, class TOutputImage = TInputImage >
class ITK_EXPORT ImageRegionDuplicator
  : public Object
{
public:
  /** Standard class typedefs. */
  typedef ImageRegionDuplicator      Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageRegionDuplicator, Object);

  /** Type definitions for the input image. */
  typedef TInputImage                        InputImageType;
  typedef typename TInputImage::ConstPointer InputImageConstPointer;
  typedef typename TInputImage::RegionType   InputRegionType;

  /** Type definitions for the output image. */
  typedef TOutputImage                        OutputImageType;
  typedef typename TOutputImage::Pointer      OutputImagePointer;
  typedef typename TOutputImage::RegionType   OutputRegionType;

  /** Set the input image. */
  itkSetConstObjectMacro( InputImage, InputImageType );

  /** Set the output image. */
  itkSetObjectMacro( OutputImage, OutputImageType );

  itkSetMacro( InputRegion, InputRegionType );
  itkGetConstMacro( InputRegion, InputRegionType );

  itkSetMacro( OutputRegion, OutputRegionType );
  itkGetConstMacro( OutputRegion, OutputRegionType );


  /** */
  void Update(void);

  static void Copy( InputImageConstPointer input, OutputImagePointer output,
                    const InputRegionType &inRegion, const OutputRegionType &outRegion);

protected:

  ImageRegionDuplicator();
  // virtual ~ImageRegionDuplicator() {} use implicit destructor
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  ImageRegionDuplicator(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented

  InputImageConstPointer m_InputImage;
  InputRegionType        m_InputRegion;
  OutputImagePointer     m_OutputImage;
  OutputRegionType       m_OutputRegion;

};
} // end namespace itk


#if ITK_TEMPLATE_TXX
#include "itkImageRegionDuplicator.txx"
#endif

#endif /* __itkImageRegionDuplicator_h */
