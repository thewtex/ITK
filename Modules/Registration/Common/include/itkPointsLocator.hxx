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
#ifndef itkPointsLocator_hxx
#define itkPointsLocator_hxx

namespace itk
{

template <typename TPointsContainer>
PointsLocator<TPointsContainer>::PointsLocator()
  : m_SampleAdaptor(SampleAdaptorType::New())
  , m_KdTreeGenerator(TreeGeneratorType::New())
{}

template <typename TPointsContainer>
void
PointsLocator<TPointsContainer>::Initialize()
{
  if (!this->m_Points)
  {
    itkExceptionMacro("The points have not been set (m_Points == nullptr)");
  }
  if (this->m_Points->Size() == 0)
  {
    itkExceptionMacro("The number of points is 0.");
  }

  this->m_SampleAdaptor = SampleAdaptorType::New();
  this->m_KdTreeGenerator = TreeGeneratorType::New();

  // Lack of const-correctness in the PointSetAdaptor should be fixed.
  this->m_SampleAdaptor->SetVectorContainer(const_cast<PointsContainer *>(this->m_Points.GetPointer()));

  this->m_SampleAdaptor->SetMeasurementVectorSize(PointDimension);

  this->m_KdTreeGenerator->SetSample(this->m_SampleAdaptor);
  this->m_KdTreeGenerator->SetBucketSize(16);

  this->m_KdTreeGenerator->Update();

  this->m_Tree = this->m_KdTreeGenerator->GetOutput();
}

template <typename TPointsContainer>
auto
PointsLocator<TPointsContainer>::FindClosestPoint(const PointType & query) const -> PointIdentifier
{
  NeighborsIdentifierType identifiers;
  this->m_Tree->Search(query, 1u, identifiers);

  return identifiers[0];
}


template <typename TPointsContainer>
void
PointsLocator<TPointsContainer>::FindClosestNPoints(const PointType &         query,
                                                    unsigned int              numberOfNeighborsRequested,
                                                    NeighborsIdentifierType & identifiers) const
{
  unsigned int N = numberOfNeighborsRequested;
  if (N > this->m_Points->Size())
  {
    N = this->m_Points->Size();

    itkWarningMacro("The number of requested neighbors is greater than the "
                    << "total number of points.  Only returning " << N << " points.");
  }
  this->m_Tree->Search(query, N, identifiers);
}

template <typename TPointsContainer>
void
PointsLocator<TPointsContainer>::FindClosestNPoints(const PointType &         query,
                                                    unsigned int              numberOfNeighborsRequested,
                                                    NeighborsIdentifierType & identifiers,
                                                    std::vector<double> &     distances) const
{
  unsigned int N = numberOfNeighborsRequested;
  if (N > this->m_Points->Size())
  {
    N = this->m_Points->Size();

    itkWarningMacro("The number of requested neighbors is greater than the "
                    << "total number of points.  Only returning " << N << " points.");
  }
  this->m_Tree->Search(query, N, identifiers, distances);
}

template <typename TPointsContainer>
void
PointsLocator<TPointsContainer>::FindPointsWithinRadius(const PointType &         query,
                                                        double                    radius,
                                                        NeighborsIdentifierType & identifiers) const
{
  this->m_Tree->Search(query, radius, identifiers);
}

/**
 * Print out internals
 */
template <typename TPointsContainer>
void
PointsLocator<TPointsContainer>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk

#endif
