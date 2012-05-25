/* ----------------------------- MNI Header -----------------------------------
@NAME       :
@DESCRIPTION:
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

#include "itkMincImageIO.h"
#include "itkIOCommon.h"
#include "itkExceptionObject.h"
#include "itkMetaDataObject.h"
#include "itkMatrix.h"
#include "itksys/SystemTools.hxx"
#include "vnl/vnl_math.h"


using namespace minc;

namespace itk
{

  MincImageIO::MincImageIO():m_Rdr(NULL),m_Wrt(NULL)
  {
  }

  void MincImageIO::clean(void)
  {
    if(m_Rdr) delete m_Rdr;
    if(m_Wrt) delete m_Wrt;
    m_Rdr=NULL;
    m_Wrt=NULL;
  }

  MincImageIO::~MincImageIO()
  {
    clean();
  }

  void MincImageIO::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    if(m_Rdr)
      os<<"MINC file is opened for reading"<<std::endl;
    if(m_Wrt)
      os<<"MINC file is opened for writing"<<std::endl;
  }

  bool MincImageIO::CanReadFile(const char * FileNameToRead)
  {
    std::string filename = FileNameToRead;
    //taken from original MINC2ImageIO

    if( filename.empty())
    {
      itkDebugMacro(<<"No filename specified.");
      return false;
    }

    std::string::size_type mncPos = filename.rfind(".mnc");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".mnc2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }

    mncPos = filename.rfind(".mnc.gz");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 7) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC.GZ");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 7) )
    {
      return true;
    }
    return false;
  }

  void MincImageIO::ReadImageInformation()
  {
    clean();
    try
    {
      m_Rdr=new minc_1_reader;
      //read in positive direction, always
      m_Rdr->open(m_FileName.c_str(),true);
      //always treat vector_dimension and time_dimension as vectors
      SetNumberOfDimensions((m_Rdr->ndim(1)>0?1:0)+(m_Rdr->ndim(2)>0?1:0)+(m_Rdr->ndim(3)>0?1:0));
      // set number of dimensions for ITK
      int image_max_length=m_Rdr->var_length(MIimagemax);
      int image_min_length=m_Rdr->var_length(MIimagemin);
      bool slice_normalized=image_max_length>1;

      switch(m_Rdr->datatype())
      {
        case NC_BYTE:
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=UCHAR;
          break;

        case NC_SHORT:
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=m_Rdr->is_signed()?SHORT:USHORT;
          break;

        case NC_INT:
          if(slice_normalized)
            m_ComponentType=FLOAT;
          else
            m_ComponentType=m_Rdr->is_signed()?INT:UINT;
          break;

        case NC_FLOAT:
          m_ComponentType=FLOAT;
          break;

        case NC_DOUBLE:
          m_ComponentType=DOUBLE;
          break;

        default:
          throw ExceptionObject(__FILE__,__LINE__,"Unsupported data type");
      }
      if( m_Rdr->ndim(0) > 0 && m_Rdr->ndim(4) == 0 ) {
        m_PixelType=VECTOR;
        SetNumberOfComponents(m_Rdr->ndim(0));
      } else if( m_Rdr->ndim(0) == 0 && m_Rdr->ndim(4) > 0 ) {
        m_PixelType=VECTOR;
        SetNumberOfComponents(m_Rdr->ndim(4));
      } else if( m_Rdr->ndim(0)>0 && m_Rdr->ndim(4)>0 ) {
          throw ExceptionObject(__FILE__,__LINE__,"Combining time and vector dimension in one file is not supported!");
      } else
        m_PixelType=SCALAR;


      for(int i=1,c=0; i < 4; i++)
      {
        if(m_Rdr->ndim(i) <= 0 ) continue;

        SetDimensions(c,m_Rdr->ndim(i));
        SetSpacing(c,m_Rdr->nspacing(i));
        SetOrigin(c,m_Rdr->nstart(i));
        c++;
      }

      if(GetNumberOfDimensions() == 3)
      {
        itk::Matrix< double, 3,3 > dir_cos;

        for(int i=1; i < 4; i++)
        {
          std::vector<double> dc(3);
          if(m_Rdr->have_dir_cos(i))
          {
            for(int j=0;j<3;j++)
            {
              dc[j]=m_Rdr->ndir_cos(i,j);
              dir_cos[j][i-1]=m_Rdr->ndir_cos(i,j);
            }
          } else {
            for(int j=0;j<3;j++)
            {
              dc[j]=((i-1) == j?1:0);
              dir_cos[j][i-1]=((i-1) == j?1:0);
            }
          }
          SetDirection(i-1,dc);
        }

        itk::Vector< double,3> origin;
        itk::Vector< double,3> o_origin;

        for(int j=0;j<3;j++)
          o_origin[j]=GetOrigin(j);
        origin=dir_cos*o_origin;

        for(int j=0;j<3;j++)
          SetOrigin(j,origin[j]);

      } else { //we are not rotating the origin according to direction cosines in this case
        for(int i=1,c=0; i < 5; i++)
        {
          if(m_Rdr->ndim(i) <= 0) continue;
          std::vector<double> dc(3);
          for(int j=0;j<3;j++)
            dc[j]=m_Rdr->ndir_cos(i,j);
          SetDirection(c,dc);
          c++;
        }
      }
      ComputeStrides();

      //now, we are just assuming that all the volumes with short,int and float data types are real,
      //and those that are char are not
      itk::MetaDataDictionary &thisDic=GetMetaDataDictionary();
      std::string classname(GetNameOfClass());
      itk::EncapsulateMetaData<std::string>(thisDic,ITK_InputFilterName, classname);
      //now let's store some metadata
      //internally image is always stored

      /*itk::EncapsulateMetaData<itk::SpatialOrientation::ValidCoordinateOrientationFlags>
          (thisDic,ITK_CoordinateOrientation, itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPI);*/

      itk::EncapsulateMetaData(thisDic,"datatype",m_Rdr->datatype());
      itk::EncapsulateMetaData(thisDic,"signed"  ,m_Rdr->is_signed());

      /*switch(m_Rdr->datatype())
      {
        case NC_SHORT:

          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,m_Rdr->is_signed()?std::string(typeid(short).name()):std::string(typeid(unsigned short).name()));
          break;

        case NC_BYTE:
          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,m_Rdr->is_signed()?std::string(typeid(char).name()):std::string(typeid(unsigned char).name()));
          break;

        case NC_FLOAT:
          itk::EncapsulateMetaData<std::string>(thisDic,ITK_OnDiskStorageTypeName,std::string(typeid(float).name()));
          break;
      }*/

      if(m_Rdr->ndim(4)) //we have got time dimension
      {
        itk::EncapsulateMetaData<double>(thisDic,"tstart",m_Rdr->nstart(4));
        itk::EncapsulateMetaData<double>(thisDic,"tstep",m_Rdr->nspacing(4));
      }

      std::vector<std::string> dimorder;
      for(int i=0;i<m_Rdr->dim_no();i++)
      {
        dimorder.push_back(m_Rdr->info()[i].name);
      }
      itk::EncapsulateMetaData(thisDic,"dimorder", dimorder);
      itk::EncapsulateMetaData<std::string>(thisDic,"history", m_Rdr->history());
      //walk through all the minc attributes, and store them
      for(int i=0;i<m_Rdr->var_number();i++)
      {
        std::string var=m_Rdr->var_name(i);

        if(var == "rootvariable" ||
           var == "image" ||
           var == "image-min" ||
           var == "image-max" ||
           var == "xspace" ||
           var == "yspace" ||
           var == "zspace" ||
           var == "time" ||
           var == "vector_dimension"  )
          continue;

        int var_id=m_Rdr->var_id(var.c_str());
        for(int j=0;j < m_Rdr->att_number(var_id);j++)
        {
          std::string att=m_Rdr->att_name(var_id,j);
          std::string path=var+":"+att;
          //std::cout<<path.c_str()<<" ";
          switch(m_Rdr->att_type(var_id,att.c_str()))
          {
            case NC_CHAR:
              itk::EncapsulateMetaData<std::string>(thisDic,path, m_Rdr->att_value_string(var_id,att.c_str()));
              //std::cout<<"string";
              break;
            case NC_INT:
              itk::EncapsulateMetaData<std::vector<int> >(thisDic,path, m_Rdr->att_value_int(var_id,att.c_str()));
              //std::cout<<"int";
              break;
            case NC_DOUBLE:
              itk::EncapsulateMetaData<std::vector<double> >(thisDic,path, m_Rdr->att_value_double(var_id,att.c_str()));
              //std::cout<<"double";
              break;
            case NC_SHORT:
              itk::EncapsulateMetaData<std::vector<short> >(thisDic,path, m_Rdr->att_value_short(var_id,att.c_str()));
              break;
            case NC_BYTE:
              itk::EncapsulateMetaData<std::vector<unsigned char> >(thisDic,path, m_Rdr->att_value_byte(var_id,att.c_str()));
              break;
            default:
              //std::cout<<"Unknown";
              break; //don't know what it is, skipping for now!
          }
          //std::cout<<std::endl;
        }
      }

    } catch(const minc::generic_error & err) {
      throw ExceptionObject(__FILE__,__LINE__,"Error reading minc file");
    }
  }

  void MincImageIO::Read(void* buffer)
  {
    //TODO: add support for IORegion ?
    //ImageIORegion regionToRead = this->GetIORegion();
    switch(GetComponentType())
    {
      case CHAR:
      case UCHAR:
        m_Rdr->setup_read_byte(false);
        load_non_standard_volume<unsigned char>(*m_Rdr,(unsigned char*)buffer);
        break;
      case INT:
        m_Rdr->setup_read_int(false);
        load_non_standard_volume<int>(*m_Rdr,(int*)buffer);
        break;
      case UINT:
        m_Rdr->setup_read_uint(false);
        load_non_standard_volume<unsigned int>(*m_Rdr,(unsigned int*)buffer);
        break;
      case SHORT:
        m_Rdr->setup_read_short(false);
        load_non_standard_volume<short>(*m_Rdr,(short*)buffer);
        break;
      case USHORT:
        m_Rdr->setup_read_ushort(false);
        load_non_standard_volume<unsigned short>(*m_Rdr,(unsigned short*)buffer);
        break;
      case FLOAT:
        m_Rdr->setup_read_float();
        load_non_standard_volume<float>(*m_Rdr,(float*)buffer);
        break;
      case DOUBLE:
        m_Rdr->setup_read_double();
        load_non_standard_volume<double>(*m_Rdr,(double*)buffer);
        break;
      default:
        throw ExceptionObject(__FILE__, __LINE__,"Unsupported data type");
    }
    delete m_Rdr;
    m_Rdr=NULL;
  }

  bool MincImageIO::CanWriteFile(const char * FileNameToWrite)
  {
    std::string filename = FileNameToWrite;
    //taken from original MINC2ImageIO

    if( filename.empty())
    {
      itkDebugMacro(<<"No filename specified.");
      return false;
    }

    std::string::size_type mncPos = filename.rfind(".mnc");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 4) )
    {
      return true;
    }

    mncPos = filename.rfind(".mnc2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }

    mncPos = filename.rfind(".MNC2");
    if ( (mncPos != std::string::npos)
          && (mncPos == filename.length() - 5) )
    {
      return true;
    }

    return false;
  }

  void MincImageIO::WriteImageInformation(void)
  {
    try
    {
      MetaDataDictionary &thisDic=GetMetaDataDictionary();
      nc_type datatype;
      bool is_signed=false;
      bool have_vectors=false;
      bool have_time=false;
      int dim_no=GetNumberOfDimensions();

      switch(GetComponentType())
      {
        case UCHAR:
        case CHAR:
          datatype=NC_BYTE;
          is_signed=false;
          break;
        case SHORT:
          datatype=NC_SHORT;
          is_signed=true;
          break;
        case USHORT:
          datatype=NC_SHORT;
          is_signed=false;
          break;
        case INT:
          datatype=NC_INT;
          is_signed=true;
          break;
        case UINT:
          datatype=NC_INT;
          is_signed=false;

          break;
        case FLOAT:
          //let's see if there is metadata available
          {
            nc_type _datatype=NC_FLOAT;
            bool _is_signed=true;

            if( itk::ExposeMetaData(thisDic,"datatype",_datatype) &&
                itk::ExposeMetaData(thisDic,"signed",_is_signed) )
            {
              datatype=_datatype;
              is_signed=_is_signed;
            } else {
              datatype=NC_FLOAT;
              is_signed=true;
            }
          }
          break;
        case DOUBLE:
          datatype=NC_DOUBLE;
          is_signed=true;
          break;
        default:
        {
          throw ExceptionObject(__FILE__, __LINE__,"Unsupported data type");
        }
      }
      std::vector<std::string> dimorder;
      std::vector<int> dimmap(5,-1);
      minc_info info;

      if(GetNumberOfComponents()>1)
      {
        if(GetNumberOfComponents()<=3 )
        {
          have_vectors=true;
          have_time=false;
        } else if(GetNumberOfComponents()>3) {
          have_vectors=false;
          have_time=true;
        }
        dim_no++;
      } else if(GetNumberOfDimensions()>3) {
        have_vectors=false;
        have_time=true;
      }

      if(itk::ExposeMetaData(thisDic,"dimorder",dimorder))
      {
        for(int i=0,j=0;i<dimorder.size();i++)
        {
          if(dimorder[i]==MIvector_dimension && have_vectors)
          {
            dimmap[0]=j++;
          }
          else if(dimorder[i]==MItime && have_time)
          {
            dimmap[4]=j++;
          } else if(dimorder[i]==MIxspace) {
            dimmap[1]=j++;
          } else if(dimorder[i]==MIyspace) {
            dimmap[2]=j++;
          } else if(dimorder[i]==MIzspace) {
            dimmap[3]=j++;
          }
        }
      } else {

        if(have_vectors)
          dimmap[0]=0;

        for(int i=0;i<dim_no-(have_vectors?1:0);i++)
          dimmap[1+i]=(have_vectors?1:0)+i;
      }

      info.resize(dim_no);

#ifdef _DEBUG
      std::cout<<"Number of components:"<<GetNumberOfComponents()<<std::endl;
      std::cout<<"Number of dimensions:"<<GetNumberOfDimensions()<<std::endl;
      std::cout<<"info.size()="<<info.size()<<std::endl;
      std::cout<<"dimmap:";
      for(int i=0;i<5;i++)
      {
        std::cout<<dimmap[i]<<",";
      }
      std::cout<<std::endl;
#endif  //_DEBUG

      for(int i=0;i<GetNumberOfDimensions();i++)
      {
        int _i=dimmap[i+1];
        if(_i<0)
        {
          throw ExceptionObject(__FILE__, __LINE__,"Internal error");
        }
         //ERROR!
        info[_i].length=GetDimensions(i);
        info[_i].step=GetSpacing(i);
        info[_i].start=GetOrigin(i);
        info[_i].have_dir_cos=i<3;

        for(int j=0;j<3;j++)
          info[_i].dir_cos[j]=GetDirection(i)[j];

        switch(i)
        {
          case 0:
            info[_i].dim=dim_info::DIM_X;
            break;
          case 1:
            info[_i].dim=dim_info::DIM_Y;
            break;
          case 2:
            info[_i].dim=dim_info::DIM_Z;
            break;
        }
      }

      if(GetNumberOfDimensions() == 3) //we are only rotating 3D part
      {
        vnl_vector< double> start(3);
        vnl_vector< double> vorigin(3);
        itk::Matrix< double, 3,3 > _dir_cos;
        for(int i=0;i<3;i++)
        {
          vorigin[i]=GetOrigin(i);
          for(int j=0;j<3;j++)
            _dir_cos[j][i]=GetDirection(i)[j];
        }

        start=_dir_cos.GetInverse()*vorigin; //this is not optimal

        for(int i=0;i<3;i++)
        {
          int _i=dimmap[i+1];
          info[_i].start=start[i];
        }
      } else {
/*        std::cout<<"Warning: maging rotation matrix for "<<GetNumberOfDimensions()<<"D volume"<<std::endl;
        std::cout<<"Origin:";
        for(int i=0;i<GetNumberOfDimensions();i++)
          std::cout<<GetOrigin(i)<<",";
        std::cout<<std::endl;
        std::cout<<"Rotation matrix:"<<std::endl;
        for(int i=0;i<GetNumberOfDimensions();i++)
        {
          std::cout<<"\t"<<i<<":";
          for(int j=0;j<GetNumberOfDimensions();j++)
            std::cout<<GetDirection(i)[j]<<",";
          std::cout<<std::endl;
        }
        std::cout<<std::endl;*/
      }

      //here we assume that we had a grid file
      if(have_vectors)
      {
        int _i=dimmap[0];
        info[_i].length=GetNumberOfComponents();
        info[_i].step=1;
        info[_i].start=0;
        info[_i].dim=dim_info::DIM_VEC;
        info[_i].have_dir_cos=false;
      } else if(have_time) {

        int _i=dimmap[4];
        info[_i].length=(GetNumberOfComponents() == 1)?GetDimensions(3):GetNumberOfComponents();
        double tstep=1;
        double tstart=0;

        if( GetNumberOfComponents() == 1 ) {
          info[_i].step=GetSpacing(3);
          info[_i].start=GetOrigin(3);
        } else if( itk::ExposeMetaData(thisDic,"tstep",tstep) &&
                  itk::ExposeMetaData(thisDic,"tstart",tstart) )
        {
          info[_i].step=tstep;
          info[_i].start=tstart;
        } else  {
          info[_i].step=1.0; //TODO: show somehow differently that we don't have info?
          info[_i].start=0;
        }
        info[_i].dim=dim_info::DIM_TIME;
        info[_i].have_dir_cos=false; //TODO: what to do here?
      }

      #ifdef _DEBUG
      std::cout<<"info:";
      for(int i=0;i<info.size();i++)
      {
        switch(info[i].dim)
        {
          case dim_info::DIM_VEC:
            std::cout<<"vector_dimension";break;
          case dim_info::DIM_Z:
            std::cout<<"zspace";break;
          case dim_info::DIM_Y:
            std::cout<<"yspace";break;
          case dim_info::DIM_X:
            std::cout<<"xspace";break;
          case dim_info::DIM_TIME:
            std::cout<<"time";break;
          default:
            std::cout<<"unknown";break;
        }
        std::cout<<"("<<info[i].length<<"),";
      }
      std::cout<<std::endl;
      #endif //_DEBUG

      //TODO:  shuffle info based on the dimnames
      m_Wrt=new minc_1_writer;
      m_Wrt->open(this->GetFileName(),
                 info,
                 (datatype==NC_FLOAT||datatype==NC_DOUBLE)?
                     dim_no:(have_time?dim_no-2:dim_no-1),
                 datatype,is_signed);

      //let's write some meta information if there is any
      for(MetaDataDictionary::ConstIterator it=thisDic.Begin();it != thisDic.End(); ++it)
      {
        const char *d=strchr((*it).first.c_str(),':');
        if(d)
        {
          std::string var((*it).first.c_str(),d-(*it).first.c_str());
          std::string att(d+1);
          itk::MetaDataObjectBase *bs=(*it).second;
          const char *tname=bs->GetMetaDataObjectTypeName();
          //std::cout<<var.c_str()<<"\t"<<att.c_str()<<"\t"<<tname<<std::endl;
          //THIS is not good OO style at all :(
          if(!strcmp(tname,typeid(std::string).name()))
          {
            m_Wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::string> *>(bs )->GetMetaDataObjectValue().c_str());
          } else if(!strcmp(tname,typeid(std::vector<double>).name())) {
            m_Wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::vector<double> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<int>).name())) {
            m_Wrt->insert(var.c_str(),att.c_str(),
              dynamic_cast<MetaDataObject<std::vector<int> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<short>).name())) {
            m_Wrt->insert(var.c_str(),att.c_str(),
                         dynamic_cast<MetaDataObject<std::vector<short> > * >(bs)->GetMetaDataObjectValue());
          } else if(!strcmp(tname,typeid(std::vector<unsigned char>).name())) {
            m_Wrt->insert(var.c_str(),att.c_str(),
                         dynamic_cast<MetaDataObject<std::vector<unsigned char> > * >(bs)->GetMetaDataObjectValue());
          }
        } else if((*it).first == "history") {
          itk::MetaDataObjectBase *bs=(*it).second;
          m_Wrt->append_history(dynamic_cast<MetaDataObject<std::string> *>(bs)->GetMetaDataObjectValue().c_str());
        }
      }
    } catch(const minc::generic_error & err) {
      ExceptionObject exception(err.file(), err.line());
      std::string ErrorMessage="MINC IO error:";
      ErrorMessage += err.msg();
      exception.SetDescription(ErrorMessage.c_str());
      throw exception;
    }
    return;
  }

  void MincImageIO::Write( const void* buffer)
  {
    try
    {
      this->WriteImageInformation(); //Write the image Information before writing data

      switch(this->GetComponentType())
      {
        case UCHAR:
        case CHAR:
          m_Wrt->setup_write_byte(false);
          save_non_standard_volume<unsigned char>(*m_Wrt,(const unsigned char*)buffer);
          break;
        case SHORT:
          m_Wrt->setup_write_short();
          save_non_standard_volume<short>(*m_Wrt,(const short*)buffer);
          break;
        case USHORT:
          m_Wrt->setup_write_ushort();
          save_non_standard_volume<unsigned short>(*m_Wrt,(const unsigned short*)buffer);
          break;
        case INT:
          m_Wrt->setup_write_int();
          save_non_standard_volume<int>(*m_Wrt,(const int*)buffer);
          break;
        case UINT:
          m_Wrt->setup_write_uint();
          save_non_standard_volume<unsigned int>(*m_Wrt,(const unsigned int*)buffer);
          break;
        case FLOAT:
          m_Wrt->setup_write_float();
          save_non_standard_volume<float>(*m_Wrt,(const float*)buffer);
          break;
        case DOUBLE:
          m_Wrt->setup_write_double();
          save_non_standard_volume<double>(*m_Wrt,(const double*)buffer);
          break;
        default:
          throw ExceptionObject(__FILE__,__LINE__,"Unsupported data type");
      }
      //finish writing, cleanup
      delete m_Wrt;
      m_Wrt=NULL;
    } catch(const minc::generic_error & err) {
      throw ExceptionObject(__FILE__, __LINE__,"Error reading minc file");
    }

  }
} // end namespace itk
