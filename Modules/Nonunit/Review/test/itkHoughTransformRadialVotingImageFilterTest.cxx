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

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkHoughTransformRadialVotingImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkThresholdImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include <itkGradientMagnitudeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkLabelOverlayImageFilter.h"
#include <list>
#include "itkCastImageFilter.h"
#include "vnl/vnl_math.h"

int itkHoughTransformRadialVotingImageFilterTest( int argc, char *argv[] )
{
  if( argc < 16 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0] << std::endl;
    std::cerr << " inputImage " << std::endl;
    std::cerr << " outputImage" << std::endl;
    std::cerr << " accumulatorImage" << std::endl;
    std::cerr << " numberOfSpheres " << std::endl;
    std::cerr << " radius Min " << std::endl;
    std::cerr << " radius Max " << std::endl;
    std::cerr << " SigmaGradient (default = 1) " << std::endl;
    std::cerr << " variance of the accumulator blurring (default = 1) " << std::endl;
    std::cerr << " radius ratio of the disk to remove from the accumulator (default = 1) "<< std::endl;
    std::cerr << " voting radius ratio (default = 0.5) "<< std::endl;
    std::cerr << " input threshold "<< std::endl;
    std::cerr << " output threshold "<< std::endl;
    std::cerr << " gradient threshold "<< std::endl;
    std::cerr << " number of threads "<< std::endl;
    std::cerr << " sampling ratio "<< std::endl;
    return 1;
    }

  const unsigned int Dimension = 2;
  typedef  unsigned char   InputPixelType;
  typedef  float           InternalPixelType;
  typedef  unsigned char   OutputPixelType;

  typedef itk::Image< InputPixelType, Dimension >    InputImageType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

   InputImageType::IndexType   localIndex;
   InputImageType::SpacingType spacing;

  typedef itk::ImageFileReader< InputImageType > ReaderType;
   ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
   InputImageType::Pointer localImage = reader->GetOutput();
  spacing = localImage->GetSpacing();
  std::cout << "Computing Hough Map" << std::endl;

  typedef itk::HoughTransformRadialVotingImageFilter< InputImageType,
               InternalImageType > HoughTransformFilterType;
   HoughTransformFilterType::Pointer houghFilter =
    HoughTransformFilterType::New();
  houghFilter->SetInput( reader->GetOutput() );
  houghFilter->SetNumberOfSpheres( atoi(argv[4]) );
  houghFilter->SetMinimumRadius(   atof(argv[5]) );
  houghFilter->SetMaximumRadius(   atof(argv[6]) );

  if( argc > 7 )
    {
    houghFilter->SetSigmaGradient( atof(argv[7]) );
    }
  if( argc > 8 )
    {
    houghFilter->SetVariance( atof(argv[8]) );
    }
  if( argc > 9 )
    {
    houghFilter->SetSphereRadiusRatio( atof(argv[9]) );
    }
  if( argc > 10 )
    {
    houghFilter->SetVotingRadiusRatio( atof(argv[10]) );
    }
  if( argc > 11 )
    {
    houghFilter->SetThreshold( atof(argv[11]) );
    }
  if( argc > 12 )
    {
    houghFilter->SetOutputThreshold( atof(argv[12]) );
    }
  if( argc > 13 )
    {
    houghFilter->SetGradientThreshold( atof(argv[13]) );
    }
  if( argc > 14 )
    {
    houghFilter->SetNbOfThreads( atoi(argv[14]) );
    }
  if( argc > 15 )
    {
    houghFilter->SetSamplingRatio( atof(argv[15]) );
    }

  try
    {
    houghFilter->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

   InternalImageType::Pointer localAccumulator =
    houghFilter->GetOutput();

   HoughTransformFilterType::SpheresListType circles;
  circles = houghFilter->GetSpheres( );

  std::cout << "Found " << circles.size() << " circle(s)." << std::endl;


  // Computing the circles output
   OutputImageType::Pointer  localOutputImage =
    OutputImageType::New();

   OutputImageType::RegionType region;
  region.SetSize( localImage->GetLargestPossibleRegion().GetSize() );
  region.SetIndex( localImage->GetLargestPossibleRegion().GetIndex() );
  localOutputImage->SetRegions( region );
  localOutputImage->SetOrigin(localImage->GetOrigin());
  localOutputImage->SetSpacing(localImage->GetSpacing());
  localOutputImage->Allocate();
  localOutputImage->FillBuffer(0);

  typedef  HoughTransformFilterType::SpheresListType SpheresListType;
   SpheresListType::const_iterator itSpheres = circles.begin();

  unsigned int count = 1;
  while( itSpheres != circles.end() )
    {
    std::cout << "Center: ";
    std::cout << (*itSpheres)->GetObjectToParentTransform()->GetOffset()
              << std::endl;
    std::cout << "Radius: " << (*itSpheres)->GetRadius()[0] << std::endl;

    for(double angle = 0;angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0 )
      {
      localIndex[0] =
         (long int)((*itSpheres)->GetObjectToParentTransform()->GetOffset()[0]
             + ( (*itSpheres)->GetRadius()[0]*vcl_cos(angle) )/spacing[0] );
      localIndex[1] =
         (long int)((*itSpheres)->GetObjectToParentTransform()->GetOffset()[1]
             + ( (*itSpheres)->GetRadius()[1]*vcl_sin(angle) )/spacing[1] );
       OutputImageType::RegionType outputRegion =
                                  localOutputImage->GetLargestPossibleRegion();

      if( outputRegion.IsInside( localIndex ) )
        {
        localOutputImage->SetPixel( localIndex, count );
        }
      }
    ++itSpheres;
    count++;
    }

  int radius = 2;
  typedef itk::BinaryBallStructuringElement< OutputPixelType, Dimension >
    SEType;
  SEType sE;
  sE.SetRadius ( radius );
  sE.CreateStructuringElement();

  typedef itk::GrayscaleDilateImageFilter< OutputImageType, OutputImageType, SEType >
    DilateFilterType;
   DilateFilterType::Pointer grayscaleDilate = DilateFilterType::New();
  grayscaleDilate->SetKernel ( sE );
  grayscaleDilate->SetInput ( localOutputImage );

  try
    {
    grayscaleDilate->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::RGBPixel< unsigned char >        RGBPixelType;
  typedef itk::Image< RGBPixelType, Dimension > RGBImageType;

  typedef itk::LabelOverlayImageFilter< InputImageType, OutputImageType, RGBImageType > OverlayType;
   OverlayType::Pointer overlay = OverlayType::New();
  overlay->SetInput( localImage );
  overlay->SetLabelImage( grayscaleDilate->GetOutput() );

  typedef itk::ImageFileWriter< RGBImageType > RGBWriterType;
   RGBWriterType::Pointer rgbwriter = RGBWriterType::New();
  rgbwriter->SetInput( overlay->GetOutput() );
  rgbwriter->SetFileName( argv[2] );

  try
    {
    rgbwriter->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  typedef  itk::ImageFileWriter< InternalImageType  > InputWriterType;
   InputWriterType::Pointer writer2 = InputWriterType::New();
  writer2->SetFileName( argv[3] );
  writer2->SetInput( localAccumulator );

  try
    {
    writer2->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
