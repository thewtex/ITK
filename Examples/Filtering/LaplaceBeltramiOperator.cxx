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

//  Software Guide : BeginLatex
//
//  The LaplaceBeltramiFilter defines scalar valued basis functions on a quad
//  edge triangulated mesh, then determines the N most significant eigenvalues
//  of the basis.
//
//  In this example, the user puts in a VTKPolyData file specifying a
//  triangulated mesh, open or closed, and a number of basis functions (N) to
//  calculate.  The example determines the Laplace-Beltrami basis functions,
//  and writes out N mesh files in VTKPolyDate format with the basis functions
//  specified as scalar data at each vertex.
//
//  Software Guide : EndLatex

#include "itkQuadEdgeMesh.h"
#include "itkLaplaceBeltramiFilter.h"
#include "itkMeshFileReader.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"

void showUsage()
{
  std::cerr << "USAGE: LaplaceBeltramioperator harmonicCount vtkMeshFile [boundaryCondition]\n";
  std::cerr << "\t\tharmonicCount : number of surface harmonics (basis functions) to calculate\n";
  std::cerr << "\t\tvtkMeshFile : file name of VTKPolyData input triangulated mesh data\n";
  std::cerr << "\t\tboundaryCondition : conditions placed on boundary points:\n";
  std::cerr << "\t\t\t1           = von Neuman (gradient of the Laplacian operator tangent to the boundary)\n";
  std::cerr << "\t\t\t2 (default) = Dirichlet (zero Laplacian operator on the boundary)\n";
  exit(1);
}


int main(int argc, char *argv[])
{
    if( argc != 3 && argc != 4 )
      {
      showUsage();
      return EXIT_FAILURE;
      }

    //  Software Guide : BeginLatex
    //
    //  Types should be selected based on required pixel type for the input
    //  and output meshes.
    //
    //  Software Guide : EndLatex


    double scale = 0.0;

    typedef float              PixelType;
    typedef double             PointDataType;
    typedef double             DDataType;
    typedef double             CoordRep;
    typedef double             InterpRep;
    const   unsigned int       Dimension = 3;

    // Declare the type of the input and output mesh
    typedef itk::QuadEdgeMeshTraits<PixelType, Dimension, PointDataType,
        DDataType, CoordRep, InterpRep>  MeshTraits;

    typedef itk::QuadEdgeMesh<float,Dimension,MeshTraits>   InMeshType;
    typedef itk::QuadEdgeMesh<double,Dimension,MeshTraits>  OutMeshType;

    typedef itk::LaplaceBeltramiFilter< InMeshType, OutMeshType >
                                    LbFilterType;

    // grab input parameters
    const unsigned int eCount = atoi( argv[1] );

    LbFilterType::BoundaryConditionEnumType boundaryCond =
                                LbFilterType::DIRICHLET;
    if( argc == 4 )
      {
      int boundaryConditionInput = atoi( argv[3] );
      switch (boundaryConditionInput)
        {
        case 1:
          boundaryCond = LbFilterType::VONNEUMAN;
          break;
        case 2:
          boundaryCond = LbFilterType::DIRICHLET;
          break;
        default:
          std::cerr << "\n** Invalid Boundary Condition Type:  " << argv[4] << ".  **\n";
          showUsage();
          break;
        }
      }

    // Here we recover the file names from the command line arguments
    const char* inFile = argv[2];

    //  We can now instantiate the types of the reader.
    typedef itk::MeshFileReader< InMeshType >  ReaderType;

    // create readers
    ReaderType::Pointer meshReader = ReaderType::New();

    //  The name of the file to be read or written is passed with the
    //  SetFileName() method.
    meshReader->SetFileName( inFile );

    try
      {
      meshReader->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Error freading file " << inFile << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }

    // get the objects
    InMeshType::Pointer mesh = meshReader->GetOutput();

    std::cout << "Vertex Count:  " <<
        mesh->GetNumberOfPoints() << std::endl;
    std::cout << "Cell Count:  " <<
        mesh->GetNumberOfCells() << std::endl;

    LbFilterType::Pointer lbFilter = LbFilterType::New();

    lbFilter->SetInput(mesh);
    lbFilter->SetEigenValueCount(eCount);
    lbFilter->SetHarmonicScaleValue(scale);
    lbFilter->SetBoundaryConditionType(boundaryCond);
    lbFilter->Update();

    OutMeshType::Pointer outMesh = lbFilter->GetOutput();

    //  We can now instantiate the types of the write.
    typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< OutMeshType >
            WriterType;

    // create writer
    WriterType::Pointer meshWriter = WriterType::New();

    std::string outfile;
    std::string outfileBase("SurfaceHarmonic");
#define VTKPDEXT ".vtk"

    for (unsigned int i = 0; i < eCount; i++)
      {
      if (lbFilter->SetSurfaceHarmonic(i))
        {
        WriterType::Pointer harmonicWriter = WriterType::New();
        outMesh = lbFilter->GetOutput();
        harmonicWriter->SetInput(outMesh);
        char countStr[8];
        sprintf(countStr, "_%d", i);
        outfile = outfileBase + countStr + VTKPDEXT;
        harmonicWriter->SetFileName(outfile.c_str());
        harmonicWriter->Update();
        }
      else
        {
        std::cerr << "Couldn't get harmonic #" << eCount << std::endl;
        }
      }
}
