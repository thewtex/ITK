/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageListSource_h
#define __itkImageListSource_h

#include "itkProcessObject.h"
#include "itkImageList.h"

namespace itk
{
/** \class ImageListSource
 *
 *  \brief Base class for all the filters producing an otbImageList
 *
 * \ingroup DataSources
 * \ingroup Images
 * \ingroup Lists
 */
template <class TOutputImage>
class ITK_EXPORT ImageListSource
  : public itk::ProcessObject
{
public:
  /** Standard typedefs */
  typedef ImageListSource               Self;
  typedef itk::ProcessObject            Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  /** Type macro */
  itkNewMacro(Self);
  /** Creation through object factory macro */
  itkTypeMacro(ImageListSource, itk::ProcessObject);
  /** Data object pointer type */
  typedef itk::DataObject::Pointer DataObjectPointer;
  /** Template parameter typedef*/
  typedef TOutputImage                               OutputImageType;
  typedef typename OutputImageType::Pointer          OutputImagePointerType;
  typedef ImageList<OutputImageType>                 OutputImageListType;
  typedef typename OutputImageListType::Pointer      OutputImageListPointerType;
  typedef typename OutputImageListType::ConstPointer OutputImageListConstPointerType;
  /** Overiding of the GetOutput() method */
  virtual OutputImageListType * GetOutput(void);

protected:
  /** Constructor */
  ImageListSource();
  /** Destructor */
  virtual ~ImageListSource() {}
  /**PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  ImageListSource(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // End namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageListSource.txx"
#endif
#endif
