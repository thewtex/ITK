/* ----------------------------- MNI Header -----------------------------------
@NAME       : itMINCImageIOTest.cxx
@DESCRIPTION: MINC IO  test based on itkHDF5ImageIOTest.cxx
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#include "itkArray.h"
#include "itkVectorImage.h"
#include "itkMincImageIO.h"
#include "itkMincImageIOFactory.h"
#include "itkIOTestHelper.h"
#include "itkMetaDataObject.h"
#include "itkObjectFactoryBase.h"
#include "itkMincHelpers.h"

static void RandomPix(vnl_random &randgen,itk::RGBPixel<unsigned char> &pix,double _max=itk::NumericTraits<unsigned char>::max())
{
  for(unsigned int i = 0; i < 3; i++)
  {
    pix[i] = randgen.lrand32(_max);
  }
}

static double abs_diff(const itk::RGBPixel<unsigned char> &pix1,const itk::RGBPixel<unsigned char> &pix2)
{
  double diff=0.0;
  for(int i=0;i<3;i++)
    diff += fabs(pix1[i]-pix2[i]);
  return diff;
}

static void RandomPix(vnl_random &randgen, itk::Vector<float,3> &pix,float _max=itk::NumericTraits<float>::max())
{
  pix[0]=randgen.drand64(_max);
  pix[1]=randgen.drand64(_max);
  pix[2]=randgen.drand64(_max);
}


static double abs_diff(const itk::Vector<float> &pix1,const itk::Vector<float> &pix2)
{
  double diff=0.0;
  for(int i=0;i<3;i++)
    diff += fabs(pix1[i]-pix2[i]);
  return diff;
}

static void RandomPix(vnl_random &randgen, double &pix,double _max=itk::NumericTraits<double>::max())
{
  pix = randgen.drand64(_max);
}

static void RandomPix(vnl_random &randgen, float &pix,float _max=itk::NumericTraits<float>::max())
{
   pix = randgen.drand64(_max);
}

template <typename TPixel>
static double abs_diff(const TPixel &pix1,const TPixel &pix2)
{
  return fabs((double)(pix1-pix2));
}

template <typename TPixel>
static void RandomVectorPix(vnl_random &randgen, itk::VariableLengthVector<TPixel> &pix,double _max=itk::NumericTraits<TPixel>::max())
{
  for(size_t i=0;i<pix.GetSize();i++)
    pix.SetElement(i,randgen.drand64(_max));
}

template <typename TPixel>
static void RandomPix(vnl_random &randgen, TPixel &pix,double _max=itk::NumericTraits<TPixel>::max())
{
  pix = randgen.lrand32((TPixel)_max);
}


template <typename TPixel>
static bool equal(const itk::VariableLengthVector<TPixel> &pix1,const itk::VariableLengthVector<TPixel> &pix2)
{
  for(size_t i=0;i<pix1.GetSize();i++)
    if( pix1[i]!=pix2[i] ) return false;
  return true;
}


template <typename TPixel>
static double abs_vector_diff(const itk::VariableLengthVector<TPixel> &pix1,const itk::VariableLengthVector<TPixel> &pix2)
{
  double diff=0.0;
  for(size_t i=0;i<pix1.GetSize();i++)
  {
    double d=fabs(pix1[i]-pix2[i]);
    if(d>diff) diff=d;
  }
  return diff;
}

template <typename TPixel>
static double eql_vector_diff(const itk::Point<TPixel,3> &v1,const itk::Point<TPixel,3> &v2)
{
  double diff=0.0;
  for(size_t i=0;i<3;i++)
    diff += (v1[i]-v2[i])*(v1[i]-v2[i]);
  return sqrt(diff);
}


template <typename TPixel,int NDIM> int MINCReadWriteTest(const char *fileName,nc_type minc_storage_type,double tolerance=0.0)
{
  int success(EXIT_SUCCESS);

  typedef typename itk::Image<TPixel,NDIM> ImageType;

  typename ImageType::SizeType size;
  typename ImageType::IndexType index;
  typename ImageType::SpacingType spacing;
  typename ImageType::PointType origin;
  typename ImageType::DirectionType myDirection;

  for(unsigned i = 0; i < NDIM; i++)
  {
    size[i] = 5;
    index[i] = 0;
    spacing[i] = 1.0 + static_cast<double>(i);
    origin[i] = static_cast<double>(i) * 5.0;
  }

  typename ImageType::Pointer im = ImageType::New();

  typename ImageType::RegionType  region;
  region.SetSize  (size);
  region.SetIndex (index);
  im->SetLargestPossibleRegion (region);
  im->SetBufferedRegion (region);
  im->SetRequestedRegion (region);
  im->SetSpacing( spacing );
  im->SetOrigin( origin );
  im->Allocate ();

  itk::Matrix<double,NDIM,NDIM> mat;

  mat.SetIdentity();

  if(NDIM == 3) { //there are problems with 4D direction cosines!
    // 30deg rotation
    mat[1][1] =
    mat[0][0] = 0.866025403784439;
    mat[0][1] = -0.5;
    mat[1][0] = 0.5;
    im->SetDirection(mat);
  }
  //
  // add some unique metadata
  itk::MetaDataDictionary & metaDict(im->GetMetaDataDictionary());

  std::vector<double> metaDataDoubleArray(5);
  metaDataDoubleArray[0] = 3.1;
  metaDataDoubleArray[1] = 1.2;
  metaDataDoubleArray[2] = 4.3;
  metaDataDoubleArray[3] = 5.4;
  metaDataDoubleArray[4] = 2.5;
  itk::EncapsulateMetaData<std::vector<double> >(metaDict,"acquisition:TestDoubleArray",metaDataDoubleArray);

  std::vector<int> metaDataIntArray(5);
  metaDataIntArray[0] = 3;
  metaDataIntArray[1] = 1;
  metaDataIntArray[2] = 4;
  metaDataIntArray[3] = 5;
  metaDataIntArray[4] = 2;
  itk::EncapsulateMetaData<std::vector<int> >(metaDict,"acquisition:TestIntArray",metaDataIntArray);

  std::string metaDataStdString("Test std::string");
  itk::EncapsulateMetaData<std::string>(metaDict,"acquisition:StdString",metaDataStdString);

  //
  // fill image buffer
  vnl_random randgen(12345678);
  itk::ImageRegionIterator<ImageType> it(im,im->GetLargestPossibleRegion());

  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    TPixel pix;
    if(tolerance>0.0)
      RandomPix(randgen,pix,100);
    else
      RandomPix(randgen,pix);
    it.Set(pix);
  }

  // set minc file storage type
  itk::set_minc_storage_type(im,minc_storage_type,minc_storage_type!=NC_BYTE);

  typename ImageType::Pointer im2;
  try
  {
    itk::IOTestHelper::WriteImage<ImageType,itk::MincImageIO>(im,std::string(fileName));
    im2 = itk::IOTestHelper::ReadImage<ImageType>(std::string(fileName));
  }
  catch(itk::ExceptionObject &err)
  {
    std::cout << "itkMINCImageIOTest" << std::endl
              << "Exception Object caught: " << std::endl
              << err << " in "<< fileName<< std::endl;
    return EXIT_FAILURE;
  }

  if(eql_vector_diff(im->GetOrigin(),im2->GetOrigin())>1e-6)
  {
    std::cout << "Origin read "
              << im2->GetOrigin() << " doesn't match origin written"
              << im->GetOrigin()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  if(im->GetSpacing() != im2->GetSpacing())
  {
    std::cout << "Spacing read "
              << im2->GetSpacing() << " doesn't match spacing written"
              << im->GetSpacing()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  if(im->GetDirection() != im2->GetDirection())
  {
    std::cout << "Direction read "
              << im2->GetDirection() << " doesn't match direction written"
              << im->GetDirection()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  //
  // Check MetaData
  itk::MetaDataDictionary & metaDict2(im2->GetMetaDataDictionary());

  std::vector<double> metaDataDoubleArray2;
  if(!itk::ExposeMetaData<std::vector<double> >(metaDict2,"acquisition:TestDoubleArray",
                                          metaDataDoubleArray2) ||
      metaDataDoubleArray2 != metaDataDoubleArray)
  {
    std::cerr << "Failure reading metaData " << "TestDoubleArray " <<  std::endl;
    std::cerr << "metaDataDoubleArray=";
    for(size_t i=0;i<metaDataDoubleArray.size();i++)
      std::cerr << metaDataDoubleArray[i]<<" ";
    std::cerr<<std::endl;

    std::cerr << "metaDataDoubleArray2=";
    for(size_t i=0;i<metaDataDoubleArray2.size();i++)
      std::cerr << metaDataDoubleArray2[i]<<" ";
    std::cerr<<std::endl;

    success = EXIT_FAILURE;
  }

  std::vector<int> metaDataIntArray2;
  if(!itk::ExposeMetaData<std::vector<int> >(metaDict2,"acquisition:TestIntArray",
                                          metaDataIntArray2) ||
  metaDataIntArray2 != metaDataIntArray)
  {
    std::cerr << "Failure reading metaData " << "TestIntArray " <<  std::endl;
    success = EXIT_FAILURE;
  }

   std::string metaDataStdString2("");
   if(!itk::ExposeMetaData<std::string>(metaDict2,"acquisition:StdString",metaDataStdString2) ||
      metaDataStdString2 != metaDataStdString)
  {
     std::cerr << "Failure reading metaData " << "StdString "
               << metaDataStdString2 << " " << metaDataStdString
               <<  std::endl;
     success = EXIT_FAILURE;
   }

  itk::ImageRegionIterator<ImageType> it2(im2,im2->GetLargestPossibleRegion());
  if(tolerance==0.0)
  {
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
  } else { //account for rounding errors
    for(it.GoToBegin(),it2.GoToBegin(); !it.IsAtEnd() && !it2.IsAtEnd(); ++it,++it2)
    {
      if(abs_diff(it.Value(),it2.Value())>tolerance)
      {
        std::cout << "Original Pixel (" << it.Value()
                  << ") doesn't match read-in Pixel ("
                  << it2.Value()
                  << " in "<< fileName <<std::endl;
        success = EXIT_FAILURE;
        break;
      }
    }
  }
  // Remove(fileName);
  return success;
}


template <typename TPixel,int NDIM> int MINCReadWriteTestVector(const char *fileName,size_t vector_length,nc_type minc_storage_type,double tolerance=0.0)
{
  int success(EXIT_SUCCESS);

  typedef typename itk::VectorImage<TPixel,NDIM>            ImageType;
  typedef typename itk::VectorImage<TPixel,NDIM>::PixelType InternalPixelType;

  typename ImageType::SizeType size;
  typename ImageType::IndexType index;
  typename ImageType::SpacingType spacing;
  typename ImageType::PointType origin;
  typename ImageType::DirectionType myDirection;

  for(unsigned i = 0; i < NDIM; i++)
  {
    size[i] = 5;
    index[i] = 0;
    spacing[i] = 1.0 + static_cast<double>(i);
    origin[i] = static_cast<double>(i) * 5.0;
  }
  typename ImageType::Pointer im = ImageType::New();

  typename ImageType::RegionType  region;
  region.SetSize  (size);
  region.SetIndex (index);
  im->SetLargestPossibleRegion (region);
  im->SetBufferedRegion (region);
  im->SetRequestedRegion (region);
  im->SetSpacing( spacing );
  im->SetOrigin( origin );
  im->SetVectorLength(vector_length);
  im->Allocate ();

  itk::Matrix<double,NDIM,NDIM> mat;

  mat.SetIdentity();

  if(NDIM == 3) { //there are problems with 4D direction cosines!
    // 30deg rotation
    mat[1][1] =
    mat[0][0] = 0.866025403784439;
    mat[0][1] = -0.5;
    mat[1][0] = 0.5;
    im->SetDirection(mat);
  }
  //
  // add some unique metadata
  itk::MetaDataDictionary & metaDict(im->GetMetaDataDictionary());

  std::vector<double> metaDataDoubleArray(5);
  metaDataDoubleArray[0] = 3.1;
  metaDataDoubleArray[1] = 1.2;
  metaDataDoubleArray[2] = 4.3;
  metaDataDoubleArray[3] = 5.4;
  metaDataDoubleArray[4] = 2.5;
  itk::EncapsulateMetaData<std::vector<double> >(metaDict,"acquisition:TestDoubleArray",metaDataDoubleArray);

  std::vector<int> metaDataIntArray(5);
  metaDataIntArray[0] = 3;
  metaDataIntArray[1] = 1;
  metaDataIntArray[2] = 4;
  metaDataIntArray[3] = 5;
  metaDataIntArray[4] = 2;
  itk::EncapsulateMetaData<std::vector<int> >(metaDict,"acquisition:TestIntArray",metaDataIntArray);

  std::string metaDataStdString("Test std::string");
  itk::EncapsulateMetaData<std::string>(metaDict,"acquisition:StdString",metaDataStdString);
  //
  // fill image buffer
  vnl_random randgen(12345678);
  itk::ImageRegionIterator<ImageType> it(im,im->GetLargestPossibleRegion());

  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
  {
    InternalPixelType pix(vector_length);
    if(tolerance>0.0)
      RandomVectorPix<TPixel>(randgen,pix,100.0);
    else
      RandomVectorPix<TPixel>(randgen,pix);
    it.Set(pix);
  }

  // set minc file storage type
  itk::set_minc_storage_type(im,minc_storage_type,minc_storage_type!=NC_BYTE);

  typename ImageType::Pointer im2;

  try
  {
    itk::IOTestHelper::WriteImage<ImageType,itk::MincImageIO>(im,std::string(fileName));
    im2 = itk::IOTestHelper::ReadImage<ImageType>(std::string(fileName));
  }
  catch(itk::ExceptionObject &err)
  {
    std::cout << "itkMINCImageIOTest" << std::endl
              << "Exception Object caught: " << std::endl
              << err << " in "<< fileName<< std::endl;
    return EXIT_FAILURE;
  }

  if(eql_vector_diff(im->GetOrigin(),im2->GetOrigin())>1e-6) //account for rounding errors
  {
    std::cout << "Origin read "
              << im2->GetOrigin() << " doesn't match origin written"
              << im->GetOrigin()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  if(im->GetSpacing() != im2->GetSpacing())
  {
    std::cout << "Spacing read "
              << im2->GetSpacing() << " doesn't match spacing written"
              << im->GetSpacing()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  if(im->GetDirection() != im2->GetDirection())
  {
    std::cout << "Direction read "
              << im2->GetDirection() << " doesn't match direction written"
              << im->GetDirection()<< " in "<< fileName << std::endl;
    return EXIT_FAILURE;
  }
  //
  // Check MetaData
  itk::MetaDataDictionary & metaDict2(im2->GetMetaDataDictionary());

  std::vector<double> metaDataDoubleArray2;
  if(!itk::ExposeMetaData<std::vector<double> >(metaDict2,"acquisition:TestDoubleArray",
                                          metaDataDoubleArray2) ||
  metaDataDoubleArray2 != metaDataDoubleArray)
  {
    std::cerr << "Failure reading metaData " << "TestDoubleArray " <<  std::endl;
    success = EXIT_FAILURE;
  }

  std::vector<int> metaDataIntArray2;
  if(!itk::ExposeMetaData<std::vector<int> >(metaDict2,"acquisition:TestIntArray",
                                          metaDataIntArray2) ||
  metaDataIntArray2 != metaDataIntArray)
  {
    std::cerr << "Failure reading metaData " << "TestIntArray " <<  std::endl;
    success = EXIT_FAILURE;
  }

   std::string metaDataStdString2("");
   if(!itk::ExposeMetaData<std::string>(metaDict2,"acquisition:StdString",metaDataStdString2) ||
      metaDataStdString2 != metaDataStdString)
  {
     std::cerr << "Failure reading metaData " << "StdString "
               << metaDataStdString2 << " " << metaDataStdString
               <<  std::endl;
     success = EXIT_FAILURE;
   }

  itk::ImageRegionIterator<ImageType> it2(im2,im2->GetLargestPossibleRegion());
  InternalPixelType pix1,pix2;
  if(tolerance==0.0)
  {
    for(it.GoToBegin(),it2.GoToBegin(); !it.IsAtEnd() && !it2.IsAtEnd(); ++it,++it2)
    {
      pix1=it.Get();
      pix2=it2.Get();
      if( !equal<TPixel>(pix1,pix2) )
      {
        std::cout << "Original Pixel (" << pix1
                  << ") doesn't match read-in Pixel ("
                  << pix2 << std::endl;
        success = EXIT_FAILURE;
        break;
      }
    }
  } else { //account for rounding errors
    for(it.GoToBegin(),it2.GoToBegin(); !it.IsAtEnd() && !it2.IsAtEnd(); ++it,++it2)
    {
      pix1=it.Get();
      pix2=it2.Get();
      if(abs_vector_diff<TPixel>(pix1,pix2)>tolerance)
      {
        std::cout << "Original Pixel (" << pix1
                  << ") doesn't match read-in Pixel ("
                  << pix2
                  << " in "<< fileName <<std::endl;
        success = EXIT_FAILURE;
        break;
      }
    }
  }
  // Remove(fileName);
  return success;
}


int itkMINCImageIOTest(int ac, char * av [] )
{
  std::string prefix("");
  if(ac > 1)
  {
    prefix = *++av;
    --ac;
    itksys::SystemTools::ChangeDirectory(prefix.c_str());
  }

  itk::ObjectFactoryBase::RegisterFactory(itk::MincImageIOFactory::New() ,itk::ObjectFactoryBase::INSERT_AT_FRONT);

  int result(0);
  // stright forward test
  result += MINCReadWriteTest<unsigned char,3>("3DUCharImage.mnc",NC_BYTE);
  result += MINCReadWriteTest<float,3>("3DFloatImage.mnc",NC_FLOAT);
  result += MINCReadWriteTest<double,3>("3DDoubleImage.mnc",NC_DOUBLE);
  result += MINCReadWriteTest<itk::RGBPixel<unsigned char>,3 >("3DRGBImage.mnc",NC_BYTE);
  result += MINCReadWriteTest<itk::Vector<float,3>,3 >("3DVectorImage.mnc",NC_FLOAT);

  // expecting rounding errors
  result += MINCReadWriteTest<float,3>("3DFloatImage_byte.mnc",NC_BYTE,0.2);
  result += MINCReadWriteTest<float,3>("3DFloatImage_short.mnc",NC_SHORT,0.01);

  result += MINCReadWriteTest<double,3>("3DDoubleImage_byte.mnc",NC_BYTE,0.2);
  result += MINCReadWriteTest<double,3>("3DDoubleImage_short.mnc",NC_SHORT,0.01);

  result += MINCReadWriteTest<itk::Vector<float,3>,3 >("3DVectorImage_byte.mnc",NC_BYTE,0.5);
  result += MINCReadWriteTest<itk::Vector<float,3>,3 >("3DVectorImage_short.mnc",NC_SHORT,0.05);

  //testing variable vector case
  // stright forward test
  result += MINCReadWriteTestVector<unsigned char,3>("4DUCharImage.mnc",10,NC_BYTE,0.0001);
  result += MINCReadWriteTestVector<float,3>("4DFloatImage.mnc",10,NC_FLOAT,0.0001);
  result += MINCReadWriteTestVector<double,3>("4DDoubleImage.mnc",10,NC_DOUBLE,0.0001);

  // expecting rounding errors
  result += MINCReadWriteTestVector<float,3>("4DFloatImage_byte.mnc",10,NC_BYTE,0.2);
  result += MINCReadWriteTestVector<float,3>("4DFloatImage_short.mnc",10,NC_SHORT,0.01);

  result += MINCReadWriteTestVector<double,3>("4DDoubleImage_byte.mnc",10,NC_BYTE,0.2);
  result += MINCReadWriteTestVector<double,3>("4DDoubleImage_short.mnc",10,NC_SHORT,0.01);

/*  result += MINCReadWriteTest<unsigned char,4>("4DUCharImage.mnc");
  result += MINCReadWriteTest<float,4>("4DFloatImage.mnc");*/

/*  result += MINCReadWriteTest<itk::RGBPixel<unsigned char>,4 >("4DRGBImage.mnc");
  result += MINCReadWriteTest<itk::Vector<float,3>,3 >("4DVectorImage.mnc");*/
  return result != 0;
}
