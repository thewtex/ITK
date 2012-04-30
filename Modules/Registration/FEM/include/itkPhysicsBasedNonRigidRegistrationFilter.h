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

#ifndef __itkPhysicsBasedNonRigidRegistrationFilter_h
#define __itkPhysicsBasedNonRigidRegistrationFilter_h


#include "itkMaskFeaturePointSelectionFilter.h"
#include "itkBlockMatchingImageFilter.h"
#include "itkFEMScatteredDataPointSetToImageFilter.h"


namespace itk
{
namespace fem
{

/** \class PhysicsBasedNonRigidRegistrationFilter
 *    \brief Physics-Based Non-Rigid Registration Filter.
 * <description>
 * . . .
 * \ingroup ITKFEMRegistration
 */

template <class TFixedImage, class TMovingImage, class TMaskImage, class TMesh, class TDeformationField>
class ITK_EXPORT  PhysicsBasedNonRigidRegistrationFilter : public ImageToImageFilter<TMovingImage, TDeformationField>
{
public:
  typedef PhysicsBasedNonRigidRegistrationFilter               Self;
  typedef ImageToImageFilter<TMovingImage, TDeformationField>  Superclass;
  typedef SmartPointer<Self>                                   Pointer;
  typedef SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( TDisplacementField, ImageToImageFilter );

  typedef TMovingImage                       MovingImageType;
  typedef TFixedImage                        FixedImageType;
  typedef TMaskImage                         MaskImageType;
  typedef TMesh                              MeshType;
  typedef TDeformationField                  DeformationFieldType;

  /** Dimensionality of input and output data is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned, FixedImageType::ImageDimension);

  /** Not input specific typedefs */
  typedef ImageRegion< ImageDimension >  ImageRegionType;
  typedef Size< ImageDimension >         ImageSizeType;
  typedef Index< ImageDimension >        ImageIndexType;

  /** set fraction of eligible points to select */
  itkSetMacro(SelectFraction, double);
  itkGetConstMacro(SelectFraction, double);

  /** set/get non-connectivity */
  itkSetMacro(NonConnectivity, unsigned);
  itkGetConstMacro(NonConnectivity, unsigned);

  /** set/get half size */
  itkSetMacro(BlockRadius, ImageSizeType);
  itkGetConstMacro(BlockRadius, ImageSizeType);

  /** set/get half window */
  itkSetMacro(SearchRadius, ImageSizeType);
  itkGetConstMacro(SearchRadius, ImageSizeType);

  /** set/get step */
  itkSetMacro(BlockStep, ImageSizeType);
  itkGetConstMacro(BlockStep, ImageSizeType);

  /** set/get number of approximation steps */
  itkSetMacro(ApproximationSteps, unsigned);
  itkGetMacro(ApproximationSteps, unsigned);

  /** set/get number of outlier rejection steps */
  itkSetMacro(OutlierRejectionSteps, unsigned);
  itkGetMacro(OutlierRejectionSteps, unsigned);

  /** set/get fixed image */
  itkSetInputMacro(FixedImage, FixedImageType);//
  itkGetInputMacro(FixedImage, FixedImageType);//

  /** set/get moving image */
  itkSetInputMacro(MovingImage, MovingImageType);//
  itkGetInputMacro(MovingImage, MovingImageType);//

  /** set/get moving image */
  itkSetInputMacro(MaskImage, MaskImageType);//
  itkGetInputMacro(MaskImage, MaskImageType);//

  /** set/get moving image */
  itkSetInputMacro(Mesh, MeshType);//
  itkGetInputMacro(Mesh, MeshType);//


  void GenerateData();


protected:
  PhysicsBasedNonRigidRegistrationFilter();
  ~PhysicsBasedNonRigidRegistrationFilter();
  void PrintSelf( std::ostream & os, Indent indent ) const;


private:
  //purposely not implemented
  PhysicsBasedNonRigidRegistrationFilter( const PhysicsBasedNonRigidRegistrationFilter & );
  void operator=( const PhysicsBasedNonRigidRegistrationFilter & );

  // algorithm parameters
  double         m_SelectFraction;
  unsigned       m_NonConnectivity;
  ImageSizeType  m_BlockRadius;
  ImageSizeType  m_SearchRadius;
  ImageSizeType  m_BlockStep;
  unsigned       m_ApproximationSteps;
  unsigned       m_OutlierRejectionSteps;
};

}
}  // end namespace itk::fem

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPhysicsBasedNonRigidRegistrationFilter.hxx"
#endif

#endif
