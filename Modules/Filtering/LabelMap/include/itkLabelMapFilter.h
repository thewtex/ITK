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
#ifndef __itkLabelMapFilter_h
#define __itkLabelMapFilter_h

#include "itkImageToImageFilter.h"
#include "itkDomainThreader.h"
#include "itkThreadedIteratorRangePartitioner.h"

namespace itk
{

/** \class LabelMapFilterThreader
 *
 * \brief DomainThreader class to operate on each sub-domain of labels.
 *
 * It runs the LabelMapFilter's ThreadedProcessLabelObject on each sub-domain of
 * labels that are processed in a thread.
 */
template< class TAssociate >
class LabelMapFilterThreader:
  public DomainThreader< ThreadedIteratorRangePartitioner< typename TAssociate::InputImageType::LabelObjectVectorType::iterator >, TAssociate >
{
public:
  /** Standard class typedefs */
  typedef LabelMapFilterThreader  Self;
  typedef DomainThreader< ThreadedIteratorRangePartitioner< typename TAssociate::InputImageType::LabelObjectVectorType::iterator >, TAssociate >
                                  Superclass;
  typedef SmartPointer< Self >    Pointer;

  /** Some convenient typedefs. */
  typedef typename Superclass::DomainType DomainType;
  typedef TAssociate                      AssociateType;

  /** Standard New method. */
  itkNewMacro( Self );

protected:
  LabelMapFilterThreader() {}

  virtual void ThreadedExecution( const DomainType & subDomain,
    const ThreadIdType threadId );

private:
  LabelMapFilterThreader( const Self & ); // purposely not implemented
  void operator=( const Self & );         // purposely not implemented
};

/** \class LabelMapFilter
 * \brief Base class for filters that take an image as input and overwrite that image as the output
 *
 * LabelMapFilter is the base class for all process objects whose
 * are using a LabelMapFilter as input. It manage several threads,
 * and run a method ThreadedGenerateData() for each object in the LabelMapFilter.
 * With that class, the developer doesn't need to take care of iterating over all the objects in
 * the image, or to manage by hand the threads.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * http://hdl.handle.net/1926/584  or
 * http://www.insight-journal.org/browse/publication/176
 *
 * \sa LabelMapToBinaryImageFilter, LabelMapToLabelImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 * \ingroup LabeledImageFilters
 * \ingroup ITKLabelMap
 */
template< class TInputImage, class TOutputImage >
class ITK_EXPORT LabelMapFilter:
  public
  ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef LabelMapFilter                                  Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(LabelMapFilter, ImageToImageFilter);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Some convenient typedefs. */
  typedef TInputImage                              InputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename InputImageType::LabelObjectType LabelObjectType;

  typedef TOutputImage                           OutputImageType;
  typedef typename OutputImageType::Pointer      OutputImagePointer;
  typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
  typedef typename OutputImageType::RegionType   OutputImageRegionType;
  typedef typename OutputImageType::PixelType    OutputImagePixelType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  /** LabelMapFilter requires the entire input to be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion();

  /** LabelMapFilter will produce the entire output. */
  void EnlargeOutputRequestedRegion( DataObject * itkNotUsed(output) );

protected:
  LabelMapFilter();
  ~LabelMapFilter();

  /** Calls ThreadedProcessLabelObject on each labelObject. */
  virtual void GenerateData();

  /** NULL method to be defined by inherited classes. */
  virtual void ThreadedProcessLabelObject(LabelObjectType *labelObject);

  /**
   * Return the label collection image to use. This method may be overloaded
   * if the label collection image to use is not the input image.
   */
  virtual InputImageType * GetLabelMap()
    {
    return static_cast< InputImageType * >( const_cast< DataObject * >( this->ProcessObject::GetInput(0) ) );
    }

private:
  LabelMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  typedef LabelMapFilterThreader< Self > ThreaderType;
  friend class LabelMapFilterThreader< Self >;
  typename ThreaderType::Pointer m_Threader;

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelMapFilter.hxx"
#endif

#endif
