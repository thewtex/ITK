/* ----------------------------- MNI Header -----------------------------------
@NAME       : itIOTransformMincTest.cxx
@DESCRIPTION: MINC IO Transform test based on itkHDF5ImageIOTest.cxx
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include <iostream>
#include <fstream>
#include "itkMincTransformIOFactory.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFileReader.h"
#include "itkAffineTransform.h"
#include "itkTransformFactory.h"
#include "itksys/SystemTools.hxx"
#include "itkMincHelpers.h"
#include "itkDisplacementFieldTransform.h"

static int oneTest(const char *goodname,const char *gridname)
{
  unsigned int i;
  typedef itk::AffineTransform<double,3>                    AffineTransformType;
  typedef itk::DisplacementFieldTransform<double,3>         DisplacementFieldTransform;
  typedef DisplacementFieldTransform::DisplacementFieldType DisplacementFieldType;

  AffineTransformType::Pointer        affine = AffineTransformType::New();
  AffineTransformType::InputPointType cor;

  itk::ObjectFactoryBase::RegisterFactory(itk::MincTransformIOFactory::New() );

  // Set it's parameters
  AffineTransformType::ParametersType p = affine->GetParameters();
  for ( i = 0; i < p.GetSize(); i++ )
  {
    p[i] = i;
  }
  affine->SetParameters ( p );
  p = affine->GetFixedParameters ();
  for ( i = 0; i < p.GetSize(); i++ )
  {
    p[i] = i;
  }
  affine->SetFixedParameters ( p );
  itk::TransformFileWriter::Pointer writer;
  itk::TransformFileReader::Pointer reader;

  reader = itk::TransformFileReader::New();
  writer = itk::TransformFileWriter::New();
  writer->AddTransform(affine);

  writer->SetFileName( goodname );
  reader->SetFileName( goodname );

  // Testing writing std::cout << "Testing write : ";
  affine->Print ( std::cout );
  try
  {
    writer->Update();
    std::cout << std::endl;
    std::cout << "Testing read : " << std::endl;
    reader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Error while saving the transforms" << std::endl;
    std::cerr << excp << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    itk::TransformFileReader::TransformListType *list;
    list = reader->GetTransformList();
    itk::TransformFileReader::TransformListType::iterator lit = list->begin();
    while ( lit != list->end() )
    {
      (*lit)->Print ( std::cout );
      lit++;
    }
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Error while reading the transforms" << std::endl;
    std::cerr << excp << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  DisplacementFieldTransform::Pointer disp = DisplacementFieldTransform::New();
  DisplacementFieldType::Pointer field=DisplacementFieldType::New();

  //create zero displacement field
  DisplacementFieldType::SizeType    imageSize3D = {{ 10, 10, 10}};
  DisplacementFieldType::IndexType   startIndex3D = { {0, 0, 0}};

  double spacing[]={2.0, 2.0, 2.0};
  double origin[]={-10.0, -10.0, -10.0};
  DisplacementFieldType::RegionType  region;

  region.SetSize  (imageSize3D);
  region.SetIndex (startIndex3D);

  field->SetLargestPossibleRegion (region);
  field->SetBufferedRegion (region);
  field->SetRequestedRegion (region);

  field->SetSpacing( spacing );
  field->SetOrigin( origin );
  field->Allocate ();

  DisplacementFieldType::PixelType zeroDisplacement;
  zeroDisplacement.Fill( 0.0 );
  field->FillBuffer( zeroDisplacement );

  disp->SetDisplacementField(field);

  disp->Print ( std::cout );

  itk::TransformFileWriter::Pointer gridwriter;
  itk::TransformFileReader::Pointer gridreader;

  gridreader = itk::TransformFileReader::New();
  gridwriter = itk::TransformFileWriter::New();
  gridwriter->AddTransform(disp);
  gridwriter->SetFileName(gridname);
  gridreader->SetFileName(gridname);

  // Testing writing
  std::cout << "Testing write of non linear transform : " << std::endl;
  std::cout << std::flush;
  try
  {
    gridwriter->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Error while saving the transforms" << std::endl;
    std::cerr << excp << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  // Testing writing
  std::cout << "Testing read of non linear transform : " << std::endl;
  std::cout << std::flush;
  bool caught = false;
  try
  {
    gridreader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Error while reading the transforms" << std::endl;
    std::cerr << excp << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}

int secondTest()
{
  std::filebuf fb;
  fb.open("Rotation.xfm",std::ios::out);
  std::ostream os(&fb);
  os << "MNI Transform File"<<std::endl;
  os << "Transform_Type = Linear;"<<std::endl;
  os << "Linear_Transform ="<<std::endl;
  os << "1 0 0 0"<<std::endl;
  os << "0 0.866025447845459 -0.5 0"<<std::endl;
  os << "0 0.5 0.866025447845459 0;"<<std::endl;
  fb.close();

  itk::TransformFileReader::Pointer reader;
  reader = itk::TransformFileReader::New();
  reader->SetFileName("Rotation.xfm");

  reader->Update();

  itk::TransformFileReader::TransformListType *list;
  list = reader->GetTransformList();
  itk::TransformFileReader::TransformListType::iterator lit =  list->begin();
  while ( lit != list->end() )
  {
    (*lit)->Print ( std::cout );
    lit++;
  }
  return EXIT_SUCCESS;
}

int itkIOTransformMincTest(int argc, char* argv[])
{
  if (argc > 1)
  {
    itksys::SystemTools::ChangeDirectory(argv[1]);
  }
  itk::TransformFactory< itk::DisplacementFieldTransform<double,3> >::RegisterTransform ();

  int result1 =  oneTest("TransformLinear.xfm", "TransformNonLinear.xfm" );
  int result2 =  secondTest();
  return !( result1 == EXIT_SUCCESS&& result2 == EXIT_SUCCESS);
}
