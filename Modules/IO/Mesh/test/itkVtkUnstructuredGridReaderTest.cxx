// ItkVtkUnstructuredGridReader.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
using namespace std;
#include <string>

#include <itkVTKUnstructuredGridReader.h>
#include <itkMesh.h>


int itkVtkUnstructuredGridReaderTest(int argc, char* argv[])
{
     if( argc != 2 )
     {
         std::cerr << "Usage: itkVtkUnstructuredGridReaderTest inputFilename"
           << std::endl;
         return EXIT_FAILURE;
      }

    typedef itk::Mesh<float, 3>                 MeshType;
    typedef itk::VTKUnstructuredGridReader< MeshType >  ReaderType;

    ReaderType::Pointer  UnstructuredGridReader = ReaderType::New();

    typedef ReaderType::PointType   PointType;
    typedef ReaderType::VectorType  VectorType;

    UnstructuredGridReader->SetFileName(argv[1]);

    try
    {
        UnstructuredGridReader->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
        std::cerr << "Error during Update() " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "UnstructuredGridReader:" << std::endl;
    std::cout << UnstructuredGridReader << std::endl;

    MeshType::Pointer mesh = UnstructuredGridReader->GetOutput();

    PointType  point;

    std::cout << "Testing itk::VTKUnstructuredGridReader" << std::endl;

    unsigned int numberOfPoints = mesh->GetNumberOfPoints();
    unsigned int numberOfCells  = mesh->GetNumberOfCells();

    std::cout << "numberOfPoints= " << numberOfPoints << std::endl;
    std::cout << "numberOfCells= " << numberOfCells << std::endl;
    std::cout << "" << std::endl;

    if( !numberOfPoints )
    {
        std::cerr << "ERROR: numberOfPoints= " << numberOfPoints << std::endl;
        return EXIT_FAILURE;
    }

    if( !numberOfCells )
    {
        std::cerr << "ERROR: numberOfCells= " << numberOfCells << std::endl;
        return EXIT_FAILURE;
    }

    for(unsigned int i=0; i<numberOfPoints; i++)
      {
      mesh->GetPoint(i, &point);
      }

    std::cout << "Test passed"<< std::endl;
    return EXIT_SUCCESS;

}
