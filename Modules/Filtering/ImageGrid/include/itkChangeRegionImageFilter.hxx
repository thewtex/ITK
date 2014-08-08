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
#ifndef __itkChangeRegionImageFilter_hxx
#define __itkChangeRegionImageFilter_hxx

#include "itkChangeRegionImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkContinuousIndex.h"
#include "itkObjectFactory.h"
#include "itkProgressReporter.h"

namespace itk
{
template< class ImageType >
ChangeRegionImageFilter< ImageType >
::ChangeRegionImageFilter()
{
  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    m_ChangeRegionFactors[j] = 1;
    }
}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ChangeRegion Factor: ";
  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    os << m_ChangeRegionFactors[j] << " ";
    }
  os << std::endl;
}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::SetBufferedRegion(typename ImageType::RegionType region)
{
    m_BufferedRegion = region;
    this->Modified();
}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::SetLargestPossibleRegion(typename ImageType::RegionType region)
{
    m_LargestPossibleRegion = region;
    this->Modified();
}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::SetOrigin(typename ImageType::PointType origin)
{
    m_Origin = origin;
    this->Modified();
}


template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::GenerateData()
{
  this->AllocateOutputs();

  // Get the inputPtr and outputPtr pointers
  InputImageConstPointer inputPtr = this->GetInput();
  OutputImagePointer     outputPtr = this->GetOutput();

  // Get regions.
  typename ImageType::RegionType ibregion = inputPtr->GetBufferedRegion();
  typename ImageType::RegionType obregion = outputPtr->GetBufferedRegion();

  // Size of buffered regions, for both inputPtr and outputs.
  int NZ,NY,NX,NC;
  typename ImageType::SizeType size = ibregion.GetSize();
  NZ = size[2];
  NY = size[1];
  NX = size[0];
  NC = inputPtr->GetNumberOfComponentsPerPixel();
  // TODO: ChangeRegionImageFilter::GenerateData: check inputPtr and outputPtr has same bufferedRegion size

  // Index of buffered region for inputPtr.
  typename ImageType::IndexType ibindex = ibregion.GetIndex();
  typename ImageType::IndexType ipixel;
  int IZ,IY,IX;
  IZ = ibindex[2];
  IY = ibindex[1];
  IX = ibindex[0];

  // Index of buffered region, for outputPtr.
  typename ImageType::IndexType obindex = obregion.GetIndex();
  typename ImageType::IndexType kpixel;
  int KZ,KY,KX;
  KZ = obindex[2];
  KY = obindex[1];
  KX = obindex[0];

  // Pixel.
  typedef itk::VariableLengthVector<double> VariableVectorType;
  VariableVectorType pixel;
  pixel.SetSize(NC);

  // Set image pixel //
  for (int iz=0; iz < NZ; ++iz) {
      ipixel[2] = IZ+iz;
      kpixel[2] = KZ+iz;
      for (int iy=0; iy < NY; ++iy) {
          ipixel[1] = IY+iy;
          kpixel[1] = KY+iy;
          for (int ix=0; ix < NX; ++ix) {
              ipixel[0] = IX+ix;
              kpixel[0] = KX+ix;
              pixel = inputPtr->GetPixel(ipixel);
              outputPtr->SetPixel(kpixel, pixel);
          }
      }
  }

}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::GenerateInputRequestedRegion()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // Get pointers to the inputPtr and outputPtr
  InputImagePointer  inputPtr = const_cast< ImageType * >( this->GetInput() );
  OutputImagePointer outputPtr = this->GetOutput();

  inputPtr->SetRequestedRegion(inputPtr->GetBufferedRegion());
}

template< class ImageType >
void
ChangeRegionImageFilter< ImageType >
::GenerateOutputInformation()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // Get pointers to the inputPtr and outputPtr
  InputImageConstPointer inputPtr  = this->GetInput();
  OutputImagePointer     outputPtr = this->GetOutput();

  outputPtr->SetBufferedRegion(m_BufferedRegion);
  outputPtr->SetRequestedRegion(m_BufferedRegion);
  outputPtr->SetLargestPossibleRegion(m_LargestPossibleRegion);
  outputPtr->SetOrigin(m_Origin);
}
} // end namespace itk

#endif
