/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef itkConnectedRegionsMeshFilter_hxx
#define itkConnectedRegionsMeshFilter_hxx

#include "itkNumericTraits.h"
#include "itkObjectFactory.h"

#include <set>

namespace itk
{
/**
 * ------------------------------------------------
 */
template <typename TInputMesh, typename TOutputMesh>
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::ConnectedRegionsMeshFilter()
  : m_ExtractionMode(Self::LargestRegion)
  , m_NumberOfCellsInRegion(SizeValueType{})
  , m_RegionNumber(IdentifierType{})

{
  m_ClosestPoint.Fill(0);
}

/**
 * ------------------------------------------------
 */
template <typename TInputMesh, typename TOutputMesh>
void
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::DeleteSeed(IdentifierType id)
{
  std::vector<IdentifierType> tmpVector;
  for (const auto & seed : m_SeedList)
  {
    if (seed != id)
    {
      tmpVector.push_back(seed);
    }
  }
  m_SeedList.clear();
  for (const auto & seedID : tmpVector)
  {
    m_SeedList.push_back(seedID);
  }
}

/**
 * ------------------------------------------------
 */
template <typename TInputMesh, typename TOutputMesh>
void
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::DeleteSpecifiedRegion(IdentifierType id)
{
  std::vector<IdentifierType> tmpVector;
  for (const auto & regionID : m_RegionList)
  {
    if (regionID != id)
    {
      tmpVector.push_back(regionID);
    }
  }
  m_RegionList.clear();
  for (const auto & regionID : tmpVector)
  {
    m_RegionList.push_back(regionID);
  }
}

/**
 * ------------------------------------------------
 */
template <typename TInputMesh, typename TOutputMesh>
void
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Extraction Mode: ";
  if (m_ExtractionMode == Self::PointSeededRegions)
  {
    os << "Point Seeded Regions" << std::endl;
  }
  else if (m_ExtractionMode == Self::CellSeededRegions)
  {
    os << "Cell Seeded Regions" << std::endl;
  }
  else if (m_ExtractionMode == Self::SpecifiedRegions)
  {
    os << "Specified Regions" << std::endl;
  }
  else if (m_ExtractionMode == Self::LargestRegion)
  {
    os << "Largest Region" << std::endl;
  }
  else if (m_ExtractionMode == Self::AllRegions)
  {
    os << "All Regions" << std::endl;
  }
  else if (m_ExtractionMode == Self::ClosestPointRegion)
  {
    os << "Closest Point Region" << std::endl;
  }
}

/**
 *
 */
template <typename TInputMesh, typename TOutputMesh>
void
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::GenerateData()
{
  const InputMeshConstPointer                  input = this->GetInput();
  const OutputMeshPointer                      output = this->GetOutput();
  const InputMeshPointsContainerConstPointer   inPts = input->GetPoints();
  const InputMeshCellsContainerConstPointer    inCells = input->GetCells();
  const InputMeshCellDataContainerConstPointer inCellData = input->GetCellData();

  itkDebugMacro("Executing connectivity");

  //  Check input/allocate storage
  const IdentifierType numCells = input->GetNumberOfCells();
  const IdentifierType numPts = input->GetNumberOfPoints();
  if (numPts < 1 || numCells < 1)
  {
    itkDebugMacro("No data to connect!");
    return;
  }

  // Initialize.  Keep track of points and cells visited.
  input->BuildCellLinks(); // needed to get neighbors

  m_RegionSizes.clear();
  m_Visited.resize(numCells);
  for (unsigned int i = 0; i < numCells; ++i)
  {
    m_Visited[i] = -1;
  }

  // Traverse all cells marking those visited.  Each new search
  // starts a new connected region. Connected region grows
  // using a connected wave propagation.
  //
  m_Wave = new std::vector<IdentifierType>;
  m_Wave2 = new std::vector<IdentifierType>;
  m_Wave->reserve(numPts / 4);
  m_Wave2->reserve(numPts / 4);

  // variable used to keep track of propagation
  m_RegionNumber = 0;
  IdentifierType maxCellsInRegion = 0;
  IdentifierType largestRegionId = 0;

  const int tenth = numCells / 10 + 1;
  int       cellId = 0;
  if (m_ExtractionMode != PointSeededRegions && m_ExtractionMode != CellSeededRegions &&
      m_ExtractionMode != ClosestPointRegion)
  { // visit all cells marking with region number
    for (CellsContainerConstIterator cell = inCells->Begin(); cell != inCells->End(); ++cell, ++cellId)
    {
      if (!(cellId % tenth))
      {
        this->UpdateProgress(static_cast<float>(cellId) / numCells);
      }

      if (m_Visited[cellId] < 0)
      {
        m_NumberOfCellsInRegion = 0;
        m_Wave->push_back(cellId);
        this->PropagateConnectedWave();

        if (m_NumberOfCellsInRegion > maxCellsInRegion)
        {
          maxCellsInRegion = m_NumberOfCellsInRegion;
          largestRegionId = m_RegionNumber;
        }

        ++m_RegionNumber;
        m_RegionSizes.push_back(m_NumberOfCellsInRegion);
        m_Wave->clear();
        m_Wave2->clear();
      }
    }
  }
  // Otherwise, seeds are used to indicate the region
  else
  {
    m_NumberOfCellsInRegion = 0;
    if (m_ExtractionMode == PointSeededRegions)
    {
      auto cellLinks = input->GetCellLinks();
      for (const auto & i : m_SeedList)
      {
        auto links = cellLinks->ElementAt(i);
        for (auto citer = links.cbegin(); citer != links.cend(); ++citer)
        {
          m_Wave->push_back(*citer);
        }
      }
    }
    // use the seeds directly
    else if (m_ExtractionMode == CellSeededRegions)
    {
      for (auto & i : m_SeedList)
      {
        m_Wave->push_back(i);
      }
    }
    // find the closest point and get the cells using it as the seeds
    else if (m_ExtractionMode == ClosestPointRegion)
    {
      // find the closest point
      double                   minDist2 = NumericTraits<double>::max();
      InputMeshPointIdentifier minId = 0;
      for (PointsContainerConstIterator piter = inPts->Begin(); piter != inPts->End(); ++piter)
      {
        const auto   x = piter->Value();
        const double dist2 = x.SquaredEuclideanDistanceTo(m_ClosestPoint);
        if (dist2 < minDist2)
        {
          minId = piter.Index();
          minDist2 = dist2;
        }
      }

      // get the cells using the closest point and use them as seeds
      const InputMeshCellLinksContainerConstPointer cellLinks = input->GetCellLinks();

      auto links = cellLinks->ElementAt(minId);
      for (auto citer = links.cbegin(); citer != links.cend(); ++citer)
      {
        m_Wave->push_back(*citer);
      }
    } // closest point

    // now propagate a wave
    this->PropagateConnectedWave();
    itkAssertOrThrowMacro(m_RegionNumber < m_RegionSizes.size(), "Region number exceeds region sizes.");
    m_RegionSizes[m_RegionNumber] = m_NumberOfCellsInRegion;
  }

  delete m_Wave;
  delete m_Wave2;
  m_Wave = m_Wave2 = nullptr;

  itkDebugMacro("Extracted " << m_RegionNumber << " region(s)");

  // Pass the point and point data through
  this->CopyInputMeshToOutputMeshPoints();
  this->CopyInputMeshToOutputMeshPointData();

  // Create output cells
  //
  const InputMeshCellsContainerPointer    outCells(InputMeshCellsContainer::New());
  const InputMeshCellDataContainerPointer outCellData(InputMeshCellDataContainer::New());
  cellId = 0;

  CellDataContainerConstIterator cellData;
  const bool                     CellDataPresent = (nullptr != inCellData && !inCellData->empty());
  InputMeshCellPointer           cellCopy; // need an autopointer to duplicate a cell
  int                            inserted_count = 0;

  if (m_ExtractionMode == PointSeededRegions || m_ExtractionMode == CellSeededRegions ||
      m_ExtractionMode == ClosestPointRegion || m_ExtractionMode == AllRegions)
  { // extract any cell that's been visited
    if (CellDataPresent)
    {
      cellData = inCellData->Begin();
    }
    for (auto cell = inCells->Begin(); cell != inCells->End(); ++cell, ++cellId)
    {
      if (m_Visited[cellId] >= 0)
      {
        cell->Value()->MakeCopy(cellCopy);
        outCells->InsertElement(inserted_count, cellCopy.GetPointer());
        ++inserted_count;
        cellCopy.ReleaseOwnership(); // Pass cell ownership to output mesh
        if (CellDataPresent)
        {
          outCellData->InsertElement(cellId, cellData->Value());
          ++cellData;
        }
      }
    }
  }
  // if specified regions, add regions
  else if (m_ExtractionMode == SpecifiedRegions)
  {
    bool inReg = false;
    if (CellDataPresent)
    {
      cellData = inCellData->Begin();
    }
    for (auto cell = inCells->Begin(); cell != inCells->End(); ++cell, ++cellId)
    {
      if (m_Visited[cellId] >= 0)
      {
        auto regionId = static_cast<IdentifierType>(m_Visited[cellId]);
        // see if cell is on region
        for (const auto & regionID : m_RegionList)
        {
          if (regionID == regionId)
          {
            inReg = true;
            break;
          }
        }

        if (inReg)
        {
          cell->Value()->MakeCopy(cellCopy);
          outCells->InsertElement(inserted_count, cellCopy.GetPointer());
          cellCopy.ReleaseOwnership(); // Pass cell ownership to output mesh
          ++inserted_count;
          if (CellDataPresent)
          {
            outCellData->InsertElement(cellId, cellData->Value());
            ++cellData;
          }
          inReg = false;
        }
      }
    }
  }
  else // we are extracting the largest region
  {
    if (CellDataPresent)
    {
      cellData = inCellData->Begin();
    }

    for (auto cell = inCells->Begin(); cell != inCells->End(); ++cell, ++cellId)
    {
      if (m_Visited[cellId] == static_cast<OffsetValueType>(largestRegionId))
      {
        cell->Value()->MakeCopy(cellCopy);
        outCells->InsertElement(inserted_count, cellCopy.GetPointer());
        cellCopy.ReleaseOwnership(); // Pass cell ownership to output mesh
        ++inserted_count;
        if (CellDataPresent)
        {
          outCellData->InsertElement(cellId, cellData->Value());
          ++cellData;
        }
      }
    }
  }

  // Set the output and clean up
  output->SetCells(outCells);
  output->SetCellData(outCellData);
  m_Visited.clear();

  // Report some statistics
#ifndef NDEBUG
  if (this->GetDebug())
  {
    SizeValueType count = 0;
    for (const auto & regionSize : m_RegionSizes)
    {
      count += regionSize;
    }
    itkDebugMacro("Total #of cells accounted for: " << count);
    itkDebugMacro("Extracted " << output->GetNumberOfCells() << " cells");
  }
#endif

  // This prevents unnecessary re-executions of the pipeline.
  output->SetBufferedRegion(output->GetRequestedRegion());
}

template <typename TInputMesh, typename TOutputMesh>
void
ConnectedRegionsMeshFilter<TInputMesh, TOutputMesh>::PropagateConnectedWave()
{
  const InputMeshConstPointer input = this->GetInput();

  const InputMeshCellLinksContainerConstPointer cellLinks = input->GetCellLinks();

  while (!m_Wave->empty())
  {
    for (const auto cellId : *m_Wave)
    {
      if (m_Visited[cellId] < 0)
      {
        m_Visited[cellId] = static_cast<OffsetValueType>(m_RegionNumber);
        ++m_NumberOfCellsInRegion;

        // now get the cell points, and then cells using these points
        InputMeshCellPointer cellPtr;
        input->GetCell(cellId, cellPtr);
        for (auto piter = cellPtr->PointIdsBegin(); piter != cellPtr->PointIdsEnd(); ++piter)
        {
          InputMeshCellLinksContainer links = cellLinks->ElementAt(*piter);
          for (auto citer = links.begin(); citer != links.end(); ++citer)
          {
            if (m_Visited[*citer] < 0)
            {
              m_Wave2->push_back(*citer);
            }
          }
        }
      } // if visited
    } // for all cells in wave

    std::vector<IdentifierType> * tmpWave = m_Wave;
    m_Wave = m_Wave2;
    m_Wave2 = tmpWave;
    tmpWave->clear();
  } // while wave is propagating
}
} // end namespace itk

#endif
