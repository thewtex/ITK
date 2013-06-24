#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkLaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints.h"
#include "VNLSparseLUSolverTraits.h"

int itkLaplacianDeformationQuadEdgeMeshFilterWithSoftConstraintsTest( int argc, char* argv[] )
{
  // ** ERROR MESSAGE AND HELP ** //
  if( argc != 3 )
    {
    std::cout <<"Requires 3 argument: " <<std::endl;
    std::cout <<"1-Input file name " <<std::endl;
    std::cout <<"2-Output file name " <<std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef double CoordType;
  typedef itk::QuadEdgeMesh< CoordType, Dimension > MeshType;
  typedef itk::VTKPolyDataReader< MeshType > ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  typedef VNLSparseLUSolverTraits< CoordType > SolverType;
  typedef itk::LaplacianDeformationQuadEdgeMeshFilterWithSoftConstraints< MeshType, MeshType, SolverType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetOrder( 1 );
  filter->SetLambda( 1. );

  typedef itk::ConformalMatrixCoefficients< MeshType > CoefficientType;
  CoefficientType coeff;
  filter->SetCoefficientsMethod( &coeff );

  MeshType::VectorType null( 0. );
  filter->SetDisplacement( 150, null );
  filter->SetDisplacement( 2027, null );
  filter->SetDisplacement( 292, null );
  filter->SetDisplacement( 185, null );
  filter->SetDisplacement( 180, null );
  filter->SetDisplacement( 153, null );
  filter->SetDisplacement( 183, null );
  filter->SetDisplacement( 226, null );

  MeshType::VectorType d( null );
  d[2] = -0.1;

  filter->SetDisplacement( 3912, d );
  filter->SetDisplacement( 729, d );
  filter->SetDisplacement( 938, d );

  d[1] = 0.1;
  filter->SetDisplacement( 40, d );
  filter->SetDisplacement( 371, d );

  filter->Update();

  typedef itk::VTKPolyDataWriter< MeshType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  return EXIT_SUCCESS;
}
