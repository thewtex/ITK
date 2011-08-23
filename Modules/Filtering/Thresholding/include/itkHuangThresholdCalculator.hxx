
#ifndef __itkHuangThresholdCalculator_hxx
#define __itkHuangThresholdCalculator_hxx

#include "itkHuangThresholdCalculator.h"
#include "itkProgressReporter.h"
#include "vnl/vnl_math.h"

namespace itk
{

/*
 * Compute the Huang's threshold
 */
template<class THistogram, class TOutput>
void
HuangThresholdCalculator<THistogram, TOutput>
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
  // find first and last non-empty bin - could replace with stl
  unsigned int first = 0;
  while( first < size && histogram->GetFrequency(first, 0) == 0 )
    {
    first++;
    }
  if (first == size)
    {
    itkWarningMacro(<< "No data in histogram");
    return;
    }
  unsigned int last = size - 1;
  while( last > first && histogram->GetFrequency(last, 0) == 0)
    {
    last--;
    }

  // calculate the cumulative density and the weighted cumulative density
  std::vector<double> S(last+1);
  std::vector<double> W(last+1);

  S[0] = histogram->GetFrequency(0, 0);

  for( unsigned int i = std::max((unsigned int)1, first); i <= last; i++ )
    {
    S[i] = S[i - 1] + histogram->GetFrequency(i, 0);
    W[i] = W[i - 1] + histogram->GetMeasurement(i, 0) * histogram->GetFrequency(i, 0);
    }

  // precalculate the summands of the entropy given the absolute difference x - mu (integral)
  double C = last - first;
  std::vector<double> Smu(last + 1 - first);

  for( unsigned int i = 1; i < Smu.size(); i++)
    {
    double mu = 1 / (1 + i / C);
    Smu[i] = -mu * vcl_log(mu) - (1 - mu) * vcl_log(1 - mu);
    }

  // calculate the threshold
  int bestThreshold = 0;
  double bestEntropy = itk::NumericTraits<double>::max();
  for( unsigned int threshold = first; threshold <= last; threshold++ )
    {
    double entropy = 0;
    int mu = (int)round(W[threshold] / S[threshold]);
    for (int i = first; i <= threshold; i++)
      entropy += Smu[vcl_abs(i - mu)] * histogram->GetFrequency(i, 0);
    mu = (int)round((W[last] - W[threshold]) / (S[last] - S[threshold]));
    for (int i = threshold + 1; i <= last; i++)
      entropy += Smu[vcl_abs(i - mu)] * histogram->GetFrequency(i, 0);

    if (bestEntropy > entropy)
      {
      bestEntropy = entropy;
      bestThreshold = threshold;
      }
    }

  this->GetOutput()->Set( static_cast<OutputType>( histogram->GetMeasurement( bestThreshold, 0 ) ) );
}

} // end namespace itk

#endif
