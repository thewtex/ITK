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
#ifndef __itkRigidVotingFilter_hxx
#define __itkRigidVotingFilter_hxx

#include "itkRigidVotingFilter.h"
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

template< class TInputVectorImage, class TImage>
void RigidVotingFilter< TInputVectorImage, TImage>
::GenerateData()
{
  const InputImageType * membershipImage = this->GetInput();
  unsigned int NumberOfPairs = membershipImage->GetLargestPossibleRegion().GetSize()[0];
  int counter=0;
  InputIndex PairIndex;
  InputPixelType PairValue;
  InputPixelType PairValue1;
  for (unsigned int i=0;i<NumberOfPairs;i++)
    {
    counter=0;
    PairIndex[0]=i;
    PairValue=membershipImage->GetPixel(PairIndex);
    for (unsigned int j=i;(j<NumberOfPairs)&&(i!=j);j++)
      {
      PairIndex[0]=j;
      PairValue1=membershipImage->GetPixel(PairIndex);
      if(abs(PairValue[2]-PairValue1[2])<=m_Voting_Interval_Angle/2)
        {
        counter++;
        }
      }
    }
  unsigned int MaxTheta = 0;
  unsigned int flag_Theta = 0;
  for (unsigned int i=0;i<NumberOfPairs;i++)
    {
    if (m_Theta_i[i]<=MaxTheta)
      {
      MaxTheta=m_Theta_i[i];
      flag_Theta=i;
      }
    }
  int Alpha=flag_Theta;
  PairIndex[0]=Alpha;
  PairValue=membershipImage->GetPixel(PairIndex);
  double Theta=PairValue[2];
  unsigned int counter1 = 0;
  unsigned int counter2 = 0;
  for (unsigned int i=0;i<NumberOfPairs;i++)
    {
    counter1 = 0;
    counter2 = 0;
    PairIndex[0]=i;
    PairValue=membershipImage->GetPixel(PairIndex);
    for (unsigned int j=i;(j<NumberOfPairs)&&(i!=j);j++)
      {
      PairIndex[0]=j;
      PairValue1=membershipImage->GetPixel(PairIndex);
      if((abs(PairValue[5]-PairValue1[5])<=m_Voting_Interval_Distance/2)&& (abs(PairValue[6]-PairValue1[6])<=m_Voting_Interval_Distance/2))
        {
        counter1++;
        }
      }
    m_Tx_i[i]=counter1;
    m_Ty_i[i]=counter2;

    }
  int MaxTx = 0;
  int MaxTy = 0;
  int flag_Tx = 0;
  int flag_Ty = 0;
  for (unsigned int i=0;i<NumberOfPairs;i++)
    {
    if (m_Tx_i[i]<=MaxTx)
      {
      MaxTx=m_Tx_i[i];
      flag_Tx=i;
      }
    if (m_Ty_i[i]<=MaxTy)
      {
      MaxTy=m_Ty_i[i];
      flag_Ty=i;
      }
    }
  int Beta=flag_Tx;
  int Gamma=flag_Ty;
  PairIndex[0]=Beta;
  PairValue=membershipImage->GetPixel(PairIndex);
  double Transform_Tx=PairValue[7];
  PairIndex[0]=Gamma;
  PairValue=membershipImage->GetPixel(PairIndex);
  double Transform_Ty=PairValue[8];
  std::cout<<"Theta"<<Theta<<"Tx"<<Transform_Tx<<"Ty"<<Transform_Ty<<std::endl;

}

}// end namespace


#endif
