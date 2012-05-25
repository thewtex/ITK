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

#include "itksys/SystemTools.hxx"
#include "itksys/SystemInformation.hxx"
#include "itkCompositeTransform.h"
#include "itkCompositeTransformIOHelper.h"
#include "itkVersion.h"
#include <iostream>
#include "itkMincTransformIO.h"
#include <string.h>

namespace itk
{

  MincTransformIO::MincTransformIO()
  {
    m_XFM_initialized=false;
  }

  MincTransformIO::~MincTransformIO()
  {
    _cleanup();
  }

  void MincTransformIO::_cleanup(void)
  {
    if(m_XFM_initialized)
        delete_general_transform(&m_XFM);
    m_XFM_initialized=false;
  }

  bool MincTransformIO::CanReadFile(const char *fileName)
  {
    std::string ext(itksys::SystemTools::GetFilenameLastExtension(fileName));
    return (ext == ".xfm" || ext==".XFM");
  }

  bool MincTransformIO::CanWriteFile(const char *fileName)
  {
    std::string ext(itksys::SystemTools::GetFilenameLastExtension(fileName));
    return (ext == ".xfm" || ext==".XFM");
  }

  static void dump_xfm_info(VIO_General_transform* xfm,int indent=0)
  {
    std::string ind(' ',indent);
    std::cout<<ind.c_str();
    if(xfm->inverse_flag)
      std::cout<<" INV ";
    VIO_Transform *lin;

    switch(get_transform_type(xfm))
    {
      case LINEAR:
        std::cout<<" Linear ";
        lin=get_linear_transform_ptr(xfm);
        for(int i=0;i<4;i++)
          for(int j=0;j<4;j++)
            std::cout<<Transform_elem(*lin,i,j)<< " ";
        break;
      case THIN_PLATE_SPLINE:
        std::cout<<" Thin plate spline ";
        break;
      case USER_TRANSFORM:
        std::cout<<" User transform ";
        break;
      case CONCATENATED_TRANSFORM:
        std::cout<<" Concatenated "<<xfm->n_transforms<<std::endl;
        for(int i=0;i<get_n_concated_transforms(xfm);i++)
        {
          std::cout<<ind.c_str()<<"Transform "<<i<<std::endl;
          dump_xfm_info(get_nth_general_transform(xfm,i),indent+1);
        }
        break;
      case GRID_TRANSFORM :
        std::cout<<" Grid ";
        break;
      default:
        std::cout<<" Unknown! ";
    }
    std::cout<<std::endl;
  }

  void MincTransformIO::ReadOneTransform(VIO_General_transform *xfm)
  {
      switch(get_transform_type(xfm))
      {
        case LINEAR:
        {
          if(xfm->inverse_flag) //TODO: invert matrix?
            itkExceptionMacro( << "Reading inverse linear transform is not supported yet" );
          VIO_Transform *lin=get_linear_transform_ptr(xfm);
          TransformPointer transform;
          this->CreateTransform(transform,"AffineTransform_double_3_3");
          ParametersType ParameterArray;
          ParameterArray.SetSize(12);
          for(int j=0;j<3;j++)
            for(int i=0;i<4;i++)
              ParameterArray.SetElement(i+j*4,Transform_elem(*lin,j,i));
          transform->SetParametersByValue(ParameterArray);
          this->GetReadTransformList().push_back(transform);
          break;
        }
        case CONCATENATED_TRANSFORM:
        {
          for(int i=0;i<get_n_concated_transforms(xfm);i++)
          {
            ReadOneTransform(get_nth_general_transform(xfm,i));
          }
          break;
        }
        case THIN_PLATE_SPLINE:
          itkExceptionMacro( << "Reading THIN_PLATE_SPLINE transform is not supported yet" );
          break;
        case USER_TRANSFORM:
          itkExceptionMacro( << "Reading USER_TRANSFORM transform is not supported yet" );
          break;
        case GRID_TRANSFORM :
        {
          //first a few checks
          Volume grid=static_cast<Volume>(xfm->displacement_volume);
          if(get_volume_n_dimensions(grid) != 4)
            itkExceptionMacro( << "Reading grid transform with less then 4 dimensions is not supported" );
          int number_of_voxels=1;
          int      sizes[4];
          Real     separations[4];
          Real     starts[4];
          Real     direction_cosines[3][3];
          int      dim_map[4]={-1,-1,-1,-1},rev_map[4]={-1,-1,-1,-1};
          get_volume_sizes(grid,sizes);
          get_volume_separations(grid,separations);
          get_volume_starts(grid,starts);
          char **dim_names=get_volume_dimension_names(grid);
          std::cout<<"inputting grid of sizes:"<<sizes[0]<<" x "<< sizes[1] <<" x "<< sizes[2] <<" x "<< sizes[3]<<std::endl;
          std::cout<<"Dimensions:"<<dim_names[0]<<","<<dim_names[1]<<","<<dim_names[2]<<","<<dim_names[3]<<std::endl;
          number_of_voxels=sizes[0]*sizes[1]*sizes[2]*sizes[3];
          //have to find mapping to sane dimensions
          for(int i=0;i<4;i++)
          {
            if(!strcmp(dim_names[i],"vector_dimension"))
            {
              dim_map[0]=i;
              rev_map[i]=0;
            }
            else if(!strcmp(dim_names[i],"xspace"))
            {
              dim_map[1]=i;
              rev_map[i]=1;
            }
            else if(!strcmp(dim_names[i],"yspace"))
            {
              dim_map[2]=i;
              rev_map[i]=2;
            }
            else if(!strcmp(dim_names[i],"zspace"))
            {
              dim_map[3]=i;
              rev_map[i]=3;
            }
          }

          delete_dimension_names(grid,dim_names);

          get_volume_direction_cosine(grid,dim_map[1],direction_cosines[0]);
          get_volume_direction_cosine(grid,dim_map[2],direction_cosines[1]);
          get_volume_direction_cosine(grid,dim_map[3],direction_cosines[2]);
          //TODO: rotate start according to direction cosines..

          const int NDimensions=3;
          ParametersType fixedParameters;
          fixedParameters.SetSize(NDimensions * ( NDimensions + 3 ));

          for( unsigned int d = 0; d < NDimensions; d++ )
          {
            fixedParameters[d]=sizes[dim_map[d+1]];
          }

          for( unsigned int d = 0; d < NDimensions; d++ )
          {
            fixedParameters[d + NDimensions]=starts[dim_map[d+1]];
          }

          for( unsigned int d = 0; d < NDimensions; d++ )
          {
            fixedParameters[d + 2 * NDimensions]=separations[dim_map[d+1]];
          }

          for( unsigned int di = 0; di < NDimensions; di++ )//these should be already in sane order
          {
            for( unsigned int dj = 0; dj < NDimensions; dj++ )
            {
              fixedParameters[3 * NDimensions + ( di * NDimensions + dj )]=direction_cosines[di][dj];
            }
          }
          ParametersType parameters;
          parameters.SetSize(number_of_voxels);

          /*--- change all values over 100 to 100 */
          int v[4];int i=0;
          for(v[3] = 0;  v[3] < sizes[dim_map[3]];  ++v[3] )  {
              for(v[2] = 0;  v[2] < sizes[dim_map[2]];  ++v[2] ) {
                  for(v[1] = 0;  v[1] < sizes[dim_map[1]];  ++v[1] ) {
                    for(v[0] = 0;  v[0] < sizes[dim_map[0]];  ++v[0] ) {
                        parameters.SetElement(i,get_volume_real_value( grid, v[rev_map[0]], v[rev_map[1]], v[rev_map[2]], v[rev_map[3]], 0 ));
                        i++;
                      }
                  }
              }
          }

          TransformPointer transform;
          this->CreateTransform(transform,"DisplacementFieldTransform_double_3_3");
          transform->SetFixedParameters(fixedParameters);
          transform->SetParametersByValue(parameters);
          this->GetReadTransformList().push_back(transform);

          break;
        }
        default:
          itkExceptionMacro( << "Reading Unknown transform is not supported yet" );
          break;
      }
  }

  void MincTransformIO::Read()
  {
    if(input_transform_file((char*)this->GetFileName(), &m_XFM) != OK)
      itkExceptionMacro( << "Error reading XFM:" << this->GetFileName() );
    m_XFM_initialized=true;
    //DEBUG
    //dump_xfm_info(&m_XFM,0);
    ReadOneTransform(&m_XFM);
    _cleanup();
  }

  void MincTransformIO::WriteOneTransform(const int transformIndex,
                      const TransformBase *curTransform,
                      std::vector<VIO_General_transform> &xfm )
  {
    //std::string transformName(GetTransformName(transformIndex));
    std::cout<<"Transform:"<<transformIndex<<" ";

    const std::string transformType = curTransform->GetTransformTypeAsString();
    std::cout<<transformType.c_str()<<" ";
    //
    // write out transform type.
    //
    // composite transform doesn't store own parameters
    if(transformType.find("CompositeTransform") != std::string::npos)
    {
      if(transformIndex != 0)
      {
        itkExceptionMacro(<< "Composite Transform can only be 1st transform in a file");
      }
    } else {
      std::cout<<" parameters:"<<curTransform->GetParameters().Size()<<" ";
      std::cout<<" fixed paramters:"<<curTransform->GetFixedParameters().Size()<<" ";

      if(transformType.find("AffineTransform") != std::string::npos && curTransform->GetNumberOfParameters() == 12)
      {
        VIO_Transform lin;
        for(int j=0;j<3;j++)
          for(int i=0;i<4;i++)
            Transform_elem(lin,j,i)=curTransform->GetParameters()[i+j*4];
        xfm.push_back(VIO_General_transform());
        create_linear_transform(&xfm[xfm.size()-1],&lin );
      } else if(transformType.find("DisplacementFieldTransform") != std::string::npos && curTransform->GetFixedParameters().Size() == 18) {
        const int NDimensions=3;
        int      sizes[4]={0,0,0,3};
        Real     separations[4]={1.0,1.0,1.0,1.0};
        Real     starts[4]={0.0,0.0,0.0,0.0};
        Real     direction_cosines[3][3]={
          {1.0,0.0,0.0},
          {0.0,1.0,0.0},
          {0.0,0.0,1.0}};
        char * dimension_names[]={ "zspace","yspace","xspace","vector_dimension" };
        for( unsigned int d = 0; d < NDimensions; d++ )
        {
          sizes[d] = static_cast<int>( curTransform->GetFixedParameters()[d] );
        }
        sizes[3]=3;//vector

        for( unsigned int d = 0; d < NDimensions; d++ )
        {
          starts[d] = static_cast<Real>( curTransform->GetFixedParameters()[d + NDimensions]);
        }

        for( unsigned int d = 0; d < NDimensions; d++ )
        {
          separations[d] = static_cast<Real>(curTransform->GetFixedParameters()[d + 2 * NDimensions]);
        }

        for( unsigned int di = 0; di < NDimensions; di++ )
        {
          for( unsigned int dj = 0; dj < NDimensions; dj++ )
          {
            direction_cosines[di][dj] = static_cast<Real>(curTransform->GetFixedParameters()[3 * NDimensions + ( di * NDimensions + dj )]);
          }
        }
        double var_min,var_max;
        var_min=var_max=curTransform->GetParameters()[0];
        for(int i=0;i<curTransform->GetParameters().Size();i++)
        {
          if(var_min>curTransform->GetParameters()[i]) var_min=curTransform->GetParameters()[i];
          else if(var_max<curTransform->GetParameters()[i]) var_max=curTransform->GetParameters()[i];
        }
        Volume grid=create_volume(4,dimension_names,NC_FLOAT,TRUE,var_min,var_max);
        set_volume_voxel_range(grid,var_min,var_max);
        set_volume_real_range(grid,var_min,var_max);
        set_volume_sizes(grid,sizes);
        set_volume_separations(grid,separations);
        set_volume_starts(grid,starts);
        set_volume_direction_cosine(grid,0,direction_cosines[0]);
        set_volume_direction_cosine(grid,1,direction_cosines[1]);
        set_volume_direction_cosine(grid,2,direction_cosines[2]);
        alloc_volume_data(grid);

        for(int v1 = 0,i=0; v1 < sizes[0];  ++v1 ) {
            for(int v2 = 0;   v2 < sizes[1];  ++v2 ) {
                for(int v3 = 0;  v3 < sizes[2];  ++v3 ) {
                  for(int v4 = 0;  v4 < sizes[3];  ++v4 ) {
                      set_volume_real_value(grid,v1, v2, v3, v4 ,0, curTransform->GetParameters()[i]);
                      i++;
                    }
                }
            }
        }
        xfm.push_back(VIO_General_transform());
        create_grid_transform_no_copy(&xfm[xfm.size()-1],grid);
        //delete_volume(grid);
      } else {
        itkExceptionMacro(<< "Transform type:"<<transformType.c_str()<<"is Unsupported");
      }
    }
    std::cout<<std::endl;
  }

  void MincTransformIO::Write()
  {
    std::vector<VIO_General_transform> xfm;

    ConstTransformListType &transformList = this->GetWriteTransformList();

    std::string compositeTransformType = transformList.front()->GetTransformTypeAsString();

    CompositeTransformIOHelper helper;

    // if the first transform in the list is a
    // composite transform, use its internal list
    // instead of the IO
    if(compositeTransformType.find("CompositeTransform") != std::string::npos)
    {
      transformList = helper.GetTransformList(transformList.front().GetPointer());
    }

    ConstTransformListType::const_iterator end = transformList.end();

    int count = 0;

    for (ConstTransformListType::const_iterator it = transformList.begin();
        it != end; ++it,++count )
    {
      this->WriteOneTransform(count,(*it).GetPointer(),xfm);
    }
    VIO_General_transform transform=xfm[0];
    for(int i=1;i<xfm.size();i++ )
    {
      VIO_General_transform concated;
      concat_general_transforms( &transform, &xfm[i], &concated );
      delete_general_transform( &transform );
      delete_general_transform( &xfm[i] );
      transform = concated;
    }
    Status wrt=output_transform_file((char*)this->GetFileName(),"ITK-XFM writer",&transform);
    if(xfm.size()>1)
      delete_general_transform(&transform);
    delete_general_transform(&xfm[0]);
    if(wrt!=OK)
      itkExceptionMacro( << "Error writing XFM:" << this->GetFileName() );
  }
}//itk
