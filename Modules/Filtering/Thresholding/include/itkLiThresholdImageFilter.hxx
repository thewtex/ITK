#ifndef __itkLiThresholdImageFilter_hxx
#define __itkLiThresholdImageFilter_hxx
#include "itkLiThresholdImageFilter.h"

#include "itkImageToHistogramFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkLiThresholdCalculator.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
LiThresholdImageFilter<TInputImage, TOutputImage>
::LiThresholdImageFilter()
{
  m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  m_InsideValue    = NumericTraits<OutputPixelType>::max();
  m_Threshold      = NumericTraits<InputPixelType>::Zero;
}

template<class TInputImage, class TOutputImage>
void
LiThresholdImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  typedef itk::Statistics::ImageToHistogramFilter<InputImageType> HistogramGeneratorType;
  typedef typename HistogramGeneratorType::HistogramType          HistogramType;
  typename HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
  histogramGenerator->SetInput( this->GetInput() );
  // histogramGenerator->SetAutoMinimumMaximum( true );
  histogramGenerator->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(histogramGenerator,.4f);

  // Compute the Li Threshold for the input image
  typedef LiThresholdCalculator<HistogramType, InputPixelType> CalculatorType;
  typename CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetInput( histogramGenerator->GetOutput() );
  calculator->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(calculator,.2f);

  typedef BinaryThresholdImageFilter<TInputImage,TOutputImage> ThresholderType;
  typename ThresholderType::Pointer thresholder = ThresholderType::New();
  thresholder->SetInput(this->GetInput());
  thresholder->SetLowerThreshold( NumericTraits<InputPixelType>::NonpositiveMin() );
  thresholder->SetUpperThresholdInput( calculator->GetOutput() );
  thresholder->SetInsideValue( this->GetInsideValue() );
  thresholder->SetOutsideValue( this->GetOutsideValue() );
  thresholder->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(thresholder,.4f);

  thresholder->GraftOutput( this->GetOutput() );
  thresholder->Update();
  this->GraftOutput( thresholder->GetOutput() );
  m_Threshold = calculator->GetOutput()->Get();
}

template<class TInputImage, class TOutputImage>
void
LiThresholdImageFilter<TInputImage, TOutputImage>
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
LiThresholdImageFilter<TInputImage,TOutputImage>
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
