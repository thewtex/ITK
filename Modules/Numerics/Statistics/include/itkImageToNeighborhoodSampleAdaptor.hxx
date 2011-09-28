/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToNeighborhoodSampleAdaptor.hxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToNeighborhoodSampleAdaptor_hxx
#define __itkImageToNeighborhoodSampleAdaptor_hxx

#include "itkImageToNeighborhoodSampleAdaptor.h"

namespace itk {
namespace Statistics {

  template < class TImage, class TBoundaryCondition>
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::ImageToNeighborhoodSampleAdaptor()
{
  m_Image = 0;
  m_Radius.Fill(0);
  NeighborhoodIndexType start;
  NeighborhoodSizeType sz;
  start.Fill(0);
  sz.Fill(0);
  m_Region.SetIndex(start);
  m_Region.SetSize(sz);
  m_UseImageRegion = true;
  this->SetMeasurementVectorSize(1);
  m_NeighborIndexInternal.Fill(0);
}

template < class TImage, class TBoundaryCondition>
const typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition >::MeasurementVectorType&
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetMeasurementVector(InstanceIdentifier id) const
{
  if( m_Image.IsNull() )
    {
    itkExceptionMacro("Image has not been set yet");
    }

  if (id == m_InstanceIdentifierInternal)
  {
    return m_MeasurementVectorInternal;
  }
  else
  {
    IndexType reqIndex;
    ImageHelper<ImageType::ImageDimension,
      ImageType::ImageDimension>::ComputeIndex(m_Region.GetIndex(),
                                               id,
                                               m_OffsetTable,
                                               reqIndex);

    OffsetType offset = reqIndex - m_NeighborIndexInternal;

    m_NeighborIndexInternal = reqIndex;
    m_MeasurementVectorInternal[0] += offset;
    m_InstanceIdentifierInternal = id;

    return m_MeasurementVectorInternal;
  }
}

/** returns the number of measurement vectors in this container*/
template < class TImage, class TBoundaryCondition>
typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>::InstanceIdentifier
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::Size() const
{
  if( m_Image.IsNull() )
    {
      return 0;
    }

  return m_Region.GetNumberOfPixels();
}

template < class TImage, class TBoundaryCondition>
inline typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>::AbsoluteFrequencyType
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetFrequency( InstanceIdentifier ) const
{
  if( m_Image.IsNull() )
    {
    itkExceptionMacro("Image has not been set yet");
    }

  return NumericTraits< AbsoluteFrequencyType >::One;
}


template < class TImage, class TBoundaryCondition>
void
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Image: ";
  if ( m_Image.IsNotNull() )
    {
    os << m_Image << std::endl;
    }
  else
    {
    os << "not set." << std::endl;
    }
  os << indent << "UseImageRegion: "
     << m_UseImageRegion << std::endl;
  os << indent << "Region: " << m_Region << std::endl;
  os << indent << "Neighborhood Radius: " << m_Radius << std::endl;
//   os << indent << "Start offset: " << m_StartOffset << std::endl;
}

template < class TImage, class TBoundaryCondition>
void
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::SetImage(const TImage* image)
{
  m_Image = image;
  if (m_UseImageRegion)
  {
    m_Region = m_Image->GetLargestPossibleRegion();
  }
  NeighborhoodIteratorType neighborIt;
  neighborIt = NeighborhoodIteratorType(m_Radius, m_Image, m_Region);
  neighborIt.GoToBegin();
  m_NeighborIndexInternal = neighborIt.GetBeginIndex();
  m_MeasurementVectorInternal.clear();
  m_MeasurementVectorInternal.push_back( neighborIt );
  m_InstanceIdentifierInternal = 0;
  OffsetValueType num=1;
  const SizeType& regionSize = m_Region.GetSize();

  m_OffsetTable[0] = num;
  for (unsigned int i=0; i < ImageType::ImageDimension; i++)
  {
    num *= regionSize[i];
    m_OffsetTable[i+1] = num;
  }

  this->Modified();
}

template < class TImage, class TBoundaryCondition>
const TImage*
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetImage() const
{
  if( m_Image.IsNull() )
    {
    itkExceptionMacro("Image has not been set yet");
    }

  return m_Image.GetPointer();
}

template < class TImage, class TBoundaryCondition>
void
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::SetRadius(const NeighborhoodRadiusType& radius)
{
  if ( radius != m_Radius )
  {
    m_Radius = radius;
    if ( m_Image.IsNotNull() )
    {
      NeighborhoodIteratorType neighborIt;
      neighborIt = NeighborhoodIteratorType(m_Radius, m_Image, m_Region);
      neighborIt.GoToBegin();
      m_NeighborIndexInternal = neighborIt.GetBeginIndex();
      m_MeasurementVectorInternal.clear();
      m_MeasurementVectorInternal.push_back( neighborIt );
      m_InstanceIdentifierInternal = 0;
    }
    this->Modified();
  }
}

template < class TImage, class TBoundaryCondition>
typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>::NeighborhoodRadiusType
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetRadius() const
{
  return m_Radius;
}

template < class TImage, class TBoundaryCondition>
void
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::SetRegion(const RegionType& region)
{
  if ( region != m_Region )
  {
    m_Region = region;
    m_UseImageRegion = false;
    if (m_Image.IsNotNull() )
    {
      NeighborhoodIteratorType neighborIt;
      neighborIt = NeighborhoodIteratorType(m_Radius, m_Image, m_Region);
      neighborIt.GoToBegin();
      m_NeighborIndexInternal = neighborIt.GetBeginIndex();
      m_MeasurementVectorInternal.clear();
      m_MeasurementVectorInternal.push_back( neighborIt );
      m_InstanceIdentifierInternal = 0;
      OffsetValueType num=1;
      const SizeType& regionSize = m_Region.GetSize();

      m_OffsetTable[0] = num;
      for (unsigned int i=0; i < ImageType::ImageDimension; i++)
      {
        num *= regionSize[i];
        m_OffsetTable[i+1] = num;
      }

    }
    this->Modified();
  }
}

template < class TImage, class TBoundaryCondition>
typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>::RegionType
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetRegion() const
{
  return m_Region;
}

template < class TImage, class TBoundaryCondition>
void
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::SetUseImageRegion(const bool& flag)
{
  if ( flag != m_UseImageRegion )
  {
    m_UseImageRegion = flag;
    if ( m_UseImageRegion && m_Image.IsNotNull() )
    {
      this->SetRegion( m_Image->GetLargestPossibleRegion() );
    }
  }
}

template < class TImage, class TBoundaryCondition>
typename ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>::TotalAbsoluteFrequencyType
ImageToNeighborhoodSampleAdaptor< TImage, TBoundaryCondition>
::GetTotalFrequency() const
{
  if( m_Image.IsNull() )
    {
    itkExceptionMacro("Image has not been set yet");
    }

  return this->Size();
}

} // end of namespace Statistics

template <typename TImage, typename TBoundaryCondition>
std::ostream & operator<<(std::ostream &os,
                          const std::vector< itk::ConstNeighborhoodIterator<TImage, TBoundaryCondition> > &mv)
{
  itk::ConstNeighborhoodIterator<TImage, TBoundaryCondition> nbhd = mv[0];
  os << "Neighborhood:" << std::endl;
  os << "    Radius: " << nbhd.GetRadius() << std::endl;
  os << "    Size: " << nbhd.GetSize() << std::endl;
  os << "    Index of Center Pixel: " << nbhd.GetIndex() << std::endl;
  return os;
}

} // end of namespace itk

#endif
