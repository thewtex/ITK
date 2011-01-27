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
#ifndef __itkDiffusionTensor3DReconstructionImageFilter_txx
#define __itkDiffusionTensor3DReconstructionImageFilter_txx

#include "itkDiffusionTensor3DReconstructionImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkArray.h"
#include "vnl/vnl_vector.h"

namespace itk
{
template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                            TGradientImagePixelType,
                                            TTensorPixelType,
                                            TMaskImageType >
::DiffusionTensor3DReconstructionImageFilter()
{
  // At least 1 inputs is necessary for a vector image.
  // For images added one at a time we need at least six
  this->SetNumberOfRequiredInputs(1);
  m_NumberOfGradientDirections = 0;
  m_NumberOfBaselineImages = 1;
  m_Threshold = NumericTraits< ReferencePixelType >::min();
  m_GradientImageTypeEnumeration = Else;
  m_GradientDirectionContainer = NULL;
  m_TensorBasis.set_identity();
  m_BValue = 1.0;
  m_MaskImagePresent = false;
}

template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::BeforeThreadedGenerateData()
{
  // If we have more than 3 inputs, then each input, except the first is a
  // gradient image. The number of gradient images must match the number of
  // gradient directions.
  const unsigned int numberOfInputs = this->GetNumberOfInputs();

  // There need to be at least 6 gradient directions to be able to compute the
  // tensor basis
  if ( m_NumberOfGradientDirections < 6 )
    {
    itkExceptionMacro(<< "At least 6 gradient directions are required");
    }

  // If there is only 1 gradient image, it must be an itk::VectorImage.
  // Otherwise
  // we must have a container of (numberOfInputs-1) itk::Image. Check to make
  // sure
  if ( (numberOfInputs == 1 ||
        (numberOfInputs == 2 && this->m_MaskImagePresent))
       && m_GradientImageTypeEnumeration != GradientIsInASingleImage )
    {
    std::string gradientImageClassName(
      this->ProcessObject::GetInput(0)->GetNameOfClass() );
    if ( strcmp(gradientImageClassName.c_str(), "VectorImage") != 0 )
      {
      itkExceptionMacro(
        << "There is only one Gradient image. I expect that to be a VectorImage. "
        << "But its of type: " << gradientImageClassName);
      }
    }

  this->ComputeTensorBasis();

  // If there's a mask, make sure it matches the dimensions of the
  // gradient image(s).
  if(this->m_MaskImagePresent)
    {
    typename MaskImageType::Pointer maskImage =
      static_cast<MaskImageType *>(this->ProcessObject::GetInput(1));

    typename MaskImageType::SizeType maskSize =
      maskImage->GetLargestPossibleRegion().GetSize();
    typename MaskImageType::SizeType refSize;

    typename MaskImageType::PointType maskOrigin =
      maskImage->GetOrigin();
    typename MaskImageType::PointType refOrigin;

    typename MaskImageType::SpacingType maskSpacing =
      maskImage->GetSpacing();
    typename MaskImageType::SpacingType refSpacing;

    typename MaskImageType::DirectionType maskDirection =
      maskImage->GetDirection();
    typename MaskImageType::DirectionType refDirection;

    if( m_GradientImageTypeEnumeration == GradientIsInManyImages )
      {
      ReferenceImageType *refImage =
        static_cast<ReferenceImageType *>(this->ProcessObject::GetInput(0));
      refSize =
        refImage->GetLargestPossibleRegion().GetSize();
      refOrigin = refImage->GetOrigin();
      refSpacing = refImage->GetSpacing();
      refDirection = refImage->GetDirection();
      }
    else if( m_GradientImageTypeEnumeration == GradientIsInASingleImage )
      {
      GradientImagesType *gradientImage =
        static_cast< GradientImagesType * >(this->ProcessObject::GetInput(0) );
      refSize =
        gradientImage->GetLargestPossibleRegion().GetSize();
      refOrigin = gradientImage->GetOrigin();
      refSpacing = gradientImage->GetSpacing();
      refDirection = gradientImage->GetDirection();
      }
    // size mismatch is a deal breaker. Iterators are useless.
    if(refSize != maskSize)
      {
      itkExceptionMacro( << "Mask size doesn't match Reference Image Size"
                         << " Mask Size " << maskSize
                         << " Ref Size " << refSize );
      }
    // Origin, Spacing, Direction, should match but it isn't fatal if
    // they don't.
    if(refOrigin != maskOrigin)
      {
      itkWarningMacro( << "Mask origin doesn't match Reference origin "
                       << "Mask Origin " << maskOrigin
                       << " Ref Origin " << refOrigin );
      }
    if(refSpacing != maskSpacing)
      {
      itkWarningMacro( << "Mask spacing doesn't match Reference spacing "
                       << "Mask Spacing " << maskSpacing
                       << " Ref Spacing " << refSpacing );
      }
    if(refDirection != maskDirection)
      {
      itkWarningMacro( << "Mask direction doesn't match Reference direction "
                       << "Mask Direction " << maskDirection
                       << " Ref Direction " << refDirection );
      }
    }
}

// POTENTIAL WARNING:
//
// Until we fix netlib svd routines, we will need to set the number of thread
// to 1.
template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                       int)
{
  typename OutputImageType::Pointer outputImage =
    static_cast< OutputImageType * >( this->ProcessObject::GetOutput(0) );

  ImageRegionIterator< OutputImageType > oit(outputImage, outputRegionForThread);
  oit.GoToBegin();

  vnl_vector< double > B(m_NumberOfGradientDirections);
  vnl_vector< double > D(6);

  // if a mask is present, iterate through mask image and skip zero voxels
  typename MaskImageType::Pointer maskImage;
  if(this->m_MaskImagePresent)
    {
    maskImage =
      static_cast<MaskImageType *>(this->ProcessObject::GetInput(1));
    }
  bool useMask(maskImage.IsNotNull());
  typedef ImageRegionConstIterator<MaskImageType>
    MaskItType;
  MaskItType maskIt;
  if(useMask)
    {
    MaskItType tmp(maskImage,outputRegionForThread);
    maskIt = tmp;
    maskIt.GoToBegin();
    }

  // Two cases here .
  // 1. If the Gradients have been specified in multiple images, we will create
  // 'n' iterators for each of the gradient images and solve the Stejskal-Tanner
  // equations for every pixel.
  // 2. If the Gradients have been specified in a single multi-component image,
  // one iterator will suffice to do the same.

  if ( m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    ImageRegionConstIterator< ReferenceImageType >
    it(static_cast< ReferenceImageType * >( this->ProcessObject::GetInput(0) ),
       outputRegionForThread);
    it.GoToBegin();

    typedef ImageRegionConstIterator< GradientImageType > GradientIteratorType;
    std::vector< GradientIteratorType * > gradientItContainer;

    for ( unsigned int i = 1; i <= m_NumberOfGradientDirections; i++ )
      {
      typename GradientImageType::Pointer gradientImagePointer = NULL;

      // Would have liked a dynamic_cast here, but seems SGI doesn't like it
      // The enum will ensure that an inappropriate cast is not done
      gradientImagePointer = static_cast< GradientImageType * >(
        this->ProcessObject::GetInput(i+1) );

      GradientIteratorType *git = new GradientIteratorType(
        gradientImagePointer, outputRegionForThread);
      git->GoToBegin();
      gradientItContainer.push_back(git);
      }

    // Iterate over the reference and gradient images and solve the steskal
    // equations to reconstruct the Diffusion tensor.
    // See splweb.bwh.harvard.edu:8000/pages/papers/westin/ISMRM2002.pdf
    // "A Dual Tensor Basis Solution to the Stejskal-Tanner Equations for
    // DT-MRI"

    while ( !it.IsAtEnd() )
      {

      ReferencePixelType b0 = it.Get();

      TensorPixelType tensor(0.0);

      //
      // if a mask is present, and we don't have a zero pixel
      // look up the voxel in the mask image corresponding to
      // the location of the current index.
      bool unmaskedPixel(true);
      if(useMask)
        {
        if(maskIt.Value() ==
           NumericTraits<typename MaskImageType::PixelType>::Zero)
          {
          unmaskedPixel = false;
          }
        ++maskIt;
        }

      if ( ( b0 != 0 ) && unmaskedPixel && ( b0 >= m_Threshold ) )
        {
        for ( unsigned int i = 0; i < m_NumberOfGradientDirections; i++ )
          {
          GradientPixelType b = gradientItContainer[i]->Get();

          if ( b == 0 )
            {
            B[i] = 0;
            }
          else
            {
            B[i] = -vcl_log( static_cast< double >( b ) / static_cast< double >( b0 ) ) / this->m_BValue;
            }

          ++( *gradientItContainer[i] );
          }

        vnl_svd< double > pseudoInverseSolver(m_TensorBasis);
        if ( m_NumberOfGradientDirections > 6 )
          {
          D = pseudoInverseSolver.solve(m_BMatrix * B);
          }
        else
          {
          D = pseudoInverseSolver.solve(B);
          }

        tensor(0, 0) = D[0];
        tensor(0, 1) = D[1];
        tensor(0, 2) = D[2];
        tensor(1, 1) = D[3];
        tensor(1, 2) = D[4];
        tensor(2, 2) = D[5];
        }
      else
        {
        for ( unsigned int i = 0; i < m_NumberOfGradientDirections; i++ )
          {
          ++( *gradientItContainer[i] );
          }
        }

      oit.Set(tensor);
      ++oit;
      ++it;
      }

    for ( unsigned int i = 0; i < gradientItContainer.size(); i++ )
      {
      delete gradientItContainer[i];
      }
    }
  // The gradients are specified in a single multi-component image
  else if ( m_GradientImageTypeEnumeration == GradientIsInASingleImage )
    {
    typedef ImageRegionConstIterator< GradientImagesType > GradientIteratorType;
    typedef typename GradientImagesType::PixelType         GradientVectorType;
    typename GradientImagesType::Pointer gradientImagePointer = NULL;

    // Would have liked a dynamic_cast here, but seems SGI doesn't like it
    // The enum will ensure that an inappropriate cast is not done
    gradientImagePointer = static_cast< GradientImagesType * >(
      this->ProcessObject::GetInput(0) );

    GradientIteratorType git(gradientImagePointer, outputRegionForThread);
    git.GoToBegin();

    // Compute the indicies of the baseline images and gradient images
    std::vector< unsigned int > baselineind; // contains the indicies of
                                             // the baseline images
    std::vector< unsigned int > gradientind; // contains the indicies of
                                             // the gradient images

    for ( GradientDirectionContainerType::ConstIterator gdcit =
            this->m_GradientDirectionContainer->Begin();
          gdcit != this->m_GradientDirectionContainer->End(); ++gdcit )
      {
      if ( gdcit.Value().one_norm() <= 0.0 )
        {
        baselineind.push_back( gdcit.Index() );
        }
      else
        {
        gradientind.push_back( gdcit.Index() );
        }
      }

    while ( !git.IsAtEnd() )
      {
      GradientVectorType b = git.Get();

      typename NumericTraits< ReferencePixelType >::AccumulateType b0 =
        NumericTraits< ReferencePixelType >::Zero;

      // Average the baseline image pixels
      for ( unsigned int i = 0; i < baselineind.size(); ++i )
        {
        b0 += b[baselineind[i]];
        }
      b0 /= this->m_NumberOfBaselineImages;

      TensorPixelType tensor(0.0);

      //
      // if a mask is present, and we don't have a zero pixel
      // look up the voxel in the mask image corresponding to
      // the location of the current index.
      bool unmaskedPixel(true);
      if(useMask)
        {
        if(maskIt.Value() ==
           NumericTraits<typename MaskImageType::PixelType>::Zero)
          {
          unmaskedPixel = false;
          }
        ++maskIt;
        }

      if ( ( b0 != 0 ) && unmaskedPixel && ( b0 >= m_Threshold ) )
        {
        for ( unsigned int i = 0; i < m_NumberOfGradientDirections; i++ )
          {
          if ( b[gradientind[i]] == 0 )
            {
            B[i] = 0;
            }
          else
            {
            B[i] =
              -vcl_log( static_cast< double >( b[gradientind[i]] ) / static_cast< double >( b0 ) ) / this->m_BValue;
            }
          }

        vnl_svd< double > pseudoInverseSolver(m_TensorBasis);
        if ( m_NumberOfGradientDirections > 6 )
          {
          D = pseudoInverseSolver.solve(m_BMatrix * B);
          }
        else
          {
          D = pseudoInverseSolver.solve(B);
          }

        tensor(0, 0) = D[0];
        tensor(0, 1) = D[1];
        tensor(0, 2) = D[2];
        tensor(1, 1) = D[3];
        tensor(1, 2) = D[4];
        tensor(2, 2) = D[5];
        }

      oit.Set(tensor);
      ++oit; // Output (reconstructed tensor image) iterator
      ++git; // Gradient  image iterator
      }
    }
}

template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::ComputeTensorBasis()
{
  if ( m_NumberOfGradientDirections < 6 )
    {
    itkExceptionMacro(<< "Not enough gradient directions supplied. Need to supply at least 6");
    }

  // This is only important if we are using a vector image.  For
  // images added one at a time, this is not needed but doesn't hurt.
  std::vector< unsigned int > gradientind;
  for ( GradientDirectionContainerType::ConstIterator gdcit = this->m_GradientDirectionContainer->Begin();
        gdcit != this->m_GradientDirectionContainer->End(); ++gdcit )
    {
    if ( gdcit.Value().one_norm() > 0.0 )
      {
      gradientind.push_back( gdcit.Index() );
      }
    }

  m_BMatrix.set_size(m_NumberOfGradientDirections, 6);
  for ( unsigned int m = 0; m < m_NumberOfGradientDirections; m++ )
    {
    m_BMatrix[m][0] =     m_GradientDirectionContainer->ElementAt(gradientind[m])[0]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[0];
    m_BMatrix[m][1] = 2 * m_GradientDirectionContainer->ElementAt(gradientind[m])[0]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[1];
    m_BMatrix[m][2] = 2 * m_GradientDirectionContainer->ElementAt(gradientind[m])[0]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[2];
    m_BMatrix[m][3] =     m_GradientDirectionContainer->ElementAt(gradientind[m])[1]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[1];
    m_BMatrix[m][4] = 2 * m_GradientDirectionContainer->ElementAt(gradientind[m])[1]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[2];
    m_BMatrix[m][5] =     m_GradientDirectionContainer->ElementAt(gradientind[m])[2]
                      * m_GradientDirectionContainer->ElementAt(gradientind[m])[2];
    }

  if ( m_NumberOfGradientDirections > 6 )
    {
    m_TensorBasis = m_BMatrix.transpose() * m_BMatrix;
    }
  else
    {
    m_TensorBasis = m_BMatrix;
    }

  m_BMatrix.inplace_transpose();
}

template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::AddGradientImage(const GradientDirectionType & gradientDirection,
                   const GradientImageType *gradientImage)
{
  // Make sure crazy users did not call both AddGradientImage and
  // SetGradientImage
  if ( m_GradientImageTypeEnumeration == GradientIsInASingleImage )
    {
    itkExceptionMacro(<< "Cannot call both methods:"
                      << "AddGradientImage and SetGradientImage. Please call only one of them.");
    }

  // If the container to hold the gradient directions hasn't been allocated
  // yet, allocate it.
  if ( !this->m_GradientDirectionContainer )
    {
    this->m_GradientDirectionContainer = GradientDirectionContainerType::New();
    }

  m_GradientDirectionContainer->InsertElement(
    m_NumberOfGradientDirections, gradientDirection / gradientDirection.two_norm() );
  ++m_NumberOfGradientDirections;
  this->ProcessObject::SetNthInput( m_NumberOfGradientDirections+1,
                                    const_cast< GradientImageType * >( gradientImage ) );
  m_GradientImageTypeEnumeration = GradientIsInManyImages;
}

template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::SetGradientImage(GradientDirectionContainerType *gradientDirection,
                   const GradientImagesType *gradientImage)
{
  // Make sure crazy users did not call both AddGradientImage and
  // SetGradientImage
  if ( m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    itkExceptionMacro(<< "Cannot call both methods:"
                      << "AddGradientImage and SetGradientImage. Please call only one of them.");
    }

  this->m_GradientDirectionContainer = gradientDirection;

  unsigned int numImages = gradientDirection->Size();
  this->m_NumberOfBaselineImages = 0;
  for ( GradientDirectionContainerType::Iterator it = this->m_GradientDirectionContainer->Begin();
        it != this->m_GradientDirectionContainer->End(); it++ )
    {
    if ( it.Value().one_norm() <= 0.0 )
      {
      this->m_NumberOfBaselineImages++;
      }
    else // Normalize non-zero gradient directions
      {
      it.Value() = it.Value() / it.Value().two_norm();
      }
    }

  this->m_NumberOfGradientDirections = numImages - this->m_NumberOfBaselineImages;

  // ensure that the gradient image we received has as many components as
  // the number of gradient directions
  if ( gradientImage->GetVectorLength() != this->m_NumberOfBaselineImages + this->m_NumberOfGradientDirections )
    {
    itkExceptionMacro(<< this->m_NumberOfGradientDirections << " gradients + " << this->m_NumberOfBaselineImages
                      << "baselines = " << this->m_NumberOfGradientDirections + this->m_NumberOfBaselineImages
                      << " directions specified but image has " << gradientImage->GetVectorLength()
                      << " components.");
    }

  this->ProcessObject::SetNthInput( 0,
                                    const_cast< GradientImagesType * >( gradientImage ) );
  m_GradientImageTypeEnumeration = GradientIsInASingleImage;
}

template< class TReferenceImagePixelType,
          class TGradientImagePixelType, class TTensorPixelType,
          class TMaskImageType >
void DiffusionTensor3DReconstructionImageFilter< TReferenceImagePixelType,
                                                 TGradientImagePixelType,
                                                 TTensorPixelType,
                                                 TMaskImageType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "TensorBasisMatrix: " << m_TensorBasis << std::endl;
  os << indent << "Coeffs: " << m_BMatrix << std::endl;
  if ( m_GradientDirectionContainer )
    {
    os << indent << "GradientDirectionContainer: "
       << m_GradientDirectionContainer << std::endl;
    }
  else
    {
    os << indent
       << "GradientDirectionContainer: (Gradient directions not set)" << std::endl;
    }
  os << indent << "NumberOfGradientDirections: "
     << m_NumberOfGradientDirections << std::endl;
  os << indent << "NumberOfBaselineImages: "
     << m_NumberOfBaselineImages << std::endl;
  os << indent << "Threshold for reference B0 image: " << m_Threshold << std::endl;
  os << indent << "BValue: " << m_BValue << std::endl;
  if ( this->m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    os << indent << "Gradient images haven been supplied " << std::endl;
    }
  else if ( this->m_GradientImageTypeEnumeration == GradientIsInManyImages )
    {
    os << indent << "A multicomponent gradient image has been supplied" << std::endl;
    }
}
}

#endif
