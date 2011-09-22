#ifndef __itkHistogramThresholdingBaseImageFilter_hxx
#define __itkHistogramThresholdingBaseImageFilter_hxx
#include "itkHistogramThresholdingBaseImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
::HistogramThresholdingBaseImageFilter()
{
  m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  m_InsideValue    = NumericTraits<OutputPixelType>::max();
  m_Threshold      = NumericTraits<InputPixelType>::Zero;
  m_HistogramGenerator = HistogramGeneratorType::New();

  // seems we can't initialize this properly until runtime.
  m_NumberOfHistogramBins = 256;
  m_Calculator = 0;
}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  m_HistogramGenerator->SetInput( this->GetInput() );
  // histogramGenerator->SetAutoMinimumMaximum( true );
  m_HistogramGenerator->SetNumberOfThreads( this->GetNumberOfThreads() );
  typename HistogramType::SizeType hsize(this->GetInput()->GetNumberOfComponentsPerPixel());
  hsize.Fill(this->GetNumberOfHistogramBins());
  m_HistogramGenerator->SetHistogramSize(hsize);
  progress->RegisterInternalFilter(m_HistogramGenerator,.4f);


  typedef BinaryThresholdImageFilter<TInputImage,TOutputImage> ThresholderType;
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  thresholder->SetInput(this->GetInput());
  thresholder->SetLowerThreshold( NumericTraits<InputPixelType>::NonpositiveMin() );
  thresholder->SetInsideValue( this->GetInsideValue() );
  thresholder->SetOutsideValue( this->GetOutsideValue() );
  thresholder->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(thresholder,.4f);

  if (!m_Calculator)
    {
    itkExceptionMacro(<< "Threshold calculator type must be set");
    }
  m_Calculator->SetInput( m_HistogramGenerator->GetOutput() );
  m_Calculator->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(m_Calculator,.2f);
  thresholder->SetUpperThresholdInput( m_Calculator->GetOutput() );

  thresholder->GraftOutput( this->GetOutput() );
  thresholder->Update();
  this->GraftOutput( thresholder->GetOutput() );

  m_Threshold = m_Calculator->GetOutput()->Get();
}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdingBaseImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  TInputImage * input = const_cast<TInputImage *>(this->GetInput());
  if( input )
    {
    input->SetRequestedRegionToLargestPossibleRegion();
    }
}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdingBaseImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "OutsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_OutsideValue) << std::endl;
  os << indent << "InsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_InsideValue) << std::endl;
  os << indent << "Threshold (computed): "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_Threshold) << std::endl;

}


}// end namespace itk
#endif
