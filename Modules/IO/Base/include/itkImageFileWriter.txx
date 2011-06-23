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
#ifndef __itkImageFileWriter_txx
#define __itkImageFileWriter_txx

#include "itkImageFileWriter.h"
#include "itkImageIOFactory.h"
#include "itkCommand.h"
#include "vnl/vnl_vector.h"
#include "itkImageRegionIterator.h"
#include "itkMatrix.h"

namespace itk
{
//---------------------------------------------------------
template< class TInputImage >
ImageFileWriter< TInputImage >
::ImageFileWriter():
  m_PasteIORegion(TInputImage::ImageDimension)
{
  m_UseCompression = false;
  m_UseInputMetaDataDictionary = true;
  m_FactorySpecifiedImageIO = false;
  m_UserSpecifiedIORegion = false;
  m_UserSpecifiedImageIO = false;
  m_NumberOfInputRequestedRegions = 1;
}

//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::SetInput(const InputImageType *input)
{
  // ProcessObject is not const_correct so this cast is required here.
  this->ProcessObject::SetNthInput( 0,
                                    const_cast< TInputImage * >( input ) );
}

//---------------------------------------------------------
template< class TInputImage >
const typename ImageFileWriter< TInputImage >::InputImageType *
ImageFileWriter< TInputImage >
::GetInput(void)
{
  if ( this->GetNumberOfInputs() < 1 )
    {
    return 0;
    }

  return static_cast< TInputImage * >
    ( this->ProcessObject::GetInput(0) );
}

//---------------------------------------------------------
template< class TInputImage >
const typename ImageFileWriter< TInputImage >::InputImageType *
ImageFileWriter< TInputImage >
::GetInput(unsigned int idx)
{
  return static_cast< TInputImage * >( this->ProcessObject::GetInput(idx) );
}

//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::SetIORegion(const ImageIORegion & region)
{
  itkDebugMacro("setting IORegion to " << region);
  if ( m_PasteIORegion != region )
    {
    m_PasteIORegion = region;
    this->Modified();
    m_UserSpecifiedIORegion = true;
    }
}


//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::Update()
{
  // This methods is overloaded because there are certain cases where
  // we don't wan't to call UpdateOutputInformation

  InputImageType *input =  const_cast< InputImageType * >( this->GetInput() );

  //  Update the meta data if needed
  // This should be moved to Update method
  if ( !m_UserSpecifiedIORegion )
    {
    input->UpdateOutputInformation();
    }
  else
    {
    // The user specified an io paste region.

    // If the input image is does not have a source filter, then we
    // do not want to update the output information, because it will
    // change the largest possible region to the buffered region. When
    // we are pasting the largest possible regions of the input must
    // match the file.
    if ( input->GetSource() )
      {
      input->UpdateOutputInformation();
      }

    }

  // Do the real processing
  this->UpdateOutputData( NULL );
}

//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::UpdateLargestPossibleRegion()
{
  // set not to use paste region, just use the largest
  m_PasteIORegion = ImageIORegion(TInputImage::ImageDimension);
  m_UserSpecifiedIORegion = false;

  this->Update();
}


//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::BeforeStreamedGenerateData()
{

  const InputImageType *input = this->GetInput();

  itkDebugMacro(<< "Writing an image file");

//---------------------------------------------------------
  // TODO Move to VerifyInputs method
  // Make sure input is available
  if ( input == 0 )
    {
    itkExceptionMacro(<< "No input to writer!");
    }

  // Make sure that we can write the file given the name
  //
  if ( m_FileName == "" )
    {
    itkExceptionMacro(<< "No filename was specified");
    }

  //---------------------------------------------------------

#if !defined(SPECIFIC_IMAGEIO_MODULE_TEST)
  if ( m_ImageIO.IsNull() ) //try creating via factory
    {
    itkDebugMacro(<< "Attempting factory creation of ImageIO for file: "
                  << m_FileName);
    m_ImageIO = ImageIOFactory::CreateImageIO(m_FileName.c_str(),
                                              ImageIOFactory::WriteMode);
    m_FactorySpecifiedImageIO = true;
    }
  else
    {
    if ( m_FactorySpecifiedImageIO && !m_ImageIO->CanWriteFile( m_FileName.c_str() ) )
      {
      itkDebugMacro(<< "ImageIO exists but doesn't know how to write file:"
                    << m_FileName);
      itkDebugMacro(<< "Attempting creation of ImageIO with a factory for file:"
                    << m_FileName);
      m_ImageIO = ImageIOFactory::CreateImageIO(m_FileName.c_str(),
                                                ImageIOFactory::WriteMode);
      m_FactorySpecifiedImageIO = true;
      }
    }
#endif

  if ( m_ImageIO.IsNull() )
    {
    ImageFileWriterException e(__FILE__, __LINE__);
    std::ostringstream       msg;
    msg << " Could not create IO object for file "
        << m_FileName.c_str() << std::endl;
    msg << "  Tried to create one of the following:" << std::endl;
    {
    std::list< LightObject::Pointer > allobjects =
      ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
    for ( std::list< LightObject::Pointer >::iterator i = allobjects.begin();
          i != allobjects.end(); ++i )
      {
      ImageIOBase *io = dynamic_cast< ImageIOBase * >( i->GetPointer() );
      msg << "    " << io->GetNameOfClass() << std::endl;
      }
    }
    msg << "  You probably failed to set a file suffix, or" << std::endl;
    msg << "    set the suffix to an unsupported type." << std::endl;
    e.SetDescription( msg.str().c_str() );
    e.SetLocation(ITK_LOCATION);
    throw e;
    }

  //
  // Setup the ImageIO
  //
  m_ImageIO->SetNumberOfDimensions(TInputImage::ImageDimension);
  InputImageRegionType largestRegion = input->GetLargestPossibleRegion();
  const typename TInputImage::SpacingType & spacing = input->GetSpacing();
  const typename TInputImage::DirectionType & direction = input->GetDirection();

  // BUG 8436: Wrong origin when writing a file with non-zero index
  // origin = input->GetOrigin();
  const typename TInputImage::IndexType & startIndex = largestRegion.GetIndex();
  typename TInputImage::PointType origin;
  input->TransformIndexToPhysicalPoint(startIndex, origin);

  for ( unsigned int i = 0; i < TInputImage::ImageDimension; i++ )
    {
    m_ImageIO->SetDimensions( i, largestRegion.GetSize(i) );
    m_ImageIO->SetSpacing(i, spacing[i]);
    m_ImageIO->SetOrigin(i, origin[i]);
    vnl_vector< double > axisDirection(TInputImage::ImageDimension);
    // Please note: direction cosines are stored as columns of the
    // direction matrix
    for ( unsigned int j = 0; j < TInputImage::ImageDimension; j++ )
      {
      axisDirection[j] = direction[j][i];
      }
    m_ImageIO->SetDirection(i, axisDirection);
    }

  // configure compression
  m_ImageIO->SetUseCompression(m_UseCompression);

  // configure meta dictionary
  if ( m_UseInputMetaDataDictionary )
    {
    m_ImageIO->SetMetaDataDictionary( input->GetMetaDataDictionary() );
    }

  // Make sure that the image is the right type
  // confiugure pixel type
  if ( strcmp(input->GetNameOfClass(), "VectorImage") == 0 )
    {
    typedef typename InputImageType::InternalPixelType VectorImageScalarType;
    m_ImageIO->SetPixelTypeInfo(static_cast<const VectorImageScalarType *>(0));
    typedef typename InputImageType::AccessorFunctorType AccessorFunctorType;
    m_ImageIO->SetNumberOfComponents( AccessorFunctorType::GetVectorLength(input) );
    }
  else
    {
    // Set the pixel and component type; the number of components.
    m_ImageIO->SetPixelTypeInfo(static_cast<const InputImagePixelType *>(0));
    }

  // Setup the image IO for writing.
  //
  m_ImageIO->SetFileName( m_FileName.c_str() );

  if ( this->GetNumberOfStreamDivisions() > 1 || m_UserSpecifiedIORegion )
    {
    m_ImageIO->SetUseStreamedWriting(true);
    }


  //
  // Compute Number of InputRequestedRegions
  //
  ImageIORegion largestIORegion(TInputImage::ImageDimension);
  ImageIORegionAdaptor< TInputImage::ImageDimension >::
    Convert( largestRegion, largestIORegion, largestRegion.GetIndex() );

  // this pasteIORegion is the region we are going to write
  ImageIORegion pasteIORegion;
  if ( m_UserSpecifiedIORegion )
    {
    pasteIORegion = m_PasteIORegion;
    }
  else
    {
    pasteIORegion = largestIORegion;
    }


  // Check whether the paste region is fully contained inside the
  // largest region or not.
  if ( !largestIORegion.IsInside(pasteIORegion) )
    {
    itkExceptionMacro(
      << "Largest possible region does not fully contain requested paste IO region"
      << "Paste IO region: " << pasteIORegion
      << "Largest possible region: " << largestRegion);
    }

  // Determin the actual number of divisions of the input. This is determined
  // by what the ImageIO can do

  // this may fail and throw an exception if the configuration is not supported
  this->m_NumberOfInputRequestedRegions =
    m_ImageIO->GetActualNumberOfSplitsForWriting(this->GetNumberOfStreamDivisions(),
                                                 pasteIORegion,
                                                 largestIORegion);
}

//---------------------------------------------------------
template< class TInputImage >
unsigned int
ImageFileWriter< TInputImage >
::GetNumberOfInputRequestedRegions( void )
{


  return this->m_NumberOfInputRequestedRegions;
}


//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::GenerateNthInputRequestedRegion(unsigned int piece)
{
  InputImageType *input =  const_cast< InputImageType * >( this->GetInput() );

  InputImageRegionType largestRegion = input->GetLargestPossibleRegion();

  ImageIORegion largestIORegion(TInputImage::ImageDimension);
  ImageIORegionAdaptor< TInputImage::ImageDimension >::
    Convert( largestRegion, largestIORegion, largestRegion.GetIndex() );


  // this pasteIORegion is the region we are going to write
  ImageIORegion pasteIORegion;
  if ( m_UserSpecifiedIORegion )
    {
    pasteIORegion = m_PasteIORegion;
    }
  else
    {
    pasteIORegion = largestIORegion;
    }

  unsigned int numDivisions = this->GetNumberOfInputRequestedRegions();

  // get the actual piece to write
  ImageIORegion streamIORegion = m_ImageIO->GetSplitRegionForWriting(piece, numDivisions,
                                                                     pasteIORegion, largestIORegion);

  // Check whether the paste region is fully contained inside the
  // largest region or not.
  if ( !pasteIORegion.IsInside(streamIORegion) )
    {
    itkExceptionMacro(
      << "ImageIO returns streamable region that is not fully contain in paste IO region"
      << "Paste IO region: " << pasteIORegion
      << "Streamable region: " << streamIORegion);
    }


  InputImageRegionType streamRegion;
  ImageIORegionAdaptor< TInputImage::ImageDimension >::
    Convert( streamIORegion, streamRegion, largestRegion.GetIndex() );

  this->m_CurrentIORegion = streamIORegion;

  input->SetRequestedRegion( streamRegion );
}


//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::StreamedGenerateData( unsigned int piece )
{
  const InputImageType *input = this->GetInput();
  InputImageRegionType largestRegion = input->GetLargestPossibleRegion();

  InputImageRegionType streamRegion;
  ImageIORegionAdaptor< TInputImage::ImageDimension >::
    Convert( m_CurrentIORegion, streamRegion, largestRegion.GetIndex() );


  // check to see if we tried to stream but got the largest possible region
  if ( piece == 0 && streamRegion != largestRegion )
    {
    InputImageRegionType bufferedRegion = input->GetBufferedRegion();
    if ( bufferedRegion == largestRegion )
      {
      // if so, then just write the entire image
      itkDebugMacro("Requested stream region  matches largest region input filter may not support streaming well.");
      itkDebugMacro("Writer is not streaming now!");
      }
    }

  m_ImageIO->SetIORegion( m_CurrentIORegion );

  InputImagePointer     cacheImage;

  itkDebugMacro(<< "Writing file: " << m_FileName);

  // now extract the data as a raw buffer pointer
  const void *dataPtr = (const void *)input->GetBufferPointer();

  // check that the image's buffered region is the same as
  // ImageIO is expecting and we requested
  InputImageRegionType ioRegion;
  ImageIORegionAdaptor< TInputImage::ImageDimension >::
    Convert( m_ImageIO->GetIORegion(), ioRegion, largestRegion.GetIndex() );
  InputImageRegionType bufferedRegion = input->GetBufferedRegion();

  // before this test, bad stuff would happend when they don't match
  if ( bufferedRegion != ioRegion )
    {
    if ( this->GetNumberOfStreamDivisions() > 1 || m_UserSpecifiedIORegion )
      {
      itkDebugMacro("Requested stream region does not match generated output");
      itkDebugMacro("input filter may not support streaming well");

      cacheImage = InputImageType::New();
      cacheImage->CopyInformation(input);
      cacheImage->SetBufferedRegion(ioRegion);
      cacheImage->Allocate();

      typedef ImageRegionConstIterator< TInputImage > ConstIteratorType;
      typedef ImageRegionIterator< TInputImage >      IteratorType;

      ConstIteratorType in(input, ioRegion);
      IteratorType      out(cacheImage, ioRegion);

      // copy the data into a buffer to match the ioregion
      for ( in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd(); ++in, ++out )
        {
        out.Set( in.Get() );
        }

      dataPtr = (const void *)cacheImage->GetBufferPointer();
      }
    else
      {
      ImageFileWriterException e(__FILE__, __LINE__);
      std::ostringstream       msg;
      msg << "Did not get requested region!" << std::endl;
      msg << "Requested:" << std::endl;
      msg << ioRegion;
      msg << "Actual:" << std::endl;
      msg << bufferedRegion;
      e.SetDescription( msg.str().c_str() );
      e.SetLocation(ITK_LOCATION);
      throw e;
      }
    }

  m_ImageIO->Write(dataPtr);

  this->UpdateProgress( (float)( piece + 1 ) / this->m_NumberOfInputRequestedRegions );
}


//---------------------------------------------------------
template< class TInputImage >
void
ImageFileWriter< TInputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "File Name: "
     << ( m_FileName.data() ? m_FileName.data() : "(none)" ) << std::endl;

  os << indent << "Image IO: ";
  if ( m_ImageIO.IsNull() )
    {
    os << "(none)\n";
    }
  else
    {
    os << m_ImageIO << "\n";
    }

  os << indent << "IO Region: " << m_PasteIORegion << "\n";

  if ( m_UseCompression )
    {
    os << indent << "Compression: On\n";
    }
  else
    {
    os << indent << "Compression: Off\n";
    }

  if ( m_UseInputMetaDataDictionary )
    {
    os << indent << "UseInputMetaDataDictionary: On\n";
    }
  else
    {
    os << indent << "UseInputMetaDataDictionary: Off\n";
    }

  if ( m_FactorySpecifiedImageIO )
    {
    os << indent << "FactorySpecifiedmageIO: On\n";
    }
  else
    {
    os << indent << "FactorySpecifiedmageIO: Off\n";
    }
}
} // end namespace itk

#endif
