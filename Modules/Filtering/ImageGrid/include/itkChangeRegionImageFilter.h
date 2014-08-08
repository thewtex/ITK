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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef __itkChangeRegionImageFilter_h
#define __itkChangeRegionImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkVariableLengthVector.h"

namespace itk
{
/** \class ChangeRegionImageFilter
 * \brief Reduce the size of an image by an integer factor in each
 * dimension.
 *
 * ChangeRegionImageFilter reduces the size of an image by an integer factor
 * in each dimension. The algorithm implemented is a simple subsample.
 * The output image size in each dimension is given by:
 *
 * outputSize[j] = max( vcl_floor(inputSize[j]/shrinkFactor[j]), 1 );
 *
 * NOTE: The physical centers of the input and output will be the
 * same. Because of this, the Origin of the output may not be the same
 * as the Origin of the input.
 * Since this filter produces an image which is a different
 * resolution, origin and with different pixel spacing than its input
 * image, it needs to override several of the methods defined
 * in ProcessObject in order to properly manage the pipeline execution model.
 * In particular, this filter overrides
 * ProcessObject::GenerateInputRequestedRegion() and
 * ProcessObject::GenerateOutputInformation().
 *
 * This filter is implemented as a multithreaded filter.  It provides a
 * ThreadedGenerateData() method for its implementation.
 *
 * \ingroup GeometricTransform Streamed
 * \ingroup ITKImageGrid
 *
 * \wiki
 * \wikiexample{Images/ChangeRegionImageFilter,ChangeRegion an image}
 * \endwiki
 */
template< class ImageType >
class ITK_EXPORT ChangeRegionImageFilter:
  public ImageToImageFilter< ImageType, ImageType >
{
public:
  /** Standard class typedefs. */
  typedef ChangeRegionImageFilter                    Self;
  typedef ImageToImageFilter< ImageType, ImageType > Superclass;
  typedef SmartPointer< Self >                       Pointer;
  typedef SmartPointer< const Self >                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ChangeRegionImageFilter, ImageToImageFilter);

  /** Typedef to images */
  typedef typename ImageType::Pointer      OutputImagePointer;
  typedef typename ImageType::Pointer      InputImagePointer;
  typedef typename ImageType::ConstPointer InputImageConstPointer;

  typedef typename ImageType::IndexType  OutputIndexType;
  typedef typename ImageType::IndexType  InputIndexType;
  typedef typename ImageType::OffsetType OutputOffsetType;

  /** Typedef to describe the output image region type. */
  typedef typename ImageType::RegionType OutputImageRegionType;

  /** ImageDimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      ImageType::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      ImageType::ImageDimension);

  typedef FixedArray< unsigned int, ImageDimension > ChangeRegionFactorsType;

  itkSetMacro(ChangeRegionFactors, ChangeRegionFactorsType);
  void SetBufferedRegion(typename ImageType::RegionType region);
  void SetLargestPossibleRegion(typename ImageType::RegionType region);
  void SetOrigin(typename ImageType::PointType origin);

  /** Get the shrink factors. */
  itkGetConstReferenceMacro(ChangeRegionFactors, ChangeRegionFactorsType);

  /** ChangeRegionImageFilter produces an image which is a different
   * resolution and with a different pixel spacing than its input
   * image.  As such, ChangeRegionImageFilter needs to provide an
   * implementation for GenerateOutputInformation() in order to inform
   * the pipeline execution model.  The original documentation of this
   * method is below.
   * \sa ProcessObject::GenerateOutputInformaton() */
  virtual void GenerateOutputInformation();

  /** ChangeRegionImageFilter needs a larger input requested region than the output
   * requested region.  As such, ChangeRegionImageFilter needs to provide an
   * implementation for GenerateInputRequestedRegion() in order to inform the
   * pipeline execution model.
   * \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( SameDimensionCheck,
                   ( Concept::SameDimension< ImageDimension, OutputImageDimension > ) );
  /** End concept checking */
#endif

protected:
  ChangeRegionImageFilter();
  ~ChangeRegionImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  void GenerateData();

private:
  ChangeRegionImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);    //purposely not implemented

  ChangeRegionFactorsType        m_ChangeRegionFactors;
  typename ImageType::RegionType m_BufferedRegion;
  typename ImageType::RegionType m_LargestPossibleRegion;
  typename ImageType::PointType  m_Origin;

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkChangeRegionImageFilter.hxx"
#endif

#endif
