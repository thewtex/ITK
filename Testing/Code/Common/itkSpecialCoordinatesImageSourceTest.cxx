#include <iostream>
#include "itkAffineTransform.h"
#include "itkGaussianImageSource.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkSpecialCoordinatesImage.h"
#include "itkSpecialCoordinatesImageSource.h"

int itkSpecialCoordinatesImageSourceTest(int, char* argv[])
{
  const unsigned int Dim = 2;
  typedef double PixelType;
  typedef itk::AffineTransform< double, Dim > TransformType;
  typedef itk::SpecialCoordinatesImage< PixelType, Dim, TransformType > ImageType;
  typedef itk::GaussianImageSource< ImageType > GaussianSourceBaseType;
  typedef itk::SpecialCoordinatesImageSource< GaussianSourceBaseType >
    GaussianSourceType;

  GaussianSourceBaseType::SizeType size = {{256, 256}};

  GaussianSourceType::Pointer filter = GaussianSourceType::New();
  filter->SetSize(size);
  double gaussianScale = 255.0;
  filter->SetScale(gaussianScale);

  GaussianSourceBaseType::ArrayType mean; mean.Fill(127.0);
  mean.Fill(0.0);
  filter->SetMean(mean);

  GaussianSourceBaseType::ArrayType sigma; sigma.Fill(45.0);
  filter->SetSigma(sigma);

  // Set the index transform
  TransformType::Pointer transform = TransformType::New();

  TransformType::OutputVectorType scale;
  scale[0] = 1.0; scale[1] = 2.0; scale[2] = 0.5;
  //transform->Scale(scale);

  TransformType::OutputVectorType translation;
  translation[0] = -50; translation[1] = 24;
  transform->SetTranslation(translation);

  transform->Shear(0, 1, 0.2);

  filter->SetIndexToPhysicalPointTransform(transform);
  filter->Update();

  ImageType::RegionType testRegion = filter->GetOutput()->GetLargestPossibleRegion();
  itk::ImageRegionConstIteratorWithIndex< ImageType > iterator(filter->GetOutput(), testRegion);

  iterator.GoToBegin();
  while ( !iterator.IsAtEnd() )
    {
    // Check the value in the image
    ImageType::IndexType index = iterator.GetIndex();
    ImageType::PixelType imageValue = filter->GetOutput()->GetPixel(index);

    // Now map the index to physical space via the transform and
    // evaluate the Gaussian function
    ImageType::PointType indexPoint;
    for ( unsigned int i = 0; i < 2; i++ ) indexPoint[i] = index[i];

    ImageType::PointType physicalPoint = transform->TransformPoint( indexPoint );

    typedef itk::GaussianSpatialFunction< ImageType::PixelType, 2 > GaussianFunctionType;
    GaussianFunctionType::Pointer gaussianFunction = GaussianFunctionType::New();
    gaussianFunction->SetMean(filter->GetMean());
    gaussianFunction->SetSigma(filter->GetSigma());
    gaussianFunction->SetScale(filter->GetScale());
    gaussianFunction->SetNormalized(filter->GetNormalized());

    double expectedValue = gaussianFunction->Evaluate( physicalPoint );
    if ( fabs(expectedValue - imageValue) > 1e-7 )
      {
      std::cerr << "Image value " << imageValue << " at index " << index
                << " does not match expected value " << expectedValue
                << std::endl;
      return EXIT_FAILURE;
      }

    ++iterator;
    }

  return EXIT_SUCCESS;
}
