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
template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::FEMScatteredDataPointSetToImageFilter()
{
  this->m_FEMObject = FEMObjectType::New();
  this->m_FEMSolver = FEMSolverType::New();
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

  if(this->m_Mesh.IsNull())
      //TODO: provide a rectilinear mesh if users do not specify one.
      itkExceptionMacro("Mesh must be specified.");

  this->GetOutput()->SetOrigin( this->m_Origin );
  this->GetOutput()->SetSpacing( this->m_Spacing );
  this->GetOutput()->SetDirection( this->m_Direction );
  this->GetOutput()->SetRegions( this->m_Size );
  this->GetOutput()->Allocate();

  this->InitializeFEMObject(this->m_FEMObject);
  this->m_FEMSolver->SetInput( this->m_FEMObject );
  this->m_FEMSolver->Update( );

  this->ProduceDeformationField();

}

template<class TInputPointSet, class TInputMesh, class TOutputImage,
class TInputConfidencePointSet, class TInputTensorPointSet>
void
FEMScatteredDataPointSetToImageFilter<TInputPointSet, TInputMesh,TOutputImage,
TInputConfidencePointSet,TInputTensorPointSet>
::InitializeFEMObject(FEMObjectPointer fem)
{
    NodeType::Pointer  n;
    PointsIterator it = this->m_Mesh->GetPoints()->Begin();

    FEMVectorType pt(ImageDimension);

    //Initialize Nodes
    while(it != this->GetInput()->GetPoints()->End())
    {
        for(unsigned i = 0; i < ImageDimension; i++)
        {
            pt[i]= it.Value()[i];
        }

        n =  NodeType::New();
        n->SetCoordinates(pt);
        n->SetGlobalNumber(it.Index());

        fem->AddNextNode(n);
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
        case CellType::TRIANGLE_CELL:
            {
                FEMTriangleType::Pointer triangleEle = FEMTriangleType::New();

                TriangleType * triangle = dynamic_cast<TriangleType *>( cell );

                PointIdIterator pointIdIter = triangle->PointIdsBegin();
                PointIdIterator pointIdEnd = triangle->PointIdsEnd();
                i = 0;
                while( pointIdIter != pointIdEnd )
                {
                    triangleEle->SetNode(i++, fem->GetNode(*pointIdIter));
                    ++pointIdIter;
                }

                triangleEle->SetGlobalNumber(gn++);
                fem->AddNextElement(triangleEle.GetPointer());

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
                    tetrahedronEle->SetNode(i++, fem->GetNode(*pointIdIter));

                    ++pointIdIter;
                }

                tetrahedronEle->SetGlobalNumber(gn++);
                fem->AddNextElement(tetrahedronEle.GetPointer());

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

        fem->AddNextLoad(fem::Load::Pointer(load.GetPointer()));

        ++itDis;
        ++it;
    }

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
} // end namespace itk

#endif
