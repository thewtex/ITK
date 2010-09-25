/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    VesselnessMeasureImageFilter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

// The example takes an image (say MRA image), computes the vesselness measure
// of the image using the HessianRecursiveGaussianImageFilter and the
// Hessian3DToVesselnessMeasureImageFilter. The goal is to detect bright tubular
// structures in the image.

#include "itkHessian3DToVesselnessMeasureImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main( int argc, char *argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: inputImage outputImage [sigma] [alpha_1] [alpha_2]" << std::endl;
    }
  
  const     unsigned int        Dimension       = 3;
  typedef   double              InputPixelType;
  typedef   float               OutputPixelType;
  
  typedef   itk::Image< InputPixelType, Dimension >   InputImageType;
  typedef   itk::Image< OutputPixelType, Dimension >  OutputImageType;
  
  typedef   itk::HessianRecursiveGaussianImageFilter< 
                            InputImageType >              HessianFilterType;
  typedef   itk::Hessian3DToVesselnessMeasureImageFilter<
              OutputPixelType > VesselnessMeasureFilterType;
  typedef   itk::ImageFileReader< InputImageType >  ReaderType;
  typedef   itk::ImageFileWriter< OutputImageType > WriterType;
  
  HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  VesselnessMeasureFilterType::Pointer vesselnessFilter = 
                            VesselnessMeasureFilterType::New();

  ReaderType::Pointer   reader = ReaderType::New();
  WriterType::Pointer   writer = WriterType::New();
  
  reader->SetFileName( argv[1] );
  hessianFilter->SetInput( reader->GetOutput() );
  if( argc >= 4 )
    {
    hessianFilter->SetSigma( static_cast< double >(atof(argv[3])) );
    }
  vesselnessFilter->SetInput( hessianFilter->GetOutput() );
  writer->SetInput( vesselnessFilter->GetOutput() );
  writer->SetFileName( argv[2] );
  if( argc >= 5 )
    {
    vesselnessFilter->SetAlpha1( static_cast< double >(atof(argv[4])));
    }
  if( argc >= 6 )
    {
    vesselnessFilter->SetAlpha2( static_cast< double >(atof(argv[5])));
    }

  writer->Update();
  return EXIT_SUCCESS;
}

    
