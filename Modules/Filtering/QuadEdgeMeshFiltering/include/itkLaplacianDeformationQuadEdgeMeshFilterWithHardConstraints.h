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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints_h
#define __itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints_h

#include "itkLaplacianDeformationQuadEdgeMeshFilterBase.h"

namespace itk
{
/**
 *  \class LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints
 *
 *  \brief Laplacian mesh deformation with hard constraints (interpolating
 *  displacement for some handle points)
 *
 *  For details, see http://hdl.handle.net/10380/3410
 *
 *  \ingroup ITKQuadEdgeMeshFiltering
 */
template< class TInputMesh, class TOutputMesh, class TSolverTraits >
class ITK_EXPORT LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints:
  public LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh, TOutputMesh, TSolverTraits >
{
public:
  /** Basic types. */
  typedef LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints       Self;
  typedef LaplacianDeformationQuadEdgeMeshFilterBase< TInputMesh,
                                            TOutputMesh, TSolverTraits >  Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Input types. */
  typedef TInputMesh                              InputMeshType;
  typedef typename InputMeshType::Pointer         InputMeshPointer;
  typedef typename InputMeshType::ConstPointer    InputMeshConstPointer;
  typedef typename InputMeshType::CoordRepType    InputCoordRepType;
  typedef typename InputMeshType::PointType       InputPointType;
  typedef typename InputPointType::VectorType     InputPointVectorType;
  typedef typename InputMeshType::PointIdentifier InputPointIdentifier;
  typedef typename InputMeshType::QEType          InputQEType;
  typedef typename InputMeshType::VectorType      InputVectorType;
  typedef typename InputMeshType::EdgeListType    InputEdgeListType;
  typedef typename InputMeshType::PixelType       InputPixelType;
  typedef typename InputMeshType::Traits          InputTraits;

  itkStaticConstMacro(InputVDimension, unsigned int, InputMeshType::PointDimension);

  typedef typename InputMeshType::PointsContainer              InputPointsContainer;
  typedef typename InputMeshType::PointsContainerConstIterator InputPointsContainerConstIterator;

  typedef typename InputMeshType::CellsContainerConstIterator InputCellsContainerConstIterator;
  typedef typename InputMeshType::EdgeCellType                InputEdgeCellType;
  typedef typename InputMeshType::PolygonCellType             InputPolygonCellType;
  typedef typename InputMeshType::PointIdList                 InputPointIdList;

  typedef typename InputQEType::IteratorGeom InputQEIterator;

  /** Output types. */
  typedef TOutputMesh                                      OutputMeshType;
  typedef typename OutputMeshType::Pointer                 OutputMeshPointer;
  typedef typename OutputMeshType::ConstPointer            OutputMeshConstPointer;
  typedef typename OutputMeshType::CoordRepType            OutputCoordRepType;
  typedef typename OutputMeshType::PointType               OutputPointType;
  typedef typename OutputMeshType::PointIdentifier         OutputPointIdentifier;
  typedef typename OutputMeshType::QEType                  OutputQEType;
  typedef typename OutputMeshType::VectorType              OutputVectorType;
  typedef typename OutputQEType::IteratorGeom              OutputQEIterator;
  typedef typename OutputMeshType::PointsContainerIterator OutputPointsContainerIterator;

  itkStaticConstMacro(OutputVDimension, unsigned int, OutputMeshType::PointDimension);

  typedef TSolverTraits                                     SolverTraits;
  typedef typename SolverTraits::ValueType                  ValueType;
  typedef typename SolverTraits::MatrixType                 MatrixType;
  typedef typename SolverTraits::VectorType                 VectorType;

  itkNewMacro(Self)
  itkTypeMacro(LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints,
               LaplacianDeformationQuadEdgeMeshFilterBase)

protected:

  LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints();
  virtual ~LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  typedef typename Superclass::OutputMapPointIdentifier         OutputMapPointIdentifier;
  typedef typename Superclass::OutputMapPointIdentifierIterator OutputMapPointIdentifierIterator;

  typedef typename Superclass::ConstraintMapType            ConstraintMapType;
  typedef typename Superclass::ConstraintMapConstIterator   ConstraintMapConstIterator;

  virtual void ComputeVertexIdMapping();

  /**
   *  \brief Fill matrix iM and vectors Bx and m_By depending on if one
   *  vertex is on the border or not.
   *  \param iM
   */
  void FillMatrix(MatrixType & iM, VectorType & iBx, VectorType & iBy, VectorType & iBz);

  void GenerateData();

private:
  LaplacianDeformationQuadEdgeMeshFilterWithHardConstraints(const Self &); // purposely not
                                                        // implemented
  void operator=(const Self &);                         // purposely not
};
} // end namespace itk

#include "itkLaplacianDeformationQuadEdgeMeshFilterWithHardConstraints.hxx"

#endif
