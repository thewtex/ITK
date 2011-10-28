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
#ifndef __itkBlockMatchingImageFilter_h
#define __itkBlockMatchingImageFilter_h

#include "itkMeshToMeshFilter.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkVector.h"
#include "itkDefaultDynamicMeshTraits.h"


namespace itk
{
/** \class BlockMatchingImageFilter
 * \brief Computes displacements of given points from a fixed image in a
 * floating image.
 *
 * BlockMatchingImageFilter takes fixed and floating Images as well as
 * PointSet of feature points as inputs. Image coordinates of input feature
 * points are stored as pixel values while physical coordinates are stored
 * as point coordinates. Points and Pixels of the input point set must have
 * unique identifiers within range 0..N-1, where N is the number of points.
 * Additionally, by default, feature points are expected to lie at least
 * (BlockHalfWindow + BlockHalfSize) voxels from a boundary. This is usually
 * achieved by using an appropriate mask during selection of feature points.
 * If you are unsure whether feature points satisfy the above condition set
 * CheckBoundary flag to true which turns on boundary checks.
 * The default output(0) is a PointSet with displacements stored as vectors.
 * Optional output(1) is a PointSet containing similarities.  The number of
 * points in the output PointSet is equal to the number of points in the
 * input PointSet.
 *
 * The filter is templated over fixed Image, floating Image, input PointSet,
 * output displacements PointSet and output similarities PointSet.
 *
 * This filter is intended to be used in the process of Physics-Based
 * Non-Rigid Registration. It computes displacement for selected points based
 * on similarity [M. Bierling, Displacement estimation by hierarchical block
 * matching, Proc. SPIE Vis. Comm. and Image Proc., vol. 1001, pp. 942–951,
 * 1988.].
 *
 * \ingroup ITKRegistration
 */

template<
  class TFixed,
  class TFloating,
  class TFeatures = PointSet< Index< TFixed::ImageDimension >, TFixed::ImageDimension >,
  class TDisplacements = PointSet< Vector< TFeatures::PointType::ValueType, TFeatures::PointDimension >, TFeatures::PointDimension >,
  class TSimilarities = PointSet< TDisplacements::PointType::ValueType, TDisplacements::PointDimension > >
class ITK_EXPORT BlockMatchingImageFilter;

template<
  class TFixedPixel,
  class TFloatingPixel,
  class TFeaturesTraits,
  class TDisplacement,
  class TDisplacementsTraits,
  class TSimilarity,
  class TSimilaritiesTraits,
  unsigned VImageDimension >
class ITK_EXPORT BlockMatchingImageFilter<
  Image< TFixedPixel, VImageDimension >,
  Image< TFloatingPixel, VImageDimension >,
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >,
  PointSet< TSimilarity, VImageDimension, TSimilaritiesTraits > >:
public MeshToMeshFilter<
  PointSet< Index< VImageDimension >, VImageDimension, TFeaturesTraits >,
  PointSet< Vector< TDisplacement, VImageDimension >, VImageDimension, TDisplacementsTraits >
>
{
public:
  /** Standard class typedefs. */
  typedef BlockMatchingImageFilter         Self;
  typedef ProcessObject                    Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  itkStaticConstMacro(ImageDimension, unsigned, VImageDimension);

  /** Not input specific typedefs */
  typedef ImageRegion< VImageDimension >  ImageRegionType;
  typedef Size< VImageDimension >         ImageSizeType;
  typedef Index< VImageDimension >        ImageIndexType;

  /** Fixed image typedefs. */
  typedef Image< TFixedPixel, VImageDimension >                  FixedImageType;
  typedef typename FixedImageType::ConstPointer                  FixedImageConstPointer;
  typedef typename FixedImageType::PixelType                     FixedImagePixelType;

  /** Floating image typedefs. */
  typedef Image< TFloatingPixel, VImageDimension >               FloatingImageType;
  typedef typename FloatingImageType::ConstPointer               FloatingImageConstPointer;

  /** Feature points pointset typedefs. */
  typedef PointSet< Index< VImageDimension >, VImageDimension,
    TFeaturesTraits >                                            FeaturePointsType;
  typedef typename FeaturePointsType::Pointer                    FeaturePointsPointer;
  typedef typename FeaturePointsType::ConstPointer               FeaturePointsConstPointer;
  typedef typename FeaturePointsType::PointType                  FeaturePointsPhysicalCoordinates;
  typedef typename FeaturePointsType::PixelType                  FeaturePointsIndexCoordinates;

  /** Displacement vectors typedefs. */
  typedef PointSet< Vector< TDisplacement, VImageDimension >,
    VImageDimension, TDisplacementsTraits >                      DisplacementsType;
  typedef typename DisplacementsType::Pointer                    DisplacementsPointer;
  typedef typename DisplacementsType::ConstPointer               DisplacementsConstPointer;
  typedef typename DisplacementsType::PixelType                  DisplacementsVector;

  /** Displacement similarities typedefs. */
  typedef PointSet< TSimilarity, VImageDimension,
    TSimilaritiesTraits >                                        SimilaritiesType;
  typedef typename SimilaritiesType::Pointer                     SimilaritiesPointer;
  typedef typename SimilaritiesType::ConstPointer                SimilaritiesConstPointer;
  typedef typename SimilaritiesType::PixelType                   SimilaritiesValue;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BlockMatchingImageFilter, MeshToMeshFilter);

  /** set/get half size */
  itkSetMacro(BlockHalfSize, ImageSizeType);
  itkGetConstMacro(BlockHalfSize, ImageSizeType);

  /** set/get half window */
  itkSetMacro(BlockHalfWindow, ImageSizeType);
  itkGetConstMacro(BlockHalfWindow, ImageSizeType);

  /** set/get step */
  itkSetMacro(BlockStep, ImageSizeType);
  itkGetConstMacro(BlockStep, ImageSizeType);

  /** set/get fixed image */
  itkSetInputMacro(FixedImage, FixedImageType);//
  itkGetInputMacro(FixedImage, FixedImageType);//

  /** set/get floating image */
  itkSetInputMacro(FloatingImage, FloatingImageType);//
  itkGetInputMacro(FloatingImage, FloatingImageType);//

  /** set/get point list */
  itkSetInputMacro(FeaturePoints, FeaturePointsType);//
  itkGetInputMacro(FeaturePoints, FeaturePointsType);//

  /** enable/disable boundary check */
  itkSetMacro(CheckBoundary, bool);
  itkGetConstMacro(CheckBoundary, bool);
  itkBooleanMacro(CheckBoundary);

  /** return the number of required inputs, enables check for required inputs to work correctly */
  std::vector< SmartPointer< DataObject > >::size_type GetNumberOfValidRequiredInputs() const;

  /** MakeOutput is provided for handling multiple outputs */
  DataObject::Pointer MakeOutput(unsigned idx);

  inline DisplacementsType * GetDisplacements()
  {
    return dynamic_cast< DisplacementsType * >( this->ProcessObject::GetOutput( 0 ) );
  }

  inline SimilaritiesType * GetSimilarities()
  {
    return dynamic_cast< SimilaritiesType * >( this->ProcessObject::GetOutput( 1 ) );
  }

  void AllocateOutputs();

  /** Generate temporary containers to be used by individual threads exclusively */
  void BeforeThreadedGenerateData();

  /** Compose pieces computed by each thread into a single output */
  void AfterThreadedGenerateData();

  /** Start multithreader here since MeshToMesh filter does not provide multithreaded support */
  void GenerateData();

  void ThreadedGenerateData( ThreadIdType threadId ) ITK_NO_RETURN;

protected:
  BlockMatchingImageFilter();
  ~BlockMatchingImageFilter();
  void PrintSelf( std::ostream & os, Indent indent ) const;

  /** Static function used as a "callback" by the MultiThreader.  The threading
   * library will call this routine for each thread, which will delegate the
   * control to ThreadedGenerateData(). */
  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void *arg);

  /** Internal structure used for passing image data into the threading library
    */
  struct ThreadStruct {
    Pointer Filter;
  };

private:
  //purposely not implemented
  BlockMatchingImageFilter( const BlockMatchingImageFilter & );
  void operator=( const BlockMatchingImageFilter & );

  // algorithm parameters
  ImageSizeType  m_BlockHalfSize;
  ImageSizeType  m_BlockHalfWindow;
  ImageSizeType  m_BlockStep;

  // temporary dynamic arrays for storing threads outputs
  SizeValueType         m_PointsCount;
  DisplacementsVector  *m_DisplacementsVectorsArray;
  SimilaritiesValue    *m_SimilaritiesValuesArray;

  // flag to check if selected points are too close to boundaries
  bool  m_CheckBoundary;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBlockMatchingImageFilter.hxx"
#endif

#endif
