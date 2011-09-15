#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkHighLevelFeatureMatchingFilter.h"

int itkHighLevelFeatureMatchingFilter(int argc, char *argv[])
{
  typedef itk::Image<unsigned char, 2>   ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader1 = ReaderType::New();

  reader1->SetFileName(argv[1]);
  reader1->Update();
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(argv[2]);

  typedef itk::HighLevelFeatureMatchingFilter<ImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInputImage1( reader1->GetOutput() );
  filter->SetInputImage2( reader2->GetOutput() );

  filter->Update();
  return EXIT_SUCCESS;
}
