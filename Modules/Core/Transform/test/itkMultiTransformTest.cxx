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

#include <iostream>

#include "itkAffineTransform.h"
#include "itkMultiTransform.h"
#include "itkTranslationTransform.h"
#include "itkTestingMacros.h"

namespace
{

const double epsilon = 1e-10;

template <typename TPoint>
bool testPoint( const TPoint & p1, const TPoint & p2 )
{
  bool pass = true;

  for( unsigned int i = 0; i < TPoint::PointDimension; i++ )
    {
    if( vcl_fabs( p1[i] - p2[i] ) > epsilon )
      {
      pass = false;
      }
    }
  return pass;
}

template <typename TMatrix>
bool testMatrix( const TMatrix & m1, const TMatrix & m2 )
{
  unsigned int i, j;
  bool         pass = true;

  for( i = 0; i < TMatrix::RowDimensions; i++ )
    {
    for( j = 0; j < TMatrix::ColumnDimensions; j++ )
      {
      if( vcl_fabs( m1[i][j] - m2[i][j] ) > epsilon )
        {
        pass = false;
        }
      }
    }
  return pass;
}

template <typename TArray2D>
bool testJacobian( const TArray2D & m1, const TArray2D & m2 )
{
  unsigned int i, j;
  bool         pass = true;

  for( i = 0; i < m1.rows(); i++ )
    {
    for( j = 0; j < m1.cols(); j++ )
      {
      if( vcl_fabs( m1[i][j] - m2[i][j] ) > epsilon )
        {
        pass = false;
        }
      }
    }
  return pass;
}

template <typename TVector>
bool testVectorArray( const TVector & v1, const TVector & v2 )
{
  bool pass = true;

  for( unsigned int i = 0; i < v1.Size(); i++ )
    {
    if( vcl_fabs( v1[i] - v2[i] ) > epsilon )
      {
      pass = false;
      }
    }
  return pass;
}

} // namespace

/******/

const unsigned int itkMultiTransformBaseTestNDimensions = 2;

template
<class TScalar = double, unsigned int NDimensions = itkMultiTransformBaseTestNDimensions>
class itkMultiTransformBaseTestTransform :
  public MultiTransformBase<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef MultiTransformBaseTestTransform                       Self;
  typedef MultiTransformBase<TScalar, NDimensions, NDimensions> Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MultiTransformBaseTestTransform, MultiTransformBase );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Sub transform type **/
  typedef typename Superclass::TransformType                TransformType;
  typedef typename Superclass::TransformTypePointer         TransformTypePointer;
  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer  InverseTransformBasePointer;

  virtual void UpdateTransformParameters( const DerivativeType &, ScalarType )
  {
  }

private:
  MultiTransformBaseTestTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );     // purposely not implemented

}

/******/

int itkMultiTransformBaseTest(int, char *[] )
{

  /* Create multi-transform */
  typedef itkMultiTransformBaseTestTransform<double, itkMultiTransformBaseTestNDimensions>  MultiTransformType;
  typedef MultiTransformType::Superclass                                                    Superclass;
  typedef Superclass::ScalarType                                                            ScalarType;

  MultiTransformType::Pointer multiTransform = MultiTransformType::New();

  /* Test obects */
  typedef  itk::Matrix<ScalarType, NDimensions, NDimensions> Matrix2Type;
  typedef  itk::Vector<ScalarType, NDimensions>              Vector2Type;

  /* Test that we have an empty the queue */
  if( multiTransform->GetNumberOfTransforms() != 0 )
    {
    std::cout << "Failed. Expected GetNumberOfTransforms to return 0." << std::endl;
    return EXIT_FAILURE;
    }

  if( ! multiTransform->IsTransformQueueEmpty() )
    {
    std::cout << "ERROR: expected IsTransformQueueEmpty to return true." << std::endl;
    return EXIT_FAILURE;
    }

  /* Add an affine transform */
  typedef itk::AffineTransform<ScalarType, NDimensions> AffineType;
  AffineType::Pointer affine = AffineType::New();
  Matrix2Type         matrix2;
  Vector2Type         vector2;
  matrix2[0][0] = 0;
  matrix2[0][1] = -1;
  matrix2[1][0] = 1;
  matrix2[1][1] = 0;
  vector2[0] = 10;
  vector2[1] = 100;
  affine->SetMatrix(matrix2);
  affine->SetOffset(vector2);

  multiTransform->AddTransform( affine );

  /* Test that we have one transform in the queue */
  if( multiTransform->IsTransformQueueEmpty() )
    {
    std::cout << "ERROR: expected IsTransformQueueEmpty to return false." << std::endl;
    return EXIT_FAILURE;
    }
  if( multiTransform->GetNumberOfTransforms() != 1 )
    {
    std::cout << "Failed adding transform to queue." << std::endl;
    return EXIT_FAILURE;
    }

  /* Retrieve the transform and check that it's the same */
  std::cout << "Retrieve 1st transform." << std::endl;
  AffineType::ConstPointer affineGet;
  affineGet = dynamic_cast<AffineType const *>( multiTransform->GetNthTransform(0).GetPointer() );
  if( affineGet.IsNull() )
    {
    std::cout << "Failed retrieving transform from queue." << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Retrieve matrix and offset. " << std::endl;
  Matrix2Type matrix2Get = affineGet->GetMatrix();
  Vector2Type vector2Get = affineGet->GetOffset();
  if( !testMatrix(matrix2, matrix2Get) || !testVectorArray(vector2, vector2Get ) )
    {
    std::cout << "Failed retrieving correct transform data." << std::endl;
    return EXIT_FAILURE;
    }

  /* Get parameters with single transform.
   * Should be same as GetParameters from affine transform. */
  std::cout << "Get Parameters: " << std::endl;
  Superclass::ParametersType parametersTest, parametersTruth;
  parametersTest = multiTransform->GetParameters();
  parametersTruth = affine->GetParameters();
  std::cout << "affine parametersTruth: " << std::endl << parametersTruth << std::endl
            << "parametersTest from Multi: " << std::endl << parametersTest << std::endl;

  if( !testVectorArray( parametersTest, parametersTruth ) )
    {
    std::cout << "Failed GetParameters() for single transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Set parameters with single transform. */
  Superclass::ParametersType parametersNew(6), parametersReturned;
  parametersNew[0] = 0;
  parametersNew[1] = 10;
  parametersNew[2] = 20;
  parametersNew[3] = 30;
  parametersNew[4] = 40;
  parametersNew[5] = 50;
  std::cout << "Set Parameters: " << std::endl;
  multiTransform->SetParameters( parametersNew );
  std::cout << "retrieving... " << std::endl;
  parametersReturned = multiTransform->GetParameters();
  std::cout << "parametersNew: " << std::endl << parametersNew << std::endl
            << "parametersReturned: " << std::endl << parametersReturned << std::endl;
  if( !testVectorArray( parametersNew, parametersReturned ) )
    {
    std::cout << "Failed SetParameters() for single transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test fixed parameters set/get */
  parametersTest = multiTransform->GetFixedParameters();
  parametersTruth = affine->GetFixedParameters();
  std::cout << "Get Fixed Parameters: " << std::endl
            << "affine parametersTruth: " << std::endl << parametersTruth << std::endl
            << "parametersTest from Multi: " << std::endl << parametersTest << std::endl;

  if( !testVectorArray( parametersTest, parametersTruth ) )
    {
    std::cout << "Failed GetFixedParameters() for single transform." << std::endl;
    return EXIT_FAILURE;
    }

  parametersNew.SetSize( parametersTruth.Size() );
  parametersNew.Fill(1);
  parametersNew[0] = 42;

  std::cout << "Set Fixed Parameters: " << std::endl;
  multiTransform->SetFixedParameters( parametersNew );
  std::cout << "retrieving... " << std::endl;
  parametersReturned = multiTransform->GetFixedParameters();
  std::cout << "parametersNew: " << std::endl << parametersNew << std::endl
            << "parametersReturned: " << std::endl << parametersReturned
            << std::endl;
  if( !testVectorArray( parametersNew, parametersReturned ) )
    {
    std::cout << "Failed SetFixedParameters() for single transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Reset affine transform to original values */
  multiTransform->ClearTransformQueue();

  if( multiTransform->GetNumberOfTransforms() != 0 )
    {
    std::cout << "Failed. Expected GetNumberOfTransforms to return 0 after ClearTransformQueue." << std::endl;
    return EXIT_FAILURE;
    }

  affine = AffineType::New();
  affine->SetMatrix(matrix2);
  affine->SetOffset(vector2);
  multiTransform->AddTransform( affine );

  /* Test inverse */
  Superclass::Pointer         inverseTransform = Superclass::New();
  Superclass::OutputPointType inverseTruth, inverseOutput;
  if( ! multiTransform->GetInverse( inverseTransform ) )
    {
    std::cout << "ERROR: GetInverse() failed." << std::endl;
    return EXIT_FAILURE;
    }
  AffineType inverseAffine = AffineType::New();
  if( ! affine->GetInverse( inverseAffine ) )
    {
    std::cout << "FAILED getting inverse of affine." << std::endl;
    return EXIT_FAILURE;
    }
  if( ! testVectorArray(inverseAffine->GetParameters(), inverseTransform->GetParameters() ) )
    {
    std::cout << "ERROR: Wrong parameters for single-transform inverse." << std::endl;
    return EXIT_FAILURE;
    }

  /*
   * Create and add 2nd transform
   */
  AffineType::Pointer affine2 = AffineType::New();
  matrix2[0][0] = 11;
  matrix2[0][1] = 22;
  matrix2[1][0] = 33;
  matrix2[1][1] = 44;
  vector2[0] = 55;
  vector2[1] = 65;
  affine2->SetMatrix(matrix2);
  affine2->SetOffset(vector2);

  multiTransform->ClearTransformQueue();
  multiTransform->AppendTransform( affine2 );
  multiTransform->PrependTransform( affine );

  std::cout << std::endl << "Two-component Multi Transform:" << std::endl << multiTransform;
);
  if( multiTransform->GetNthTransform(0) != affine )
    {
    std::cout << "ERROR: 1st transform is not affine as expected." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test that we have two tranforms in the queue */
  if( multiTransform->GetNumberOfTransforms() != 2 )
    {
    std::cout << "Failed adding 2nd transform to queue." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test inverse with two transforms. */
  if( !multiTransform->GetInverse( inverseTransform ) )
    {
    std::cout << "Expected GetInverse() to succeed." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Inverse two-component transform: " << inverseTransform;

  /* Test inverse parameters using settings from above. */
  if( ! testVectorArray(inverseAffine->GetParameters(), inverseTransform->GetNthTransform(0)->GetParameters() ) )
    {
    std::cout << "ERROR: Wrong parameters for affine in two-transform inverse." << std::endl;
    return EXIT_FAILURE;
    }
  AffineType inverseAffine2 = AffineType::New();
  if( ! affine2->GetInverse( inverseAffine2 ) )
    {
    std::cout << "FAILED getting inverse of affine2." << std::endl;
    return EXIT_FAILURE;
    }
  if( ! testVectorArray(inverseAffine2->GetParameters(), inverseTransform->GetNthTransform(1)->GetParameters() ) )
    {
    std::cout << "ERROR: Wrong parameters for affine2 in two-transform inverse." << std::endl;
    return EXIT_FAILURE;
    }

  /* Get inverse transform again, but using other accessor. */
  Superclass::ConstPointer inverseTransform2;
  std::cout << "Call GetInverseTransform():" << std::endl;
  inverseTransform2 = dynamic_cast<const MultiTransformType *>( multiTransform->GetInverseTransform().GetPointer() );
  if( !inverseTransform2 )
    {
    std::cout << "Failed calling GetInverseTransform()." << std::endl;
    return EXIT_FAILURE;
    }
  if( ! testVectorArray(inverseTransform->GetParameters(), inverseTransform2->GetParameters() ) )
    {
    std::cout << "ERROR: parameters of inverseTransform do not match inverseTransform2" << std::endl;
    return EXIT_FAILURE;
    }

  /* Test IsLinear() */
  std::cout << "Test IsLinear" << std::endl;
  if( ! multiTransform->IsLinear() )
    {
    std::cout << "ERROR: expected true for IsLinear but got false." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test GetNumberOfParameters */
  std::cout << "GetNumberOfParameters: " << std::endl;
  unsigned int affineParamsN = affine->GetNumberOfParameters();
  unsigned int affine2ParamsN = affine2->GetNumberOfParameters();
  unsigned int nParameters = multiTransform->GetNumberOfParameters();
  std::cout << "Number of parameters: " << nParameters << std::endl;
  if( nParameters != affineParamsN + affine2ParamsN )
    {
    std::cout << "GetNumberOfParameters failed for two-transform." << std::endl << "Expected " << affineParamsN + affine2ParamsN << std::endl;
    }

  /* Test GetNumberOfLocalParameters */
  std::cout << "GetNumberOfLocalParameters: " << std::endl;
  unsigned int affineLocalParamsN = affine->GetNumberOfLocalParameters();
  unsigned int affine2LocalParamsN = affine2->GetNumberOfLocalParameters();
  unsigned int nLocalParameters = multiTransform->GetNumberOfLocalParameters();
  std::cout << "Number of local parameters: " << nParameters << std::endl;
  if( nLocalParameters != affineLocalParamsN + affine2LocalParamsN )
    {
    std::cout << "GetNumberOfLocalParameters failed for two-transform." << std::endl << "Expected " << affineLocalParamsN + affine2LocalParamsN << std::endl;
    }

  /* Get parameters with multi-transform. They're filled from transforms in
   * same order as transforms are applied. */
  parametersTest = multiTransform->GetParameters();
  parametersTruth.SetSize( affine2ParamsN + affineParamsN );
  /* Fill using different method than is used in the class. */
  for( unsigned int n = 0; n < affineParamsN; n++ )
    {
    parametersTruth.SetElement( n, affine->GetParameters().GetElement( n ) );
    }
  for( unsigned int n = 0; n < affine2ParamsN; n++ )
    {
    parametersTruth.SetElement( n + affine1ParamsN, affine2->GetParameters().GetElement( n ) );
    }
  std::cout << "Get Multi-transform Parameters: " << std::endl << "parametersTruth: " << std::endl << parametersTruth << std::endl
            << "parametersTest from Multi: " << std::endl << parametersTest << std::endl;

  if( !testVectorArray( parametersTest, parametersTruth ) )
    {
    std::cout << "Failed GetParameters() for two-transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Set parameters with multi transform. */
  parametersNew.SetSize( parametersTruth.Size() );
  parametersNew.Fill( 3.14 );
  parametersNew[0] = 19;
  parametersNew[parametersTruth.Size() - 1] = 71;
  std::cout << "Set Multi-transform Parameters: " << std::endl;
  multiTransform->SetParameters( parametersNew );
  std::cout << "retrieving... " << std::endl;
  parametersReturned = multiTransform->GetParameters();
  std::cout << "parametersNew: " << std::endl << parametersNew << std::endl << "parametersReturned: " << std::endl << parametersReturned << std::endl;
  if( !testVectorArray( parametersNew, parametersReturned ) )
    {
    std::cout << "Failed SetParameters() for two-transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test get fixed parameters with multi-transform */
  parametersTest = multiTransform->GetFixedParameters();
  affineParamsN = affine->GetFixedParameters().Size();
  affine2ParamsN = affine2->GetFixedParameters().Size();
  parametersTruth.SetSize( affine2ParamsN + affineParamsN );
  parametersTruth.Fill(0); // Try this to quiet valgrind
  for( unsigned int n = 0; n < affineParamsN; n++ )
    {
    parametersTruth.SetElement( n, affine->GetFixedParameters().GetElement( n ) );
    }
  for( unsigned int n = 0; n < affine2ParamsN; n++ )
    {
    parametersTruth.SetElement( n + affine1ParamsN, affine2->GetFixedParameters().GetElement( n ) );
    }
  std::cout << "Get Multi-transform Fixed Parameters: " << std::endl << "parametersTruth: " << std::endl << parametersTruth << std::endl
            << "parametersTest: " << std::endl << parametersTest << std::endl;

  if( !testVectorArray( parametersTest, parametersTruth ) )
    {
    std::cout << "Failed GetFixedParameters() for multi transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test set fixed parameters with multi-transform */
  std::cout << "Set Multi-transform Fixed Parameters: " << std::endl;
  multiTransform->SetFixedParameters( parametersTruth );
  std::cout << "retrieving... " << std::endl;
  parametersReturned = multiTransform->GetFixedParameters();
  std::cout << "parametersTruth: " << std::endl << parametersTruth << std::endl
            << "parametersReturned: " << std::endl << parametersReturned
            << std::endl;

  if( ! testVectorArray( parametersTruth, parametersReturned ) )
    {
    std::cout << "Failed SetFixedParameters() for multi transform." << std::endl;
    return EXIT_FAILURE;
    }

  /* Test GetNumberOfFixedParameters */
  std::cout << "GetNumberOfFixedParameters: " << std::endl;
  unsigned int affineFixedParamsN = affine->GetNumberOfFixedParameters();
  unsigned int affine2FixedParamsN = affine2->GetNumberOfFixedParameters();
  unsigned int nFixedParameters = multiTransform->GetNumberOfFixedParameters();
  std::cout << "Number of Fixed parameters: " << nParameters << std::endl;
  if( nFixedParameters != affineFixedParamsN + affine2FixedParamsN )
    {
    std::cout << "GetNumberOfFixedParameters failed for two-transform." << std::endl << "Expected " << affineFixedParamsN + affine2FixedParamsN << std::endl;
    }


  /*
   * Add a third transform
   */
  AffineType::Pointer affine3 = AffineType::New();
  matrix2[0][0] = 1.1;
  matrix2[0][1] = 2.2;
  matrix2[1][0] = 3.3;
  matrix2[1][1] = 4.4;
  vector2[0] = 5.5;
  vector2[1] = 6.5;
  affine3->SetMatrix(matrix2);
  affine3->SetOffset(vector2);

  multiTransform->AddTransform( affine3 );

  /* Reset first affine to non-singular values */
  matrix2[0][0] = 1;
  matrix2[0][1] = 2;
  matrix2[1][0] = 3;
  matrix2[1][1] = 4;
  vector2[0] = 5;
  vector2[1] = 6;
  affine->SetMatrix(matrix2);
  affine->SetOffset(vector2);

  /* Test accessors */
  Superclass::TransformQueueType transformQueue = multiTransform->GetTransformQueue();
  if( transformQueue.size() != 3 )
    {
    std::cout << "Failed getting transform queue." << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Got TransformQueue." << std::endl;

  /* Get inverse */
  Superclass::ConstPointer inverseTransform3;
  inverseTransform3 = dynamic_cast<const MultiTransformType *>( multiTransform->GetInverseTransform().GetPointer() );
  if( !inverseTransform3 )
    {
    std::cout << "Failed calling GetInverseTransform() (3)." << std::endl;
    return EXIT_FAILURE;
    }

//consider adding a displ field txf instead of affine2 above.

  /* Test UpdateTransformParameters.
   * NOTE Once there are transforms that do something other than simple
   * addition in TransformUpdateParameters, this should be updated here.
   */
    {
    std::cout << "Testing UpdateTransformParameters 1. " << std::endl;
    Superclass::ParametersType truth = multiTransform->GetParameters();
    Superclass::DerivativeType
    update( multiTransform->GetNumberOfParameters() );
    update.Fill(10.0);
    AffineType::ScalarType factor = 0.5;
    truth += update * factor;
    multiTransform->UpdateTransformParameters( update, factor );
    Superclass::ParametersType updateResult = multiTransform->GetParameters();
    if( !testVectorArray( truth, updateResult ) )
      {
      std::cout << "UpdateTransformParameters 1 failed. " << std::endl << " truth:  " << truth << std::endl << " result: " << updateResult << std::endl;
      return EXIT_FAILURE;
      }

    /* Test with wrong size for update */
    update.SetSize(1);
    TRY_EXPECT_EXCEPTION( multiTransform->UpdateTransformParameters( update, factor ) );
    }

  /* Test SetParameters with wrong size array */
  std::cout << std::endl << "Test SetParameters with wrong size array. EXPECT EXCEPTION:" << std::endl;
  parametersTruth.SetSize(1);
  TRY_EXPECT_EXCEPTION( multiTransform->SetParameters( parametersTruth ) );

  /* Test SetFixedParameters with wrong size array */
  std::cout << std::endl << "Test SetFixedParameters with wrong size array. EXPECT EXCEPTION:" << std::endl;
  TRY_EXPECT_EXCEPTION( multiTransform->SetFixedParameters( parametersTruth ) );

  /* GetTransformCategory */
  if( multiTransform->GetTransformCategory() != affine->GetTransformCategory() )
    {
    std::cout << "ERROR: GetTransformCategory returned " << multiTransform->GetTransformCategory() << " instead of " << affine->GetTransformCategory() << std::endl;
    return EXIT_FAILURE;
    }

  /* RemoveTransform. From end of queue */
  multiTransform->RemoveTransform();
  if( multiTransform->GetNumberOfTransforms() != 2 )
    {
    std::cout << "ERROR: Wrong number of transfroms after RemoveTransform: " << multiTransform->GetNumberOfTransforms() << std::endl;
    return EXIT_FAILURE;
    }

  /* Get front and back transforms */
  if( multiTransform->GetFrontTransform() != affine )
    {
    std::cout << "ERROR: GetFrontTransform failed." << std::endl;
    }
  if( multiTransform->GetBackTransform() != affine2 )
    {
    std::cout << "ERROR: GetBackTransform failed." << std::endl;
    }

  /* Test printing */
  multiTransform->Print(std::cout);

  std::cout << "Passed test!" << std::endl;
  return EXIT_SUCCESS;

}
