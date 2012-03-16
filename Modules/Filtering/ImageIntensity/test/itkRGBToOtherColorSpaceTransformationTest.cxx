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

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageAdaptor.h"
#include "itkRedPixelAccessor.h"
#include "itkGreenPixelAccessor.h"
#include "itkBluePixelAccessor.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkRGBToXYZColorSpacePixelAccessor.h"
#include "itkRGBToYuvColorSpacePixelAccessor.h"
#include "itkRGBToYUV2ColorSpacePixelAccessor.h"
#include "itkRGBToHSIColorSpacePixelAccessor.h"
#include "itkRGBToHSVColorSpacePixelAccessor.h"
#include "itkRGBToLabColorSpacePixelAccessor.h"
#include "itkRGBToLuvColorSpacePixelAccessor.h"
#include "itkRGBToHSLColorSpacePixelAccessor.h"
#include "itkRGBToCMYColorSpacePixelAccessor.h"
#include "itkRGBToCMYKColorSpacePixelAccessor.h"

/*
RGBImageConverter takes as input an RGB image and
outputs images in a user-specified color space.
Supported color spaces include RGB, HSI, HSV, Luv, and Lab.
Each channel of the color space is written out as a separate
image.

Acceptable options for outputColorSpace are:  HSI, XYZ, Yuv, YUV, HSV, Lab,
Luv, HSL, CMY, and CMYK.
*/

int itkRGBToOtherColorSpaceTransformationTest( int argc, char * argv[])
{
  if( argc != 3 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " rgbImage outputColorSpace";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  // Set input/output variables.
  std::string outputColorSpace = std::string(argv[2]);

  // Typedefs
  typedef itk::RGBPixel<unsigned char>  RGBPixelType;
  typedef unsigned char                 CharPixelType;
  typedef float                         FloatPixelType;
  typedef signed short                  SShortPixelType;

  typedef itk::Vector<float, 3>               VectorPixelType;
  typedef itk::Image<RGBPixelType, 2>         RGBImageType;
  typedef itk::Image<CharPixelType, 2>        CharImageType;
  typedef itk::Image<FloatPixelType, 2>       FloatImageType;
  typedef itk::ImageFileReader<RGBImageType>  ImageFileReaderType;
  typedef itk::ImageFileWriter<CharImageType> CharImageWriterType;

  // Instantiate reader for input image.
  ImageFileReaderType::Pointer imageReader = ImageFileReaderType::New();
  imageReader->SetFileName( argv[1] );
  imageReader->Update();

  // Set input images from reader.
  RGBImageType::Pointer rgbImage = RGBImageType::New();
  rgbImage = imageReader->GetOutput();

  std::string imageFileName = std::string( "output" );

  if(outputColorSpace == std::string("XYZ"))
    {
    std::cout << "Writing X, Y, and Z channels of XYZ color space." << std::endl;

    // Convert RGB image to XYZ color space
    typedef itk::Accessor::RGBToXYZColorSpacePixelAccessor<unsigned char, float> RGBToXYZColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToXYZColorSpaceAccessorType> RGBToXYZImageAdaptorType;
    RGBToXYZImageAdaptorType::Pointer rgbToXYZAdaptor = RGBToXYZImageAdaptorType::New();
    rgbToXYZAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToXYZImageAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToXYZAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out X component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_XYZ_X.png"));
    writer->Update();

    // Write out Y component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_XYZ_Y.png"));
    writer->Update();

    // Write out Z component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_XYZ_Z.png"));
    writer->Update();
    } // end XYZ
  else if(outputColorSpace == std::string("Yuv"))
    {
    std::cout << "Writing Y, u, and v channels of Yuv color space." << std::endl;

    // Convert RGB image to Yuv color space
    typedef itk::Accessor::RGBToYuvColorSpacePixelAccessor<unsigned char, float> RGBToYuvColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToYuvColorSpaceAccessorType> RGBToYuvImageAdaptorType;
    RGBToYuvImageAdaptorType::Pointer rgbToYuvAdaptor = RGBToYuvImageAdaptorType::New();
    rgbToYuvAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToYuvImageAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToYuvAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out Y component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_Yuv_Y.png"));
    writer->Update();

    // Write out u component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_Yuv_u.png"));
    writer->Update();

    // Write out v component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_Yuv_v.png"));
    writer->Update();
    } // end Yuv
  else if(outputColorSpace == std::string("YUV"))
    {
    std::cout << "Writing Y, U, and V channels of YUV color space." << std::endl;

    // Convert RGB image to Yuv color space
    typedef itk::Accessor::RGBToYUV2ColorSpacePixelAccessor<unsigned char, float> RGBToYUVColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToYUVColorSpaceAccessorType> RGBToYUVImageAdaptorType;
    RGBToYUVImageAdaptorType::Pointer rgbToYUVAdaptor = RGBToYUVImageAdaptorType::New();
    rgbToYUVAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToYUVImageAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToYUVAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out Y component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_YUV2_Y.png"));
    writer->Update();

    // Write out U component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_YUV2_U.png"));
    writer->Update();

    // Write out V component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_YUV2_V.png"));
    writer->Update();
    } // end YUV
  else if(outputColorSpace == std::string("HSI"))
    {
    std::cerr << "Writing H, S, and I channels of HSI color space." << std::endl;

    // Convert RGB image to HSI color space
    typedef itk::Accessor::RGBToHSIColorSpacePixelAccessor<unsigned char, float> RGBToHSIColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToHSIColorSpaceAccessorType> RGBToHSIAdaptorType;
    RGBToHSIAdaptorType::Pointer rgbToHSIAdaptor = RGBToHSIAdaptorType::New();
    rgbToHSIAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToHSIAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToHSIAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out H component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_HSI_H.png"));
    writer->Update();

    // Write out S component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_HSI_S.png"));
    writer->Update();

    // Write out I component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_HSI_I.png"));
    writer->Update();
    } // end HSI
  else if(outputColorSpace == std::string("HSV"))
    {
    std::cerr << "Writing H, S, and V channels of HSV color space." << std::endl;

    // Convert RGB image to HSV color space
    typedef itk::Accessor::RGBToHSVColorSpacePixelAccessor<unsigned char, float> RGBToHSVColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToHSVColorSpaceAccessorType> RGBToHSVAdaptorType;
    RGBToHSVAdaptorType::Pointer rgbToHSVAdaptor = RGBToHSVAdaptorType::New();
    rgbToHSVAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToHSVAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToHSVAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out H component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_HSV_H.png"));
    writer->Update();

    // Write out S component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_HSV_S.png"));
    writer->Update();

    // Write out V component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_HSV_V.png"));
    writer->Update();
    } // end HSV
  else if(outputColorSpace == std::string("Lab"))
    {
    std::cerr << "Writing L, a, and b channels of Lab color space." << std::endl;
    // Convert RGB image to Lab color space
    typedef itk::Accessor::RGBToLabColorSpacePixelAccessor<unsigned char, float> RGBToLabColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToLabColorSpaceAccessorType> RGBToLabAdaptorType;
    RGBToLabAdaptorType::Pointer rgbToLabAdaptor = RGBToLabAdaptorType::New();
    rgbToLabAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToLabAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToLabAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out L component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_Lab_L.png"));
    writer->Update();

    // Write out a component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_Lab_a.png"));
    writer->Update();

    // Write out b component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_Lab_b.png"));
    writer->Update();
    } // end Lab
  else if(outputColorSpace == std::string("Luv"))
    {
    std::cerr << "Writing L, u, and v channels of Lab color space." << std::endl;

    // Convert RGB image to Luv color space
    typedef itk::Accessor::RGBToLuvColorSpacePixelAccessor<unsigned char, float> RGBToLuvColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToLuvColorSpaceAccessorType> RGBToLuvAdaptorType;
    RGBToLuvAdaptorType::Pointer rgbToLuvAdaptor = RGBToLuvAdaptorType::New();
    rgbToLuvAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToLuvAdaptorType, FloatImageType> VectorCastFilterType;
  VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
  vectorCastFilter->SetInput(rgbToLuvAdaptor);
  vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out L component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_Luv_L.png"));
    writer->Update();

    // Write out a component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_Luv_u.png"));
    writer->Update();

    // Write out b component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_Luv_v.png"));
    writer->Update();
    } // end Luv
  else if(outputColorSpace == std::string("HSL"))
    {
    std::cerr << "Writing H, S, and L channels of HSL color space." << std::endl;

    // Convert RGB image to HSL color space
    typedef itk::Accessor::RGBToHSLColorSpacePixelAccessor<unsigned char, float> RGBToHSLColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToHSLColorSpaceAccessorType> RGBToHSLAdaptorType;
    RGBToHSLAdaptorType::Pointer rgbToHSLAdaptor = RGBToHSLAdaptorType::New();
    rgbToHSLAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToHSLAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToHSLAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out H component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_HSL_H.png"));
    writer->Update();

    // Write out S component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_HSL_S.png"));
    writer->Update();

    // Write out L component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_HSL_L.png"));
    writer->Update();
    } // end HSL
  else if(outputColorSpace == std::string("CMY"))
    {
    std::cerr << "Writing C, M, and Y channels of CMY color space." << std::endl;

    // Convert RGB image to CMY color space
    typedef itk::Accessor::RGBToCMYColorSpacePixelAccessor<unsigned char, float> RGBToCMYColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToCMYColorSpaceAccessorType> RGBToCMYAdaptorType;
    RGBToCMYAdaptorType::Pointer rgbToCMYAdaptor = RGBToCMYAdaptorType::New();
    rgbToCMYAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToCMYAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToCMYAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out H component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_CMY_C.png"));
    writer->Update();

    // Write out S component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_CMY_M.png"));
    writer->Update();

    // Write out L component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_CMY_Y.png"));
    writer->Update();
    } // end CMY
  else if(outputColorSpace == std::string("CMYK"))
    {
    std::cout << "Writing C, M, Y, and K channels of CMYK color space." << std::endl;

    // Convert RGB image to CMYK color space
    typedef itk::Accessor::RGBToCMYKColorSpacePixelAccessor<unsigned char, float> RGBToCMYKColorSpaceAccessorType;
    typedef itk::ImageAdaptor<RGBImageType, RGBToCMYKColorSpaceAccessorType> RGBToCMYKAdaptorType;
    RGBToCMYKAdaptorType::Pointer rgbToCMYKAdaptor = RGBToCMYKAdaptorType::New();
    rgbToCMYKAdaptor->SetImage(rgbImage);

    typedef itk::VectorIndexSelectionCastImageFilter<RGBToCMYKAdaptorType, FloatImageType> VectorCastFilterType;
    VectorCastFilterType::Pointer vectorCastFilter = VectorCastFilterType::New();
    vectorCastFilter->SetInput(rgbToCMYKAdaptor);
    vectorCastFilter->SetIndex(0);

    // Rescale float image to char image.
    typedef itk::RescaleIntensityImageFilter<FloatImageType, CharImageType> FloatRescalerType;
    FloatRescalerType::Pointer floatRescaler = FloatRescalerType::New();
    floatRescaler->SetOutputMinimum(0);
    floatRescaler->SetOutputMaximum(255);
    floatRescaler->SetInput(vectorCastFilter->GetOutput());

    // Write out C component
    CharImageWriterType::Pointer writer = CharImageWriterType::New();
    writer->SetInput(floatRescaler->GetOutput());
    writer->SetFileName(imageFileName + std::string("_CMYK_C.png"));
    writer->Update();

    // Write out M component
    vectorCastFilter->SetIndex(1);
    writer->SetFileName(imageFileName + std::string("_CMYK_M.png"));
    writer->Update();

    // Write out Y component
    vectorCastFilter->SetIndex(2);
    writer->SetFileName(imageFileName + std::string("_CMYK_Y.png"));
    writer->Update();

    // Write out K component
    vectorCastFilter->SetIndex(3);
    writer->SetFileName(imageFileName + std::string("_CMYK_K.png"));
    writer->Update();
    } // end CMYK
  else
    {
    std::cerr << "Oops, unrecognized color space. " << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
