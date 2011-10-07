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

    std::string InputFilename;
    //InputFilename = "C:\\DEVELOPING\\ItkVtkUnstructuredGridReader\\InputVTKFiles\\VerificationExamples\\ThreeTetrs.vtk";
    //InputFilename = "C:\\DEVELOPING\\ItkVtkUnstructuredGridReader\\InputVTKFiles\\VerificationExamples\\Brain1.vtk";
    //InputFilename = "C:\\DEVELOPING\\ItkVtkUnstructuredGridReader\\InputVTKFiles\\VerificationExamples\\Brain2.vtk";
    InputFilename = "C:\\DEVELOPING\\ItkVtkUnstructuredGridReader\\InputVTKFiles\\VerificationExamples\\Ventricle.vtk";


    std::cout << "Input file: " << InputFilename << std::endl;

    //typedef itk::PointSet<double, 3 > PointSetType;
    //PointSetType::Pointer pointsSet = PointSetType::New();
    //typedef PointSetType::PointType PointType;

    typedef itk::Mesh<float, 3>                 MeshType;
    typedef itk::VTKUnstructuredGridReader< MeshType >  ReaderType;

    ReaderType::Pointer  UnstructuredGridReader = ReaderType::New();

    typedef ReaderType::PointType   PointType;
    typedef ReaderType::VectorType  VectorType;

    UnstructuredGridReader->SetFileName(InputFilename.c_str());

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
    //polyDataReader->Update();

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

    /*
    for(unsigned int i=0; i<numberOfPoints; i++)
    {
    mesh->GetPoint(i, &point);
    }
    */

    // Retrieve points
    //for(unsigned int i = 0; i < numberOfPoints; i++)
    //{
    //	PointType pp;
    //	bool pointExists = mesh->GetPoint(i, &pp);

    //	if(pointExists)
    //	{
    //		std::cout << "Point is = " << pp << std::endl;
    //	}
    //}


    // Retrieve cells
    //std::cout << "" << std::endl;
    //MeshType::CellAutoPointer pCell;
    //const unsigned long* ids =0;
    //for(unsigned int i = 0; i < numberOfCells; i++)
    //{
    //	bool bCellExist = mesh->GetCell(i,pCell);

    //	if(bCellExist)
    //	{
    //		ids = pCell->GetPointIds();
    //		std::cout << "Cell Points = [" << ids[0] << "," << ids[1] << "," << ids[2]<< "," << ids[3] << "]" << std::endl;
    //	}
    //}

    return EXIT_SUCCESS;
}
