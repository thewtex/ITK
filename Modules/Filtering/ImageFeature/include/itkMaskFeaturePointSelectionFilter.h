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
#ifndef __itkMaskFeaturePointSelectionFilter_h
#define __itkMaskFeaturePointSelectionFilter_h

#include "itkImageToMeshFilter.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"

namespace itk
{
/** \class MaskFeaturePointSelectionFilter
 * \brief Generate a PointSet containing the feature points
 * selected from a masked 3D input image.
 *
 * MaskFeaturePointSelectionFilter takes 3D image and 3D mask as inputs
 * and generates a PointSet of feature points as output.
 *
 * This filter is intended to be used for initializing the process
 * of Physics-Based Non-Rigid Registration. It selects a fraction
 * of non-masked points with highest variance, avoiding picking
 * points near each other based on connexity [ M. Bierling, Displacement estimation
 * by hierarchical block matching, Proc. SPIE Vis. Comm. and Image Proc., vol. 1001,
 * pp. 942–951, 1988. ].
 *
 * The filter is templated over input image and mask and output pointset.
 * Only 3D implementation is available.
 *
 * \ingroup ImageFeatureExtraction
 * \ingroup ITKImageFeature
 */
template< class TImage, class TMask, class TPointSet >
class ITK_EXPORT MaskFeaturePointSelectionFilter;

template< class TImagePixel, class TMaskPixel >
class ITK_EXPORT MaskFeaturePointSelectionFilter<
  Image< TImagePixel, 3 >, Image< TMaskPixel, 3 >, PointSet< Index< 3 >, 3 > >:
public ImageToMeshFilter< Image< TImagePixel, 3 >, PointSet< Index< 3 >, 3 > >
{
public:
  /** Standard class typedefs. */
  typedef MaskFeaturePointSelectionFilter  Self;
  typedef ProcessObject                    Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaskFeaturePointSelectionFilter, ImageToMeshFilter);

  /** Some typedefs associated with the input image and mask. */
  typedef Image< TImagePixel, 3 >                InputImageType;
  typedef typename InputImageType::SizeType      InputImageSizeType;
  typedef typename InputImageType::ConstPointer  InputImageConstPointer;
  typedef typename InputImageType::RegionType    InputImageRegionType;
  typedef typename InputImageType::PixelType     InputImagePixelType;
  typedef Image< TMaskPixel, 3 >                 MaskImageType;
  typedef Index< 3 >                             IndexType;
  typedef typename MaskImageType::ConstPointer   MaskImageConstPointer;
  typedef typename MaskImageType::RegionType     MaskImageRegionType;
  typedef typename MaskImageType::PixelType      MaskImagePixelType;

  typedef ConstNeighborhoodIterator< InputImageType >  InputImageIterator;
  typedef ImageRegionConstIterator< MaskImageType >    MaskImageIterator;

  /** Some typedefs associated with the output pointset. */
  typedef PointSet< Index< 3 >, 3 >                                OutputPointSetType;
  typedef typename OutputPointSetType::PointType                   PointType;
  typedef typename OutputPointSetType::PixelType                   OutputPointSetTypePixelType;
  typedef typename OutputPointSetType::Pointer                     OutputPointSetPointer;
  typedef typename OutputPointSetType::ConstPointer                OutputPointSetConstPointer;
  typedef typename OutputPointSetType::PointsContainer             PointsContainer;
  typedef typename OutputPointSetType::PointIdentifier             PointIdentifier;
  typedef typename PointsContainer::Pointer                        PointsContainerPointer;
  typedef typename PointsContainer::Iterator                       PointsContainerIterator;
  typedef typename OutputPointSetType::PointDataContainer          PointDataContainer;
  typedef typename PointDataContainer::Pointer                     PointDataContainerPointer;
  typedef typename PointDataContainer::Iterator                    PointDataContainerIterator;


  void GenerateData();

  /** set/get mask */
  itkSetInputMacro(MaskImage, MaskImageType);//
  itkGetInputMacro(MaskImage, MaskImageType);//

  /** set/get half size of the block for calculating variance */
  itkSetMacro(BlockHalfSize, InputImageSizeType);
  itkGetConstMacro(BlockHalfSize, InputImageSizeType);

  /** set/get connexity where surrounding points cannot be selected  */
  void SetBlockNonConnexity(int connex) throw ( ExceptionObject );
  itkGetConstMacro(BlockNonConnexity, unsigned char);

  /** set fraction of eligible points to be rejected */
  itkSetMacro(BlockRejectedFraction, double);
  itkGetConstMacro(BlockRejectedFraction, double);

protected:
  MaskFeaturePointSelectionFilter();
  ~MaskFeaturePointSelectionFilter();
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  //purposely not implemented purposely
  MaskFeaturePointSelectionFilter(const MaskFeaturePointSelectionFilter &);
  void operator=(const MaskFeaturePointSelectionFilter &);

  unsigned char       m_BlockNonConnexity;
  InputImageSizeType  m_BlockHalfSize;
  double              m_BlockRejectedFraction;

  // helper structure, stores temporary information about feature points
  struct FeaturePoint
  {
    IndexType idx;
    PointType coor;
    double variance;

    bool operator<( const FeaturePoint& r ) const
    {
      return variance < r.variance;
    }
  };
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaskFeaturePointSelectionFilter.hxx"
#endif

#endif
