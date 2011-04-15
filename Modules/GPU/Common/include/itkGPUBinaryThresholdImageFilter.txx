#ifndef __itkGPUBinaryThresholdImageFilter_txx
#define __itkGPUBinaryThresholdImageFilter_txx

#include "itkGPUBinaryThresholdImageFilter.h"

namespace itk
{
/**
 * Constructor
 */
template< class TInputImage, class TOutputImage >
GPUBinaryThresholdImageFilter< TInputImage, TOutputImage >
::GPUBinaryThresholdImageFilter()
{
  std::ostringstream defines;

  if(TInputImage::ImageDimension > 3)
  {
    itkExceptionMacro("GPUBinaryThresholdImageFilter supports 1/2/3D image.");
  }

  defines << "#define DIM_" << TInputImage::ImageDimension << "\n";
  defines << "#define InPixelType ";

  if ( typeid ( typename TInputImage::PixelType ) == typeid ( unsigned char ) )
  {
    defines << "unsigned char\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( char ) )
  {
    defines << "char\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( int ) )
  {
    defines << "int\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( unsigned int ) )
  {
    defines << "unsigned int\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( float ) )
  {
    defines << "float\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( double ) )
  {
    defines << "double\n";
  }
  else
  {
    itkExceptionMacro("GPUBinaryThresholdImageFilter supports unsigned char, short, int and float images.");
  }

  defines << "#define OutPixelType ";
  if ( typeid ( typename TInputImage::PixelType ) == typeid ( unsigned char ) )
  {
    defines << "unsigned char\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( char ) )
  {
    defines << "char\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( int ) )
  {
    defines << "int\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( unsigned int ) )
  {
    defines << "unsigned int\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( float ) )
  {
    defines << "float\n";
  }
  else if ( typeid ( typename TInputImage::PixelType ) == typeid ( double ) )
  {
    defines << "double\n";
  }
  else
  {
    itkExceptionMacro("GPUBinaryThresholdImageFilter supports unsigned char, short, int and float images.");
  }


  // OpenCL source path
  std::string oclSrcPath = "./../OpenCL/GPUBinaryThresholdImageFilter.cl";

  std::cout << "Defines: " << defines.str() << "Source code path: " << oclSrcPath << std::endl;

  // load and build program
  this->m_KernelManager->LoadProgramFromFile( oclSrcPath.c_str(), defines.str().c_str() );

  // create kernel
  this->m_KernelHandle = this->m_KernelManager->CreateKernel("BinaryThresholdFilter");
}
/**
 *
 */
template< class TInputImage, class TOutputImage >
void
GPUBinaryThresholdImageFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  GPUSuperclass::PrintSelf( os, indent );
}


template< class TInputImage, class TOutputImage >
void
GPUBinaryThresholdImageFilter< TInputImage, TOutputImage >
::GPUGenerateData()
{
  // set up the functor values
  typename InputPixelObjectType::Pointer lowerThreshold = this->GetLowerThresholdInput();
  typename InputPixelObjectType::Pointer upperThreshold = this->GetUpperThresholdInput();

  if ( lowerThreshold->Get() > upperThreshold->Get() )
    {
    itkExceptionMacro(<< "Lower threshold cannot be greater than upper threshold.");
    }

  // Setup up the functor
  this->GetFunctor().SetLowerThreshold( lowerThreshold->Get() );
  this->GetFunctor().SetUpperThreshold( upperThreshold->Get() );

  this->GetFunctor().SetInsideValue( this->GetInsideValue() );
  this->GetFunctor().SetOutsideValue( this->GetOutsideValue() );

  GPUSuperclass::GPUGenerateData();
}


}// end of namespace itk

#endif
