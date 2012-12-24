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
#include "itkConditionalCellSubdivisionQuadEdgeMeshFilter.h"
#include "itkTriangleAreaCellSubdivisionCriterion.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#if ( ITK_VERSION_MAJOR < 4 )
#include "itkQuadEdgeMeshSmoothing.h"
#else
#include "itkSmoothingQuadEdgeMeshFilter.h"
#endif
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

int itkCriterionCellSubdivisionQuadEdgeMeshFilterTest(int argc, char *argv[])
{
  if ( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputMeshFile  outputMeshFile subdivisionType area" << std::endl;
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

  typedef itk::ModifiedButterflyTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >   ModifiedButterflySubdivisionFilterType;
  typedef itk::LinearTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >      LinearSubdivisionFilterType;
  typedef itk::LoopTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType >        LoopSubdivisionFilterType;
  typedef itk::SquareThreeTriangleCellSubdivisionQuadEdgeMeshFilter< OutputMeshType, OutputMeshType > SquareThreeSubdivisionFilterType;

  typedef itk::TriangleAreaCellSubdivisionCriterion< OutputMeshType > CriterionType;
  typedef itk::ConditionalCellSubdivisionQuadEdgeMeshFilter< InputMeshType, ModifiedButterflySubdivisionFilterType, CriterionType > ConditionalModifiedButterflySubdivisionFilterType;
  typedef itk::ConditionalCellSubdivisionQuadEdgeMeshFilter< InputMeshType, LinearSubdivisionFilterType, CriterionType > ConditionalLinearSubdivisionFilterType;
  typedef itk::ConditionalCellSubdivisionQuadEdgeMeshFilter< InputMeshType, LoopSubdivisionFilterType, CriterionType > ConditionalLoopSubdivisionFilterType;
  typedef itk::ConditionalCellSubdivisionQuadEdgeMeshFilter< InputMeshType, SquareThreeSubdivisionFilterType, CriterionType > ConditionalSquareThreeSubdivisionFilterType;

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

  ConditionalModifiedButterflySubdivisionFilterType::Pointer butterFlySubdivision = ConditionalModifiedButterflySubdivisionFilterType::New();
  ConditionalLinearSubdivisionFilterType::Pointer linearSubdivision = ConditionalLinearSubdivisionFilterType::New();
  ConditionalLoopSubdivisionFilterType::Pointer loopSubdivision = ConditionalLoopSubdivisionFilterType::New();
  ConditionalSquareThreeSubdivisionFilterType::Pointer squareThreeSubdivision = ConditionalSquareThreeSubdivisionFilterType::New();

  CriterionType::Pointer criterion = CriterionType::New();
  criterion->SetMaximumArea(1.0);
  if ( argc >= 5 )
    {
    float area = std::atof(argv[4]);
    criterion->SetMaximumArea(area);
    }

  butterFlySubdivision->SetSubdivisionCriterion( criterion );
  linearSubdivision->SetSubdivisionCriterion( criterion );
  loopSubdivision->SetSubdivisionCriterion( criterion );
  squareThreeSubdivision->SetSubdivisionCriterion( criterion );

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

  bool smoothing = false;
  if ( argc >= 6 )
  {
  smoothing = true;
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
