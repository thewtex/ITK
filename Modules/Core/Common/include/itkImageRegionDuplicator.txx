#ifndef __itkImageRegionDuplicator_txx
#define __itkImageRegionDuplicator_txx

#include "itkImageRegionDuplicator.h"
#include "itkImageRegionIterator.h"

namespace itk
{
// Constructor
template< class TInputImage, class TOutputImage >
ImageRegionDuplicator< TInputImage, TOutputImage >
::ImageRegionDuplicator( void )
{
}

// Update method
template< class TInputImage, class TOutputImage >
void
ImageRegionDuplicator< TInputImage, TOutputImage >
::Update( void )
{
  if ( this->m_InputImage.IsNull() )
    {
    itkExceptionMacro(<< "Input image has not been set.");
    }
  if ( this->m_OutputImage.IsNull() )
    {
    itkExceptionMacro(<< "Output image has not been set.");
    }

  Self::Copy( this->m_InputImage, this->m_OutputImage,
              this->m_InputRegion, this->m_OutputRegion );
}

// PrintSelf method
template< class TInputImage, class TOutputImage >
void
ImageRegionDuplicator< TInputImage, TOutputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "InputImage: " << m_InputImage << std::endl;
  os << indent << "OutputImage: " << m_OutputImage << std::endl;
  os << indent << "InputRegion: " << m_InputRegion << std::endl;
  os << indent << "OutputRegion: " << m_OutputRegion << std::endl;
}

// static method to actually do the work
template< class TInputImage, class TOutputImage >
void
ImageRegionDuplicator< TInputImage, TOutputImage >
::Copy( InputImageConstPointer input, OutputImagePointer output,
        const InputRegionType &inRegion, const OutputRegionType &outRegion)
{
  itk::ImageRegionConstIterator<TInputImage> it( input, inRegion );
  itk::ImageRegionIterator<TOutputImage> ot( output, outRegion );

  while( !it.IsAtEnd() )
    {
    ot.Set( static_cast< typename TInputImage::PixelType >( it.Get() ) );
    ++ot;
    ++it;
    }
}


} // end namespace itk

#endif __itkImageRegionDuplicator_txx
