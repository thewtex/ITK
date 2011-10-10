#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPatchBasedDenoisingImageFilter.h"
#include "itkMeasurementVectorTraits.h"
#include "itkGaussianRandomSpatialNeighborSubsampler.h"
#include "itkStdStreamLogOutput.h"

template <class ImageT>
int doDenoising(std::string inputFileName, std::string outputFileName, int numIterations,
           int numThreads, std::string noiseModel, float fidelityWeight)
{
  typedef itk::ImageFileReader< ImageT >     ReaderType;
  typedef itk::PatchBasedDenoisingImageFilter<ImageT, ImageT> FilterType;
  typedef typename FilterType::PatchWeightsType PatchType;
  typedef itk::Statistics::GaussianRandomSpatialNeighborSubsampler<typename FilterType::PatchSampleType,
    typename ImageT::RegionType> SamplerType;
  typedef typename FilterType::OutputImageType OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Problem encountered while reading image file : " << inputFileName << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // create filter and initialize
  // give image to filter and run it
  // get filter output and write to file

  typename FilterType::Pointer filter = FilterType::New();
//   filter->DebugOn();
  filter->SetInput(reader->GetOutput());


  // radius of 4 from original code, TODO command-line arg?
  unsigned int radius=4;
//   unsigned int patchLength = pow(2*radius+1, ImageT::ImageDimension);

//   PatchType patchWeights;
//   itk::Statistics::MeasurementVectorTraits::SetLength(patchWeights, patchLength);
//   ComputeWeights(patchWeights);

  // patch radius is same for all n dimensions of image
  filter->SetPatchRadius(radius);
  // patch weights should be provided as a measurement vector
  // try creating a gaussian mask for the weights
//   filter->SetPatchWeights(patchWeights);

  // instead of directly setting the weights, could also
  // specify type to use ie:
  filter->UseSmoothDiscPatchWeightsOn();

  // TODO: not sure what is meant here by bandwidth estimation
  //filter->UseBandwidthEstimation();

  // update sigma every x iterations
  filter->SetSigmaUpdateFrequency(5);

  // use 20% of the pixels for the sigma update calculation
  filter->SetFractionPixelsForSigmaUpdate(0.30);

  // weight for regularization term (see equation)
  filter->SetRegularizationWeight(1.0);

  // weight for fidelity term (see equation)
  filter->SetFidelityWeight(fidelityWeight);

  // number of iterations of outmost loop
  filter->SetNumberOfIterations(numIterations);

  // noise model to use
  if (noiseModel == "GAUSSIAN")
  {
    filter->SetNoiseModel(FilterType::GAUSSIAN);
  }
  else if (noiseModel == "RICIAN")
  {
    filter->SetNoiseModel(FilterType::RICIAN);
  }
  else if (noiseModel == "POISSON")
  {
    filter->SetNoiseModel(FilterType::POISSON);
  }

  filter->SetNumberOfThreads(numThreads); // ?? TODO Should this be part of the official interface?

  typename SamplerType::Pointer sampler = SamplerType::New();
  // 32 comes from ~sqrt(1000) ie for 2-D image 32x32 = 1024
  // use half of 32 for the radius
  sampler->SetRadius(16);
  // number of random "patches" to use for computations
  sampler->SetNumberOfResultsRequested(300);
  sampler->SetVariance(400);

  // Sampler can be kdTree sampler, complete neighborhood sampler, random neighborhood sampler, gaussian sampler, etc.

  filter->SetSampler(sampler);

  typename itk::StdStreamLogOutput::Pointer coutstream = itk::StdStreamLogOutput::New();
  coutstream->SetStream(std::cout);
  typename FilterType::LoggerPointer logger = FilterType::LoggerType::New();

  logger->SetName("UINTA");
  logger->SetPriorityLevel(itk::LoggerBase::DEBUG);
  logger->SetLevelForFlushing(itk::LoggerBase::DEBUG);
  logger->SetTimeStampFormat(itk::LoggerBase::HUMANREADABLE);
  logger->SetHumanReadableFormat("%k:%M:%S");
  logger->AddLogOutput(coutstream);
  logger->Write(itk::LoggerBase::DEBUG, "Created Logger\n");
  logger->Flush();

  filter->SetLogger(logger);

  std::cout << "Filter prior to update:\n";
  filter->Print( std::cout );

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject & excp)
  {
    std::ostringstream itkmsg;                                            \
    itkmsg << "Error: In " __FILE__ ", line " << __LINE__ << "\n"
           << "Caught exception <" << excp
           << "> while running patch-based denoising image filter."
           << "\n\n";
    ::itk::OutputWindowDisplayWarningText(itkmsg.str().c_str());
    return EXIT_FAILURE;
  }

  typename WriterType::Pointer writer = WriterType::New();

  writer->SetFileName( outputFileName );

  writer->SetInput( filter->GetOutput() );
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;


}

int itkPatchBasedDenoisingImageFilterTest( int argc, char * argv [] )
{

  if( argc < 3 )
  {
    std::cerr << "Missing command line arguments" << std::endl;
    std::cerr << "Usage :  " << argv[0]
              << " inputImageFileName outputImageFileName numDimensions numComponents [numIterations] [numThreads] [noiseModel] [fidelityWeight]"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string inFileName(argv[1]);
  std::string outFileName(argv[2]);
  unsigned int numDimensions = atoi(argv[3]);
  unsigned int numComponents = atoi(argv[4]);
  unsigned int numIterations = 1;
  if (argc > 5)
  {
    numIterations = atoi(argv[5]);
  }

  unsigned int numThreads = 1;
  if (argc > 6)
  {
    numThreads = atoi(argv[6]);
  }

  std::vector< std::string > modelChoices;
  modelChoices.push_back("GAUSSIAN");
  modelChoices.push_back("RICIAN");
  modelChoices.push_back("POISSON");
  std::string noiseModel;
  noiseModel = modelChoices[0];

  float fidelityWeight = 0.0;
  if (argc > 7)
  {
    noiseModel = argv[7];
    bool validChoice = false;
    for (unsigned int ii = 0; ii < modelChoices.size(); ++ii)
    {
      if( noiseModel == modelChoices[ii])
      {
        validChoice = true;
      }
    }
    if (!validChoice)
    {
      std::cerr << noiseModel << " is not a valid noise model choice.  Please choose one of: ";
      for (unsigned int ii = 0; ii < modelChoices.size(); ++ii)
      {
        std::cerr << modelChoices[ii] << " " << std::endl;
      }
      return EXIT_FAILURE;
    }
    if (argc > 8)
    {
      fidelityWeight = atof(argv[8]);
    }
    else
    {
      std::cerr << "Must also specify a fidelity weight when a noise model is specified."
                << std::endl;
      return EXIT_FAILURE;
    }
  }

  typedef float PixelComponentType;
  typedef itk::Vector< PixelComponentType, 1 > OneComponentType;
  typedef itk::Vector< PixelComponentType, 2 > TwoComponentType;
  typedef itk::Vector< PixelComponentType, 3 > ThreeComponentType;
  typedef itk::Vector< PixelComponentType, 4 > FourComponentType;
  typedef itk::Vector< PixelComponentType, 6 > SixComponentType;
  typedef itk::Vector< PixelComponentType, 9 > NineComponentType;

  typedef itk::Image< OneComponentType, 2 > OneComponent2DImage;
  typedef itk::Image< OneComponentType, 3 > OneComponent3DImage;
  typedef itk::Image< OneComponentType, 4 > OneComponent4DImage;
  typedef itk::Image< TwoComponentType, 2 > TwoComponent2DImage;
  typedef itk::Image< TwoComponentType, 3 > TwoComponent3DImage;
  typedef itk::Image< TwoComponentType, 4 > TwoComponent4DImage;
  typedef itk::Image< ThreeComponentType, 2 > ThreeComponent2DImage;
  typedef itk::Image< ThreeComponentType, 3 > ThreeComponent3DImage;
  typedef itk::Image< ThreeComponentType, 4 > ThreeComponent4DImage;
  typedef itk::Image< FourComponentType, 2 > FourComponent2DImage;
  typedef itk::Image< FourComponentType, 3 > FourComponent3DImage;
  typedef itk::Image< FourComponentType, 4 > FourComponent4DImage;
  typedef itk::Image< SixComponentType, 2 > SixComponent2DImage;
  typedef itk::Image< SixComponentType, 3 > SixComponent3DImage;
  typedef itk::Image< SixComponentType, 4 > SixComponent4DImage;
  typedef itk::Image< NineComponentType, 2 > NineComponent2DImage;
  typedef itk::Image< NineComponentType, 3 > NineComponent3DImage;
  typedef itk::Image< NineComponentType, 4 > NineComponent4DImage;

  if (numComponents == 1 && numDimensions == 2)
  {
    return doDenoising<OneComponent2DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 2 && numDimensions == 2)
  {
    return doDenoising<TwoComponent2DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 3 && numDimensions == 2)
  {
    return doDenoising<ThreeComponent2DImage>(inFileName, outFileName,
                                              numIterations, numThreads,
                                              noiseModel, fidelityWeight);
  }
  else if (numComponents == 4 && numDimensions == 2)
  {
    return doDenoising<FourComponent2DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  else if (numComponents == 6 && numDimensions == 2)
  {
    return doDenoising<SixComponent2DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 9 && numDimensions == 2)
  {
    return doDenoising<NineComponent2DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  if (numComponents == 1 && numDimensions == 3)
  {
    return doDenoising<OneComponent3DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 2 && numDimensions == 3)
  {
    return doDenoising<TwoComponent3DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 3 && numDimensions == 3)
  {
    return doDenoising<ThreeComponent3DImage>(inFileName, outFileName,
                                              numIterations, numThreads,
                                              noiseModel, fidelityWeight);
  }
  else if (numComponents == 4 && numDimensions == 3)
  {
    return doDenoising<FourComponent3DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  else if (numComponents == 6 && numDimensions == 3)
  {
    return doDenoising<SixComponent3DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 9 && numDimensions == 3)
  {
    return doDenoising<NineComponent3DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  if (numComponents == 1 && numDimensions == 4)
  {
    return doDenoising<OneComponent4DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 2 && numDimensions == 4)
  {
    return doDenoising<TwoComponent4DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 3 && numDimensions == 4)
  {
    return doDenoising<ThreeComponent4DImage>(inFileName, outFileName,
                                              numIterations, numThreads,
                                              noiseModel, fidelityWeight);
  }
  else if (numComponents == 4 && numDimensions == 4)
  {
    return doDenoising<FourComponent4DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  else if (numComponents == 6 && numDimensions == 4)
  {
    return doDenoising<SixComponent4DImage>(inFileName, outFileName,
                                            numIterations, numThreads,
                                            noiseModel, fidelityWeight);
  }
  else if (numComponents == 9 && numDimensions == 4)
  {
    return doDenoising<NineComponent4DImage>(inFileName, outFileName,
                                             numIterations, numThreads,
                                             noiseModel, fidelityWeight);
  }
  else
  {
    std::cout << "Combination of " << numComponents << " components and "
              << numDimensions << " dimensions is not supported." << std::endl;
    return EXIT_FAILURE;
  }
  // shouldn't reach this point, return failure here to keep the compiler happy
  return EXIT_FAILURE;
}
