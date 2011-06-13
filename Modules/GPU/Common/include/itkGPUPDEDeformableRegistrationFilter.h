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
#ifndef __itkGPUPDEDeformableRegistrationFilter_h
#define __itkGPUPDEDeformableRegistrationFilter_h

#include "itkGPUDenseFiniteDifferenceImageFilter.h"
#include "itkGPUPDEDeformableRegistrationFunction.h"

namespace itk
{
/**
 * \class GPUPDEDeformableRegistrationFilter
 * \brief Deformably register two images using a PDE algorithm with GPU.
 *
 * GPUPDEDeformableRegistrationFilter is a base case for filter implementing
 * a PDE deformable algorithm that register two images by computing the
 * deformation field which will map a moving image onto a fixed image.
 *
 * A deformation field is represented as a image whose pixel type is some
 * vector type with at least N elements, where N is the dimension of
 * the fixed image. The vector type must support element access via operator
 * []. It is assumed that the vector elements behave like floating point
 * scalars.
 *
 * This class is templated over the fixed image type, moving image type
 * and the deformation Field type.
 *
 * The input fixed and moving images are set via methods SetFixedImage
 * and SetMovingImage respectively. An initial deformation field maybe set via
 * SetInitialDeformationField or SetInput. If no initial field is set,
 * a zero field is used as the initial condition.
 *
 * The output deformation field can be obtained via methods GetOutput
 * or GetDeformationField.
 *
 * The PDE algorithm is run for a user defined number of iterations.
 * Typically the PDE algorithm requires period Gaussin smoothing of the
 * deformation field to enforce an elastic-like condition. The amount
 * of smoothing is governed by a set of user defined standard deviations
 * (one for each dimension).
 *
 * In terms of memory, this filter keeps two internal buffers: one for storing
 * the intermediate updates to the field and one for double-buffering when
 * smoothing the deformation field. Both buffers are the same type and size as the
 * output deformation field.
 *
 * This class make use of the finite difference solver hierarchy. Update
 * for each iteration is computed using a GPUPDEDeformableRegistrationFunction.
 *
 * \warning This filter assumes that the fixed image type, moving image type
 * and deformation field type all have the same number of dimensions.
 *
 * \sa GPUPDEDeformableRegistrationFunction.
 * \ingroup DeformableImageRegistration
 */
template< class TFixedImage, class TMovingImage, class TDeformationField,
          class TParentImageFilter = itk::PDEDeformableRegistrationFilter< TFixedImage, TMovingImage, TDeformationField >
        >
class ITK_EXPORT GPUPDEDeformableRegistrationFilter:
  public GPUDenseFiniteDifferenceImageFilter< TDeformationField, TDeformationField, TParentImageFilter >
{
public:
  /** Standard class typedefs. */
  typedef GPUPDEDeformableRegistrationFilter                                       Self;
  typedef GPUDenseFiniteDifferenceImageFilter< TDeformationField, TDeformationField, TParentImageFilter > GPUSuperclass;
  typedef TParentImageFilter                                                       CPUSuperclass;
  typedef SmartPointer< Self >                                                     Pointer;
  typedef SmartPointer< const Self >                                               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(GPUPDEDeformableRegistrationFilter,
               GPUDenseFiniteDifferenceImageFilter);

  /** FixedImage image type. */
  typedef TFixedImage                           FixedImageType;
  typedef typename FixedImageType::Pointer      FixedImagePointer;
  typedef typename FixedImageType::ConstPointer FixedImageConstPointer;

  /** MovingImage image type. */
  typedef TMovingImage                           MovingImageType;
  typedef typename MovingImageType::Pointer      MovingImagePointer;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;

  /** Deformation field type. */
  typedef TDeformationField                      DeformationFieldType;
  typedef typename DeformationFieldType::Pointer DeformationFieldPointer;

  /** Types inherithed from the superclass */
  typedef typename GPUSuperclass::OutputImageType OutputImageType;

  /** FiniteDifferenceFunction type. */
  typedef typename GPUSuperclass::FiniteDifferenceFunctionType
  FiniteDifferenceFunctionType;

  /** GPUPDEDeformableRegistrationFilterFunction type. */
  typedef GPUPDEDeformableRegistrationFunction< FixedImageType, MovingImageType,
                                             DeformationFieldType >  GPUPDEDeformableRegistrationFunctionType;

  /** Inherit some enums and typedefs from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

protected:
  GPUPDEDeformableRegistrationFilter();
  ~GPUPDEDeformableRegistrationFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  bool Halt()
  {
    if ( m_StopRegistrationFlag )
      {
      return true;
      }

    return this->Superclass::Halt();
  }

  /** A simple method to copy the data from the input to the output.
   * If the input does not exist, a zero field is written to the output. */
  virtual void CopyInputToOutput();

  /** Initialize the state of filter and equation before each iteration.
   * Progress feeback is implemented as part of this method. */
  //virtual void InitializeIteration();

  /** Utility to smooth the deformation field (represented in the Output)
   * using a Guassian operator. The amount of smoothing can be specified
   * by setting the StandardDeviations. */
  void SmoothDeformationField();
  virtual void GPUSmoothDeformationField();

  /** Utility to smooth the UpdateBuffer using a Gaussian operator.
   * The amount of smoothing can be specified by setting the
   * UpdateFieldStandardDeviations. */
  //void SmoothUpdateField();

  /** This method is called after the solution has been generated. In this case,
   * the filter release the memory of the internal buffers. */
  //virtual void PostProcessOutput();

  /** This method is called before iterating the solution. */
  virtual void Initialize();

  virtual void AllocateSmoothingBuffer();

private:
  GPUPDEDeformableRegistrationFilter(const Self &); //purposely not implemented
  void operator=(const Self &);                  //purposely not implemented

  int                              m_SmoothingKernelSize;
  float*                           m_SmoothingKernel;
  typename GPUDataManager::Pointer m_GPUSmoothingKernel;

private:

  /* GPU kernel handle for GPUSmoothDeformationField */
  int m_SmoothDeformationFieldGPUKernelHandle;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGPUPDEDeformableRegistrationFilter.txx"
#endif

#endif
