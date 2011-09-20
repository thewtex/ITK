#ifndef __itkHistogramThresholdingImageFilter_hxx
#define __itkHistogramThresholdingImageFilter_hxx
#include "itkHistogramThresholdingImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template<class TInputImage, class TOutputImage>
HistogramThresholdingImageFilter<TInputImage, TOutputImage>
::HistogramThresholdingImageFilter()
{
  m_OutsideValue   = NumericTraits<OutputPixelType>::Zero;
  m_InsideValue    = NumericTraits<OutputPixelType>::max();
  m_Threshold      = NumericTraits<InputPixelType>::Zero;
  m_ThreshMethod   = OTSU;
  m_HistogramGenerator = HistogramGeneratorType::New();

  m_HuangCalc = HuangCalculatorType::New();
  m_IntermodesCalc = IntermodesCalculatorType::New();
  m_IsoDataCalc = IsoDataCalculatorType::New();
  m_KittlerIllingworthCalc = KittlerIllingworthCalculatorType::New();
  m_LiCalc = LiCalculatorType::New();
  m_MaximumEntropyCalc = MaximumEntropyCalculatorType::New();
  m_MomentsCalc = MomentsCalculatorType::New();
  m_OtsuCalc = OtsuCalculatorType::New();
  m_RenyiEntropyCalc = RenyiEntropyCalculatorType::New();
  m_ShanbhagCalc = ShanbhagCalculatorType::New();
  m_TriangleCalc = TriangleCalculatorType::New();
  m_YenCalc = YenCalculatorType::New();

  m_MaximumSmoothingIterations = 10000;
  m_UseInterMode = true;
  // seems we can't initialize this properly until runtime.
  m_NumberOfHistogramBins = 256;

}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdingImageFilter<TInputImage, TOutputImage>
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

  switch (m_ThreshMethod)
    {
    case HUANG:
      m_HuangCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_HuangCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_HuangCalc,.2f);
      thresholder->SetUpperThresholdInput( m_HuangCalc->GetOutput() );
      break;
    case INTERMODES:
      m_IntermodesCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_IntermodesCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      m_IntermodesCalc->SetMaximumSmoothingIterations( m_MaximumSmoothingIterations );
      m_IntermodesCalc->SetUseInterMode( m_UseInterMode );

      progress->RegisterInternalFilter(m_IntermodesCalc,.2f);
      thresholder->SetUpperThresholdInput( m_IntermodesCalc->GetOutput() );
      break;
    case ISODATA:
      m_IsoDataCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_IsoDataCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_IsoDataCalc,.2f);
      thresholder->SetUpperThresholdInput( m_IsoDataCalc->GetOutput() );

      break;
    case KITTLERILLINGWORTH:
      m_KittlerIllingworthCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_KittlerIllingworthCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_KittlerIllingworthCalc,.2f);
      thresholder->SetUpperThresholdInput( m_KittlerIllingworthCalc->GetOutput() );

      break;
    case LI:
      m_LiCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_LiCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_LiCalc,.2f);
      thresholder->SetUpperThresholdInput( m_LiCalc->GetOutput() );
      break;
    case MAXIMUMENTROPY:
      m_MaximumEntropyCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_MaximumEntropyCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_MaximumEntropyCalc,.2f);
      thresholder->SetUpperThresholdInput( m_MaximumEntropyCalc->GetOutput() );
      break;
    case MOMENTS:
      m_MomentsCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_MomentsCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_MomentsCalc,.2f);
      thresholder->SetUpperThresholdInput( m_MomentsCalc->GetOutput() );
      break;
    case OTSU:
      m_OtsuCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_OtsuCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_OtsuCalc,.2f);
      thresholder->SetUpperThresholdInput( m_OtsuCalc->GetOutput() );
      break;
    case RENYIENTROPY:
      m_RenyiEntropyCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_RenyiEntropyCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_RenyiEntropyCalc,.2f);
      thresholder->SetUpperThresholdInput( m_RenyiEntropyCalc->GetOutput() );
      break;
    case SHANBHAG:
      m_ShanbhagCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_ShanbhagCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_ShanbhagCalc,.2f);
      thresholder->SetUpperThresholdInput( m_ShanbhagCalc->GetOutput() );
      break;
    case TRIANGLE:
      m_TriangleCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_TriangleCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_TriangleCalc,.2f);
      thresholder->SetUpperThresholdInput( m_TriangleCalc->GetOutput() );
      break;
    case YEN:
      m_YenCalc->SetInput( m_HistogramGenerator->GetOutput() );
      m_YenCalc->SetNumberOfThreads( this->GetNumberOfThreads() );
      progress->RegisterInternalFilter(m_YenCalc,.2f);
      thresholder->SetUpperThresholdInput( m_YenCalc->GetOutput() );
      break;
    default:
      break;
    }

  thresholder->GraftOutput( this->GetOutput() );
  thresholder->Update();
  this->GraftOutput( thresholder->GetOutput() );

  switch (m_ThreshMethod)
    {
    case HUANG:
      m_Threshold = m_HuangCalc->GetOutput()->Get();
      break;
    case INTERMODES:
      m_Threshold = m_IntermodesCalc->GetOutput()->Get();
      break;
    case ISODATA:
      m_Threshold = m_IsoDataCalc->GetOutput()->Get();
      break;
    case KITTLERILLINGWORTH:
      m_Threshold = m_KittlerIllingworthCalc->GetOutput()->Get();
      break;
    case LI:
      m_Threshold = m_LiCalc->GetOutput()->Get();
      break;
    case MAXIMUMENTROPY:
      m_Threshold = m_MaximumEntropyCalc->GetOutput()->Get();
      break;
    case MOMENTS:
      m_Threshold = m_MomentsCalc->GetOutput()->Get();
      break;
    case OTSU:
      m_Threshold = m_OtsuCalc->GetOutput()->Get();
      break;
    case RENYIENTROPY:
      m_Threshold = m_RenyiEntropyCalc->GetOutput()->Get();
      break;
    case SHANBHAG:
      m_Threshold = m_ShanbhagCalc->GetOutput()->Get();
      break;
    case TRIANGLE:
      m_Threshold = m_TriangleCalc->GetOutput()->Get();
      break;
    case YEN:
      m_Threshold = m_YenCalc->GetOutput()->Get();
      break;
    default:
      break;
    }

}

template<class TInputImage, class TOutputImage>
void
HistogramThresholdingImageFilter<TInputImage, TOutputImage>
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
HistogramThresholdingImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  std::string methodname;
  switch (m_ThreshMethod)
    {
    case HUANG:
      methodname="Huang";
      break;
    case INTERMODES:
      methodname="Intermodes";
      break;
    case ISODATA:
      methodname="IsoData";
      break;
    case KITTLERILLINGWORTH:
      methodname="KittlerIllingworth";
      break;
    case LI:
      methodname="Li";
      break;
    case MAXIMUMENTROPY:
      methodname="MaximumEntropy";
      break;
    case MOMENTS:
      methodname="Moments";
      break;
    case OTSU:
      methodname="Otsu";
      break;
    case RENYIENTROPY:
      methodname="RenyiEntropy";
      break;
    case SHANBHAG:
      methodname="Shanbhag";
      break;
    case TRIANGLE:
      methodname="Triangle";
      break;
    case YEN:
      methodname="Yen";
      break;
    default:
      methodname="unknown";
      break;
    }

  os << indent << "Method: " << methodname << " " << std::endl;
  os << indent << "OutsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_OutsideValue) << std::endl;
  os << indent << "InsideValue: "
     << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_InsideValue) << std::endl;
  os << indent << "Threshold (computed): "
     << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_Threshold) << std::endl;

}


}// end namespace itk
#endif
