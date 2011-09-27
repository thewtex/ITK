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
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkPointSet.h"
#include "itkFEMScatteredDataPointSetToImageFilter.h"
/**
 * In this test, we create a simple 2D mesh and feature points to do evaluation.
 *
 * Example:
 *
 * 4   ----------------
 *   |  * |   |   |   |
 * 3 |----|---|---*---|
 *   |    |   |   |   |
 * 2 |----|---|---|---|
 *   |    | * |   |   |
 * 1 |----|---|---*---|
 *   |    |   |   |   |
 *   ----------------
 *  0     1   2   3   4
 *
 */
int itkFEMScatteredDataPointSetToImageFilterTest( int argc, char * argv [] )
{
  const unsigned int ParametricDimension = 2;
  const unsigned int DataDimension = 2;

  typedef float                                         RealType;
  typedef itk::Vector<RealType, DataDimension>          VectorType;
  typedef itk::Matrix<RealType, DataDimension,
      DataDimension>                                    MatrixType;
  typedef itk::Image<VectorType, ParametricDimension>   DeformationFieldType;
  typedef itk::PointSet
    <VectorType, ParametricDimension>                   FeaturePointSetType;
  typedef itk::PointSet
    <MatrixType, ParametricDimension>                   TensorPointSetType;
  typedef itk::PointSet
    <RealType, ParametricDimension>                     ConfidencePointSetType;
  typedef itk::Mesh< VectorType, ParametricDimension >  MeshType;
  typedef FeaturePointSetType::PointType                PointType;

  typedef itk::fem::FEMScatteredDataPointSetToImageFilter
    <FeaturePointSetType, MeshType, DeformationFieldType, ConfidencePointSetType,
    TensorPointSetType>                FilterType;

  FilterType::Pointer filter = FilterType::New();

  //Construct a feature point set
  FeaturePointSetType::Pointer featurePoints = FeaturePointSetType::New();
  PointType p0;
  PointType p1;
  PointType p2;
  PointType p3;
  p0[0] = 3.0;
  p0[1] = 1.0;
  p1[0] = 1.5;
  p1[1] = 1.5;
  p2[0] = 3.0;
  p2[1] = 3.0;
  p3[0] = 0.5;
  p3[1] = 3.5;


  VectorType u0;
  VectorType u1;
  VectorType u2;
  VectorType u3;
  u0[0] = 1.0;
  u0[1] = 0.0;
  u1[0] = 1.0;
  u1[1] = 0.0;
  u2[0] = 1.0;
  u2[1] = 0.0;
  u3[0] = 1.0;
  u3[1] = 0.0;

  featurePoints->SetPoint(0, p0);
  featurePoints->SetPoint(1, p1);
  featurePoints->SetPoint(2, p2);
  featurePoints->SetPoint(3, p3);

  featurePoints->SetPointData(0, u0);
  featurePoints->SetPointData(1, u1);
  featurePoints->SetPointData(2, u2);
  featurePoints->SetPointData(3, u3);

  //Set the feature point set
  filter->SetInput(featurePoints);

  // Set the parameters for a rectilinear mesh.
  // Ingore this setting if users provide a mesh
  DeformationFieldType::SpacingType elementSpacing;
  elementSpacing[0] = 2.0;
  elementSpacing[1] = 2.0;
  filter->SetElementSpacing(elementSpacing);

  //Set the output
  DeformationFieldType::SizeType size;
  size[0] = 5;
  size[1] = 5;
  filter->SetSize(size);

  DeformationFieldType::SpacingType spacing;
  spacing[0] = 1.0;
  spacing[1] = 1.0;
  filter->SetSpacing(spacing);

  DeformationFieldType::PointType origin;
  origin[0] = 0.0;
  origin[1] = 0.0;
  filter->SetOrigin(origin);

  filter->Update();

  return 0;

}
