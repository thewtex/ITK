/* ----------------------------- MNI Header -----------------------------------
@NAME       : itkMincHelpers.h
@DESCRIPTION: minc helper functions for ITK
@COPYRIGHT  :
              Copyright 2012 Vladimir Fonov, McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef __itkMincHelpers_h
#define __itkMincHelpers_h

#include <complex>
#include <vector>
#include <algorithm>

#include "itkArray.h"

#if ( ITK_VERSION_MAJOR > 3 )
#include "itkImage.h"
#else
#include "itkOrientedImage.h"
#endif

#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageIOFactory.h"

#include "itkMincImageIOFactory.h"
#include "itkMincImageIO.h"
#include <minc_io_fixed_vector.h>

/**
  * \ingroup  ITKIOMINC
  * \brief various helper functions for interacting with minc library
  */
namespace itk
{
  //! default label voxel type
  typedef unsigned char                                MINC_MASK_VOXEL;
  //! default minc file voxel type
  typedef float                                        MINC_VOXEL_TYPE;
  //! default minc volume dimension
  const int MINC_VOLUME_DIMENSIONS = 3;
  //! default minc grid volume voxel type
  typedef itk::Vector<float,MINC_VOLUME_DIMENSIONS>    MINC_DEF_VECTOR;
  //! minc tag point
  typedef itk::Point<double,MINC_VOLUME_DIMENSIONS>    MINC_TAG_POINT;
  typedef std::vector<MINC_TAG_POINT>                  MINC_TAG_POINTS;
  //! default minc MINC_COMPLEX voxel type
  typedef std::complex < MINC_VOXEL_TYPE >             MINC_COMPLEX;

#if ( ITK_VERSION_MAJOR > 3 )
  typedef itk::Image < MINC_COMPLEX, MINC_VOLUME_DIMENSIONS >                MINC_IMAGE3D_COMPLEX;
  typedef itk::Image < MINC_VOXEL_TYPE, MINC_VOLUME_DIMENSIONS >        MINC_IMAGE3D;
  typedef itk::Image < MINC_MASK_VOXEL, MINC_VOLUME_DIMENSIONS >        MINC_MASK3D;
  typedef itk::Image < MINC_DEF_VECTOR, MINC_VOLUME_DIMENSIONS >        MINC_DEF3D;
#else
  typedef itk::OrientedImage < MINC_COMPLEX, MINC_VOLUME_DIMENSIONS >         MINC_IMAGE3D_COMPLEX;
  typedef itk::OrientedImage < MINC_VOXEL_TYPE, MINC_VOLUME_DIMENSIONS > MINC_IMAGE3D;
  typedef itk::OrientedImage < MINC_MASK_VOXEL, MINC_VOLUME_DIMENSIONS > MINC_MASK3D;
  typedef itk::OrientedImage < MINC_DEF_VECTOR, MINC_VOLUME_DIMENSIONS > MINC_DEF3D;
#endif


  typedef itk::ImageRegionIteratorWithIndex < MINC_IMAGE3D > MINC_IMAGE3D_iterator;
  typedef itk::ImageRegionConstIteratorWithIndex < MINC_IMAGE3D > MINC_IMAGE3D_const_iterator;

  typedef itk::ImageRegionIteratorWithIndex < MINC_MASK3D > MINC_MASK3D_iterator;
  typedef itk::ImageRegionConstIteratorWithIndex < MINC_MASK3D > MINC_MASK3D_const_iterator;

  typedef itk::ImageRegionIteratorWithIndex < MINC_DEF3D > MINC_DEF3D_iterator;
  typedef itk::ImageRegionConstIteratorWithIndex < MINC_DEF3D > MINC_DEF3D_const_iterator;

  typedef itk::ImageRegionIteratorWithIndex < MINC_IMAGE3D_COMPLEX > MINC_IMAGE3D_COMPLEX_iterator;
  typedef itk::ImageRegionConstIteratorWithIndex < MINC_IMAGE3D_COMPLEX > MINC_IMAGE3D_COMPLEX_const_iterator;

  //! find a maximum of elements
  template<class T> float v_max(const T& c)
  {
    float s=std::numeric_limits < float >::min();
    for(unsigned int i=0;i<3;i++)
      if(c[i]>s) s=c[i];
    return s;
  }

  //! find a minimum of elements
  template<class T> float v_min(const T& c)
  {
    float s=std::numeric_limits < float >::max();
    for(unsigned int i=0;i<3;i++)
      if(c[i]<s) s=c[i];
    return s;
  }

  //! allocate volume of the same dimension,spacing and origin
  template<class T,class S> void allocate_same(typename T::Pointer &image,const typename S::Pointer &sample)
  {
    image->SetLargestPossibleRegion(sample->GetLargestPossibleRegion());
    image->SetBufferedRegion(sample->GetLargestPossibleRegion());
    image->SetRequestedRegion(sample->GetLargestPossibleRegion());
    image->SetSpacing( sample->GetSpacing() );
    image->SetOrigin ( sample->GetOrigin() );
    image->SetDirection(sample->GetDirection());
    image->Allocate();
  }

  //! allocate volume of the same dimension,spacing and origin
  template<class T,class S> void allocate_same(typename T::Pointer &image,const typename S::ConstPointer &sample)
  {
    image->SetLargestPossibleRegion(sample->GetLargestPossibleRegion());
    image->SetBufferedRegion(sample->GetLargestPossibleRegion());
    image->SetRequestedRegion(sample->GetLargestPossibleRegion());
    image->SetSpacing( sample->GetSpacing() );
    image->SetOrigin ( sample->GetOrigin() );
    image->SetDirection(sample->GetDirection());
    image->Allocate();
  }

  //! allocate volume of the same dimension,spacing and origin
  template<class T,class S> typename T::Pointer allocate_same(const typename S::Pointer &sample)
  {
    typename T::Pointer image=T::New();
    image->SetLargestPossibleRegion(sample->GetLargestPossibleRegion());
    image->SetBufferedRegion(sample->GetLargestPossibleRegion());
    image->SetRequestedRegion(sample->GetLargestPossibleRegion());
    image->SetSpacing( sample->GetSpacing() );
    image->SetOrigin ( sample->GetOrigin() );
    image->SetDirection(sample->GetDirection());
    image->Allocate();
    return image;
  }

  //! allocate volume of the same dimension,spacing and origin and do nearest neighbour resampling
  template<class T,class S,class E,class D> void nearest_resample_like(typename T::Pointer &dst,const typename S::Pointer &sample,const typename E::Pointer &src, const D& def)
  {
    allocate_same<T,S>(dst,sample);
    itk::ImageRegionIteratorWithIndex<T> it(dst, dst->GetLargestPossibleRegion());
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      MINC_TAG_POINT p;
      typename E::IndexType idx;
      dst->TransformIndexToPhysicalPoint(it.GetIndex(),p);
      if(src->TransformPhysicalPointToIndex(p,idx))
      {
        it.Value()=src->GetPixel(idx);
      }else{
        it.Value()=def;
      }
    }
  }

  //! check if volumes have the same dimensions, spacing and origin
  template<class T,class S> bool check_same(typename T::Pointer image,typename S::Pointer sample)
  {
    return
      (image->GetLargestPossibleRegion() == sample->GetLargestPossibleRegion()) &&
      (image->GetSpacing() == sample->GetSpacing()) &&
      (image->GetOrigin() == sample->GetOrigin()) &&
      (image->GetDirection().GetVnlMatrix() == sample->GetDirection().GetVnlMatrix()); // carefull here! , maybe we should calculate some kind of difference here ?
      // this is warkaround a bug in itk
  }

  //! allocate volume
  //! \param[out] image - volume to allocate
  //! \param dims - dimensions (voxels)
  //! \param spacing - volume spacing (mm)
  //! \param origin  - volume origin (mm)
  template<class T> void allocate_MINC_IMAGE3D(typename T::Pointer &image,
      const itk::Array<unsigned int> &dims,
      const itk::Array<double>& spacing,
      const itk::Array<double>& origin)
  {
    typename T::SizeType  imageSize3D = {{ dims[0], dims[1], dims[2]}};
    typename T::IndexType startIndex3D = { {0, 0, 0}};
    typename T::RegionType region;
    region.SetSize  (imageSize3D);
    region.SetIndex (startIndex3D);
    image->SetLargestPossibleRegion (region);
    image->SetBufferedRegion (region);
    image->SetRequestedRegion (region);
    image->SetSpacing( spacing );
    image->SetOrigin( origin );
    image->Allocate ();
  }

  //! allocate volume
  //! \param[out] image - volume to allocate
  //! \param dims - dimensions (voxels)
  //! \param spacing - volume spacing (mm)
  //! \param origin  - volume origin (mm)
  template<class T> void allocate_MINC_IMAGE3D(typename T::Pointer &image,
      const itk::Vector<unsigned int,3> &dims,
      const itk::Vector<double,3>& spacing,
      const itk::Vector<double,3>& origin)
  {
    typename T::SizeType  imageSize3D = {{ dims[0], dims[1], dims[2]}};
    typename T::IndexType startIndex3D = { {0, 0, 0}};
    typename T::RegionType region;
    region.SetSize  (imageSize3D);
    region.SetIndex (startIndex3D);
    image->SetLargestPossibleRegion (region);
    image->SetBufferedRegion (region);
    image->SetRequestedRegion (region);
    image->SetSpacing( spacing );
    image->SetOrigin( origin.GetDataPointer () );
    image->Allocate ();
  }

  //! allocate volume
  //! \param[out] image - volume to allocate
  //! \param dims - dimensions (voxels)
  //! \param spacing - volume spacing (mm)
  //! \param origin  - volume origin (mm)
  template<class T> void allocate_MINC_IMAGE3D(typename T::Pointer &image,
        const minc::fixed_vec<3, unsigned int>&dims,
        const minc::fixed_vec<3, double>& spacing=minc::fixed_vec<3, double>(1.0),
        const minc::fixed_vec<3, double>& origin=minc::fixed_vec<3, double>(0.0))
  {
    typename T::SizeType  imageSize3D = {{ dims[0], dims[1], dims[2]}};
    typename T::IndexType startIndex3D = { {0, 0, 0}};
    typename T::RegionType region;
    region.SetSize  (imageSize3D);
    region.SetIndex (startIndex3D);
    image->SetLargestPossibleRegion (region);
    image->SetBufferedRegion (region);
    image->SetRequestedRegion (region);
    image->SetSpacing( spacing.c_buf() );
    image->SetOrigin( origin.c_buf() );
    image->Allocate ();
  }

  //! allocate volume
  //! \param[out] image - volume to allocate
  //! \param dims - dimensions (voxels)
  //! \param spacing - volume spacing (mm)
  //! \param origin  - volume origin (mm)
  template<class T> void allocate_MINC_IMAGE3D(typename T::Pointer &image,
      const itk::Size<3> &dims)
  {
    //typename T::SizeType  imageSize3D = {{ dims[0], dims[1], dims[2]}};
    typename T::IndexType startIndex3D = { {0, 0, 0}};
    typename T::RegionType region;
    double spacing[3]= { 1.0,1.0,1.0};
    double origin[3]= { 0.0,0.0,0.0};
    region.SetSize  (dims);
    region.SetIndex (startIndex3D);
    image->SetLargestPossibleRegion (region);
    image->SetBufferedRegion (region);
    image->SetRequestedRegion (region);
    image->SetSpacing( spacing );
    image->SetOrigin( origin );
    image->Allocate ();
  }

  inline MINC_IMAGE3D::SizeType operator/= (MINC_IMAGE3D::SizeType & s, int d)
  {
    s[0] /= d;
    s[1] /= d;
    s[2] /= d;
    return s;
  }

  inline MINC_IMAGE3D::SizeType operator*= (MINC_IMAGE3D::SizeType & s, int d)
  {
    s[0] *= d;
    s[1] *= d;
    s[2] *= d;
    return s;
  }

  //! a helper function for minc reading
  template <class T> typename T::Pointer load_minc(const char *file)
  {
    typedef itk::MincImageIO ImageIOType;
    ImageIOType::Pointer minc2ImageIO = ImageIOType::New();

    typename itk::ImageFileReader<T>::Pointer reader = itk::ImageFileReader<T>::New();

    reader->SetFileName(file);
    reader->SetImageIO( minc2ImageIO );
    reader->Update();

    return reader->GetOutput();
  }

  //! set minc file storage type
  void set_minc_storage_type(itk::Object* image,nc_type datatype,bool is_signed);

  //! copy metadata
  void copy_metadata(itk::Object* dst,itk::Object* src);

  //! copy minc dimension order
  void copy_dimorder(itk::Object* dst,itk::Object* src);

  //! append minc-style history
  void append_history(itk::Object* dst,const std::string& history);

  //! a helper function for minc writing
  template <class T> void save_minc(const char *file,typename T::Pointer img)
  {
    typedef itk::MincImageIO ImageIOType;
    ImageIOType::Pointer minc2ImageIO = ImageIOType::New();
    typename itk::ImageFileWriter< T >::Pointer writer = itk::ImageFileWriter<T>::New();
    writer->SetFileName(file);
    writer->SetImageIO( minc2ImageIO );
    writer->SetInput( img );
    writer->Update();
  }

  //! calculate volume min and max
  int get_image_limits(MINC_IMAGE3D::Pointer, MINC_VOXEL_TYPE &min, MINC_VOXEL_TYPE &max);
  //! calculate volume min and max
  int get_image_limits(MINC_DEF3D::Pointer, MINC_VOXEL_TYPE &min, MINC_VOXEL_TYPE &max);
  //! calculate volume min and max
  int get_image_limits(MINC_MASK3D::Pointer img, MINC_VOXEL_TYPE &min,MINC_VOXEL_TYPE &max);
  //! store tags to minc tags file
  void write_tags(const MINC_TAG_POINTS& tags, const char * file);
  //! store two sets of tags to minc tags file
  void write_2tags(const MINC_TAG_POINTS& tags,const MINC_TAG_POINTS& tags2, const char * file);
  //! store tags and labels
  void write_tags(const MINC_TAG_POINTS& tags,const std::vector<int>& labels, const char * file);
  //! store tags and values
  void write_tags(const MINC_TAG_POINTS& tags,const std::vector<float>& values, const char * file);
  //! store tags and values
  void write_tags(const MINC_TAG_POINTS& tags,const std::vector<double>& values, const char * file);
  //! read tags from the minc tag file
  void read_tags(MINC_TAG_POINTS& tags, const char * file,int vol=1);
  //! read tags and labels from minc tag file
  void read_tags(MINC_TAG_POINTS& tags, std::vector<int>& labels, const char * file, int vol=1);
  //! read tags and labels from minc tag file
  void read_tags(MINC_TAG_POINTS& tags, std::vector<float>& labels, const char * file, int vol=1);
  //! read tags and labels from minc tag file
  void read_tags(MINC_TAG_POINTS& tags, std::vector<double>& labels, const char * file, int vol=1);
  //! read tags and labels from minc tag file
  void read_tags(MINC_TAG_POINTS& tag1, MINC_TAG_POINTS& tag2,std::vector<double>& labels, const char * file);
  //! read array from the text file
  void load_parameters(const char *file,itk::Array<double> &param);
  //! save array to the text file
  void save_parameters(const char *file,const itk::Array<double> &param);
  //! read array from the text file, up to components
  void load_parameters(const char *file,itk::Array<double> &param,size_t no);
  //! make sure that all voxels are 0 or 1
  void normalize_mask(MINC_MASK3D::Pointer img);
  void read_linear_xfm(const char *xfm,itk::Matrix<double,3,3>& rot, itk::Vector<double,3>& tran );
  void read_linear_xfm(const char *xfm,itk::Matrix<double,2,2>& rot, itk::Vector<double,2>& tran );
  void write_linear_xfm(const char *xfm,const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran);
  void write_linear_xfm(const char *xfm,const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran);
  void write_combined_xfm(const char *xfm,const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran, const char *grid );
  void write_combined_xfm(const char *xfm,const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran, const char *grid );
  void write_combined_xfm(const char *xfm,const char *grid, const itk::Matrix<double,3,3>& rot,const itk::Vector<double,3>& tran );
  void write_combined_xfm(const char *xfm,const char *grid, const itk::Matrix<double,2,2>& rot,const itk::Vector<double,2>& tran );

  void write_nonlinear_xfm(const char *xfm,const char *grid);

  //! minc4itk compatibility function
  template <class T> void  load_minc(const char *file,typename T::Pointer& img)
  {
    img=load_minc<T>(file);
  }

  template<class T> void setup_itk_image(const minc::minc_1_base& minc_rw, typename T::Pointer& img)
  {
    itk::Vector< unsigned int,3> dims;
    itk::Vector< double,3> spacing;
    itk::Vector< double,3> origin;
    itk::Vector< double,3> start;
    itk::Matrix< double, 3, 3> dir_cos;
    dir_cos.SetIdentity();
    //std::cout<<"setup_itk_image"<<std::endl;
    for(int i=0;i<3;i++)
    {
      dims[i]=minc_rw.ndim(i+1);
      spacing[i]=minc_rw.nspacing(i+1);
      start[i]=minc_rw.nstart(i+1);
      if(minc_rw.have_dir_cos(i+1))
      {
        for(int j=0;j<3;j++)
          dir_cos[j][i]=minc_rw.ndir_cos(i+1,j); //TODO: check transpose?
      }
      //std::cout<<start[i]<<"\t";
    }
    //std::cout<<std::endl;
    origin=dir_cos*start;
    allocate_MINC_IMAGE3D<T>(img,dims,spacing,origin);
    img->SetDirection(dir_cos);
  }

  template<class T> void imitate_minc (const char *path, typename T::Pointer &img)
  {
    minc::minc_1_reader rdr;
    rdr.open(path,true,true);
    setup_itk_image<T>(rdr,img);
  }

}//itk

#endif //__itkMincHelpers_h
