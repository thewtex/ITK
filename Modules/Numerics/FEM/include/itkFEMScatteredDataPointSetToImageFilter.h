 /*==========================================================================
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

#ifndef __itkFEMScatteredDataPointSetToImageFilter_h
#define __itkFEMScatteredDataPointSetToImageFilter_h

#include "itkPointSetToImageFilter.h"
#include "itkVectorContainer.h"
#include "vnl/vnl_matrix.h"

#include "itkMesh.h"
#include "itkFEMObject.h"
#include "itkFEMElement3DC0LinearTetrahedronStrain.h"
#include "itkFEMElement2DC0LinearTriangularStrain.h"
#include "itkFEMElement2DC0LinearQuadrilateralStrain.h"
#include "itkFEMElement2DC0LinearQuadrilateralMembrane.h"
#include "itkFEMRobustSolver.h"
#include "itkFEMLoadNoisyLandmark.h"
#include "itkImageToRectilinearFEMObjectFilter.h"
#include "itkTriangleCell.h"
#include "itkTetrahedronCell.h"
#include "itkQuadrilateralCell.h"
#include "itkHexahedronCell.h"

namespace itk
{
namespace fem
{
/** \class FEMScatteredDataPointSetToImageFilter
 * \brief Scattered data approximation to interpolation
 * in the presence of outliers
 *
 * Given a 2- or 3-D scattered and noisy data, this filter is able to approximate
 * the data while rejecting outliers, and then advances toward interpolation.
 *
 * This filter also takes the confidence and structrual information into account,
 * if users can provide a scalar to represent the confidence and a tensor
 * to represent the structural information for each feature point.
 *
 * This filter requires a point set, of which each point is associated with a
 * 2-D or 3-D displacement, as input, and outputs a deformation field. Two
 * optional point sets are suppoted.
 * Confidence point set: this point set defines the confidence assocaited with
 * each point, which will make the solver behavior like a weighted Least Square.
 * Tensor point set: this point set defines a tensor associated with each point,
 * such as a structural tensor.
 *
 * The purpose of this filter is to facilitate the use of the FEMRobustSolver,
 * which does the concrete work on finding the solution. See FEMRobustSolver for
 * details.
 *
 * \code
 *
 *  const unsigned int ParametricDimension = 3;
 *  const unsigned int DataDimension = 3;
 *
 *  typedef int                                           PixelType;
 *  typedef itk::Image<PixelType, ParametricDimension>    InputImageType;
 *  typedef float                                         RealType;
 *  typedef itk::Vector<RealType, DataDimension>          VectorType;
 *  typedef itk::Matrix<RealType, DataDimension,
 *  DataDimension>                                        MatrixType;
 *  typedef itk::Image<VectorType, ParametricDimension>   VectorImageType;
 *  typedef itk::PointSet <VectorType,
 *  ParametricDimension>                                  PointSetType;
 *
 *  typedef itk::PointSet <MatrixType,
    ParametricDimension>                                  TensorPointSetType;
 *
 *  typedef itk::PointSet <RealType, ParametricDimension> ConfidencePointSetType;
 *
 *
 *
 *  typedef itk::Mesh< VectorType, ParametricDimension >  MeshType;
 *
 *  typedef itk::FEMScatteredDataPointSetToImageFilter
 *  <PointSetType, MeshType, VectorImageType,
 *  ConfidencePointSetType, TensorPointSetType>           FilterType;
 *
 *  FilterType::Pointer filter = FilterType::New();
 *
 *  filter->SetInput(displacementPointSet);
 *  filter->SetConfidencePointSet(confidencePointSet); //optional
 *  filter->SetTensorPointSet(tensorPointSet); //optional
 *  filter->SetMesh(aITKMesh);
 *  filter->Updata();
 *
 *  DeformationField::Pointer = filter->GetOutput();
 *
 * \endcode

 * \author Yixun Liu
 *
 * \par REFERENCE
 * O. Clatz, H. Delingette, I.-F. Talos, A. Golby, R. Kikinis, F. Jolesz,
 * N. Ayache, and S. Warfield, "Robust non-rigid registration to capture brain
 * shift from intra-operative MRI", IEEE Trans. Med. Imag.,
 * 24(11);1417-27, 2005.
 *
 * \par REFERENCE
 * Yixun Liu, Andriy Fedorov, Ron Kikinis and Nikos Chrisochoides,
 * "Real-time Non-rigidRegistration of Medical Images on a Cooperative Parallel
 * Architecture", IEEE International Conference on Bioinformatics & Biomedicine,
 * pp. 401-404, November 2009.
 *
 * \ingroup ITKImageGrid
 */

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
class FEMScatteredDataPointSetToImageFilter:
  public PointSetToImageFilter< TInputPointSet, TOutputImage >
{
public:
  typedef FEMScatteredDataPointSetToImageFilter                 Self;
  typedef PointSetToImageFilter<TInputPointSet, TOutputImage>   Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Extract dimension from the output image. */
  itkStaticConstMacro( ImageDimension, unsigned int,
      TOutputImage::ImageDimension );

  /** Displacement point set typedef support*/
  typedef TInputPointSet                  PointSetType;

  typedef typename PointSetType::PointType          PointType;
  typedef typename PointSetType::Pointer            PointSetPointer;

  typedef typename PointSetType::PointsContainer::
      ConstIterator                                 PointsIterator;

  typedef typename PointSetType::PixelType          PointDataType;

  typedef typename PointSetType::PointDataContainer         PointDataContainerType;
  typedef typename PointDataContainerType::ConstIterator    PointDataIterator;

  /** Confidence point set typedef support*/
  typedef TInputConfidencePointSet                  ConfidencePointSetType;

  typedef typename ConfidencePointSetType::
      Pointer                                       ConfidencePointSetPointer;

  typedef typename ConfidencePointSetType::
      PointsContainer::ConstIterator                ConfidencePointsIterator;

  typedef typename ConfidencePointSetType::
      PixelType                             ConfidencePointDataType;

  typedef typename ConfidencePointSetType::
      PointDataContainer                      ConfidencePointDataContainerType;

  typedef typename ConfidencePointDataContainerType::
      Iterator                          ConfidencePointDataIterator;

  /** Tensor point set typedef support */
  typedef TInputTensorPointSet                      TensorPointSetType;
  typedef typename TensorPointSetType::Pointer      TensorPointSetPointer;

  typedef typename TensorPointSetType::
      PointsContainer::ConstIterator            TensorPointsIterator;

  typedef typename TensorPointSetType::PixelType  TensorPointDataType;

  typedef typename TensorPointSetType::
      PointDataContainer                  TensorPointDataContainerType;

  typedef typename TensorPointDataContainerType::
      Iterator                              TensorPointDataIterator;

  /** Mesh typedef support */
  typedef TInputMesh                                MeshType;
  typedef typename MeshType::Pointer                MeshPointer;
  typedef typename MeshType::CellType               CellType;
  typedef typename CellType::CellAutoPointer        CellAutoPointer;

  typedef typename MeshType::CellsContainer::ConstIterator     CellIterator;

  typedef TriangleCell<CellType>                            TriangleType;
  typedef TetrahedronCell<CellType>                         TetrahedronType;
  typedef QuadrilateralCell<CellType>                       QuadrilateralType;
  typedef HexahedronCell<CellType>                          HexahedronType;
  typedef typename CellType::PointIdIterator                PointIdIterator;

  /** Image typedef support */
  typedef TOutputImage                              ImageType;
  typedef typename ImageType::PixelType             PixelType;
  typedef typename ImageType::RegionType            RegionType;
  typedef typename ImageType::SizeType              SizeType;
  typedef typename ImageType::IndexType             IndexType;
  typedef typename ImageType::SpacingType           SpacingType;
  typedef ContinuousIndex<double, ImageDimension>   ContinuousIndexType;

  typedef ImageToRectilinearFEMObjectFilter<ImageType> ImageToRectilinearFEMObjectFilterType;

  /** FEMObject typedef support */
  typedef FEMObject<ImageDimension>                 FEMObjectType;
  typedef typename FEMObjectType::Pointer           FEMObjectPointer;

  /** FEM solver typedef support */
  typedef RobustSolver<ImageDimension>              FEMSolverType;

  typedef typename FEMSolverType::Pointer           FEMSolverPointer;

  /** FEM element typedef support */
  typedef Element3DC0LinearTetrahedronStrain        FEMTetrahedronType;
  typedef Element2DC0LinearTriangularStrain         FEM2DTriangleType;
  typedef Element2DC0LinearQuadrilateralStrain      FEM2DQuadrilateralType;

  /** FEM node typedef support */
  typedef Element::Node            NodeType;

  /** FEM Load typedef support */
  typedef LoadNoisyLandmark          LoadType;

  /** FEM material typedef support */
  typedef MaterialLinearElasticity           MaterialType;
  typedef MaterialType::Pointer              MaterialPointerType;

  typedef Element::VectorType                  FEMVectorType;
  typedef Element::MatrixType                  FEMMatrixType;

  // Helper functions
  itkSetMacro(ConfidencePointSet, ConfidencePointSetPointer);
  itkGetMacro(ConfidencePointSet, ConfidencePointSetPointer);

  itkSetMacro(TensorPointSet, TensorPointSetPointer);
  itkGetMacro(TensorPointSet, TensorPointSetPointer);

  itkSetMacro(Mesh, MeshPointer);
  itkGetMacro(Mesh, MeshPointer);

  /**
  * Get/Set the number of voxels/pixels in each dimension used
  * during the mesh generation
  */
  itkGetMacro(PixelsPerElement, ContinuousIndexType);
  itkSetMacro(PixelsPerElement, ContinuousIndexType);

  /**
  * Set/Get the spacing of the rectilinear element
  */
  void SetElementSpacing(SpacingType elementSpacing);
  itkGetMacro(SpacingPerElement, SpacingType);

  /**Get the number of element in each dimension of the generated mesh*/
  itkGetMacro(NumberOfElements, SizeType);

  /**Get/Set the material used for the mesh */
  //itkGetMacro(Material, MaterialPointerType);
  //itkSetMacro(Material, MaterialPointerType);

protected:

  FEMScatteredDataPointSetToImageFilter();
  virtual ~FEMScatteredDataPointSetToImageFilter();

  /** Generate 2D/3D rectilinear mesh */
  void GenerateRectilinearMesh();

  /** Generate a 2D quadrilateral mesh */
  void Generate2DQuadrilateralMesh();

  /** generate 3D hexahedral mesh */
  void Generate3DHexahedralMesh();

  /** Initialize FEMObject from a mesh and feature points */
  void InitializeFEMObject(FEMObjectPointer femObject);

  /**
  * Run the solver and call ProduceDeformationField to produce deformation field
  */
  void GenerateData();

  void ProduceDeformationField();

  void PrintSelf(std::ostream & os, Indent indent) const;

private:

  //purposely not implemented.
  FEMScatteredDataPointSetToImageFilter( const Self & );
  void operator=( const Self & );

  FEMObjectPointer m_FEMObject;

  FEMSolverPointer m_FEMSolver;

  FEMSolverPointer m_FEMDeformedObject;

  MeshPointer m_Mesh;

  ConfidencePointSetPointer m_ConfidencePointSet;

  TensorPointSetPointer m_TensorPointSet;

  /** Rectilinear mesh */
  SizeType                 m_NumberOfElements;
  ContinuousIndexType      m_PixelsPerElement;
  SpacingType              m_SpacingPerElement;

  /** Material */
  MaterialPointerType m_Material;

};

}// end namespace fem
}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFEMScatteredDataPointSetToImageFilter.hxx"
#endif

#endif
