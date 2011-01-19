/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRToITKImageImport.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRToITKImage_txx
#define __itkRToITKImage_txx

#include "itkRToITKImageImport.h"
#include "itkObjectFactory.h"

//these were used in the vtkRadapter, figured they might be of some use.

#define R_NO_REMAP /* AVOID SOME SERIOUS STUPIDITY. DO NOT REMOVE. */

#include "R.h"
#include "Rdefines.h"
#include "R_ext/Parse.h"
#include "R_ext/Rdynload.h"

namespace itk{
/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
RToITKImageImport<TPixel, VImageDimension>
::RToITKImageImport()
{
  unsigned int idx;

  for (idx = 0; idx < VImageDimension; ++idx)
    {
    m_Spacing[idx] = 1.0;
    m_Origin[idx] = 0.0;
    }
  m_Direction.SetIdentity();

  m_ImportPointer = 0;
}

/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
RToITKImageImport<TPixel, VImageDimension>
::~RToITKImageImport()
{
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  int i;

  Superclass::PrintSelf(os,indent);

  if (m_ImportPointer)
    {
    os << indent << "Imported pointer: (" << m_ImportPointer  << ")" << std::endl;
    }
  else
    {
    os << indent << "Imported pointer: (None)" << std::endl;
    }

  os << indent << "Spacing: [";
  for (i=0; i < static_cast<int>(VImageDimension) - 1; i++)
    {
    os << m_Spacing[i] << ", ";
    }
  os << m_Spacing[i] << "]" << std::endl;

  os << indent << "Origin: [";
  for (i=0; i < static_cast<int>(VImageDimension) - 1; i++)
    {
    os << m_Origin[i] << ", ";
    }
  os << m_Origin[i] << "]" << std::endl;
  os << indent << "Direction: " << std::endl << this->GetDirection() << std::endl;
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::SetImportPointer(SEXP ptr)
{
  if (ptr != m_ImportPointer)
    {
    m_ImportPointer = ptr;
    this->Modified();
    }
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
SEXP
RToITKImageImport<TPixel, VImageDimension>
::GetImportPointer()
{
  return m_ImportPointer;
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  // call the superclass' implementation of this method
  Superclass::EnlargeOutputRequestedRegion(output);

  // get pointer to the output
  OutputImagePointer outputPtr = this->GetOutput();

  // set the requested region to the largest possible region (in this case
  // the amount of data that we have)
  outputPtr->SetRequestedRegion( outputPtr->GetLargestPossibleRegion() );
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::GenerateOutputInformation(void)
{
  if (!this->m_ImportPointer)
    {
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("The input SEXP object does not exist.");
    e.SetDataObject(0);
    throw e;
    }

  SizeType dims;
  dims[0] = Rf_ncols(this->m_ImportPointer);
  dims[1] = Rf_nrows(this->m_ImportPointer);

  typename RToITKImageImport::OutputImageType *output;
  output = this->GetOutput( 0 );

  typename RToITKImageImport::OutputImageType::IndexType index;
  index.Fill( 0 );


  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointer to the output
  typename RToITKImageImport::OutputImagePointer outputPtr = this->GetOutput();

  typename RToITKImageImport::RegionType         largestPossibleRegion;
  largestPossibleRegion.SetSize( dims );

  // we need to compute the output spacing, the output origin, the
  // output image size, and the output image start index
  outputPtr->SetSpacing( m_Spacing );
  outputPtr->SetOrigin( m_Origin );
  outputPtr->SetDirection( m_Direction );
  outputPtr->SetLargestPossibleRegion( largestPossibleRegion );
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::GenerateData(void)
{

  // Set up the output pointer.
  typename RToITKImageImport::OutputImageType *output;
  output = this->GetOutput( 0 );

  // Only if this is a valid SEXP, then walk through the entries of the SEXP and create an ITK image.
  if( Rf_isMatrix(this->m_ImportPointer) || Rf_isVector(this->m_ImportPointer) )
    {
    SizeType dims;
    dims[0] = Rf_ncols(this->m_ImportPointer);
    dims[1] = Rf_nrows(this->m_ImportPointer);
    TPixel data;

    for(unsigned int i=0; i<dims[1]; i++)
      {
      for(unsigned int j=0; j<dims[0]; j++)
        {
         IndexType pixelLocation;
        pixelLocation[0] = i;
         pixelLocation[1] = j;
        if ( Rf_isReal(this->m_ImportPointer) )
          {
          data = static_cast<TPixel>(REAL(this->m_ImportPointer)[j*dims[1] + i]);
          }
        else if ( Rf_isInteger(this->m_ImportPointer) )
          {
          data = static_cast<TPixel>(INTEGER(this->m_ImportPointer)[j*dims[1] + i]);
          }
        else
          {
          InvalidRequestedRegionError e(__FILE__, __LINE__);
          e.SetLocation(ITK_LOCATION);
          e.SetDescription("Bad return this->m_ImportPointer, tried REAL and INTEGER.");
          e.SetDataObject(0);
          throw e;
          }
        output->SetPixel(pixelLocation, data);
        }
      }
    }
  else
    {
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("SEXP variable does not represent a matrix or vector.");
    e.SetDataObject(0);
    throw e;
    }
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::SetSpacing( const SpacingType & spacing )
{
  double dspacing[VImageDimension];
  for(unsigned int i=0; i<VImageDimension; i++)
    {
    dspacing[i] = spacing[i];
    }
  this->SetSpacing( dspacing );
}


/**
 *
 */
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::SetOrigin( const OriginType & origin )
{
  double dorigin[VImageDimension];
  for(unsigned int i=0; i<VImageDimension; i++)
    {
    dorigin[i] = origin[i];
    }
  this->SetOrigin( dorigin );
}

//----------------------------------------------------------------------------
template <class TPixel, unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::SetDirection(const DirectionType direction )
{
  bool modified = false;
  for (unsigned int r = 0; r < VImageDimension; r++)
    {
    for (unsigned int c = 0; c < VImageDimension; c++)
      {
      if (m_Direction[r][c] != direction[r][c])
        {
        m_Direction[r][c] = direction[r][c];
        modified = true;
        }
      }
    }
  if (modified)
    {
    this->Modified();
    }
}

} // end namespace itk

#endif
 unsigned int VImageDimension>
void
RToITKImageImport<TPixel, VImageDimension>
::SetDirection(const DirectionType direction )
{
  bool modified = f
