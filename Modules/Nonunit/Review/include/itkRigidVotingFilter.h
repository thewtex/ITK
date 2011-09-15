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
#ifndef __itkRigidVotingFilter_h
#define __itkRigidVotingFilter_h

#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include <vector>
const unsigned dimension = 2;
namespace itk
{
template< class TInputVectorImage, class TImage>
class RigidVotingFilter:
  public ImageToImageFilter< TInputVectorImage, TImage>
{
public:
/** Standard class typedefs. */
  typedef RigidVotingFilter                              Self;
  typedef ImageToImageFilter< TInputVectorImage, TImage> Superclass;
  typedef SmartPointer< Self >                           Pointer;

  typedef typename Superclass::InputImageType        InputImageType;
  typedef typename InputImageType::ConstPointer      InputImagePointer;
  typedef typename InputImageType::RegionType        ImageRegionType;
  typedef typename InputImageType::IndexType         InputIndex;
  typedef ImageRegionConstIterator< InputImageType > InputImageIteratorType;
  typedef typename InputImageType::PixelType         InputPixelType;
/** Method for creation through the object factory. */
  itkNewMacro(Self);

/** Run-time type information (and related methods). */
  itkTypeMacro(RigidVotingFilter, ImageToImageFilter);

protected:
  RigidVotingFilter(){}
  ~RigidVotingFilter(){}

/** Does the real work. */
  virtual void GenerateData();

private:
  RigidVotingFilter(const Self &); //purposely not implemented
  void operator=(const Self &);  //purposely not implemented

  std::vector<float> m_Theta_i;
  std::vector<float> m_Tx_i;
  std::vector<float> m_Ty_i;
  float              m_Voting_Interval_Angle;
  float              m_Voting_Interval_Distance;
};
} //namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRigidVotingFilter.hxx"
#endif


#endif // __itkImageFilter_h
