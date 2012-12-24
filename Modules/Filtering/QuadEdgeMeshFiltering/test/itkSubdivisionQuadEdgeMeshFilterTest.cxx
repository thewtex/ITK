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
#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#include "itkModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkLinearTriangleCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkLoopTriangleCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkSquareThreeTriangleCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkIterativeCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#if ( ITK_VERSION_MAJOR < 4 )
#include "itkQuadEdgeMeshSmoothing.h"
#else
#include "itkSmoothingQuadEdgeMeshFilter.h"
#endif
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

int itkSubdivisionQuadEdgeMeshFilterTest(int argc, char *argv[])
{
  if ( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputMeshFile  outputMeshFile subdivisionType Resolution non-uniform" << std::endl;
    std::cerr << " 0 : ModifiedButterfly " << std::endl;
    std::cerr << " 1 : Linear " << std::endl;
    std::cerr << " 2 : Loop " << std::endl;
    std::cerr << " 3 : Squarethree " << std::endl;
    return EXIT_FAILURE;
    }

  typedef float MeshPixelType;
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension > InputMeshType;
  typedef itk::QuadEdgeMesh< MeshPixelType, Dimension > OutputMeshType;

  typedef itk::QuadEdgeMeshToQuadEdgeMeshFilter< InputMeshType, OutputMeshType >                      QuadEdgeMeshFilterType;
  typedef itk::ModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >   ModifiedButterflySubdivisionFilterType;
  typedef itk::LinearTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >      LinearSubdivisionFilterType;
  typedef itk::LoopTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >        LoopSubdivisionFilterType;
  typedef itk::SquareThreeTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType > SquareThreeSubdivisionFilterType;

  typedef itk::IterativeCellSubdivisionQuadEdgeMeshFilter< InputMeshType, ModifiedButterflySubdivisionFilterType > IterativeModifiedButterflySubdivisionFilterType;
  typedef itk::IterativeCellSubdivisionQuadEdgeMeshFilter< InputMeshType, LinearSubdivisionFilterType > IterativeLinearSubdivisionFilterType;
  typedef itk::IterativeCellSubdivisionQuadEdgeMeshFilter< InputMeshType, LoopSubdivisionFilterType > IterativeLoopSubdivisionFilterType;
  typedef itk::IterativeCellSubdivisionQuadEdgeMeshFilter< InputMeshType, SquareThreeSubdivisionFilterType > IterativeSquareThreeSubdivisionFilterType;

  typedef itk::MeshFileReader< InputMeshType >  ReaderType;
  typedef itk::MeshFileWriter< OutputMeshType > WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  IterativeModifiedButterflySubdivisionFilterType::Pointer butterFlySubdivision = IterativeModifiedButterflySubdivisionFilterType::New();
  IterativeLinearSubdivisionFilterType::Pointer linearSubdivision = IterativeLinearSubdivisionFilterType::New();
  IterativeLoopSubdivisionFilterType::Pointer loopSubdivision = IterativeLoopSubdivisionFilterType::New();
  IterativeSquareThreeSubdivisionFilterType::Pointer squareThreeSubdivision = IterativeSquareThreeSubdivisionFilterType::New();

  if ( argc >= 5 )
    {
    unsigned int n = std::atoi(argv[4]);
    butterFlySubdivision->SetResolutionLevels(n);
    linearSubdivision->SetResolutionLevels(n);
    loopSubdivision->SetResolutionLevels(n);
    squareThreeSubdivision->SetResolutionLevels(n);
    }

  if ( argc >= 6 )
    {
    int type = std::atoi(argv[5]);
    if ( type )
      {
      IterativeModifiedButterflySubdivisionFilterType::OutputCellIdentifierListType cellsToBeSubdivided;

      cellsToBeSubdivided.push_back(0);
      cellsToBeSubdivided.push_back(1);
      cellsToBeSubdivided.push_back(2);
      cellsToBeSubdivided.push_back(3);
      cellsToBeSubdivided.push_back(5);
      cellsToBeSubdivided.push_back(6);
      cellsToBeSubdivided.push_back(9);

      butterFlySubdivision->SetCellsToBeSubdivided(cellsToBeSubdivided);
      linearSubdivision->SetCellsToBeSubdivided(cellsToBeSubdivided);
      loopSubdivision->SetCellsToBeSubdivided(cellsToBeSubdivided);
      squareThreeSubdivision->SetCellsToBeSubdivided(cellsToBeSubdivided);
      }
    else
      {
      butterFlySubdivision->AddSubdividedCellId(0);
      butterFlySubdivision->AddSubdividedCellId(1);
      butterFlySubdivision->AddSubdividedCellId(2);
      butterFlySubdivision->AddSubdividedCellId(3);
      butterFlySubdivision->AddSubdividedCellId(5);
      butterFlySubdivision->AddSubdividedCellId(6);
      butterFlySubdivision->AddSubdividedCellId(9);

      linearSubdivision->AddSubdividedCellId(0);
      linearSubdivision->AddSubdividedCellId(1);
      linearSubdivision->AddSubdividedCellId(2);
      linearSubdivision->AddSubdividedCellId(3);
      linearSubdivision->AddSubdividedCellId(5);
      linearSubdivision->AddSubdividedCellId(6);
      linearSubdivision->AddSubdividedCellId(9);

      loopSubdivision->AddSubdividedCellId(0);
      loopSubdivision->AddSubdividedCellId(1);
      loopSubdivision->AddSubdividedCellId(2);
      loopSubdivision->AddSubdividedCellId(3);
      loopSubdivision->AddSubdividedCellId(5);
      loopSubdivision->AddSubdividedCellId(6);
      loopSubdivision->AddSubdividedCellId(9);

      squareThreeSubdivision->AddSubdividedCellId(0);
      squareThreeSubdivision->AddSubdividedCellId(1);
      squareThreeSubdivision->AddSubdividedCellId(2);
      squareThreeSubdivision->AddSubdividedCellId(3);
      squareThreeSubdivision->AddSubdividedCellId(5);
      squareThreeSubdivision->AddSubdividedCellId(6);
      squareThreeSubdivision->AddSubdividedCellId(9);
      }
    }

  butterFlySubdivision->SetInput( reader->GetOutput() );
  linearSubdivision->SetInput( reader->GetOutput() );
  loopSubdivision->SetInput( reader->GetOutput() );
  squareThreeSubdivision->SetInput( reader->GetOutput() );

  OutputMeshType::Pointer output;
  if ( argc >= 4 )
    {
    int type = std::atoi(argv[3]);

    switch ( type )
      {
      case 0:
        butterFlySubdivision->Update();
        output = butterFlySubdivision->GetOutput();
        break;
      case 1:
        linearSubdivision->Update();
        output = linearSubdivision->GetOutput();
        break;
      case 2:
        loopSubdivision->Update();
        output = loopSubdivision->GetOutput();
        break;
      case 3:
        squareThreeSubdivision->Update();
        output = squareThreeSubdivision->GetOutput();
        break;
      default:
        std::cerr << "Invalid subdivision type : " << type << std::endl;
        return EXIT_FAILURE;
      }
    }
  else
    {
    std::cerr << "You must have subdivision type " << std::endl;
    return EXIT_FAILURE;
    }

  bool smoothing = true;
  if ( argc >= 7 )
  {
  smoothing = false;
  }

  if ( smoothing )
    {
#if ( ITK_VERSION_MAJOR < 4 )
    typedef itk::QuadEdgeMeshSmoothing< OutputMeshType, OutputMeshType > OutputMeshSmoothingFilterType;
#else
    typedef itk::SmoothingQuadEdgeMeshFilter< OutputMeshType, OutputMeshType > OutputMeshSmoothingFilterType;
#endif
    typedef itk::MatrixCoefficients< OutputMeshType >                          MatrixCoefficientsType;
    typedef itk::OnesMatrixCoefficients< OutputMeshType >                      OnesMatrixCoefficientsType;
    typedef itk::InverseEuclideanDistanceMatrixCoefficients< OutputMeshType >  InverseEuclideanDistanceMatrixCoefficientsType;
    typedef itk::ConformalMatrixCoefficients< OutputMeshType >                 ConformalMatrixCoefficientsType;
    typedef itk::AuthalicMatrixCoefficients< OutputMeshType >                  AuthalicMatrixCoefficientsType;
    typedef itk::IntrinsicMatrixCoefficients< OutputMeshType >                 IntrinsicMatrixCoefficientsType;
    typedef itk::HarmonicMatrixCoefficients< OutputMeshType >                  HarmonicMatrixCoefficientsType;

    OnesMatrixCoefficientsType             coef;
    OutputMeshSmoothingFilterType::Pointer meshSmoothingFilter = OutputMeshSmoothingFilterType::New();
    meshSmoothingFilter->SetInput( output );
    meshSmoothingFilter->SetCoefficientsMethod(&coef);
    meshSmoothingFilter->SetDelaunayConforming(1);
    meshSmoothingFilter->SetNumberOfIterations(1);
    meshSmoothingFilter->Update();

    output->Graft( meshSmoothingFilter->GetOutput() );
    }

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput( output );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while writting the output file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
