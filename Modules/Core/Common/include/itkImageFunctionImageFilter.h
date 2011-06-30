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
#ifndef __itkImageFunctionImageFilter_h
#define __itkImageFunctionImageFilter_h

#include "itkInPlaceImageFilter.h"
#include "itkImageFunction.h"

namespace itk
{
/** \class ImageFunctionImageFilter
 * \brief Applies an ImageFunction to every pixel of an input image.
 *
 * This class applies an ImageFunction to every pixel of an input image.
 *
 * Caveats:
 * 1) By default, this filter sets the number of threads
 * to use to 1. If you are sure the ImageFilter you are using is thread-safe,
 * you can use this as a multi-threaded filter by using
 * this->SetNumberOfThreads(N).
 *
 * 2) By default, this filter is not run InPlace. If you are sure the
 * ImageFilter you are using allows for this, you can set
 * this->InPlaceOn().
 *
 * ImageFunctionImageFilter allows the output dimension of the filter
 * to be larger than the input dimension. Thus subclasses of the
 * ImageFunctionImageFilter (like the CastImageFilter) can be used
 * to promote a 2D image to a 3D image, etc.
 *
 * This class is parameterized over the type of the input image and
 * the type of the output image.  It is also parameterized by the
 * operation to be applied, using an ImageFunction style.
 *
 * \sa UnaryFunctorImageFilter
 *
 * \ingroup   IntensityImageFilters     Multithreaded
 * \ingroup ITK-Common
 */
template< class TInputImage, class TOutputImage, class TFunction >
class ITK_EXPORT ImageFunctionImageFilter:public InPlaceImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef ImageFunctionImageFilter                        Self;
  typedef InPlaceImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageFunctionImageFilter, InPlaceImageFilter);

  /** Some typedefs. */
  typedef TInputImage                              InputImageType;
  typedef typename    InputImageType::ConstPointer InputImagePointer;
  typedef typename    InputImageType::RegionType   InputImageRegionType;
  typedef typename    InputImageType::PixelType    InputImagePixelType;

  typedef TOutputImage                             OutputImageType;
  typedef typename     OutputImageType::Pointer    OutputImagePointer;
  typedef typename     OutputImageType::RegionType OutputImageRegionType;
  typedef typename     OutputImageType::PixelType  OutputImagePixelType;

  typedef typename     TFunction::Pointer          ImageFunctionPointer;

  /** Get the function object. */
  ImageFunctionPointer       GetFunction() { return m_Function; }

  /** Set the functoin object. This replaces the current Function pointer
   * with a new Function pointer.*/
  void SetFunction(ImageFunctionPointer function)
  {
    if ( m_Function != function )
      {
      m_Function = function;
      this->Modified();
      }
  }

protected:
  ImageFunctionImageFilter();
  virtual ~ImageFunctionImageFilter() {}

  /** ImageFunctionImageFilter can produce an image which is a different
   * resolution than its input image.  As such, ImageFunctionImageFilter
   * needs to provide an implementation for
   * GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below.
   *
   * \sa ProcessObject::GenerateOutputInformaton()  */
  virtual void GenerateOutputInformation();

  /** ImageFunctionImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData() routine
   * which is called for each processing thread. The output image data is
   * allocated automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to the
   * portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()  */
  void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                            ThreadIdType threadId);

private:
  ImageFunctionImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);          //purposely not implemented

  ImageFunctionPointer m_Function;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageFunctionImageFilter.txx"
#endif

#endif
