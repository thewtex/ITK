/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDeformationFieldSourceTest.cxx
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


#include "itkImage.h"
#include "itkVector.h"
#include "itkDeformationFieldSource.h"
#include "itkImageFileWriter.h"
#include "itkFilterWatcher.h"

#include <fstream>


int itkDeformationFieldSourceTest( int argc, char * argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile outputImage" << std::endl;
    return EXIT_FAILURE;
    }

  const     unsigned int   Dimension = 2;
  typedef   float          VectorComponentType;

  typedef   itk::Vector< VectorComponentType, Dimension >    VectorType;

  typedef itk::Image< VectorType,  Dimension >   DeformationFieldType;

  typedef itk::DeformationFieldSource<
                                DeformationFieldType 
                                             >  FilterType;

  FilterType::Pointer filter = FilterType::New();

  FilterWatcher watcher(filter);

  DeformationFieldType::SpacingType spacing;
  spacing.Fill( 1.0 );

  DeformationFieldType::PointType origin;
  origin.Fill( 0.0 );

  DeformationFieldType::RegionType     region;
  DeformationFieldType::SizeType       size;
  DeformationFieldType::IndexType      start;

  size[0] = 128;
  size[1] = 128;

  start[0] = 0;
  start[1] = 0;

  region.SetSize( size );
  region.SetIndex( start );

  DeformationFieldType::DirectionType direction;
  direction.SetIdentity();


  filter->SetOutputSpacing( spacing );
  filter->SetOutputOrigin( origin );
  filter->SetOutputRegion( region );
  filter->SetOutputDirection( direction );



  //  Create source and target landmarks.
  //  
  typedef FilterType::LandmarkContainerPointer   LandmarkContainerPointer;
  typedef FilterType::LandmarkContainer          LandmarkContainerType;
  typedef FilterType::LandmarkPointType          LandmarkPointType;

  LandmarkContainerType::Pointer sourceLandmarks = LandmarkContainerType::New();
  LandmarkContainerType::Pointer targetLandmarks = LandmarkContainerType::New();

  LandmarkPointType sourcePoint;
  LandmarkPointType targetPoint;

  std::ifstream pointsFile;
  pointsFile.open( argv[1] );

  unsigned int pointId = 0;

  pointsFile >> sourcePoint;
  pointsFile >> targetPoint;

  while( !pointsFile.fail() )
    {
    sourceLandmarks->InsertElement( pointId, sourcePoint );
    targetLandmarks->InsertElement( pointId, targetPoint );
    pointId++;

    std::cout << sourcePoint << "  -->> " << targetPoint << std::endl;

    pointsFile >> sourcePoint;
    pointsFile >> targetPoint;

    }

  pointsFile.close();

  
  filter->SetSourceLandmarks( sourceLandmarks.GetPointer() );
  filter->SetTargetLandmarks( targetLandmarks.GetPointer() );

  try
    {
    filter->UpdateLargestPossibleRegion();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    }

  // Write an image for regression testing
  typedef itk::ImageFileWriter<  DeformationFieldType  > WriterType;

  WriterType::Pointer writer = WriterType::New();

  writer->SetInput (filter->GetOutput());
  writer->SetFileName( argv[2] );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown by writer" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;

}

