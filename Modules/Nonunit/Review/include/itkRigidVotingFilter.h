#ifndef __itkRigidVotingFilter_H
#define __itkRigidVotingFilter_H

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
  typedef RigidVotingFilter Self;
  typedef ImageToImageFilter< TInputVectorImage, TImage> Superclass;

  typedef SmartPointer< Self >        Pointer;

  typedef typename Superclass::InputImageType        InputImageType;
  typedef typename InputImageType::ConstPointer      InputImagePointer;
  typedef typename InputImageType::RegionType        ImageRegionType;
  typedef typename InputImageType::IndexType	     InputIndex;
  typedef ImageRegionConstIterator< InputImageType > InputImageIteratorType;
  typedef typename InputImageType::PixelType              InputPixelType;
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

  std::vector<float> Theta_i;
  std::vector<float> Tx_i;
  std::vector<float> Ty_i;
  float m_Voting_Interval_Angle;
  float m_Voting_Interval_Distance;
};
} //namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRigidVotingFilter.hxx"
#endif


#endif // __itkImageFilter_h
