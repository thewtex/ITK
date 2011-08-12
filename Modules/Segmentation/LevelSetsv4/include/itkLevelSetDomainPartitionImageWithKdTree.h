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
#ifndef __itkLevelSetDomainPartitionImageWithKdTree_h
#define __itkLevelSetDomainPartitionImageWithKdTree_h

#include "itkLevelSetDomainPartitionImageBase.h"

#include "itkListSample.h"
#include "itkKdTreeGenerator.h"

namespace itk
{
/** \class LevelSetDomainPartitionImageWithKdTree
 *
 * \brief Helper class used to share data in the ScalarChanAndVeseLevelSetFunction.
 *
 */
template< class TImage >
class LevelSetDomainPartitionImageWithKdTree:
  public LevelSetDomainPartitionImageBase< TImage >
{
public:

  typedef LevelSetDomainPartitionImageWithKdTree  Self;
  typedef LevelSetDomainPartitionBase< TImage >   Superclass;
  typedef SmartPointer< Self >                    Pointer;
  typedef SmartPointer< const Self >              ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned int, Superclass::ImageDimension);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  itkTypeMacro( LevelSetDomainPartitionImageWithKdTree,
                LevelSetDomainPartitionImageBase);

  typedef TImage                                   ImageType;
  typedef typename Superclass::ImagePointer        ImagePointer;
  typedef typename Superclass::ImageConstPointer   ImageConstPointer;
  typedef typename Superclass::PixelType           PixelType;
  typedef typename Superclass::RegionType          RegionType;
  typedef typename Superclass::SizeType            SizeType;
  typedef typename Superclass::SizeValueType       SizeValueType;
  typedef typename Superclass::SpacingType         SpacingType;
  typedef typename Superclass::IndexType           IndexType;
  typedef typename Superclass::IndexValueType      IndexValueType;
  typedef typename Superclass::PointType           PointType;

  typedef typename Superclass::IdentifierListType       IdentifierListType;
  typedef typename Superclass::ListImageType            ListImageType;
  typedef typename Superclass::ListImagePointer         ListImagePointer;
  typedef typename Superclass::ListImageConstPointer    ListImageConstPointer;
  typedef typename Superclass::ListRegionType           ListRegionType;
  typedef typename Superclass::ListSizeType             ListSizeType;
  typedef typename Superclass::ListSizeValueType        ListSizeValueType;
  typedef typename Superclass::ListSpacingType          ListSpacingType;
  typedef typename Superclass::ListIndexType            ListIndexType;
  typedef typename Superclass::ListIndexValueType       ListIndexValueType;
  typedef typename Superclass::ListPointType            ListPointType;
  typedef typename Superclass::ListIteratorType         ListIteratorType;

  typedef typename ListPointType::VectorType                    CentroidVectorType;
  typedef typename Statistics::ListSample< CentroidVectorType > SampleType;
  typedef typename Statistics::KdTreeGenerator< SampleType >    TreeGeneratorType;
  typedef typename TreeGeneratorType::Pointer                   TreePointer;
  typedef typename TreeGeneratorType::KdTreeType                TreeType;
  typedef typename TreeType::Pointer                            KdTreePointer;

  /** Initialize with a precomputed kd-tree */
  void SetKdTree(KdTreePointer kdtree)
  {
    this->m_KdTree = kdtree;
  }

  /** Set number of neighbors in the kd-tree leaf node */
  void SetNumberOfNeighbors( const unsigned int& iN )
  {
    m_NumberOfNeighbors = iN;
  }

  /** Get number of neighbors in the kd-tree leaf node */
  void GetNumberOfNeighbors( ) const
    {
    return m_NumberOfNeighbors;
    }

protected:
  LevelSetDomainPartitionImageWithKdTree():Superclass(),
    m_KdTree(NULL),
    m_NumberOfNeighbors( 10 ){}

  ~LevelSetDomainPartitionImageWithKdTree(){}

  KdTreePointer m_KdTree;
  unsigned int  m_NumberOfNeighbors;

  /** Populate a list image with each pixel being a list of overlapping
   *  level set support at that pixel */
  void PopulateListDomain()
  {
    if( this->m_KdTree.IsNotNull() )
      {
      this->PopulateDomainWithKdTree();
      }
    else
      {
      Superclass::PopulateListDomain();
      }
  }

  /** Populate a list image with each pixel being a list of overlapping
   *  level set support at that pixel */
  void PopulateDomainWithKdTree()
  {
    ListSpacingType spacing = this->m_ListDomain->GetSpacing();

    ListRegionType region = this->m_ListDomain->GetLargestPossibleRegion();

    ListIteratorType lIt(this->m_ListDomain, region);

    for ( lIt.GoToBegin(); !lIt.IsAtEnd(); ++lIt )
      {
      ListIndexType ind = lIt.GetIndex();
      ListPointType pt;

      this->m_ListDomain->TransformIndexToPhysicalPoint( ind, pt );

      CentroidVectorType queryPoint = pt.GetVectorFromOrigin();

      typename TreeType::InstanceIdentifierVectorType neighbors;
      this->m_KdTree->Search(queryPoint, this->m_NumberOfNeighbors, neighbors);

      IdentifierListType L;
      for ( unsigned int i = 0; i < this->m_NumberOfNeighbors; ++i )
        {
        // this is not yet defined, but it will have to be !!!
        if ( this->m_LevelSetDataPointerVector[i]->VerifyInsideRegion(ind) )
          {
          L.push_back(neighbors[i]);
          }
        }
      lIt.Set(L);
      }
  }

private:
  //purposely not implemented
  LevelSetDomainPartitionImageWithKdTree(const Self &);
  //purposely not implemented
  void operator=(const Self &);
};
} //end namespace itk

#endif
