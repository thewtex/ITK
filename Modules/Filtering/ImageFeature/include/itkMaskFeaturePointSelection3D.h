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
#ifndef __itkMaskFeaturePointSelection3D_h
#define __itkMaskFeaturePointSelection3D_h

#include "itkImageToMeshFilter.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"

namespace itk
{
/** \class MaskFeaturePointSelection3D
 * \brief Generate a PointSet containing the feature points
 * selected from a masked 3D input image.
 *
 * MaskFeaturePointSelection3D takes 3D image and 3D mask as inputs
 * and generates a PointSet as output.
 *
 * This filter is intended to be used for initializing the process
 * of Physics-Based Non-Rigid Registration.
 *
 * The filter is templated over the input image pixel type and
 * the mask pixel type.
 */
template< class TRealCoor, class TImagePixel, class TMaskPixel >
class ITK_EXPORT MaskFeaturePointSelection3D:
  public ImageToMeshFilter< Image< TImagePixel, 3 >, PointSet< Index< 3 >, 3,
                            DefaultStaticMeshTraits< Index< 3 >, 3, 3, TRealCoor > > >
{
public:
  /** Standard class typedefs. */
  typedef MaskFeaturePointSelection3D		   Self;
  typedef ProcessObject                        Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer< const Self >           ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaskFeaturePointSelection3D, ImageToMeshFilter);

  /** Some typedefs associated with the input image and mask. */
  typedef Image< TImagePixel, 3 >               InputImageType;
  typedef typename InputImageType::SizeType     InputImageSizeType;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef Image< TMaskPixel, 3 >                MaskImageType;
  typedef typename MaskImageType::ConstPointer  MaskImageConstPointer;
  typedef typename MaskImageType::RegionType    MaskImageRegionType;
  typedef typename MaskImageType::PixelType     MaskImagePixelType;

  typedef ConstNeighborhoodIterator< InputImageType >	InputImageIterator;
  typedef ImageRegionConstIterator< MaskImageType >     MaskImageIterator;

  /** Some typedefs associated with the output pointset. */
  typedef DefaultStaticMeshTraits< Index< 3 >, 3, 3, TRealCoor > MeshTraitsType;
  typedef PointSet< Index< 3 >, 3, MeshTraitsType >       OutputPointSetType;
  typedef typename OutputPointSetType::PointType          PointType;
  typedef typename OutputPointSetType::Pointer            OutputPointSetPointer;
  typedef typename OutputPointSetType::ConstPointer       OutputPointSetConstPointer;
  typedef typename OutputPointSetType::PointsContainer    PointsContainer;
  typedef typename OutputPointSetType::PointIdentifier    PointIdentifier;
  typedef typename PointsContainer::Pointer               PointsContainerPointer;
  typedef typename PointsContainer::Iterator              PointsContainerIterator;
  typedef typename OutputPointSetType::PointDataContainer PointDataContainer;
  typedef typename PointDataContainer::Pointer            PointDataContainerPointer;
  typedef typename PointDataContainer::Iterator           PointDataContainerIterator;

  typedef Index< 3 > IndexType;
  struct FeaturePointType
  {
    IndexType idx;
    PointType coor;
    double mean;
    double variance;
    bool selected;

    bool operator<( const Self::FeaturePointType& r ) const { return variance < r.variance; }
  };

  /**  */
  void GenerateData(void);

  /**  */
  void GenerateOutputInformation(void);

  /** accept the input image */
  void SetInputImage(const InputImageType *inputImage);

  /** accept the mask */
  void SetMaskImage(const MaskImageType *inputMask);

  void SetBlockNonConnexityType(int connex);

  void SetBlockRejectedFraction(double fraction);

protected:
  MaskFeaturePointSelection3D();
  ~MaskFeaturePointSelection3D();
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  //purposely not implemented purposely
  MaskFeaturePointSelection3D(const MaskFeaturePointSelection3D &);
  void operator=(const MaskFeaturePointSelection3D &);

  int m_BlockNonConnexityType;
  InputImageSizeType m_BlockHalfSize;
  double m_BlockRejectedFraction;

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaskFeaturePointSelection3D.hxx"
#endif

#endif
