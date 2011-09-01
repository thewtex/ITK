/*=========================================================================
 *
 *   Program:   Insight Segmentation & Registration Toolkit
 *   Module:
 *   Language:
 *   Date:
 *   Version:
 *
 *   Copyright (c) Insight Software Consortium. All rights reserved.
 *   See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notices for more information.
 *
 *=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkSupervisedBayesianClassifierInitializationImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//This is an example of the itk::SupervisedBayesianClassifierInitializationImageFilter.
//The example's goal is to serve as an initializer for the
//BayesianClassifier.cxx example also found in this directory.
//
//This example takes an input image (to be classified) and generates membership
//images. The membership images determine the degree to which each pixel
//belongs to a class.
//
//
int itkSupervisedBayesianClassifierInitializationImageFilterTest(int argc, char *argv[])
{

  const unsigned int Dimension = 2;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage MembershipImage numberOfClasses "
      <<"mean1 mean2 mean3 mean4 mean5... cov1 cov2 cov3 cov4 cov5..." << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< unsigned char, Dimension > ImageType;
  typedef itk::SupervisedBayesianClassifierInitializationImageFilter< ImageType >
                                                BayesianInitializerType;
  BayesianInitializerType::Pointer bayesianInitializer
                                          = BayesianInitializerType::New();

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  unsigned int numberOfClasses = atoi( argv[3] );

  bayesianInitializer->SetInput( reader->GetOutput() );
  //input the number of classes
  bayesianInitializer->SetNumberOfClasses( numberOfClasses );
  //input the Mean and Cov to the filter
  for( unsigned k=0; k < numberOfClasses; k++ )
  {
    const double Mean = atof( argv[k+3] );
    const double Cov  = atof( argv[k+3+numberOfClasses]);
    bayesianInitializer->AddMean( Mean );
    bayesianInitializer->AddCov(Cov);
  }



  typedef itk::ImageFileWriter<
    BayesianInitializerType::OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( bayesianInitializer->GetOutput() );
  writer->SetFileName( argv[2] );

  try
    {
    bayesianInitializer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }



  return EXIT_SUCCESS;
}
