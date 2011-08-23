#ifndef __itkHistogramThresholdImageFilter_hxx
#define __itkHistogramThresholdImageFilter_hxx
#include "itkHistogramThresholdImageFilter.h"

#include "itkImageToHistogramFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkKittlerIllingworthThresholdCalculator.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
HistogramThresholdImageFilter<TInputImage, TOutputImage>
::HistogramThresholdImageFilter()
{
  m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  m_InsideValue    = NumericTraits<OutputPixelType>::max();
  m_Threshold      = NumericTraits<InputPixelType>::Zero;
  m_Calculator     = NULL;
}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  if( m_Calculator.IsNull() )
    {
    itkExceptionMacro(<<"No threshold calculator set.");
    }
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  typedef itk::Statistics::ImageToHistogramFilter<InputImageType> HistogramGeneratorType;
  typename HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
  histogramGenerator->SetInput( this->GetInput() );
  // histogramGenerator->SetAutoMinimumMaximum( true );
  histogramGenerator->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(histogramGenerator,.4f);

  m_Calculator->SetInput( histogramGenerator->GetOutput() );
  m_Calculator->SetNumberOfThreads( this->GetNumberOfThreads() );
  this->ConfigureCalculator();
  progress->RegisterInternalFilter(m_Calculator,.2f);

  typedef BinaryThresholdImageFilter<TInputImage,TOutputImage> ThresholderType;
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  thresholder->SetInput(this->GetInput());
  thresholder->SetLowerThreshold( NumericTraits<InputPixelType>::NonpositiveMin() );
  thresholder->SetUpperThresholdInput( m_Calculator->GetOutput() );
  thresholder->SetInsideValue( this->GetInsideValue() );
  thresholder->SetOutsideValue( this->GetOutsideValue() );
  thresholder->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(thresholder,.4f);

  thresholder->GraftOutput( this->GetOutput() );
  thresholder->Update();
  this->GraftOutput( thresholder->GetOutput() );
  m_Threshold = m_Calculator->GetThreshold();
  m_Calculator->SetInput( NULL );
}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdImageFilter<TInputImage, TOutputImage>
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
HistogramThresholdImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "OutsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_OutsideValue) << std::endl;
  os << indent << "InsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_InsideValue) << std::endl;
  os << indent << "Calculator: ";
  m_Calculator->Print( os, indent.GetNextIndent() );
  os << indent << "Threshold (computed): "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_Threshold) << std::endl;
}


}// end namespace itk
#endif
