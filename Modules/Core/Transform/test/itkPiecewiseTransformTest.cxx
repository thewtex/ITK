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
#include "itkPiecewiseTransform.h"
#include "itkTranslationTransform.h"
#include "itkTestingMacros.h"
#include "itkDisplacementFieldTransform.h"

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
bool testVectorArray( const itk::SizeValueType size, const TVector & v1, const TVector & v2 )
{
  bool pass = true;

  for( unsigned int i = 0; i < size; i++ )
    {
    if( vcl_fabs( v1[i] - v2[i] ) > epsilon )
      {
      pass = false;
      }
    }
  return pass;
}

} // namespace

/*******************************************************/

const unsigned int itkPiecewiseTransformTestNDimensions = 2;

template
<class TScalar = double, unsigned int NDimensions = itkPiecewiseTransformTestNDimensions>
class PiecewiseTransformTestTransform : public itk::PiecewiseTransform<TScalar, NDimensions>
{
public:
  /** Standard class typedefs. */
  typedef PiecewiseTransformTestTransform                Self;
  typedef itk::PiecewiseTransform<TScalar, NDimensions>  Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( PiecewiseTransformTestTransform, PiecewiseTransform );

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Sub transform type **/
  typedef typename Superclass::TransformType                TransformType;
  typedef typename Superclass::TransformTypePointer         TransformTypePointer;
  /** InverseTransform type. */
  typedef typename Superclass::InverseTransformBasePointer  InverseTransformBasePointer;
  typedef typename Superclass::NumberOfParametersType       NumberOfParametersType;
  typedef typename Superclass::TransformQueueType           TransformQueueType;
  typedef typename Superclass::WeightArrayType              WeightArrayType;
  typedef typename Superclass::OffsetArrayType              OffsetArrayType;
  typedef typename Superclass::InputPointType               InputPointType;

  virtual NumberOfParametersType GetNumberOfLocalParametersAtPoint(const InputPointType  & point) const
  {
    TransformQueueType transforms;
    WeightArrayType    weights;
    OffsetArrayType    offsets;
    this->GetTransformListAndNormalizedWeightsAtPoint( point, transforms, weights, offsets );
    NumberOfParametersType result = 0;
    for( itk::SizeValueType n=0; n < transforms.size(); n++ )
      {
      result += transforms[n]->GetNumberOfLocalParametersAtPoint( point );
      }
    return result;
  }

  virtual bool GetNumberOfLocalParametersAtPointIsConstant() const
  {
    return false;
  }

  virtual void Initialize()
  {
  if( this->GetNumberOfTransforms() != 2 )
    {
    itkExceptionMacro("Expected 2 transforms.");
    }
  }

protected:
  PiecewiseTransformTestTransform(){};
  virtual ~PiecewiseTransformTestTransform(){};

  virtual typename itk::LightObject::Pointer InternalClone() const
  {
    return NULL;
  }

  virtual void GetTransformListAndNormalizedWeightsAtPoint( const InputPointType & point, TransformQueueType & transforms, WeightArrayType & weights, OffsetArrayType & fullOffsets ) const
  {
    //Simple test method
    transforms.clear();
    weights.clear();
    fullOffsets.clear();

    if( point[0] < 3.3 )
      {
      transforms.push_back( this->GetNthTransform(0) );
      weights.SetSize(1);
      weights[0] = 1.0;
      fullOffsets.SetSize(1);
      fullOffsets[0] = 0;
      return;
      }

    if( point[0] < 6.7 )
      {
      transforms.push_back( this->GetNthTransform(1) );
      weights.SetSize(1);
      weights[0] = 1.0;
      fullOffsets.SetSize(1);
      fullOffsets[0] = this->GetNthTransform(0)->GetNumberOfParameters();
      return;
      }

    //Otherwise, both are active
    transforms.push_back( this->GetNthTransform(0) );
    transforms.push_back( this->GetNthTransform(1) );
    weights.SetSize(2);
    //Unequal weights for better testing
    weights[0] = 1.0/3.0;
    weights[1] = 1.0 - weights[0];
    fullOffsets.SetSize(2);
    fullOffsets[0] = 0;
    fullOffsets[1] = this->GetNthTransform(0)->GetNumberOfParameters();
    return;
  }

private:
  PiecewiseTransformTestTransform( const Self & ); // purposely not implemented
  void operator=( const Self & );     // purposely not implemented

};

/********************************************/

int itkPiecewiseTransformTest(int, char *[] )
{
  const unsigned int NDimensions = itkPiecewiseTransformTestNDimensions;

  /* Create transform */
  typedef PiecewiseTransformTestTransform<double, NDimensions>  PiecewiseTransformType;
  typedef PiecewiseTransformType::Superclass                    Superclass;
  typedef Superclass::ScalarType                                ScalarType;

  PiecewiseTransformType::Pointer piecewiseTransform = PiecewiseTransformType::New();

  /* Add an affine transform */
  std::cout << "Adding affine" << std::endl;
  typedef itk::AffineTransform<ScalarType, NDimensions> AffineType;
  AffineType::Pointer affine = AffineType::New();
  AffineType::MatrixType         matrix2;
  AffineType::OffsetType         vector2;
  matrix2[0][0] = 0;
  matrix2[0][1] = 1;
  matrix2[1][0] = 1;
  matrix2[1][1] = 0;
  vector2[0] = -10.0;
  vector2[1] = 100.0;
  affine->SetMatrix(matrix2);
  affine->SetOffset(vector2);

  piecewiseTransform->AddTransform( affine );

  /* Add a displacement field transform */
  std::cout << std::endl << "Adding displacement transform" << std::endl << std::endl;
  typedef itk::DisplacementFieldTransform<double, NDimensions> DisplacementTransformType;
  DisplacementTransformType::Pointer displacementTransform = DisplacementTransformType::New();

  typedef DisplacementTransformType::DisplacementFieldType FieldType;
  FieldType::Pointer field = FieldType::New(); // This is based on itk::Image
  FieldType::SizeType   size;
  FieldType::IndexType  start;
  FieldType::RegionType region;
  int                   dimLength = 10;
  size.Fill( dimLength );
  start.Fill( 0 );
  region.SetSize( size );
  region.SetIndex( start );
  field->SetRegions( region );
  field->Allocate();

  DisplacementTransformType::OutputVectorType dispVector;
  dispVector[0] = 0.5;
  dispVector[1] = -0.5;
  field->FillBuffer( dispVector );
  displacementTransform->SetDisplacementField( field );
  piecewiseTransform->AddTransform( displacementTransform );

  /* weights to match those in test transform */
  PiecewiseTransformType::WeightArrayType weights(2);
  PiecewiseTransformType::WeightArrayType::ValueType weightAff, weightDisp;
  weightAff = weights[0] = 1.0/3.0;
  weightDisp = weights[1] = 1.0 - weights[0];

  piecewiseTransform->Initialize();

  /* TransformPoint
   */
  {
  std::cout << "Test TransformPoint" << std::endl;
  PiecewiseTransformType::OutputPointType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType point;
  point.Fill(1.0);

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformPoint( point );
  output = piecewiseTransform->TransformPoint( point );
  if( ! testPoint( output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformPoint( point );
  output = piecewiseTransform->TransformPoint( point );
  if( ! testPoint( output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformPoint( point );
  truthDisp = displacementTransform->TransformPoint( point );
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformPoint( point );
  if( ! testPoint( output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }
  }

  /* ComputeJacobianWithRespectToParameters */
  {
  std::cout << "Test ComputeJacobianWithRespectToParameters" << std::endl;
  PiecewiseTransformType::JacobianType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType point;
  point.Fill(1.0);

  /* affine only */
  point[0] = 1.0;
  affine->ComputeJacobianWithRespectToParameters( point, truthAff );
  piecewiseTransform->ComputeJacobianWithRespectToParameters( point, output );
  if( ! testJacobian( output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Jacobian truthAff: " << truthAff << std::endl;

  /* displacement only */
  point[0] = 4.0;
  displacementTransform->ComputeJacobianWithRespectToParameters( point, truthDisp );
  piecewiseTransform->ComputeJacobianWithRespectToParameters( point, output );
  if( ! testJacobian( output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "Jacobian truthDisp: " << truthDisp << std::endl;

  /* both */
  point[0] = 7.0;
  affine->ComputeJacobianWithRespectToParameters( point, truthAff );
  displacementTransform->ComputeJacobianWithRespectToParameters( point, truthDisp );
  truthBoth.SetSize( truthAff.rows(), truthAff.cols() + truthDisp.cols() );
  std::cout << "truthAff size: " << truthAff.rows() << ", " << truthAff.cols() << std::endl;
  for( itk::IndexValueType i=0; i < truthAff.rows(); i++ )
    {
    for( itk::IndexValueType j=0; j < truthAff.cols(); j++ )
      {
      truthBoth[i][j] = truthAff[i][j] * weightAff;
      }
    for( itk::IndexValueType j=0; j < truthDisp.cols(); j++ )
      {
      truthBoth[i][j+truthAff.cols()] = truthDisp[i][j] * weightDisp;
      }
    }
  piecewiseTransform->ComputeJacobianWithRespectToParameters( point, output );
  if( ! testJacobian( output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }
  }

  /* UpdateFullArrayWithLocalParametersAtPoint */
  {
  std::cout << "Test UpdateFullArrayWithLocalParametersAtPoint" << std::endl;
  PiecewiseTransformType::DerivativeType fullArray, localAff, localDisp, localBoth, truthAff, truthDisp, truthBoth;
  fullArray.SetSize( piecewiseTransform->GetNumberOfParameters() );
  truthAff.SetSize( piecewiseTransform->GetNumberOfParameters() );
  truthDisp.SetSize( piecewiseTransform->GetNumberOfParameters() );
  truthBoth.SetSize( piecewiseTransform->GetNumberOfParameters() );

  localAff.SetSize( affine->GetAggregateNumberOfLocalParameters() );
  localDisp.SetSize( displacementTransform->GetAggregateNumberOfLocalParameters() );
  localBoth.SetSize( localAff.Size() + localDisp.Size() );

  PiecewiseTransformType::InputPointType pointAff, pointDisp, pointBoth;
  pointAff[0] = 1.0;
  pointAff[1] = 1.0;
  pointDisp[0] = 4.0;
  pointDisp[1] = 0.0;
  pointBoth[0] = 7.0;
  pointBoth[1] = 1.0;

  truthAff.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  truthBoth.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  for( itk::IndexValueType i=0; i < localAff.Size(); i++ )
    {
    localAff[i] = 1.0 + i;
    localBoth[i] = localAff[i];
    truthAff[i] = localAff[i];
    truthBoth[i] = localAff[i]; //weights are not used for update
    }

  truthDisp.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  for( itk::IndexValueType i=0; i < localDisp.Size(); i++ )
    {
    localDisp[i] = 10.0 + i;
    localBoth[ localAff.Size() + i ] = localDisp[i];
    FieldType::IndexType index;

    field->TransformPhysicalPointToIndex( pointDisp, index );
    itk::OffsetValueType offsetDisp = affine->GetNumberOfParameters() + NDimensions * field->ComputeOffset( index );
    truthDisp[offsetDisp+i] = localDisp[i];


    field->TransformPhysicalPointToIndex( pointBoth, index );
    itk::OffsetValueType offsetBoth = affine->GetNumberOfParameters() + NDimensions * field->ComputeOffset( index );
    truthBoth[offsetBoth+i] = localDisp[i];
    }

  fullArray.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  piecewiseTransform->UpdateFullArrayWithLocalParametersAtPoint( fullArray, localAff, pointAff );
  if( ! testVectorArray( fullArray.Size(), fullArray, truthAff ) )
    {
    std::cout << "FAILED with UpdateFullArrayWithLocalParamaetersAtPoint for affine-only." << std::endl;
    return EXIT_FAILURE;
    }

  fullArray.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  piecewiseTransform->UpdateFullArrayWithLocalParametersAtPoint( fullArray, localDisp, pointDisp );
  if( ! testVectorArray( fullArray.Size(), fullArray, truthDisp ) )
    {
    std::cout << "FAILED with UpdateFullArrayWithLocalParamaetersAtPoint for displacement-only." << std::endl
              << "fullArray: " << std::endl << fullArray << std::endl << "truthDisp: " << std::endl << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  fullArray.Fill( itk::NumericTraits<PiecewiseTransformType::DerivativeType::ValueType>::ZeroValue() );
  piecewiseTransform->UpdateFullArrayWithLocalParametersAtPoint( fullArray, localBoth, pointBoth );
  if( ! testVectorArray( fullArray.Size(), fullArray, truthBoth ) )
    {
    std::cout << "FAILED with UpdateFullArrayWithLocalParamaetersAtPoint for both." << std::endl
              << "fullArray: " << std::endl << fullArray << std::endl << "truthBoth: " << std::endl << truthBoth << std::endl;
    return EXIT_FAILURE;
    }
  }

  /* TransformVector 1 */
  {
  std::cout << "Test TransformVector 1" << std::endl;
  PiecewiseTransformType::OutputVectorType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType   point;
  PiecewiseTransformType::InputVectorType  vector;
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformVector( vector, point );
  truthDisp = displacementTransform->TransformVector( vector, point );
  truthBoth = truthAff; //to set size
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformVector( vector ) );
  }

  /* TransformVector 2 */
  {
  std::cout << "Test TransformVector 2" << std::endl;
  PiecewiseTransformType::OutputVnlVectorType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType      point;
  PiecewiseTransformType::InputVnlVectorType  vector;
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformVector( vector, point );
  truthDisp = displacementTransform->TransformVector( vector, point );
  truthBoth = truthAff; //to set size
  for( itk::SizeValueType i=0; i < output.size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformVector( vector ) );
  }

  /* TransformVector 3 */
  {
  std::cout << "Test TransformVector 3" << std::endl;
  PiecewiseTransformType::OutputVectorPixelType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType        point;
  PiecewiseTransformType::InputVectorPixelType  vector(NDimensions);
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformVector( vector, point );
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformVector( vector, point );
  truthDisp = displacementTransform->TransformVector( vector, point );
  truthBoth = truthAff; //to set size
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformVector( vector ) );
  }

  /* TransformCovariantVector 1 */
  {
  std::cout << "Test TransformCovariantVector 1" << std::endl;
  PiecewiseTransformType::OutputCovariantVectorType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType            point;
  PiecewiseTransformType::InputCovariantVectorType  vector;
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformCovariantVector( vector, point );
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformCovariantVector( vector, point );
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformCovariantVector( vector, point );
  truthDisp = displacementTransform->TransformCovariantVector( vector, point );
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformCovariantVector( vector ) );
  }

  /* TransformCovariantVector 2 */
  {
  std::cout << "Test TransformCovariantVector 2" << std::endl;
  PiecewiseTransformType::OutputVectorPixelType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType        point;
  PiecewiseTransformType::InputVectorPixelType  vector(NDimensions);
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformCovariantVector( vector, point );
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformCovariantVector( vector, point );
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformCovariantVector( vector, point );
  truthDisp = displacementTransform->TransformCovariantVector( vector, point );
  truthBoth = truthAff; //get size
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformCovariantVector( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformCovariantVector( vector ) );
  }

  /* TransformDiffusionTensor3D 1 */
  {
  std::cout << "Test TransformDiffusionTensor3D 1" << std::endl;
  PiecewiseTransformType::OutputDiffusionTensor3DType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType              point;
  PiecewiseTransformType::InputDiffusionTensor3DType  vector;
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformDiffusionTensor3D( vector, point );
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformDiffusionTensor3D( vector, point );
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformDiffusionTensor3D( vector, point );
  truthDisp = displacementTransform->TransformDiffusionTensor3D( vector, point );
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformDiffusionTensor3D( vector ) );
  }

  /* TransformDiffusionTensor3D 2 */
  {
  std::cout << "Test TransformDiffusionTensor3D 2" << std::endl;
  PiecewiseTransformType::OutputVectorPixelType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType        point;
  PiecewiseTransformType::InputVectorPixelType  vector;
  point.Fill(1.0);
  vector.SetSize(3 * NDimensions);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformDiffusionTensor3D( vector, point );
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformDiffusionTensor3D( vector, point );
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformDiffusionTensor3D( vector, point );
  truthDisp = displacementTransform->TransformDiffusionTensor3D( vector, point );
  truthBoth = truthAff; //get size
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformDiffusionTensor3D( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformDiffusionTensor3D( vector ) );
  }

  /* TransformSymmetricSecondRankTensor 1 */
  {
  std::cout << "Test TransformSymmetricSecondRankTensor 1" << std::endl;
  PiecewiseTransformType::OutputSymmetricSecondRankTensorType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType                      point;
  PiecewiseTransformType::InputSymmetricSecondRankTensorType  vector;
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformSymmetricSecondRankTensor( vector, point );
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformSymmetricSecondRankTensor( vector, point );
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformSymmetricSecondRankTensor( vector, point );
  truthDisp = displacementTransform->TransformSymmetricSecondRankTensor( vector, point );
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformSymmetricSecondRankTensor( vector ) );
  }

  /* TransformSymmetricSecondRankTensor 2 */
  {
  std::cout << "Test TransformSymmetricSecondRankTensor 2" << std::endl;
  PiecewiseTransformType::OutputVectorPixelType output, truthAff, truthDisp, truthBoth;
  PiecewiseTransformType::InputPointType        point;
  PiecewiseTransformType::InputVectorPixelType  vector;
  vector.SetSize( NDimensions*NDimensions );
  point.Fill(1.0);
  for( itk::IndexValueType i=0; i < vector.Size(); i++ )
    {
    vector[i] = 1.0 + i;
    }

  /* affine only */
  point[0] = 1.0;
  truthAff  = affine->TransformSymmetricSecondRankTensor( vector, point );
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthAff ) )
    {
    std::cout << "FAILED for affine-only. output: " << output << " truth: " << truthAff << std::endl;
    return EXIT_FAILURE;
    }

  /* displacement only */
  point[0] = 4.0;
  truthDisp = displacementTransform->TransformSymmetricSecondRankTensor( vector, point );
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthDisp ) )
    {
    std::cout << "FAILED for displacement-only. output: " << output << " truth: " << truthDisp << std::endl;
    return EXIT_FAILURE;
    }

  /* both */
  point[0] = 7.0;
  truthAff  = affine->TransformSymmetricSecondRankTensor( vector, point );
  truthDisp = displacementTransform->TransformSymmetricSecondRankTensor( vector, point );
  truthBoth = truthAff; //get size
  for( itk::SizeValueType i=0; i < output.Size(); i++ )
    {
    truthBoth[i] = truthAff[i] * weightAff + truthDisp[i] * weightDisp;
    }
  output = piecewiseTransform->TransformSymmetricSecondRankTensor( vector, point );
  if( ! testVectorArray( output.Size(), output, truthBoth ) )
    {
    std::cout << "FAILED for both. output: " << output << " truth: " << truthBoth << std::endl;
    return EXIT_FAILURE;
    }

  /* Test for exception */
  TRY_EXPECT_EXCEPTION( piecewiseTransform->TransformSymmetricSecondRankTensor( vector ) );
  }

  return EXIT_SUCCESS;
}
