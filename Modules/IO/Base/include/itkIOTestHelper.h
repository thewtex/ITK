#ifndef __itkIOTestHelper_h
#define __itkIOTestHelper_h
#include <string>

#include "itksys/SystemTools.hxx"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "vnl/vnl_random.h"

template <typename TImage>
typename TImage::Pointer ReadImage( const std::string &fileName,
                                    const bool zeroOrigin = false )
{
  typedef itk::ImageFileReader<TImage> ReaderType;

  typename ReaderType::Pointer reader = ReaderType::New();
  {
  reader->SetFileName( fileName.c_str() );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "Caught an exception: " << std::endl;
    std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
    throw err;
    }
  catch(...)
    {
    std::cout << "Error while reading in image for patient " << fileName << std::endl;
    throw;
    }
  }
  typename TImage::Pointer image = reader->GetOutput();
  if(zeroOrigin)
    {
    double origin[TImage::ImageDimension];
    for(unsigned int i = 0; i < TImage::ImageDimension; i++)
      {
      origin[i]=0;
      }
    image->SetOrigin(origin);
    }
  return image;
}

template <class ImageType,class ImageIOType>
void
WriteImage(typename ImageType::Pointer &image ,
           const std::string &filename)
{

  typedef itk::ImageFileWriter< ImageType > WriterType;
  typename  WriterType::Pointer writer = WriterType::New();

  typename ImageIOType::Pointer imageio(ImageIOType::New());

  writer->SetImageIO(imageio);

  writer->SetFileName(filename.c_str());

  writer->SetInput(image);

  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject &err) {
  std::cout << "Exception Object caught: " << std::endl;
  std::cout << err << std::endl;
  throw;
  }
}

//
// generate random pixels of various types
inline void
RandomPix(vnl_random &randgen,itk::RGBPixel<unsigned char> &pix)
{
  for(unsigned int i = 0; i < 3; i++)
    {
    pix[i] = randgen.lrand32(itk::NumericTraits<unsigned char>::max());
    }
}

template <typename TPixel>
void
RandomPix(vnl_random &randgen, TPixel &pix)
{
  pix = randgen.lrand32(itk::NumericTraits<TPixel>::max());
}

inline void
RandomPix(vnl_random &randgen, double &pix)
{
  pix = randgen.drand64(itk::NumericTraits<double>::max());
}

inline void
RandomPix(vnl_random &randgen, float &pix)
{
  pix = randgen.drand64(itk::NumericTraits<float>::max());
}

static inline int Remove(const char *fname)
{
  return itksys::SystemTools::RemoveFile(fname);
}

template <class ImageType>
void SetIdentityDirection(typename ImageType::Pointer &im)
{
  typename ImageType::DirectionType dir;
  dir.SetIdentity();
  im->SetDirection(dir);
}

#define AllocateImageFromRegionAndSpacing(ImageType,rval,region,spacing) \
{ \
  rval = ImageType::New(); \
  SetIdentityDirection<ImageType>(rval);       \
  rval->SetSpacing(spacing); \
  rval->SetRegions(region); \
  rval->Allocate(); \
}

#define AllocateVecImageFromRegionAndSpacing(ImageType,rval,region,spacing,vecLength) \
{ \
  rval = ImageType::New(); \
  rval->SetSpacing(spacing); \
  rval->SetRegions(region); \
  rval->SetVectorLength(vecLength); \
  rval->Allocate(); \
}

#endif // __itkIOTestHelper_h
