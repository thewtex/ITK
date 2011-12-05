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
#ifndef __itkTriangleThresholdCalculator_hxx
#define __itkTriangleThresholdCalculator_hxx

#include "itkTriangleThresholdCalculator.h"
#include "itkProgressReporter.h"
#include "vnl/vnl_math.h"

#include <algorithm>

namespace itk
{
/*
 * Compute the Triangle's threshold
 */
template<class THistogram, class TOutput>
void
TriangleThresholdCalculator<THistogram, TOutput>
::GenerateData(void)
{
  const HistogramType * histogram = this->GetInput();
  // histogram->Print(std::cout);
  if ( histogram->GetTotalFrequency() == 0 )
    {
    itkExceptionMacro(<< "Histogram is empty");
    }
  ProgressReporter progress(this, 0, histogram->GetSize(0) );
  if( histogram->GetSize(0) == 1 )
    {
    this->GetOutput()->Set( histogram->GetMeasurement(0,0) );
    }

  unsigned int size = histogram->GetSize(0);

  // create a histogram
  std::vector<double> cumSum(size, 0.0);
  std::vector<double> triangle(size, 0.0);

  // Triangle method needs the maximum and minimum indexes
  // Minimum indexes for this purpose are poorly defined - can't just
  // take a index with zero entries.
  double Mx = itk::NumericTraits<double>::min();
  unsigned long MxIdx=0;

  for ( unsigned int j = 0; j < size; j++ )
    {
    //std::cout << histogram->GetFrequency(j, 0) << std::endl;
    if (histogram->GetFrequency(j, 0) > Mx)
      {
      MxIdx=j;
      Mx=histogram->GetFrequency(j, 0);
      }
    }


  cumSum[0] = histogram->GetFrequency(0, 0);
  for ( unsigned int j = 1; j < size; j++ )
    {
    cumSum[j] = histogram->GetFrequency(j, 0) + cumSum[j-1];
    }

  typename HistogramType::MeasurementVectorType onePC(1), nnPC(1);
  onePC.Fill(histogram->Quantile(0, 0.01));
  itk::IndexValueType onePCIdx = histogram->GetIndex(onePC)[0];
  nnPC.Fill(histogram->Quantile(0, 0.99));
  itk::IndexValueType nnPCIdx = histogram->GetIndex(nnPC)[0];

  // figure out which way we are looking - we want to construct our
  // line between the max index and the further of 1% and 99%
  unsigned ThreshIdx=0;
  if (fabs((float)MxIdx - (float)onePCIdx) > fabs((float)MxIdx - (float)nnPCIdx))
    {
    // line to 1 %
    double slope = Mx/(MxIdx - onePCIdx);
    for (unsigned k=onePCIdx; k<MxIdx; k++)
      {
      float line = (slope*(k-onePCIdx));
      triangle[k]= line - histogram->GetFrequency(k);
      // std::cout << relativeFrequency[k] << "," << line << "," << triangle[k] << std::endl;
      }

    ThreshIdx = onePCIdx + std::distance(&(triangle[onePCIdx]), std::max_element(&(triangle[onePCIdx]), &(triangle[MxIdx])));
    }
  else
    {
    // line to 99 %
    double slope = -Mx/(nnPCIdx - MxIdx);
    for (unsigned k=MxIdx; k < nnPCIdx; k++)
      {
      float line = (slope*(k-MxIdx) + Mx);
      triangle[k]= line - histogram->GetFrequency(k);
//      std::cout << relativeFrequency[k] << "," << line << "," << triangle[k] << std::endl;
      }
    ThreshIdx = MxIdx + std::distance(&(triangle[MxIdx]), std::max_element(&(triangle[MxIdx]), &(triangle[nnPCIdx])));
    }

  this->GetOutput()->Set( static_cast<OutputType>( histogram->GetMeasurement( ThreshIdx + 1, 0 ) ) );

  // for (unsigned k = 0; k < size ; k++)
  //   {
  //   std::cout << relativeFrequency[k] << std::endl;
  //   }

}

} // end namespace itk

#endif
