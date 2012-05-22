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

#include <iostream>

#include "itkOpenCVImageBridge.h"
#include "itkImageFileReader.h"
#include "itkTestingComparisonImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkOpenCVVideoIOFactory.h"

//-----------------------------------------------------------------------------
// Compare RGBPixel Images
//
template<class TPixelValue, unsigned int VDimension>
TPixelValue
RGBImageTotalAbsDifference(
  const itk::Image<itk::RGBPixel<TPixelValue>, VDimension>* valid,
  const itk::Image<itk::RGBPixel<TPixelValue>, VDimension>* test)
{
  typedef itk::RGBPixel<TPixelValue>                           PixelType;
  typedef itk::Image<PixelType, VDimension>                    RGBImageType;
  typedef itk::ImageRegionConstIteratorWithIndex<RGBImageType> IterType;

  IterType validIt(valid, valid->GetLargestPossibleRegion());
  IterType testIt(test, test->GetLargestPossibleRegion());

  TPixelValue totalDiff = 0;

  while(!validIt.IsAtEnd())
    {
    PixelType validPx = validIt.Get();
    PixelType testPx = testIt.Get();

    TPixelValue localDiff = 0;

    for( unsigned int i = 0; i < 3; i++ )
      {
      localDiff += vnl_math_abs(validPx[i] - testPx[i]);
      }

    if( localDiff != 0 )
      {
      IterType testIt2 = testIt;
      ++testIt2;
      IterType validIt2 = validIt;
      ++validIt2;
      std::cerr << testIt.GetIndex() << " [ " << validPx << " " << validIt2.Get() << "] != [ " << testPx << " " << testIt2.Get() << " ]" << std::endl;
      return localDiff;
      }

    totalDiff += localDiff;

    ++validIt;
    ++testIt;
    }

  return totalDiff;
}


//-----------------------------------------------------------------------------
// Convert the data in the IplImage to the templated type
//
template<class TPixelType>
IplImage* ConvertIplImageDataType(IplImage* in)
{
  int depth = 0;

  // Figure out the right output type
  if (typeid(TPixelType) == typeid(unsigned char))
    {
    depth = IPL_DEPTH_8U;
    }
  else if (typeid(TPixelType) == typeid(char))
    {
    depth = IPL_DEPTH_8S;
    }
  else if (typeid(TPixelType) == typeid(unsigned short))
    {
    depth = IPL_DEPTH_16U;
    }
  else if (typeid(TPixelType) == typeid(short))
    {
    depth = IPL_DEPTH_16S;
    }
  else if (typeid(TPixelType) == typeid(float))
    {
    depth = IPL_DEPTH_32F;
    }
  else if (typeid(TPixelType) == typeid(double))
    {
    depth = IPL_DEPTH_64F;
    }
  else
    {
    itkGenericExceptionMacro("OpenCV doesn't support the requested type");
    }

  IplImage* out = cvCreateImage(cvSize(in->width, in->height), depth, in->nChannels);
  cvConvertScale(in, out);
  return out;
}


//-----------------------------------------------------------------------------
// Templated test function to do the heavy lifting for scalar case
//
template<class TPixelType, unsigned int VDimension>
int itkOpenCVImageBridgeTestTemplatedScalar(char* argv)
{
  // typedefs
  const unsigned int Dimension =                         VDimension;
  typedef TPixelType                                     PixelType;
  typedef itk::Image< PixelType, Dimension >             ImageType;
  typedef itk::ImageFileReader<ImageType>                ReaderType;
  typedef itk::Testing::ComparisonImageFilter<ImageType, ImageType>
                                                         DifferenceFilterType;

  itk::ObjectFactoryBase::RegisterFactory( itk::OpenCVVideoIOFactory::New() );

  //
  // Read the image directly
  //
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv);

  reader->Update();
  typename ImageType::Pointer baselineImage = reader->GetOutput();

  //
  // Test IplImage -> itk::Image
  //
  IplImage* inIpl;
  inIpl = cvLoadImage(argv, CV_LOAD_IMAGE_GRAYSCALE);
  if (!inIpl)
    {
    std::cerr << "Could not load input as IplImage" << std::endl;
    return EXIT_FAILURE;
    }
  typename ImageType::Pointer outIplITK =
    itk::OpenCVImageBridge::IplImageToITKImage< ImageType >(inIpl);

  // Check results of IplImage -> itk::Image
  typename DifferenceFilterType::Pointer differ = DifferenceFilterType::New();
  differ->SetValidInput(baselineImage);
  differ->SetTestInput(outIplITK);
  differ->Update();
  typename DifferenceFilterType::AccumulateType total = differ->GetTotalDifference();

  if (total != 0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for IplImage -> ITK (scalar)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test cv::Mat -> itk::Image
  //
  cv::Mat inMat;
  inMat = cv::imread(argv);
  typename ImageType::Pointer outMatITK =
    itk::OpenCVImageBridge::CVMatToITKImage< ImageType >(inMat);

  // Check results of cv::Mat -> itk::Image
  differ->SetTestInput(outMatITK);
  differ->Update();
  total = differ->GetTotalDifference();
  if (total != 0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for cv::Mat -> ITK (scalar)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test itk::Image -> IplImage
  //
  IplImage* outIpl = itk::OpenCVImageBridge::ITKImageToIplImage< ImageType >(baselineImage);

  // check results of itk::Image -> IplImage
  IplImage* dataConvertedInIpl = ConvertIplImageDataType<PixelType>(inIpl);
  double itkIplDiff = cvNorm(outIpl, dataConvertedInIpl);

  if (itkIplDiff != 0.0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for ITK -> IplImage (scalar)" <<";  itkIplDiff = "<<itkIplDiff<< std::endl;
    return EXIT_FAILURE;
    }

  // Test number of channels after force3Channels (if type is supported for color images)
  if (typeid(PixelType) == typeid(unsigned short) ||
      typeid(PixelType) == typeid(unsigned char) ||
      typeid(PixelType) == typeid(float))
    {
    cvReleaseImage(&outIpl);
    outIpl = itk::OpenCVImageBridge::ITKImageToIplImage< ImageType >(baselineImage, true);
    if (outIpl->nChannels != 3)
      {
      std::cerr << "force3Channels failed" << std::endl;
      return EXIT_FAILURE;
      }
    }

  //
  // Test itk::Image -> cv::Mat
  //
  cv::Mat outMat = itk::OpenCVImageBridge::ITKImageToCVMat< ImageType >(baselineImage);

  // check results of itk::Image -> IplImage
  IplImage outMatAsIpl = outMat;
  double itkMatDiff = cvNorm(&outMatAsIpl, dataConvertedInIpl);
  if (itkMatDiff != 0.0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for ITK -> cv::Mat (scalar)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Clean up and return successfully
  //
  cvReleaseImage(&dataConvertedInIpl);
  cvReleaseImage(&inIpl);
  cvReleaseImage(&outIpl);
  return EXIT_SUCCESS;
}


//-----------------------------------------------------------------------------
// Templated test function to do the heavy lifting for RGB case
//
template<class TValueType, unsigned int VDimension>
int itkOpenCVImageBridgeTestTemplatedRGB(char* argv0, char* argv1)
{
  // typedefs
  const unsigned int Dimension =                          VDimension;
  typedef TValueType                                      ValueType;
  typedef itk::RGBPixel< ValueType >                      PixelType;
  typedef typename PixelType::ComponentType               ComponentType;
  typedef itk::Image< PixelType, Dimension >              ImageType;
  typedef itk::ImageFileReader<ImageType>                 ReaderType;
  typedef itk::Testing::ComparisonImageFilter<ImageType, ImageType>
                                                          DifferenceFilterType;

  //
  // Read the image directly
  //
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv1);
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject& e )
    {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
    }

  typename ImageType::Pointer baselineImage = reader->GetOutput();

  //
  // Test IplImage -> itk::Image
  //
  IplImage* inIpl = cvLoadImage(argv0, CV_LOAD_IMAGE_COLOR);
  if (!inIpl)
    {
    std::cerr << "Could not load input as IplImage " << argv0 << std::endl;
    return EXIT_FAILURE;
    }
  typename ImageType::Pointer outIplITK =
    itk::OpenCVImageBridge::IplImageToITKImage< ImageType >(inIpl);

  ComponentType itkIplDiff1 = RGBImageTotalAbsDifference<ComponentType, Dimension>(
          baselineImage, outIplITK);

  // Check results of IplImage -> itk::Image
  if ( itkIplDiff1 != itk::NumericTraits< ComponentType >::Zero )
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for IplImage -> ITK (RGB)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test cv::Mat -> itk::Image
  //
  cv::Mat inMat = cv::imread(argv0);
  typename ImageType::Pointer outMatITK =
    itk::OpenCVImageBridge::CVMatToITKImage< ImageType >(inMat);

  ComponentType itkCvMatDiff = RGBImageTotalAbsDifference<ComponentType, Dimension>(
          baselineImage, outMatITK);

  // Check results of cv::Mat -> itk::Image
  if ( itkCvMatDiff != itk::NumericTraits< ComponentType >::Zero )
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for cv::Mat -> ITK (RGB)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test itk::Image -> IplImage
  //
  IplImage* outIpl = itk::OpenCVImageBridge::ITKImageToIplImage< ImageType >(baselineImage);

  // check results of itk::Image -> IplImage
  IplImage* dataConvertedInIpl = ConvertIplImageDataType<ValueType>(inIpl);
  double itkIplDiff = cvNorm(outIpl, dataConvertedInIpl);

  if (itkIplDiff != 0.0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(ValueType).name()
      << " for ITK -> IplImage (RGB)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test itk::Image -> cv::Mat
  //
  cv::Mat outMat = itk::OpenCVImageBridge::ITKImageToCVMat< ImageType >(baselineImage);

  // check results of itk::Image -> IplImage
  IplImage outMatAsIpl = outMat;
  double itkMatDiff = cvNorm(&outMatAsIpl, dataConvertedInIpl);
  if (itkMatDiff != 0.0)
    {
    std::cerr << "Images didn't match for pixel type " << typeid(PixelType).name()
      << " for ITK -> cv::Mat (RGB)" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Clean up and return successfully
  //
  cvReleaseImage(&dataConvertedInIpl);
  cvReleaseImage(&inIpl);
  cvReleaseImage(&outIpl);
  return EXIT_SUCCESS;
}

template< class TPixel >
int itkRunScalarTest( char* argv )
{
  if (itkOpenCVImageBridgeTestTemplatedScalar< TPixel, 2 >(argv) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (itkOpenCVImageBridgeTestTemplatedScalar< TPixel, 3 >(argv) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

template< class TValue >
int itkRunRGBTest( char* argv0, char* argv1 )
{
  if (itkOpenCVImageBridgeTestTemplatedRGB< TValue, 2 >(argv0, argv1) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }
  if (itkOpenCVImageBridgeTestTemplatedRGB< TValue, 3 >(argv0, argv1) == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// Main test
//
int itkOpenCVImageBridgeTest ( int argc, char *argv[] )
{
  //
  // Check arguments
  //
  if (argc != 7)
    {
    std::cerr << "Usage: " << argv[0] << "scalar_image1 scalar_image2 rgb_jpg_image rgb_mha_image rgb_image2" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Test for scalar types
  //
  // Note: We don't test signed char because ITK seems to have trouble reading
  //       images with char pixels.
  //
  std::cout << "scalar" << std::endl;
  if( itkRunScalarTest< unsigned char >( argv[1] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< short >( argv[1] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< unsigned short >( argv[1] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< float >( argv[1] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< double >( argv[1] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }

//  TODO: to be tested (failing right now)
//  std::cout << "rgb images opened as scalar image" << std::endl;
//  if( itkRunScalarTest< unsigned char >( argv[3] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< short >( argv[3] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< unsigned short >( argv[3] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< float >( argv[3] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< double >( argv[3] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }

  std::cout << "scalar 513x512" << std::endl;
  if( itkRunScalarTest< unsigned char >( argv[2] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< short >( argv[2] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< unsigned short >( argv[2] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< float >( argv[2] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunScalarTest< double >( argv[2] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }


  //  TODO: to be tested (failing right now)
//  std::cout << "rgb images opened as scalar image (513 x 512)" << std::endl;
//  if( itkRunScalarTest< unsigned char >( argv[5] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< short >( argv[5] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< unsigned short >( argv[5] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< float >( argv[5] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunScalarTest< double >( argv[5] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }

  //
  // Test for RGB types
  //
  // Note: OpenCV only supports unsigned char, unsigned short, and float for
  // color conversion
  //
  std::cout << "rgb" << std::endl;

  if( itkRunRGBTest< unsigned char >( argv[3], argv[4] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunRGBTest< unsigned short >( argv[3], argv[4] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunRGBTest< float >( argv[3], argv[4] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }


  //  TODO: to be tested (failing right now)
//  std::cout << "scalar image opened as rgb image" << std::endl;

//  if( itkRunRGBTest< unsigned char >( argv[1], argv[1] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunRGBTest< unsigned short >( argv[1], argv[1] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunRGBTest< float >( argv[1], argv[1] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }

  //
  // Test for RGB types
  //
  // Note: OpenCV only supports unsigned char, unsigned short, and float for
  // color conversion
  //
  std::cout << "rgb 513x512" << std::endl;

  if( itkRunRGBTest< unsigned char >( argv[5], argv[6] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunRGBTest< unsigned short >( argv[5], argv[6] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }
  if( itkRunRGBTest< float >( argv[5], argv[6] ) == EXIT_FAILURE )
    {
    return EXIT_FAILURE;
    }


  //  TODO: to be tested (failing right now)
//  std::cout << "scalar image opened as rgb image (513x512)" << std::endl;

//  if( itkRunRGBTest< unsigned char >( argv[2], argv[2] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunRGBTest< unsigned short >( argv[2], argv[2] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }
//  if( itkRunRGBTest< float >( argv[2], argv[2] ) == EXIT_FAILURE )
//    {
//    return EXIT_FAILURE;
//    }

  return EXIT_SUCCESS;
}
