#include "itkHDF5ImageIO.h"
#include "itkHDF5ImageIOFactory.h"
#include "itkRGBPixel.h"
#include "itkIOTestHelper.h"

template <typename TPixel>
int RWTest(const char *fileName)
{
  int success(EXIT_SUCCESS);
  typedef typename itk::Image<TPixel,3> ImageType;
  typename ImageType::RegionType imageRegion;
  typename ImageType::SizeType size;
  typename ImageType::IndexType index;
  typename ImageType::SpacingType spacing;
  typename ImageType::PointType origin;
  typename ImageType::DirectionType myDirection;
  for(unsigned i = 0; i < 3; i++)
    {
    size[i] = 5;
    index[i] = 0;
    spacing[i] = 1.0;
    origin[i] = 0;
    }
  imageRegion.SetSize(size);
  imageRegion.SetIndex(index);
  typename ImageType::Pointer im;
  AllocateImageFromRegionAndSpacing(ImageType,im,imageRegion,spacing);
  vnl_random randgen(12345678);
  itk::ImageRegionIterator<ImageType> it(im,im->GetLargestPossibleRegion());
  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    TPixel pix;
    RandomPix(randgen,pix);
    it.Set(pix);
    }
  typename ImageType::Pointer im2;
  try
    {
    WriteImage<ImageType,itk::HDF5ImageIO>(im,std::string(fileName));
    im2 = ReadImage<ImageType>(std::string(fileName));
    }
  catch(itk::ExceptionObject &err)
    {
    std::cout << "itkHDF5ImageIOTest9" << std::endl
              << "Exception Object caught: " << std::endl
              << err << std::endl;
    return EXIT_FAILURE;
    }
  itk::ImageRegionIterator<ImageType> it2(im2,im2->GetLargestPossibleRegion());
  for(it.GoToBegin(),it2.GoToBegin(); !it.IsAtEnd() && !it2.IsAtEnd(); ++it,++it2)
    {
    if(it.Value() != it2.Value())
      {
      std::cout << "Original Pixel (" << it.Value()
                << ") doesn't match read-in Pixel ("
                << it2.Value() << std::endl;
      success = EXIT_FAILURE;
      break;
      }
    }
  // Remove(fileName);
  return success;
}

int
itkHDF5ImageIOTest(int ac, char **av)
{
  std::string prefix("");
  if(ac > 1)
    {
    prefix = *++av;
    --ac;
    itksys::SystemTools::ChangeDirectory(prefix.c_str());
    }
  itk::ObjectFactoryBase::RegisterFactory(itk::HDF5ImageIOFactory::New() );

  int result(0);
  result += RWTest<unsigned char>("UCharImage.hdf5");
  result += RWTest<float>("FloatImage.hdf5");
  result += RWTest<itk::RGBPixel<unsigned char> >("RGBImage.hdf5");
  return result != 0;
}
