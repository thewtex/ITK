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
#ifndef __itkParameterScaleEstimator_txx
#define __itkParameterScaleEstimator_txx

#include "itkParameterScaleEstimator.h"

namespace itk
{

template< class TFixedImage, class TMovingImage, class TTransform >
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::ParameterScaleEstimator()
{
  SetLNorm(2);               //by default, use Euclidean distance
  SetGlobalScalingFactor(200.0);
  SetTransformForward(true); //true means to transform from fixed domain to moving domain
}

/** Compute parameter scales */
template< class TFixedImage, class TMovingImage, class TTransform >
void
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::ComputeParameterScales(typename TransformType::Pointer transform, ScalesType &parameterScales)
{
  unsigned int numPara = transform->GetParameters().size();
  double maxShift;

  if (parameterScales.size() != numPara)
  {
    std::cerr << "Error in ParameterScaleEstimator: the dimension of parameterScales doesn't match the transform.\n";
    return;
  }
  // Save the original parameters
  ParametersType oldParameters = transform->GetParameters();

  SampleImageDomain();

  ParametersType deltaParameters(numPara);
  for (unsigned int j=0; j<numPara; j++)
    deltaParameters[j] = 0;

  for (unsigned int i=0; i<numPara; i++) //variation for each transform parameter
  {
    deltaParameters[i] = 1;
    this->ComputeMaximumShift(transform, deltaParameters, maxShift);

    parameterScales[i] = maxShift;
    deltaParameters[i] = 0;
  }

  for (unsigned int i=0; i<numPara; i++)
  {
    deltaParameters[i] = 1.0 / parameterScales[i];
  }
  this->ComputeMaximumShift(transform, deltaParameters, maxShift);

  for (unsigned int i=0; i<numPara; i++)
    parameterScales[i] = parameterScales[i] * maxShift / this->m_GlobalScalingFactor;

  // Restore the original parameters
  transform->SetParameters(oldParameters);
}

/** Set the sample points for computing pixel shifts */
template< class TFixedImage, class TMovingImage, class TTransform >
void
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::SampleImageDomain()
{
  if (this->m_TransformForward)
    GetImageCornerPoints<TFixedImage>(this->m_FixedImage.GetPointer(), this->m_FixedImageSamples);
  else
    GetImageCornerPoints<TMovingImage>(this->m_MovingImage.GetPointer(), this->m_MovingImageSamples);
}

/**
 * Get the physical coordinates of image corners
 */
template< class TFixedImage, class TMovingImage, class TTransform >
template<class ImageType>
void
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::GetImageCornerPoints(const ImageType *image, std::vector<PointType> &samples)
{
  samples.clear();

  ImageRegionType region = image->GetLargestPossibleRegion();
  IndexType firstCorner = region.GetIndex();
  SizeType size = region.GetSize();
  int cornerNumber = 1 << ImageDimension; //2^ImageDimension

  for(int i=0; i<cornerNumber; i++)
  {
    IndexType corner;
    int bit;
    for (int d=0; d<ImageDimension; d++)
    {
      bit = (int) (( i & (1 << d) ) != 0); //0 or 1
      corner[d] = firstCorner[d] + bit * (size[d] - 1);
    }
    PointType point;
    image->TransformIndexToPhysicalPoint(corner, point);
    samples.push_back(point);
  }

}

/**
 * Compute the maximum shift when one transform is changed to another
 */
template < class TFixedImage, class TMovingImage, class TTransform >
void
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::ComputeMaximumShift(typename TransformType::Pointer transform, ParametersType deltaParameters, double &maxShift)
{
  // Save the original parameters
  ParametersType oldParameters = transform->GetParameters();

  ParametersType newParameters(oldParameters.size());
  for (unsigned int p=0; p<oldParameters.size(); p++)
    newParameters[p] = oldParameters[p] + deltaParameters[p];

  maxShift = 0;
  double distance;

  unsigned int numSamples = 0;
  if (this->m_TransformForward) numSamples = m_FixedImageSamples.size();
  else numSamples = m_MovingImageSamples.size();

  //find max shift by checking each sample point
  for (unsigned int c=0; c<numSamples; c++)
  {
    PointType point, oldMappedPoint, newMappedPoint;
    ContinuousIndex<double, ImageDimension> oldMappedIndex, newMappedIndex, diffIndex;

    if (this->m_TransformForward) point = this->m_FixedImageSamples[c];
    else point = this->m_MovingImageSamples[c];

    transform->SetParameters(oldParameters);
    oldMappedPoint = transform->TransformPoint(point);

    transform->SetParameters(newParameters);
    newMappedPoint = transform->TransformPoint(point);

    if (this->m_TransformForward) {
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex(oldMappedPoint, oldMappedIndex);
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex(newMappedPoint, newMappedIndex);
    } else {
      this->m_FixedImage->TransformPhysicalPointToContinuousIndex(oldMappedPoint, oldMappedIndex);
      this->m_FixedImage->TransformPhysicalPointToContinuousIndex(newMappedPoint, newMappedIndex);
    }
    for (unsigned int d=0; d<ImageDimension; d++)
      diffIndex[d] = oldMappedIndex[d] - newMappedIndex[d];

    distance = ComputeLNorm(diffIndex);
    if ( maxShift < distance ) maxShift = distance;
  }

  // Restore the original parameters
  transform->SetParameters(oldParameters);
}

/**
 * Compute the L-norm of a point
 */
template< class TFixedImage, class TMovingImage, class TTransform >
double
ParameterScaleEstimator<TFixedImage, TMovingImage, TTransform>
::ComputeLNorm(Point<double, ImageDimension> point)
{
  double distance = 0;

  if (m_LNorm == 2) //Euclidean distance
  {
    for (unsigned int d=0; d<ImageDimension; d++)
      distance += point[d] * point[d];
    distance = sqrt(distance);
  }
  else if (m_LNorm == 1)
  {
    for (unsigned int d=0; d<ImageDimension; d++)
      distance += abs(point[d]);
  }
  else if (m_LNorm == -1) //L-infinity norm
  {
    for (unsigned int d=0; d<ImageDimension; d++)
      if (distance < abs(point[d]))
        distance = abs(point[d]);
  }

  return distance;
}

template < class TFixedImage, class TMovingImage, class TTransform >
void
ParameterScaleEstimator< TFixedImage, TMovingImage, TTransform >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "TransformType   = " << std::endl;
  os << indent << typeid(TransformType).name()  << std::endl;

  os << indent << "m_LNorm   = " << this->m_LNorm << std::endl;
  os << indent << "m_GlobalScalingFactor   = " << this->m_GlobalScalingFactor << std::endl;
  os << indent << "TransformForward   = " << this->m_TransformForward << std::endl;

  os << indent << "FixedImage   = " << std::endl;
  if( this->m_FixedImage )
    {
    os << indent << this->m_FixedImage  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

  os << indent << "MovingImage   = " << std::endl;
  if( this->m_MovingImage )
    {
    os << indent << this->m_MovingImage  << std::endl;
    }
  else
    {
    os << indent << "None" << std::endl;
    }

  os << indent << "FixedImageSamples Size   = " << std::endl;
  os << indent << this->m_FixedImageSamples.size()  << std::endl;

  os << indent << "MovingImageSamples Size  = " << std::endl;
  os << indent << this->m_MovingImageSamples.size()  << std::endl;

}

}  // namespace itk

#endif /* __itkParameterScaleEstimator_txx */
