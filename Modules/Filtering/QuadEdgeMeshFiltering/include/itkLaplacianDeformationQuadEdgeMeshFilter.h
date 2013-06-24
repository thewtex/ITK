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
#ifndef __itkLaplacianDeformationQuadEdgeMeshFilter_h
#define __itkLaplacianDeformationQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"

#include "itkConceptChecking.h"

#include "itksys/hash_map.hxx"

namespace itk
{
/**
 *  \class LaplacianDeformationQuadEdgeMeshFilter
 *
 *  \brief  class for laplacian surface mesh deformation
 *
 *  For details, see http://hdl.handle.net/10380/3410
 *
 *  \ingroup ITKQuadEdgeMeshFiltering
 */
template< class TInputMesh, class TOutputMesh, class TSolverTraits >
class LaplacianDeformationQuadEdgeMeshFilter:
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >
{
public:
  /** Basic types. */
  typedef LaplacianDeformationQuadEdgeMeshFilter          Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh,
                                            TOutputMesh > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

  itkTypeMacro(LaplacianDeformationQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter)

  /** Input types. */
  typedef TInputMesh                                        InputMeshType;
  typedef typename Superclass::InputMeshConstPointer        InputMeshConstPointer;
  typedef typename Superclass::InputPointType               InputPointType;

  itkStaticConstMacro(InputVDimension, unsigned int, InputMeshType::PointDimension);

  /** Output types. */
  typedef TOutputMesh                                        OutputMeshType;
  typedef typename Superclass::OutputMeshPointer             OutputMeshPointer;
  typedef typename Superclass::OutputCoordRepType            OutputCoordRepType;
  typedef typename Superclass::OutputPointType               OutputPointType;
  typedef typename Superclass::OutputPointIdentifier         OutputPointIdentifier;
  typedef typename Superclass::OutputQEPrimal                OutputQEPrimal;
  typedef typename Superclass::OutputVectorType              OutputVectorType;
  typedef typename Superclass::OutputQEIterator              OutputQEIterator;
  typedef typename Superclass::OutputPointsContainerPointer  OutputPointsContainerPointer;
  typedef typename Superclass::OutputPointsContainerIterator OutputPointsContainerIterator;

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

  /** Constrain vertex vId to the given location iP */
  void SetConstrainedNode(OutputPointIdentifier vId, const OutputPointType & iP);

  /** Set the displacement vector iV for the vertex vId */
  void SetDisplacement(OutputPointIdentifier vId, const OutputVectorType &iV);

  /** Get the displacement vector oV for the vertex vId.
   * Returns true if the vertex vId is a constraint, else false.
   */
  bool GetDisplacement( OutputPointIdentifier vId, OutputVectorType& oV ) const;

  /** Clear all constraints added by the means of SetConstrainedNode or SetDisplacement.*/
  void ClearConstraints();

  itkSetMacro(Order, unsigned int)
  itkGetMacro(Order, unsigned int)

  enum AreaType
  {
    /** Do not use any area information*/
    NONE = 0,
    /** Use a mixed area*/
    MIXEDAREA
  };

  itkSetMacro(AreaComputationType, AreaType );
  itkGetMacro(AreaComputationType, AreaType );

#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( SameDimensionCheck1,
                   ( Concept::SameDimension< InputVDimension, OutputVDimension > ) );
  itkConceptMacro( SameDimensionCheck2,
                   ( Concept::SameDimension< InputVDimension, 3 > ) );
#endif

protected:

  /** Default constructor*/
  LaplacianDeformationQuadEdgeMeshFilter();
  virtual ~LaplacianDeformationQuadEdgeMeshFilter() {}

  typedef itksys::hash_map< OutputPointIdentifier, OutputPointIdentifier >  OutputMapPointIdentifier;
  typedef typename OutputMapPointIdentifier::iterator                       OutputMapPointIdentifierIterator;
  typedef typename OutputMapPointIdentifier::const_iterator                 OutputMapPointIdentifierConstIterator;

  typedef itksys::hash_map< OutputPointIdentifier, OutputVectorType > ConstraintMapType;
  typedef typename ConstraintMapType::const_iterator                  ConstraintMapConstIterator;

  struct HashOutputQEPrimal
  {
    size_t operator() ( OutputQEPrimal* qe ) const
    {
      return reinterpret_cast< size_t >( qe );
    }
  };

  typedef itksys::hash_map< OutputQEPrimal*, OutputCoordRepType, HashOutputQEPrimal > CoefficientMapType;
  typedef typename CoefficientMapType::const_iterator                                 CoefficientMapConstIterator;

  typedef itksys::hash_map< OutputPointIdentifier, OutputCoordRepType > AreaMapType;
  typedef typename AreaMapType::const_iterator                          AreaMapConstIterator;

  typedef itksys::hash_map< OutputPointIdentifier, OutputCoordRepType > RowType;
  typedef typename RowType::iterator                                    RowIterator;
  typedef typename RowType::const_iterator                              RowConstIterator;

  OutputMapPointIdentifier  m_InternalMap;
  ConstraintMapType         m_Constraints;
  CoefficientMapType        m_CoefficientMap;
  AreaMapType               m_MixedAreaMap;

  CoefficientsComputationType* m_CoefficientsMethod;

  unsigned int              m_Order;
  AreaType                  m_AreaComputationType;

  void PrintSelf(std::ostream & os, Indent indent) const;

  OutputCoordRepType ComputeMixedAreaForGivenVertex(OutputPointIdentifier vId);
  OutputCoordRepType ComputeMixedArea(OutputQEPrimal *iQE1, OutputQEPrimal *iQE2);

  virtual void ComputeVertexIdMapping();

  void ComputeLaplacianMatrix( MatrixType &ioL );

  void
  FillMatrixRow(OutputPointIdentifier iId,
                unsigned int iDegree,
                OutputCoordRepType iWeight,
                RowType & ioRow);

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
  LaplacianDeformationQuadEdgeMeshFilter(const Self &); // purposely not implemented
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

#include "itkLaplacianDeformationQuadEdgeMeshFilter.hxx"

#endif
