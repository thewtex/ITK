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
#ifndef __itkFEMScatteredDataPointSetToImageFilter_hxx
#define __itkFEMScatteredDataPointSetToImageFilter_hxx

#include "itkFEMScatteredDataPointSetToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageDuplicator.h"
#include "itkCastImageFilter.h"
#include "itkNumericTraits.h"

#include "vnl/vnl_math.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_vector.h"
#include "vcl_limits.h"

namespace itk
{
namespace fem
{
template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::FEMScatteredDataPointSetToImageFilter()
{
  this->m_FEMObject = FEMObjectType::New();
  this->m_Material = MaterialType::New();
  this->m_FEMSolver = FEMSolverType::New();

  itk::FEMFactoryBase::GetFactory()->RegisterDefaultTypes();
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::~FEMScatteredDataPointSetToImageFilter()
{
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::SetElementSpacing(SpacingType elementSpacing)
{
    this->m_SpacingPerElement = elementSpacing;
    ImageType::Pointer image = this->GetOutput();
    SpacingType imageSpacing = image->GetSpacing();

    for(unsigned int i = 0; i < ImageDimension; i++)
    {
        this->m_PixelsPerElement[i] = elementSpacing[i]/imageSpacing[i];
    }
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::GenerateData()
{
  /**
   *  Create the output image
   */
  itkDebugMacro( "Size: " << this->m_Size );
  itkDebugMacro( "Origin: " << this->m_Origin );
  itkDebugMacro( "Spacing: " << this->m_Spacing );
  itkDebugMacro( "Direction: " << this->m_Direction );

  //error checking
  for( unsigned int i = 0; i < ImageDimension; i++ )
    {
    if( this->m_Size[i] == 0 )
      {
      itkExceptionMacro("Size must be specified.");
      }
    }

  this->GetOutput()->SetOrigin( this->m_Origin );
  this->GetOutput()->SetSpacing( this->m_Spacing );
  this->GetOutput()->SetDirection( this->m_Direction );
  this->GetOutput()->SetRegions( this->m_Size );
  this->GetOutput()->Allocate();

  if(this->m_Mesh.IsNull())
  {
      //provide a rectilinear mesh based on the output deformation field
      //if users do not specify one.
      GenerateRectilinearMesh();
  }

  //convert the mesh and feature points into a FEMObject
  this->InitializeFEMObject(this->m_FEMObject);

  this->m_FEMSolver->SetInput( this->m_FEMObject );

  //set the interpolation grid of the FEMSolver
  //note that let the interpolation grid be same with the deformation field
  //in order to accelarate the generation of the deformation field.
  this->m_FEMSolver->SetOrigin(this->m_Origin);
  this->m_FEMSolver->SetSpacing(this->m_Spacing);
  RegionType region;
  region.SetSize(this->m_Size);
  this->m_FEMSolver->SetRegion(region);
  this->m_FEMSolver->SetDirection(this->m_Direction);

  //if the feature points are the grid point of the interpolation grid, set true.
  //note that since feature points come from the image, this setting is always true.
  this->m_FEMSolver->SetUseInterpolationGrid(true);

  this->m_FEMSolver->Update( );

  this->ProduceDeformationField();

}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::GenerateRectilinearMesh()
{
    if(this->m_Mesh.IsNotNull())
        return;

    if( ImageDimension == 2 )
    {
        Generate2DQuadrilateralMesh();
    }
    else
    {
        Generate3DHexahedralMesh();
    }
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::Generate2DQuadrilateralMesh()
{
    ImageType::Pointer image = this->GetOutput();
    RegionType   region = image->GetLargestPossibleRegion();
    SizeType     size   = region.GetSize();

    this->m_NumberOfElements[0] = int((size[0] - 1) / m_PixelsPerElement[0]);
    this->m_NumberOfElements[1] = int((size[1] - 1) / m_PixelsPerElement[1]);
    if((size[0] - 1) / m_PixelsPerElement[0] - this->m_NumberOfElements[0] != 0)
        this->m_NumberOfElements[0]++;
    if((size[1] - 1) / m_PixelsPerElement[1] - this->m_NumberOfElements[1] != 0)
        this->m_NumberOfElements[1]++;

    this->m_Mesh = MeshType::New();

     // Create nodes
    ContinuousIndexType  pointIndex;
    ImageType::PointType pointCoordinate;
    //MeshType::PointType  nodeCoordinate
    int gn = 0;
    for( float j = 0; j <= m_NumberOfElements[1]; j++ )
    {
        pointIndex[1] = j * m_PixelsPerElement[1];
        for( float i = 0; i <= m_NumberOfElements[0]; i++ )
        {
            pointIndex[0] = i * m_PixelsPerElement[0];
            image->TransformContinuousIndexToPhysicalPoint(pointIndex,
                pointCoordinate);
            //nodeCoordinate[0] = pointCoordinate[0];
            //nodeCoordinate[1] = pointCoordinate[1];
            this->m_Mesh->SetPoint(gn, pointCoordinate);
            gn++;
        }
    }

    // Create elements
    gn = 0; // global number of the element
    for( unsigned int j = 0; j < m_NumberOfElements[1]; j++ )
    {
        for( unsigned int i = 0; i < m_NumberOfElements[0]; i++ )
        {
            CellAutoPointer cell;
            cell.TakeOwnership(new QuadrilateralType);
            cell->SetPointId(0, i + ( m_NumberOfElements[0] + 1 ) * j );
            cell->SetPointId(1, i + 1 + ( m_NumberOfElements[0] + 1 ) * j);
            cell->SetPointId(2, i + 1 + ( m_NumberOfElements[0] + 1 ) * ( j + 1 ));
            cell->SetPointId(3, i + ( m_NumberOfElements[0] + 1 ) * ( j + 1 ));

            this->m_Mesh->SetCell(gn, cell);
            gn++;
        }
    }
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::Generate3DHexahedralMesh()
{
    ImageType::Pointer image = this->GetOutput();
    RegionType   region = image->GetLargestPossibleRegion();
    SizeType     size   = region.GetSize();

    this->m_NumberOfElements[0] = int((size[0] - 1) / m_PixelsPerElement[0]);
    this->m_NumberOfElements[1] = int((size[1] - 1) / m_PixelsPerElement[1]);
    this->m_NumberOfElements[2] = int((size[2] - 1) / m_PixelsPerElement[2]);
    if((size[0] - 1) / m_PixelsPerElement[0] - this->m_NumberOfElements[0] != 0)
        this->m_NumberOfElements[0]++;
    if((size[1] - 1) / m_PixelsPerElement[1] - this->m_NumberOfElements[1] != 0)
        this->m_NumberOfElements[1]++;
    if((size[2] - 1) / m_PixelsPerElement[2] - this->m_NumberOfElements[2] != 0)
        this->m_NumberOfElements[2]++;

    this->m_Mesh = MeshType::New();

    // Create nodes
    ContinuousIndexType  pointIndex;
    ImageType::PointType pointCoordinate;
    int gn = 0;
    for( float k = 0; k <= m_NumberOfElements[2]; k++ )
    {
        pointIndex[2] = k * m_PixelsPerElement[2];
        for( float j = 0; j <= m_NumberOfElements[1]; j++ )
        {
            pointIndex[1] = j * m_PixelsPerElement[1];
            for( float i = 0; i <= m_NumberOfElements[0]; i++ )
            {
                pointIndex[0] = i * m_PixelsPerElement[0];
                image->TransformContinuousIndexToPhysicalPoint(pointIndex,
                    pointCoordinate);
                this->m_Mesh->SetPoint(gn, pointCoordinate);
                gn++;

            }
        }
    }

    // Create elements
    gn = 0;
    for( unsigned int k = 0; k < m_NumberOfElements[2]; k++ )
    {
        for( unsigned int j = 0; j < m_NumberOfElements[1]; j++ )
        {
            for( unsigned int i = 0; i < m_NumberOfElements[0]; i++ )
            {

                CellAutoPointer cell;
                cell.TakeOwnership(new HexahedronType);

                cell->SetPointId(0, unsigned int( i +  ( m_NumberOfElements[0]
                + 1 ) * ( j  + ( m_NumberOfElements[1] + 1 ) * k ) ) );
                cell->SetPointId(1, unsigned int( i + 1 + ( m_NumberOfElements[0]
                + 1 ) * ( j  + ( m_NumberOfElements[1] + 1 ) * k ) ) );
                cell->SetPointId(2, unsigned int( i + 1 + ( m_NumberOfElements[0]
                + 1 ) * ( j + 1 + ( m_NumberOfElements[1] + 1 ) * k ) ) );
                cell->SetPointId(3, unsigned int( i + ( m_NumberOfElements[0]
                + 1 ) * ( j + 1 + ( m_NumberOfElements[1] + 1 ) * k ) ) );
                cell->SetPointId(4, unsigned int( i + ( m_NumberOfElements[0]
                + 1 ) * ( j  + ( m_NumberOfElements[1] + 1 ) * ( k + 1 ) ) ) );
                cell->SetPointId(5, unsigned int( i + 1 + ( m_NumberOfElements[0]
                + 1 ) * ( j  + ( m_NumberOfElements[1] + 1 ) * ( k + 1 ) ) ) );
                cell->SetPointId(6, unsigned int( i + 1 + ( m_NumberOfElements[0]
                + 1 ) * ( j + 1 + ( m_NumberOfElements[1] + 1 ) * ( k + 1 ) ) ) );
                cell->SetPointId(7, unsigned int( i + ( m_NumberOfElements[0]
                + 1 ) * ( j + 1 + ( m_NumberOfElements[1] + 1 ) * ( k + 1 ) ) ) );

                this->m_Mesh->SetCell(gn, cell);
                gn++;
            }
        }
    }
}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::InitializeFEMObject(FEMObjectPointer femObject)
{
    femObject->GetLoadContainer()->Initialize();
    femObject->GetElementContainer()->Initialize();
    femObject->GetNodeContainer()->Initialize();
    femObject->GetMaterialContainer()->Initialize();

    //fix material to linear elasticity
    femObject->AddNextMaterial(this->m_Material);

    NodeType::Pointer  n;
    PointsIterator it = this->m_Mesh->GetPoints()->Begin();

    FEMVectorType pt(ImageDimension);

    //Initialize nodes
    while(it != this->m_Mesh->GetPoints()->End())
    {
        for(unsigned i = 0; i < ImageDimension; i++)
        {
            pt[i]= it.Value()[i];
        }

        n =  NodeType::New();
        n->SetCoordinates(pt);
        n->SetGlobalNumber(it.Index());

        femObject->AddNextNode(n);
        ++it;
    }

    //Initialize Elements
    unsigned int gn = 0;
    unsigned int i;
    unsigned j;

    //Mesh cell iterator
    CellIterator cellIterator = this->m_Mesh->GetCells()->Begin();
    CellIterator cellEnd = this->m_Mesh->GetCells()->End();

    while( cellIterator != cellEnd )
    {
        CellType * cell = cellIterator.Value();

        switch( cell->GetType() )
        {
        //TODO: add 3D TRIANGLE CELL
        case CellType::TRIANGLE_CELL:
            {
                FEM2DTriangleType::Pointer triangleEle = FEM2DTriangleType::New();

                TriangleType * triangleCell = dynamic_cast<TriangleType *>( cell );

                PointIdIterator pointIdIter = triangleCell->PointIdsBegin();
                PointIdIterator pointIdEnd = triangleCell->PointIdsEnd();
                i = 0;
                while( pointIdIter != pointIdEnd )
                {
                    triangleEle->SetNode(i++, femObject->GetNode(*pointIdIter));
                    ++pointIdIter;
                }

                triangleEle->SetGlobalNumber(gn++);
                triangleEle->SetMaterial( dynamic_cast<MaterialType *>
                    ( femObject->GetMaterial(0).GetPointer() ) );
                femObject->AddNextElement(triangleEle.GetPointer());

                break;
            }

        case CellType::TETRAHEDRON_CELL:
            {
                FEMTetrahedronType::Pointer tetrahedronEle =
                    FEMTetrahedronType::New();

                TetrahedronType * tetrahedron =
                    dynamic_cast<TetrahedronType *>( cell );

                PointIdIterator pointIdIter = tetrahedron->PointIdsBegin();
                PointIdIterator pointIdEnd = tetrahedron->PointIdsEnd();

                i = 0;
                while( pointIdIter != pointIdEnd )
                {
                    tetrahedronEle->SetNode(i++, femObject->GetNode(*pointIdIter));

                    ++pointIdIter;
                }

                tetrahedronEle->SetGlobalNumber(gn++);
                tetrahedronEle->SetMaterial( dynamic_cast<MaterialType *>
                    ( femObject->GetMaterial(0).GetPointer() ) );
                femObject->AddNextElement(tetrahedronEle.GetPointer());

                break;

            }

        //TODO: add 3D QUADRILATERAL_CELL
        case CellType::QUADRILATERAL_CELL:
            {
                FEM2DQuadrilateralType::Pointer quadrilateralEle =
                    FEM2DQuadrilateralType::New();

                //use Cell and Ele to distinguish itk element and FEM element
                QuadrilateralType * quadrilateralCell =
                    dynamic_cast<QuadrilateralType *>( cell );

                PointIdIterator pointIdIter = quadrilateralCell->PointIdsBegin();
                PointIdIterator pointIdEnd = quadrilateralCell->PointIdsEnd();

                i = 0;
                while( pointIdIter != pointIdEnd )
                {
                    quadrilateralEle->SetNode(i++, femObject->GetNode(*pointIdIter));

                    ++pointIdIter;
                }

                quadrilateralEle->SetGlobalNumber(gn++);
                quadrilateralEle->SetMaterial( dynamic_cast<MaterialType *>
                    ( femObject->GetMaterial(0).GetPointer() ) );
                femObject->AddNextElement(quadrilateralEle.GetPointer());

                break;

            }

        default:
            {
                itkExceptionMacro("Do not support element type: " <<
                    cell->GetType());
                 break;
            }

        }//end of switch

        ++cellIterator;

    }//end of while

    //Initialize Loads
    LoadType::Pointer load;
    FEMMatrixType ten(ImageDimension, ImageDimension);
    it=this->GetInput()->GetPoints()->Begin();
    PointDataIterator itDis = this->GetInput()->GetPointData()->Begin();
    ConfidencePointDataIterator  itCon;
    TensorPointDataIterator itTen;

    if(this->m_ConfidencePointSet.IsNotNull() )
        itCon = this->m_ConfidencePointSet->GetPointData()->Begin();

    if(this->m_TensorPointSet.IsNotNull() )
        itTen = this->m_TensorPointSet->GetPointData()->Begin();

    while(it != this->GetInput()->GetPoints()->End())
    {
        FEMVectorType dis(ImageDimension);
        FEMVectorType pt(ImageDimension);

        for(i = 0; i < ImageDimension; i++)
        {
            pt[i] = it.Value()[i];
            dis[i] = itDis.Value()[i];
        }

        load =  LoadType::New();

        load->SetSource(pt);
        load->SetRealDisplacement(dis);

        if(this->m_ConfidencePointSet.IsNotNull() )
        {
            load->SetConfidence( ConfidencePointDataType(itCon.Value()));
            ++itCon;
        }

        if(this->m_TensorPointSet.IsNotNull() )
        {
            for(i = 0; i < ImageDimension; i++)
                for(j = 0; j < ImageDimension; j++)
                    ten[i][j]= itTen.Value()[i][j];

                load->SetStructureTensor(ten);
                ++itTen;
        }

        femObject->AddNextLoad(fem::Load::Pointer(load.GetPointer()));

        ++itDis;
        ++it;
    }

    //produce DOF
    femObject->FinalizeMesh();

}

/**
* Produce deformation field based on the solution.
*/
template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::ProduceDeformationField()
{
    RegionType region = this->GetOutput()->GetLargestPossibleRegion();
    ImageRegionIterator<ImageType> iter(this->GetOutput(), region);
    PointType pt;
    unsigned solutionTIndex = 0;
    FEMVectorType globalPoint(ImageDimension);
    FEMVectorType localPoint;
    FEMVectorType shape;
    PixelType displacement;

    // Step over all points within the region
    for( iter.GoToBegin(); !iter.IsAtEnd(); ++iter )
    {
        //Element::Pointer ep = intepolationGrid->GetPixel(iter.GetIndex());
        this->GetOutput()->TransformIndexToPhysicalPoint(iter.GetIndex(), pt);
        for( unsigned int d = 0; d < ImageDimension; d++ )
        {
            globalPoint[d] = pt[d];
        }
        fem::Element::ConstPointer ep = m_FEMSolver->GetElementAtPoint(globalPoint);

        //the point is inside the element.
        if( ep->GetLocalFromGlobalCoordinates(globalPoint, localPoint) )
        {
            shape = ep->ShapeFunctions(localPoint);

            const unsigned int NnDOF = ep->GetNumberOfDegreesOfFreedomPerNode();
            const unsigned int Nnodes = ep->GetNumberOfNodes();

            FEMVectorType simulatedDisplacement(NnDOF, 0.0);
            FEMVectorType nodeSolution(NnDOF);

            for(unsigned int m = 0; m < Nnodes; m++)
            {
                for(unsigned int j = 0; j < NnDOF; ++j)
                    nodeSolution[j] = m_FEMSolver->GetSolution(
                    ep->GetDegreeOfFreedom(m*NnDOF+j),solutionTIndex);

                simulatedDisplacement += shape[m] * nodeSolution;

            }

            for(unsigned i = 0; i < ImageDimension; i++)
                displacement[i] = simulatedDisplacement[i];

            iter.Set(displacement);
        }

    }
}

/**
 * Standard "PrintSelf" method
 */
template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "  Origin:    " << this->m_Origin << std::endl;
  os << indent << "  Spacing:   " << this->m_Spacing << std::endl;
  os << indent << "  Size:      " << this->m_Size << std::endl;
  os << indent << "  Direction: " << this->m_Direction << std::endl;
}

} // end namespace fem
} // end namespace itk

#endif
