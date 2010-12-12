#include "itkBSplineControlPointImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkN4MRIBiasFieldCorrectionImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkShrinkImageFilter.h"

template<class TFilter>
class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate   Self;
  typedef itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *) caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    const TFilter * filter =
      dynamic_cast< const TFilter * >( object );
    if( typeid( event ) != typeid( itk::IterationEvent ) )
      { return; }
    if( filter->GetElapsedIterations() == 1 )
      {
      std::cout << "Current level = " << filter->GetCurrentLevel() + 1
        << std::endl;
      }
    std::cout << "  Iteration " << filter->GetElapsedIterations()
      << " (of "
      << filter->GetMaximumNumberOfIterations()[filter->GetCurrentLevel()]
      << ").  ";
    std::cout << " Current convergence value = "
      << filter->GetCurrentConvergenceMeasurement()
      << " (threshold = " << filter->GetConvergenceThreshold()
      << ")" << std::endl;
    }
};

#include <string>
#include <vector>

template<class TValue>
TValue Convert( std::string optionString )
{
  TValue value;
  std::istringstream iss( optionString );
  iss >> value;
  return value;
}

template<class TValue>
std::vector<TValue> ConvertVector( std::string optionString )
{
  std::vector<TValue> values;
  std::string::size_type crosspos = optionString.find( 'x', 0 );

  if ( crosspos == std::string::npos )
    {
    values.push_back( Convert<TValue>( optionString ) );
    }
  else
    {
    std::string element = optionString.substr( 0, crosspos ) ;
    TValue value;
    std::istringstream iss( element );
    iss >> value;
    values.push_back( value );
    while ( crosspos != std::string::npos )
      {
      std::string::size_type crossposfrom = crosspos;
      crosspos = optionString.find( 'x', crossposfrom + 1 );
      if ( crosspos == std::string::npos )
        {
        element = optionString.substr( crossposfrom + 1, optionString.length() );
        }
      else
        {
        element = optionString.substr( crossposfrom + 1, crosspos ) ;
        }
      std::istringstream iss( element );
      iss >> value;
      values.push_back( value );
      }
    }
  return values;
}

template <unsigned int ImageDimension>
int itkN4MRIBiasFieldCorrectionImageFilterTest( int argc, char *argv[] )
{
  typedef float RealType;

  typedef itk::Image<RealType, ImageDimension> ImageType;
  typename ImageType::Pointer inputImage = NULL;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  inputImage = reader->GetOutput();
  inputImage->Update();
  inputImage->DisconnectPipeline();

  /**
   * handle the mask image
   */
  typedef itk::Image<unsigned char, ImageDimension> MaskImageType;
  typename MaskImageType::Pointer maskImage = NULL;

  if( argc > 6 )
    {
    typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
    typename MaskReaderType::Pointer maskreader = MaskReaderType::New();
    maskreader->SetFileName( argv[6] );
    try
      {
      maskImage = maskreader->GetOutput();
      maskImage->Update();
      maskImage->DisconnectPipeline();
      }
    catch( ... )
      {
      maskImage = NULL;
      }
    }

  if( !maskImage )
    {
    std::cout << "Mask not read.  Creating Otsu mask." << std::endl;
    typedef itk::OtsuThresholdImageFilter<ImageType, MaskImageType>
      ThresholderType;
    typename ThresholderType::Pointer otsu = ThresholderType::New();
    otsu->SetInput( inputImage );
    otsu->SetNumberOfHistogramBins( 200 );
    otsu->SetInsideValue( 0 );
    otsu->SetOutsideValue( 1 );

    maskImage = otsu->GetOutput();
    maskImage->Update();
    maskImage->DisconnectPipeline();
    }

  /**
   * instantiate N4 and assign variables not exposed to the user in this test.
   */
  typedef itk::N4MRIBiasFieldCorrectionImageFilter<ImageType, MaskImageType,
    ImageType> CorrecterType;
  typename CorrecterType::Pointer correcter = CorrecterType::New();
  correcter->SetMaskLabel( 1 );
  correcter->SetSplineOrder( 3 );
  correcter->SetWeinerFilterNoise( 0.01 );
  correcter->SetBiasFieldFullWidthAtHalfMaximum( 0.15 );
  correcter->SetConvergenceThreshold( 0.0000001 );

  /**
   * handle the number of iterations
   */
  std::vector<unsigned int> numIters = ConvertVector<unsigned int>(
    std::string( "100x50x50" ) );
  if( argc > 5 )
    {
    numIters = ConvertVector<unsigned int>( argv[5] );
    }
  typename CorrecterType::VariableSizeArrayType
     maximumNumberOfIterations( numIters.size() );
  for( unsigned int d = 0; d < numIters.size(); d++ )
    {
    maximumNumberOfIterations[d] = numIters[d];
    }
  correcter->SetMaximumNumberOfIterations( maximumNumberOfIterations );

  typename CorrecterType::ArrayType numberOfFittingLevels;
  numberOfFittingLevels.Fill( numIters.size() );
  correcter->SetNumberOfFittingLevels( numberOfFittingLevels );

  /**
   * B-spline options -- we place this here to take care of the case where
   * the user wants to specify things in terms of the spline distance.
   * the user wants to specify things in terms of spline distance.
   *  1. need to pad the images to get as close to possible to the
   *     requested domain size.
   */
  typename ImageType::IndexType inputImageIndex =
    inputImage->GetLargestPossibleRegion().GetIndex();
  typename ImageType::SizeType inputImageSize =
    inputImage->GetLargestPossibleRegion().GetSize();
  typename ImageType::IndexType maskImageIndex =
    maskImage->GetLargestPossibleRegion().GetIndex();
  typename ImageType::SizeType maskImageSize =
    maskImage->GetLargestPossibleRegion().GetSize();

  typename ImageType::PointType newOrigin = inputImage->GetOrigin();

  typename CorrecterType::ArrayType numberOfControlPoints;

  float splineDistance = 200;
  if( argc > 7 )
    {
    splineDistance = atof( argv[7] );
    }

  unsigned long lowerBound[ImageDimension];
  unsigned long upperBound[ImageDimension];

  for( unsigned int d = 0; d < ImageDimension; d++ )
    {
    float domain = static_cast<RealType>( inputImage->
       GetLargestPossibleRegion().GetSize()[d] - 1 ) * inputImage->GetSpacing()[d];
    unsigned int numberOfSpans = static_cast<unsigned int>(
             vcl_ceil( domain / splineDistance ) );
    unsigned long extraPadding = static_cast<unsigned long>( ( numberOfSpans *
              splineDistance - domain ) / inputImage->GetSpacing()[d] + 0.5 );
    lowerBound[d] = static_cast<unsigned long>( 0.5 * extraPadding );
    upperBound[d] = extraPadding - lowerBound[d];
    newOrigin[d] -= ( static_cast<RealType>( lowerBound[d] ) *
        inputImage->GetSpacing()[d] );
    numberOfControlPoints[d] = numberOfSpans + correcter->GetSplineOrder();
    }

  typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadderType;
  typename PadderType::Pointer padder = PadderType::New();
  padder->SetInput( inputImage );
  padder->SetPadLowerBound( lowerBound );
  padder->SetPadUpperBound( upperBound );
  padder->SetConstant( 0 );
  padder->Update();

  inputImage = padder->GetOutput();
  inputImage->DisconnectPipeline();

  typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
  typename MaskPadderType::Pointer maskPadder = MaskPadderType::New();
  maskPadder->SetInput( maskImage );
  maskPadder->SetPadLowerBound( lowerBound );
  maskPadder->SetPadUpperBound( upperBound );
  maskPadder->SetConstant( 0 );
  maskPadder->Update();

  maskImage = maskPadder->GetOutput();
  maskImage->DisconnectPipeline();

  correcter->SetNumberOfControlPoints( numberOfControlPoints );

  /**
   * handle the shrink factor
   */
  typedef itk::ShrinkImageFilter<ImageType, ImageType> ShrinkerType;
  typename ShrinkerType::Pointer shrinker = ShrinkerType::New();
  shrinker->SetInput( reader->GetOutput() );
  shrinker->SetShrinkFactors( 1 );

  typedef itk::ShrinkImageFilter<MaskImageType, MaskImageType>
    MaskShrinkerType;
  typename MaskShrinkerType::Pointer maskshrinker = MaskShrinkerType::New();
  maskshrinker->SetInput( maskImage );
  maskshrinker->SetShrinkFactors( 1 );

  if( argc > 4 )
    {
    shrinker->SetShrinkFactors( atoi( argv[4] ) );
    maskshrinker->SetShrinkFactors( atoi( argv[4] ) );
    }
  shrinker->Update();
  inputImage = shrinker->GetOutput();
  inputImage->DisconnectPipeline();

  maskshrinker->Update();
  maskImage = maskshrinker->GetOutput();
  maskImage->DisconnectPipeline();

  /**
   * set the input image and mask image
   */
  correcter->SetInput( inputImage );
  correcter->SetMaskImage( maskImage );

  typedef CommandIterationUpdate<CorrecterType> CommandType;
  typename CommandType::Pointer observer = CommandType::New();
  correcter->AddObserver( itk::IterationEvent(), observer );

  try
    {
    correcter->Update();
    }
  catch( itk::ExceptionObject &excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  correcter->Print( std::cout, 3 );

  /**
   * Reconstruct the bias field at full image resolution.
   * Divide the original input image by the bias field to get the final
   * corrected image.
   */

  typedef itk::BSplineControlPointImageFilter<typename
    CorrecterType::BiasFieldControlPointLatticeType, typename
    CorrecterType::ScalarImageType> BSplinerType;
  typename BSplinerType::Pointer bspliner = BSplinerType::New();
  bspliner->SetInput( correcter->GetLogBiasFieldControlPointLattice() );
  bspliner->SetSplineOrder( correcter->GetSplineOrder() );
  bspliner->SetSize(
    reader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  bspliner->SetOrigin( reader->GetOutput()->GetOrigin() );
  bspliner->SetDirection( reader->GetOutput()->GetDirection() );
  bspliner->SetSpacing( reader->GetOutput()->GetSpacing() );
  bspliner->Update();

  typename ImageType::Pointer logField = ImageType::New();
  logField->SetOrigin( bspliner->GetOutput()->GetOrigin() );
  logField->SetSpacing( bspliner->GetOutput()->GetSpacing() );
  logField->SetRegions(
    bspliner->GetOutput()->GetLargestPossibleRegion().GetSize() );
  logField->SetDirection( bspliner->GetOutput()->GetDirection() );
  logField->Allocate();

  itk::ImageRegionIterator<typename CorrecterType::ScalarImageType> ItB(
    bspliner->GetOutput(),
    bspliner->GetOutput()->GetLargestPossibleRegion() );
  itk::ImageRegionIterator<ImageType> ItF( logField,
    logField->GetLargestPossibleRegion() );
  for( ItB.GoToBegin(), ItF.GoToBegin(); !ItB.IsAtEnd(); ++ItB, ++ItF )
    {
    ItF.Set( ItB.Get()[0] );
    }

  typedef itk::ExpImageFilter<ImageType, ImageType> ExpFilterType;
  typename ExpFilterType::Pointer expFilter = ExpFilterType::New();
  expFilter->SetInput( logField );
  expFilter->Update();

  typedef itk::DivideImageFilter<ImageType, ImageType, ImageType> DividerType;
  typename DividerType::Pointer divider = DividerType::New();
  divider->SetInput1( reader->GetOutput() );
  divider->SetInput2( expFilter->GetOutput() );
  divider->Update();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( divider->GetOutput() );
  writer->Update();

  if( argc > 8 )
    {
    typedef itk::ImageFileWriter<ImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( argv[8] );
    writer->SetInput( expFilter->GetOutput() );
    writer->Update();
    }

  return EXIT_SUCCESS;
}

int main( int argc, char *argv[] )
{
  if ( argc < 4 )
    {
    std::cerr << "Usage: " << argv[0] << " imageDimension inputImage "
     << "outputImage [shrinkFactor,default=1] [numberOfIterations,default=100x50x50] "
     << " [maskImageWithLabelEqualTo1] [splineDistance,default=200] [outputBiasField] "
     << std::endl;
    exit( EXIT_FAILURE );
    }

  switch( atoi( argv[1] ) )
   {
   case 2:
     itkN4MRIBiasFieldCorrectionImageFilterTest<2>( argc, argv );
     break;
   case 3:
     itkN4MRIBiasFieldCorrectionImageFilterTest<3>( argc, argv );
     break;
   default:
      std::cerr << "Unsupported dimension" << std::endl;
      exit( EXIT_FAILURE );
   }
}