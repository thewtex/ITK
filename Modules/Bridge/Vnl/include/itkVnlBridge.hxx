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
#ifndef __itkVnlBridge_hxx
#define __itkVnlBridge_hxx

#include "itkVnlBridge.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{
template < class TGenericImage, class MatrixType >
typename TGenericImage::Pointer VnlBridge::VnlMatrixToImage( MatrixType inputMatrix )
{
  typedef TGenericImage                 ImageType;
  typedef typename MatrixType::iterator MatrixIteratorType;

  MatrixIteratorType inputMatrixIterator;

  typename ImageType::Pointer image = ImageType::New();

  typename ImageType::SizeType size;
  size[0] = inputMatrix.columns();
  size[1] = inputMatrix.rows();

  typename ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  typename ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );

  double spacing[2];
  spacing[0] = 1;
  spacing[1] = 1;

  double origin[2];
  origin[0] = 0;
  origin[1] = 0;

  image->SetRegions( region );
  image->Allocate();
  image->SetSpacing( spacing );
  image->SetOrigin( origin );

  typedef itk::ImageRegionIterator< ImageType> IteratorType;
  IteratorType it( image, region);
  it.GoToBegin();
  inputMatrixIterator = inputMatrix.begin();

  // Elements in VNL matrices are stored in row-major order, just like ITK images.
  // So we can simply iterate through the image and the matrix and copy the values directly.
  while( !it.IsAtEnd() )
    {
    it.Set( *inputMatrixIterator );
    ++it;
    ++inputMatrixIterator;
    }

  return image;
}

template < class TGenericImage, class MatrixType >
MatrixType VnlBridge::ImageToVnlMatrix( const TGenericImage * image )
{
  typedef TGenericImage ImageType;
  //typedef vnl_matrix< double > MatrixType;
  //typedef vnl_matrix< double >::iterator MatrixIteratorType;
  typedef typename MatrixType::iterator MatrixIteratorType;

  typename ImageType::RegionType region;
  region = image->GetBufferedRegion();
  typename ImageType::SizeType size = region.GetSize();
  typename ImageType::IndexType start = region.GetIndex();

  MatrixType outputMatrix(size[1],size[0]);
  MatrixIteratorType outputMatrixIterator;

  typedef itk::ImageRegionConstIterator< ImageType> IteratorType;
  IteratorType it( image, region);
  it.GoToBegin();
  outputMatrixIterator = outputMatrix.begin();

  // Elements in VNL matrices are stored in row-major order, just like ITK images.
  // So we can simply iterate through the image and the matrix and copy the values directly.
  while( !it.IsAtEnd() )
    {
    *outputMatrixIterator = it.Get();
    ++it;
    ++outputMatrixIterator;
    }

  return outputMatrix;
}
} // end namespace itk

#endif
