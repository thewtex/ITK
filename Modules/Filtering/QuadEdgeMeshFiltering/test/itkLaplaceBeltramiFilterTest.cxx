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
#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkLaplaceBeltramiFilter.h"

int itkLaplaceBeltramiFilterTest( int argc, char *argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " meshFile" << std::endl;
    std::cerr << " harmonicCount" << std::endl;
    return EXIT_FAILURE;
    }

  typedef float      MeshPixelType1;
  typedef double     MeshPixelType2;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType1, Dimension >   InMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType2, Dimension >   OutMeshType;

  typedef itk::VTKPolyDataReader< InMeshType >   ReaderType;

  typedef VNLSparseSymmetricEigensystemTraits<double>                     SSEType;
  typedef itk::LaplaceBeltramiFilter< InMeshType, OutMeshType, SSEType >  FilterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject& e )
    {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int eigenvalueCount = atoi( argv[2] );



  FilterType::Pointer filter = FilterType::New();

  std::cout << "Name of Class = " << filter->GetNameOfClass() << std::endl;

  std::cout << "Test Print() = " << std::endl;
  filter->Print( std::cout );

  // Intentionally run it without an input to test error management
  try
    {
    filter->Update();
    std::cerr << "Failed to throw expected exception" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << "SUCCESSFULLY caught expected exception" << std::endl;
    std::cout << excp << std::endl;
    }

  // Now setting it to an value before calculating LB
  try
    {
    filter->SetSurfaceHarmonic( 2000 );
    std::cerr << "Failed to throw expected exception" << std::endl;
    return EXIT_FAILURE;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cout << "SUCCESSFULLY caught expected exception" << std::endl;
    std::cout << excp << std::endl;
    }

  filter->SetEigenValueCount( eigenvalueCount );

  //
  //  Now connect the input and verify that it runs fine.
  //
  filter->SetInput( reader->GetOutput() );

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetSurfaceHarmonic( eigenvalueCount - 1 );

  // Now run the filter without computing eigenvalues.
  filter->SetEigenValueCount( 0 );
  std::cout << "Eigenvalue Count " << filter->GetEigenValueCount() << std::endl;

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
