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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilterBase_h
#define __itkLaplacianDeformationQuadEdgeMeshFilterBase_h

#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

namespace itk
{
/**
 *  \class LaplacianDeformationQuadEdgeMeshFilterBase
 *
 *  \brief Base class for laplacian surface mesh deformation
 *
 *  For details, see http://hdl.handle.net/10380/3410
 *
 *  \ingroup ITKQuadEdgeMeshFiltering
 */
template< class TInputMesh, class TOutputMesh, class TSolverTraits >
class ITK_EXPORT LaplacianDeformationQuadEdgeMeshFilterBase:
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  /** Basic types. */
  typedef LaplacianDeformationQuadEdgeMeshFilterBase      Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh,
                                            TOutputMesh > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  itkTypeMacro(LaplacianDeformationQuadEdgeMeshFilterBase, QuadEdgeMeshToQuadEdgeMeshFilter)

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

  typedef typename InputMeshType::PointsContainer               InputPointsContainer;
  typedef typename InputMeshType::PointsContainerConstIterator  InputPointsContainerConstIterator;

  typedef typename InputMeshType::CellsContainerConstIterator   InputCellsContainerConstIterator;
  typedef typename InputMeshType::EdgeCellType                  InputEdgeCellType;
  typedef typename InputMeshType::PolygonCellType               InputPolygonCellType;
  typedef typename InputMeshType::PointIdList                   InputPointIdList;

  typedef typename InputQEType::IteratorGeom                    InputQEIterator;

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
  typedef typename OutputMeshType::PointsContainerPointer  OutputPointsContainerPointer;
  typedef typename OutputMeshType::PointsContainerIterator OutputPointsContainerIterator;

  itkStaticConstMacro(OutputVDimension, unsigned int, OutputMeshType::PointDimension);

  typedef TSolverTraits                     SolverTraits;
  typedef typename SolverTraits::ValueType  ValueType;
  typedef typename SolverTraits::MatrixType MatrixType;
  typedef typename SolverTraits::VectorType VectorType;

  typedef MatrixCoefficients< OutputMeshType > CoefficientsComputationType;

  /** Set the coefficient method to compute the Laplacian matrix of the input mesh*/
  void SetCoefficientsMethod(CoefficientsComputationType *iMethod)
  {
    this->m_CoefficientsMethod = iMethod;
    this->Modified();
  }

  typedef TriangleHelper< OutputPointType > TriangleType;

  /** Constrain vertex id to the given location iP */
  void SetConstrainedNode(OutputPointIdentifier id, const OutputPointType & iP);

  /** Set the displacement vector iV for the vertex id*/
  void SetDisplacement(OutputPointIdentifier id, const OutputVectorType &iV);

  /** Get the displacement vector oV for the vertex id.
   * Returns true if the vertex id is a constraint, else false.
   */
  bool GetDisplacement( OutputPointIdentifier id, OutputVectorType& oV ) const;

  /** Clear all constraints added by the means of SetConstrainedNode or SetDisplacement.*/
  void ClearConstraints();

  itkSetMacro(Order, unsigned int)
  itkGetMacro(Order, unsigned int)

  enum AreaType
  {
    /** Do not use any area information*/
    None = 0,
    /** Use a mixed area*/
    MixedArea
  };

  itkSetMacro(AreaComputationType, AreaType );
  itkGetMacro(AreaComputationType, AreaType );

protected:

  /** Default constructor*/
  LaplacianDeformationQuadEdgeMeshFilterBase();
  virtual ~LaplacianDeformationQuadEdgeMeshFilterBase() {}

  typedef std::map< OutputPointIdentifier, OutputPointIdentifier >  OutputMapPointIdentifier;
  typedef typename OutputMapPointIdentifier::iterator               OutputMapPointIdentifierIterator;

  typedef std::map< OutputPointIdentifier, OutputVectorType >       ConstraintMapType;
  typedef typename ConstraintMapType::const_iterator                ConstraintMapConstIterator;

  typedef std::map< OutputQEType*, OutputCoordRepType >             CoefficientMapType;
  typedef typename CoefficientMapType::const_iterator               CoefficientMapConstIterator;

  typedef std::map< OutputPointIdentifier, OutputCoordRepType >     AreaMapType;
  typedef typename AreaMapType::const_iterator                      AreaMapConstIterator;

  OutputMapPointIdentifier  m_InternalMap;
  ConstraintMapType         m_Constraints;
  CoefficientMapType        m_CoefficientMap;
  AreaMapType               m_MixedAreaMap;

  CoefficientsComputationType* m_CoefficientsMethod;

  unsigned int              m_Order;
  AreaType                  m_AreaComputationType;

  void PrintSelf(std::ostream & os, Indent indent) const;

  OutputCoordRepType ComputeMixedAreaForGivenVertex(OutputPointIdentifier id);
  OutputCoordRepType ComputeMixedArea(OutputQEType *iQE1, OutputQEType *iQE2);

  virtual void ComputeVertexIdMapping();

  void ComputeLaplacianMatrix( MatrixType &ioL );

  void
  FillMatrixRow(OutputPointIdentifier iId,
                unsigned int iDegree,
                OutputCoordRepType iWeight,
                std::map< OutputPointIdentifier, OutputCoordRepType > & ioRow);

  /**
   *  \brief Fill matrix iM and vectors Bx, m_By and m_Bz depending on if one
   *  vertex is on the border or not.
   */
  void FillMatrix(MatrixType & iM, VectorType & iBx, VectorType & iBy, VectorType & iBz);

  /**
   *  \brief Solve linears systems : \f$ iM \cdot oX = iBx \f$ and
   * \f$ iM \cdot oY = iBy \f$ and \f$ iM \cdot oZ = iBz \f$
   *
   *  \param[in] iM
   *  \param[in] iBx
   *  \param[in] iBy
   *  \param[in] iBz
   *  \param[out] oX
   *  \param[out] oY
   *  \param[out] oZ
   */
  void SolveLinearSystems(const MatrixType & iM,
                          const VectorType & iBx,
                          const VectorType & iBy,
                          const VectorType & iBz,
                          VectorType & oX,
                          VectorType & oY,
                          VectorType & oZ);


private:
  LaplacianDeformationQuadEdgeMeshFilterBase(const Self &); // purposely not implemented
  void operator=(const Self &);                         // purposely not implemented

  struct Triple
  {
    Triple() {}
    Triple(OutputPointIdentifier iV, OutputCoordRepType iWeight, unsigned int iDegree):
      m_Id(iV), m_Weight(iWeight), m_Degree(iDegree) {}

    OutputPointIdentifier m_Id;
    OutputCoordRepType    m_Weight;
    unsigned int          m_Degree;
  };
};
} // end namespace itk

#include "itkLaplacianDeformationQuadEdgeMeshFilterBase.hxx"

#endif
