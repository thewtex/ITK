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
#ifndef __itkComposeImageFilter_h
#define __itkComposeImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkVectorImage.h"
#include "itkImageRegionConstIterator.h"
#include <vector>

namespace itk
{
/** \class ComposeImageFilter
 * \brief This class takes as input 'n' itk::Image's and composes them into
 * a single itk::VectorImage.
 *
 * \par Inputs and Usage
 * \code
 *    filter->SetInput( 0, image0 );
 *    filter->SetInput( 1, image1 );
 *    ...
 *    filter->Update();
 *    itk::VectorImage< PixelType, dimension >::Pointer = filter->GetOutput();
 * \endcode
 * All input images are expected to have the same template parameters and have
 * the same size and origin.
 *
 * \sa VectorImage
 * \ingroup ITK-ImageCompose
 *
 * \wiki
 * \wikiexample{VectorImages/ImageToVectorImageFilter,Create a vector image from a collection of scalar images}
 * \endwiki
 */

template< class TInputImage, class TOutputImage=VectorImage<typename TInputImage::PixelType, TInputImage::ImageDimension> >
class ITK_EXPORT ComposeImageFilter:
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  typedef ComposeImageFilter                               Self;
  typedef SmartPointer< Self >                             Pointer;
  typedef SmartPointer< const Self >                       ConstPointer;
  typedef ImageToImageFilter< TInputImage, TOutputImage >  Superclass;
  itkNewMacro(Self);
  itkTypeMacro(ComposeImageFilter, ImageToImageFilter);

  itkStaticConstMacro(Dimension, unsigned int, TInputImage::ImageDimension);

  typedef TInputImage                          InputImageType;
  typedef TOutputImage                         OutputImageType;
  typedef typename InputImageType::PixelType   InputPixelType;
  typedef typename OutputImageType::PixelType  OutputPixelType;
  typedef typename InputImageType::RegionType  RegionType;

  void SetInput1(const InputImageType *image1);
  void SetInput2(const InputImageType *image2);
  void SetInput3(const InputImageType *image3);

//   virtual void SetNthInput(unsigned int idx, const InputImageType *inputImage)
//   { this->SetInput(idx, inputImage); }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  // Check if the pixeltype is a scalar, (native pixel type).
  /** End concept checking */
#endif
protected:
  ComposeImageFilter();

  virtual void GenerateOutputInformation(void);

  virtual void BeforeThreadedGenerateData();

  virtual void ThreadedGenerateData(const RegionType & outputRegionForThread, int);

//   virtual void SetNthInput(unsigned int num, DataObject *input)
//   {
//     Superclass::SetNthInput(num, input);
//   }

private:
  ComposeImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented


  // we have to specialize the code for complex, because it provides no operator[]
  // method
  typedef ImageRegionConstIterator< InputImageType > InputIteratorType;
  typedef std::vector< InputIteratorType * >         InputIteratorContainerType;

  void ComputeOutputPixel(std::complex<float> & pix, InputIteratorContainerType & inputItContainer )
    {
    pix = std::complex<float>(inputItContainer[0]->Get(), inputItContainer[1]->Get());
    ++( *inputItContainer[0] );
    ++( *inputItContainer[1] );
    }
  void ComputeOutputPixel(std::complex<double> & pix, InputIteratorContainerType & inputItContainer)
    {
    pix = std::complex<double>(inputItContainer[0]->Get(), inputItContainer[1]->Get());
    ++( *inputItContainer[0] );
    ++( *inputItContainer[1] );
    }
  void ComputeOutputPixel(std::complex<long double> & pix, InputIteratorContainerType & inputItContainer)
    {
    pix = std::complex<long double>(inputItContainer[0]->Get(), inputItContainer[1]->Get());
    ++( *inputItContainer[0] );
    ++( *inputItContainer[1] );
    }
  template<class TPixel>
  void ComputeOutputPixel(TPixel & pix, InputIteratorContainerType & inputItContainer)
    {
    for ( unsigned int i = 0; i < this->GetNumberOfInputs(); i++ )
      {
      pix[i] = inputItContainer[i]->Get();
      ++( *inputItContainer[i] );
      }
    }
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkComposeImageFilter.txx"
#endif

#endif
