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
=========================================================================*/
#ifndef __itkLaplaceBeltramiFilter_h
#define __itkLaplaceBeltramiFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"
#include "itkArray2D.h"

// vnl headers
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_sparse_matrix.h"


namespace itk
{

/** \class LaplaceBeltramiFilter
 * \brief Laplace Beltrami operator on QE meshes
 *
 * LaplaceBeltramiFilter defines basis functions on a quad edge mesh, then (if
 * requested) determines the N most significant eigenvalues of the basis.
 *
 * \ingroup ITKQuadEdgeMeshFiltering
 */

template <class TInputMesh, class TOutputMesh>
class ITK_EXPORT LaplaceBeltramiFilter :
    public QuadEdgeMeshToQuadEdgeMeshFilter<TInputMesh, TOutputMesh>
{
public:
  /** Standard class typedefs. */
  typedef LaplaceBeltramiFilter  Self;

  typedef TInputMesh   InputMeshType;
  typedef TOutputMesh  OutputMeshType;

  typedef QuadEdgeMeshToQuadEdgeMeshFilter<TInputMesh,TOutputMesh> Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef typename InputMeshType::ConstPointer  InputMeshConstPointer;
  typedef typename OutputMeshType::ConstPointer OutputMeshConstPointer;
  typedef typename InputMeshType::Pointer       InputMeshPointer;
  typedef typename OutputMeshType::Pointer      OutputMeshPointer;
  typedef typename InputMeshType::PointType     InputPointType;
  typedef typename OutputMeshType::PointType    OutputPointType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LaplaceBeltramiFilter, QuadEdgeMeshToQuadEdgeMeshFilter);

  /** Convenient constants obtained from TMeshTraits template parameter. */
  itkStaticConstMacro(InputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TInputMesh >::PointDimension );
  itkStaticConstMacro(OutputPointDimension, unsigned int,
     ::itk::GetMeshDimension< TOutputMesh >::PointDimension );

  itkSetMacro( EigenValueCount, unsigned int );
  itkGetMacro( EigenValueCount, unsigned int );

  itkSetMacro( HarmonicScaleValue, double );
  itkGetMacro( HarmonicScaleValue, double );

  typedef  enum {
    VONNEUMAN = 0, // zero on the boundaries
    DIRICHLET      // gradient tangent to surface boundary
    } BoundaryConditionEnumType;

  itkSetMacro( BoundaryConditionType, BoundaryConditionEnumType );
  itkGetMacro( BoundaryConditionType, BoundaryConditionEnumType );

  typedef typename OutputMeshType::PointsContainer             OutputPointsContainer;
  typedef typename OutputMeshType::CellsContainer              OutputCellsContainer;
  typedef typename InputMeshType::PointsContainerPointer       PointsContainerPointer;
  typedef typename InputMeshType::CellsContainerPointer        CellsContainerPointer;
  typedef typename InputMeshType::CellsContainerConstPointer   InputCellsContainerConstPointer;
  typedef typename OutputMeshType::CellsContainerConstPointer  OutputCellsContainerConstPointer;
  typedef typename InputMeshType::PointIdentifier              InputPointIdentifier;
  typedef typename InputMeshType::CellIdentifier               InputCellIdentifier;
  typedef typename OutputPointsContainer::ConstPointer         OutputPointsContainerConstPointer;
  typedef typename OutputPointsContainer::ConstIterator        OutputPointIterator;
  typedef typename OutputCellsContainer::ConstIterator         OutputCellIterator;
  typedef typename InputMeshType::CellType                     InputCellType;
  typedef typename OutputMeshType::CellType                    OutputCellType;
  typedef typename InputCellType::PointIdIterator              InputPointIdIterator;
  typedef typename InputCellType::CellAutoPointer              InputCellAutoPointer;
  typedef typename OutputCellType::PointIdIterator             OutputPointIdIterator;
  typedef typename OutputCellType::CellAutoPointer             OutputCellAutoPointer;

  typedef typename OutputMeshType::PointDataContainer          OutputPointDataContainer;
  typedef typename OutputPointDataContainer::ConstPointer      OutputPointDataContainerConstPointer;

  typedef vnl_sparse_matrix< double >  LBMatrixType;
  typedef vnl_vector< double >         EigenvalueSetType;
  typedef Array2D< double >            HarmonicSetType;


  itkGetConstReferenceMacro(LBOperator, LBMatrixType);
  itkGetConstReferenceMacro(VertexAreas, LBMatrixType);
  itkGetConstReferenceMacro(Eigenvalues, EigenvalueSetType);
  itkGetConstReferenceMacro(Harmonics, HarmonicSetType);

  /**
  * \brief Get a single surface harmonic
  * \param[in] harmonic
  * \return success
  */
  bool SetSurfaceHarmonic( unsigned int harmonic );

protected:
  LaplaceBeltramiFilter();
  ~LaplaceBeltramiFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /**
  * \brief Generate Requested Data
  */
  virtual void GenerateData( void );


private:
  //purposely not implemented
  LaplaceBeltramiFilter(const LaplaceBeltramiFilter&);
  //purposely not implemented
  void operator=(const LaplaceBeltramiFilter&);

  /** Number of most significant eigenvalues to include */
  unsigned int m_EigenValueCount;

  /** Scale eigenvalue point data in harmonic mesh to +- this value */
  double m_HarmonicScaleValue;

  /** Type of condition for vertex values on the boundary of open surfaces */
  BoundaryConditionEnumType m_BoundaryConditionType;

  /** The LB operator */
  LBMatrixType m_LBOperator;

  /** The areas for each vertex */
  LBMatrixType m_VertexAreas;

  /** Eignevalues of the solution **/
  EigenvalueSetType m_Eigenvalues;

  /** Harmonics for the most significant basis functions */
  HarmonicSetType m_Harmonics;

  /**
  * \brief Generate areas associated with each vertex
  */
  virtual bool ComputeVertexAreas(Array<double> &vertexAreas);
  virtual unsigned int GetEdges(Array2D<unsigned int> &edges,
                                Array<unsigned int>   &boundaryVertex,
                                Array2D<double>       &edgeAngles);
  virtual void ComputeLaplacian(Array2D<unsigned int> &edges,
                                Array2D<double>       &edgeAngles,
                                Array<double>         &vertexAreas);
  virtual bool ComputeHarmonics(Array<unsigned int>   &boundaryVertex);

};
} // end namespace itk

#include "itkLaplaceBeltramiFilter.hxx"
#endif
