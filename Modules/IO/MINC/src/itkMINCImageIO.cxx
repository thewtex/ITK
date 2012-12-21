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
#include "itkMINCImageIO.h"
#include <stdio.h>
#include "vnl/vnl_vector.h"
#include "itkMetaDataObject.h"

namespace itk
{


bool MINCImageIO::CanReadFile(const char *file)
{
  if ( *file == 0 )
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }
  std::string filename(file);
  
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

//   mihandle_t volume;
// 
//   if ( miopen_volume(file, MI2_OPEN_READ, &volume) < 0 )
//     {
//     itkDebugMacro(<< " Can not open File:" << file << "\n");
//     return false;
//     }
//   if ( miclose_volume(volume) < 0 )
//     {
//     itkDebugMacro(<< " Can not close File:" << file << "\n");
//     return false;
//     }

  return true;
}

void MINCImageIO::Read(void *buffer)
{
  const unsigned int nDims = this->GetNumberOfDimensions();
  const unsigned int nComp = this->GetNumberOfComponents();

  misize_t *start=new misize_t[nDims+(nComp>1?1:0)];
  misize_t *count=new misize_t[nDims+(nComp>1?1:0)];
  
  if(nComp>1)
  {
    start[0]=0;
    count[0]=nComp;
  }
  
 for ( unsigned int i = 0; i < nDims; i++ )
  {
  if ( i < m_IORegion.GetImageDimension() )
    {
    start[i+(nComp>1?1:0)] = m_IORegion.GetIndex()[i];
    count[i+(nComp>1?1:0)] = m_IORegion.GetSize()[i];
    }
  else
    {
    start[i+(nComp>1?1:0)] = 0;
    count[i+(nComp>1?1:0)] = 1;
    }
  }
  mitype_t volume_data_type=MI_TYPE_UBYTE;
  
  switch(this->GetComponentType())
  {
    case UCHAR:
      volume_data_type=MI_TYPE_UBYTE;
      break;
    case CHAR:
      volume_data_type=MI_TYPE_BYTE;
      break;
    case USHORT:
      volume_data_type=MI_TYPE_USHORT;
      break;
    case SHORT:
      volume_data_type=MI_TYPE_SHORT;
      break;
    case UINT:  
      volume_data_type=MI_TYPE_UINT;
      break;
    case INT:
      volume_data_type=MI_TYPE_INT;
      break;
//     case ULONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_ULONG;
//       break;
//     case LONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_LONG;
//       break;
    case FLOAT://TODO: make sure we are cross-platform here!
      volume_data_type=MI_TYPE_FLOAT;
      break;
    case DOUBLE://TODO: make sure we are cross-platform here!
      volume_data_type=MI_TYPE_DOUBLE;
      break;
    default:
      itkDebugMacro(<<"Could read datatype " << this->GetComponentType() );
      return;
  }
  
  if ( miget_real_value_hyperslab(m_volume, volume_data_type, start, count, buffer) < 0 )
    {
    itkDebugMacro(" Can not get real value hyperslab!!\n");
    }
}


void MINCImageIO::CleanupDimensions(void)
{
  for ( int i = 0; i < this->m_NDims; i++ )
    {
     if(this->m_DimensionName[i])
       free((void*)this->m_DimensionName[i]);
     this->m_DimensionName[i]=NULL;
    }
    
  if(this->m_DimensionName) delete[] this->m_DimensionName;
  if(this->m_DimensionSize) delete[] this->m_DimensionSize;
  if(this->m_DimensionStart) delete[] this->m_DimensionStart;
  if(this->m_DimensionStep) delete[] this->m_DimensionStep;
  if(this->m_MincFileDims) delete [] this->m_MincFileDims;
  if(this->m_MincApparentDims) delete [] this->m_MincApparentDims;
  
  this->m_DimensionName  = NULL;
  this->m_DimensionSize  = NULL;
  this->m_DimensionStart = NULL;
  this->m_DimensionStep  = NULL;
  this->m_MincFileDims = NULL;
  this->m_MincApparentDims= NULL;
 
}


void MINCImageIO::AllocateDimensions(int nDims)
{
  this->CleanupDimensions();
  
  m_NDims=nDims;
  
  this->m_DimensionName  = new const char*[m_NDims];
  this->m_DimensionSize  = new misize_t[m_NDims];
  this->m_DimensionStart = new double[m_NDims];
  this->m_DimensionStep  = new double[m_NDims];
  this->m_MincFileDims   = new midimhandle_t[m_NDims];
  this->m_MincApparentDims = new midimhandle_t[m_NDims];
  
  for ( int i = 0; i < this->m_NDims; i++ )
    {
    this->m_DimensionName[i]  = NULL;
    this->m_DimensionSize[i]  = 0;
    this->m_DimensionStart[i] = 0.0;
    this->m_DimensionStep[i]  = 0.0;
    }
    
  for ( int i = 0; i < 5 ; i++ )
  {
    this->m_DimensionIndices[i] = -1;
  }
  
}

// close existing volume, cleanup internal structures
void MINCImageIO::CloseVolume(void)
{
  if( m_volume )
    miclose_volume( m_volume );
  m_volume = NULL;
  
  this->CleanupDimensions();
}

MINCImageIO::MINCImageIO()
{
  this->m_NDims = 0;
  this->m_DimensionName  = NULL;
  this->m_DimensionSize  = NULL;
  this->m_DimensionStart = NULL;
  this->m_DimensionStep  = NULL;
  this->m_MincFileDims   = NULL;
  this->m_MincApparentDims = NULL;
  this->m_volume = NULL;
  
  for ( int i = 0; i < 5 ; i++ )
  {
    this->m_DimensionIndices[i] = -1;
  }
}

MINCImageIO::~MINCImageIO()
{
  CloseVolume();
}

void MINCImageIO::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NDims: " << m_NDims << std::endl;
}

void MINCImageIO::ReadImageInformation()
{
  this->CloseVolume();
  // call to minc2.0 function to open the file
  if ( miopen_volume(m_FileName.c_str(), MI2_OPEN_READ, &m_volume) < 0 )
    {
    // Error opening the volume
    itkDebugMacro("Could not open file \"" << m_FileName.c_str() << "\".");
    return;
    }

  // find out how many dimensions are there regularly sampled
  // dimensions only
  int ndims;
  if ( miget_volume_dimension_count(m_volume, MI_DIMCLASS_ANY, MI_DIMATTR_ALL, &ndims) < 0 )
    {
    itkDebugMacro("Could not get the number of dimensions in the volume!");
    return;
    }
  this->AllocateDimensions(ndims);
  
 
  // get dimension handles in FILE ORDER (i.e, the order as they are
  // submitted to file)
  
  if ( miget_volume_dimensions(m_volume, MI_DIMCLASS_ANY, MI_DIMATTR_ALL, MI_DIMORDER_FILE, m_NDims,
                               this->m_MincFileDims) < 0 )
    {
    itkDebugMacro("Could not get dimension handles!");
    return;
    }

  for (int i = 0; i < m_NDims; i++ )
    {
    char *name;
    if ( miget_dimension_name(this->m_MincFileDims[i], &name) < 0 )
      {
      // Error getting dimension name
      itkDebugMacro("Could not get dimension name!");
      return;
      }

    this->m_DimensionName[i] = name;
    if(!strcmp(name,MIxspace) || !strcmp(name,MIxfrequency)) //this is X space
      this->m_DimensionIndices[1]=i;
    else if(!strcmp(name,MIyspace) || !strcmp(name,MIyfrequency)) //this is Y space
      this->m_DimensionIndices[2]=i;
    else if(!strcmp(name,MIzspace) || !strcmp(name,MIzfrequency)) //this is Z space
      this->m_DimensionIndices[3]=i;
    else if(!strcmp(name,MIvector_dimension) ) //this is vector space
      this->m_DimensionIndices[0]=i;
    else if(!strcmp(name,MItime) || !strcmp(name,MItfrequency)) //this is time space
      this->m_DimensionIndices[4]=i;
    else 
      {
      itkDebugMacro(<<"Unsupported MINC dimension:"<<name);
      return;
      }
    }

  // fill the DimensionSize by calling the following MINC2.0 function
  if ( miget_dimension_sizes(this->m_MincFileDims, m_NDims, m_DimensionSize) < 0 )
    {
    // Error getting dimension sizes
    itkDebugMacro("Could not get dimension sizes!");
    return;
    }
 
  if ( miget_dimension_separations(this->m_MincFileDims, MI_ORDER_FILE, m_NDims, m_DimensionStep) < 0 )
    {
    itkDebugMacro(<<" Could not dimension sizes");
    return;
    }
    
  if ( miget_dimension_starts(this->m_MincFileDims, MI_ORDER_FILE, this->m_NDims, m_DimensionStart) < 0 )
    {
    itkDebugMacro(<<" Could not dimension sizes");
    return;
    }
    
  
  mitype_t volume_data_type;
  if ( miget_data_type(m_volume, &volume_data_type) < 0 )
    {
    itkDebugMacro(" Can not get volume data type!!\n");
    }

  // find out whether the data has slice scaling
  miboolean_t slice_scaling_flag=0;
  miboolean_t global_scaling_flag=0;
  
  if ( miget_slice_scaling_flag(m_volume, &slice_scaling_flag) < 0 )
    {
    itkDebugMacro(" Can not get slice scaling flag!!\n");
    }
    
  //voxel valid range
  double valid_min,valid_max;
  //get the voxel valid range
  if(miget_volume_valid_range(m_volume,&valid_max,&valid_min) < 0 )
  {
    itkDebugMacro(" Can not get volume valid range!!\n");
  }
  
  //real volume range, only awailable when slice scaling is off
  double volume_min=0.0,volume_max=1.0;
  if( !slice_scaling_flag )
  {
    if( miget_volume_range(m_volume,&valid_max,&valid_min) < 0 )
    {
      itkDebugMacro(" Can not get volume range!!\n");
    }
    global_scaling_flag=(volume_min==valid_min && volume_max==valid_max);
  }
  
  int spatial_dimension_count=0;
  
  // extract direction cosines
  for(int i=1;i<4;i++)
  {
    if(this->m_DimensionIndices[i]!=-1) //this dimension is present
    {
      spatial_dimension_count++;
    }
  }
  
  if ( spatial_dimension_count==0 ) // sorry, this is metaphysical question
  {
    itkDebugMacro(<< " minc files without spatial dimensions are not supported!");
    return;
  }
   
  if ( this->m_DimensionIndices[0]!=-1 && this->m_DimensionIndices[4]!=-1 )
  {
    itkDebugMacro(<< " 4D minc files vector dimension are not supported currently");
    return;
  }
  
  this->SetNumberOfDimensions(spatial_dimension_count);
  
  int numberOfComponents=1;
  int usable_dimensions=0;
  if(this->m_DimensionIndices[0]!=-1) // have vector dimension
  {
    //micopy_dimension(this->m_MincFileDims[this->m_DimensionIndices[0]],&apparent_dimension_order[usable_dimensions]);
    this->m_MincApparentDims[usable_dimensions]=this->m_MincFileDims[this->m_DimensionIndices[0]];
    //always use positive 
    miset_dimension_apparent_voxel_order(this->m_MincApparentDims[usable_dimensions],MI_POSITIVE);
    misize_t sz;
    miget_dimension_size(this->m_MincApparentDims[usable_dimensions],&sz);
    numberOfComponents=sz;
    usable_dimensions++;
  }
  
  if(this->m_DimensionIndices[4]!=-1) // have time dimension
  {
    //micopy_dimension(hdim[this->m_DimensionIndices[4]],&apparent_dimension_order[usable_dimensions]);
    this->m_MincApparentDims[usable_dimensions]=this->m_MincFileDims[this->m_DimensionIndices[4]];
    //always use positive 
    miset_dimension_apparent_voxel_order(this->m_MincApparentDims[usable_dimensions],MI_POSITIVE);
    misize_t sz;
    miget_dimension_size(this->m_MincApparentDims[usable_dimensions],&sz);
    numberOfComponents=sz;
    usable_dimensions++;
  }
  
  itk::Matrix< double, 3,3 > dir_cos;
  dir_cos.Fill(0.0);
  dir_cos.SetIdentity();
  
  itk::Vector< double,3> origin,sep;
  itk::Vector< double,3> o_origin;
  origin.Fill(0.0);
  o_origin.Fill(0.0);
  
  
  int spatial_dimension=0;
  for(int i=1;i<4;i++)
  {
    if(this->m_DimensionIndices[i]!=-1) 
    {
      //MINC2: bad design!
      //micopy_dimension(hdim[this->m_DimensionIndices[i]],&apparent_dimension_order[usable_dimensions]);
      this->m_MincApparentDims[usable_dimensions]=this->m_MincFileDims[this->m_DimensionIndices[i]];
      //always use positive 
      miset_dimension_apparent_voxel_order(this->m_MincApparentDims[usable_dimensions],MI_POSITIVE);
      misize_t sz;
      miget_dimension_size(this->m_MincApparentDims[usable_dimensions],&sz);
      this->SetDimensions(spatial_dimension,static_cast<unsigned int>(sz));
      
      std::vector< double > _dir(3);
      double _sep,_start;
      
      miget_dimension_separation(this->m_MincApparentDims[usable_dimensions],MI_ORDER_APPARENT,&_sep);
      miget_dimension_cosines(this->m_MincApparentDims[usable_dimensions],&_dir[0]);
      miget_dimension_start(this->m_MincApparentDims[usable_dimensions],MI_ORDER_APPARENT,&_start);
      
      for(int j=0;j<3;j++)
        dir_cos[j][i-1]=_dir[j];
      
      origin[i-1]=_start;
      sep[i-1]=_sep;
      
      this->SetDirection(spatial_dimension,_dir);
      this->SetSpacing(spatial_dimension,_sep);
      
      spatial_dimension++;
      usable_dimensions++;
    }
  }
  
  //Set apparent dimension order to the MINC2 api
  if(miset_apparent_dimension_order(m_volume,usable_dimensions,this->m_MincApparentDims)<0)
  {
    itkDebugMacro(<<" Can't set apparent dimension order!");
    return;
  }
  
  o_origin=dir_cos*origin;
  
  for(int i=0;i<spatial_dimension_count;i++)
    this->SetOrigin(i,o_origin[i]);
  
  miclass_t volume_data_class;

  if ( miget_data_class(m_volume, &volume_data_class) < 0 )
    {
    itkDebugMacro(" Could not get data class");
    return;
    }
    
  // set the file data type
  if(slice_scaling_flag || global_scaling_flag)
  {
    switch ( volume_data_type )
      {
      case MI_TYPE_FLOAT:
        this->SetComponentType(FLOAT);
        break;
      case MI_TYPE_DOUBLE:
        this->SetComponentType(DOUBLE);
        break;
      case MI_TYPE_FCOMPLEX:
        this->SetComponentType(FLOAT);
        break;
      case MI_TYPE_DCOMPLEX:
        this->SetComponentType(DOUBLE);
        break;
      default:
        this->SetComponentType(FLOAT);
        break;
      } //end of switch
    //file will have do 
  } else {
    switch ( volume_data_type )
      {
      case MI_TYPE_BYTE:
        this->SetComponentType(CHAR);
        break;
      case MI_TYPE_UBYTE:
        this->SetComponentType(UCHAR);
        break;
      case MI_TYPE_SHORT:
        this->SetComponentType(SHORT);
        break;
      case MI_TYPE_USHORT:
        this->SetComponentType(USHORT);
        break;
      case MI_TYPE_INT:
        this->SetComponentType(INT);
        break;
      case MI_TYPE_UINT:
        this->SetComponentType(UINT);
        break;
      case MI_TYPE_FLOAT:
        this->SetComponentType(FLOAT);
        break;
      case MI_TYPE_DOUBLE:
        this->SetComponentType(DOUBLE);
        break;
      case MI_TYPE_SCOMPLEX:
        this->SetComponentType(SHORT);
        break;
      case MI_TYPE_ICOMPLEX:
        this->SetComponentType(INT);
        break;
      case MI_TYPE_FCOMPLEX:
        this->SetComponentType(FLOAT);
        break;
      case MI_TYPE_DCOMPLEX:
        this->SetComponentType(DOUBLE);
        break;
      default:
        itkDebugMacro("Bad data type ");
        return;
      } //end of switch
  }

  switch ( volume_data_class )
    {
    case MI_CLASS_REAL:
      if(numberOfComponents==1)
        this->SetPixelType(SCALAR);
      else
        this->SetPixelType(VECTOR);//TODO: handle more types (i.e matrix, tensor etc)
      break;
    case MI_CLASS_INT:
      if(numberOfComponents==1)
        this->SetPixelType(SCALAR);
      else
        this->SetPixelType(VECTOR);//TODO: handle more types (i.e matrix, tensor etc)
      break;
    case MI_CLASS_LABEL:
      if(numberOfComponents==1)
        this->SetPixelType(SCALAR);
      else
        this->SetPixelType(VECTOR);
      // create an array of label names and values
      // not sure how to pass this to itk yet!
      break;
    case MI_CLASS_COMPLEX:
      //m_Complex = 1;
      this->SetPixelType(COMPLEX);
      numberOfComponents *= 2;
      break;
    default:
      itkDebugMacro("Bad data class ");
      return;
    } //end of switch
  
  this->SetNumberOfComponents(numberOfComponents);
  this->ComputeStrides();

  // Read Meta Data
  MetaDataDictionary & dict = this->GetMetaDataDictionary();

  dict.Clear();

  milisthandle_t hlist;
  char valuebuffer[256];

  int entryId = milist_start(m_volume, "/", 1, &hlist);

  if ( entryId == MI_NOERROR )
     {
     while( milist_grp_next(hlist, valuebuffer, sizeof(valuebuffer)) == MI_NOERROR )
       {
       std::string stringkey = "key";
       std::string stringvalue = valuebuffer;
       std::cout << stringvalue << std::endl;
       itk::EncapsulateMetaData< std::string >( dict, stringkey, stringvalue );
       }
     }

  milist_finish(hlist);

}

bool MINCImageIO::CanWriteFile(const char *name)
{
  std::string filename = name;

  // transform filename to lower case to make checks case-insensitive
  std::transform(filename.begin(), filename.end(), filename.begin(), ( int ( * )(int) ) std::tolower);

  if (  filename == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }

  std::string::size_type mncPos = filename.rfind(".mnc");
  if ( ( mncPos != std::string::npos )
       && ( mncPos > 0 )
       && ( mncPos == filename.length() - 4 ) )
    {
    return true;
    }

  mncPos = filename.rfind(".mnc2");
  if ( ( mncPos != std::string::npos )
       && ( mncPos > 0 )
       && ( mncPos == filename.length() - 5 ) )
    {
    return true;
    }

  return false;
}

/*
 * fill out the appropriate header information
*/
void MINCImageIO::WriteImageInformation(void)
{
  const unsigned int nDims = this->GetNumberOfDimensions();
  const unsigned int nComp = this->GetNumberOfComponents();

  CloseVolume();
  
  AllocateDimensions(nDims+(nComp>1?1:0));
  
  int minc_dimensions=0;
  if(nComp>3) //first dimension will be either vector or time
  {
     micreate_dimension(MItime, MI_DIMCLASS_TIME, MI_DIMATTR_REGULARLY_SAMPLED, nComp, &m_MincFileDims[minc_dimensions] );
     minc_dimensions++;
  } else if(nComp>1) {
     micreate_dimension(MIvector_dimension,MI_DIMCLASS_RECORD, MI_DIMATTR_REGULARLY_SAMPLED, nComp, &m_MincFileDims[minc_dimensions] );
     minc_dimensions++;
  }
  
  micreate_dimension(MIxspace,MI_DIMCLASS_SPATIAL, MI_DIMATTR_REGULARLY_SAMPLED, GetDimensions(0), &m_MincFileDims[minc_dimensions] );
  minc_dimensions++;
 
  if(nDims > 1)
  {
    micreate_dimension(MIyspace,MI_DIMCLASS_SPATIAL, MI_DIMATTR_REGULARLY_SAMPLED, GetDimensions(1), &m_MincFileDims[minc_dimensions] );
    minc_dimensions++;
  }
  
  if(nDims > 2)
  {
    micreate_dimension(MIzspace,MI_DIMCLASS_SPATIAL, MI_DIMATTR_REGULARLY_SAMPLED, GetDimensions(2), &m_MincFileDims[minc_dimensions] );
    minc_dimensions++;
  }
  
  if(nDims > 3)
  {
    itkDebugMacro("Unfortunately, only up to 3D volume are supported now.");
    return;
  }

  //allocating dimensions
  vnl_matrix< double > dircosmatrix(3, 3);
  dircosmatrix.set_identity();
  vnl_vector<double> origin(3);
  
  for (int i = 0; i < nDims; i++ )
    {
    for (int j = 0; j < 3; j++ )
      {
      dircosmatrix[i][j] = this->GetDirection(i)[j];
      }
     origin[i] = this->GetOrigin(i);
    }
  
  vnl_matrix< double > inverseDirectionCosines = vnl_matrix_inverse< double >(dircosmatrix);
  origin*=inverseDirectionCosines; //transform to minc convention
 
  for (int i = 0; i < nDims; i++ )
    {
      int j=i+(nComp>1?1:0);
      double dir_cos[3];
      for(int k=0;k<3;k++)
        dir_cos[k]=dircosmatrix[i][k];
      
      miset_dimension_separation(m_MincFileDims[j],this->GetSpacing(i));
      miset_dimension_start(m_MincFileDims[j],origin[i]);
      miset_dimension_cosines(m_MincFileDims[j],dir_cos);
    }

  //TODO: fix this to appropriate 
  m_volume_type=MI_TYPE_FLOAT;
  m_volume_class=MI_CLASS_REAL;
  

  switch(this->GetComponentType())
  {
    case UCHAR:
      m_volume_type=MI_TYPE_UBYTE;
      break;
    case CHAR:
      m_volume_type=MI_TYPE_BYTE;
      break;
    case USHORT:
      m_volume_type=MI_TYPE_USHORT;
      break;
    case SHORT:
      m_volume_type=MI_TYPE_SHORT;
      break;
    case UINT:  
      m_volume_type=MI_TYPE_UINT;
      break;
    case INT:
      m_volume_type=MI_TYPE_INT;
      break;
//     case ULONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_ULONG;
//       break;
//     case LONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_LONG;
//       break;
    case FLOAT://TODO: make sure we are cross-platform here!
      m_volume_type=MI_TYPE_FLOAT;
      break;
    case DOUBLE://TODO: make sure we are cross-platform here!
      m_volume_type=MI_TYPE_DOUBLE;
      break;
    default:
      itkDebugMacro(<<"Could read datatype " << this->GetComponentType() );
      return;
  }
  
  
  if ( micreate_volume ( m_FileName.c_str(), minc_dimensions, m_MincFileDims, m_volume_type,
                    m_volume_class, NULL, &m_volume )<0 )
    {
    // Error opening the volume
    itkDebugMacro("Could not open file \"" << m_FileName.c_str() << "\".");
    return;
    }
    
 if (  micreate_volume_image ( m_volume ) <0 )
 {
    // Error opening the volume
    itkDebugMacro("Could not create image in  file \"" << m_FileName.c_str() << "\".");
    return;
 }
 
  if( miset_slice_scaling_flag(m_volume, 0 )<0)
  {
    itkDebugMacro("Could not set slice scaling flag");
    return;
  }

  double valid_min,valid_max;
  miget_volume_valid_range(m_volume,&valid_max,&valid_min);
  //by default valid range will be equal to range, to avoid scaling
  miset_volume_range(m_volume,valid_max,valid_min);
  
  //TODO: write out metainformation somewhere
}


template<class T> void get_buffer_min_max(const void* _buffer,size_t len,double &buf_min,double &buf_max)
{
  const T* buf=static_cast<const T*>(_buffer);
  buf_min=buf_max=buf[0];
  for(size_t i=0;i<len;i++)
  {
    if(buf[i]<(double)buf_min) buf_min=(double)buf[i];
    if(buf[i]>(double)buf_max) buf_max=(double)buf[i];
  }
}

void MINCImageIO::Write(const void *buffer)
{
  const unsigned int nDims = this->GetNumberOfDimensions();
  const unsigned int nComp = this->GetNumberOfComponents();
  size_t buffer_length=1;

  misize_t *start=new misize_t[nDims+(nComp>1?1:0)];
  misize_t *count=new misize_t[nDims+(nComp>1?1:0)];
  
  if(nComp>1)
  {
    start[0]=0;
    count[0]=nComp;
    buffer_length*=nComp;
  }
  
 for ( unsigned int i = 0; i < nDims; i++ )
  {
  if ( i < m_IORegion.GetImageDimension() )
    {
    start[i+(nComp>1?1:0)] = m_IORegion.GetIndex()[i];
    count[i+(nComp>1?1:0)] = m_IORegion.GetSize()[i];
    buffer_length*=m_IORegion.GetSize()[i];
    }
  else
    {
    start[i+(nComp>1?1:0)] = 0;
    count[i+(nComp>1?1:0)] = 1;
    }
  }
  
  double buffer_min,buffer_max;
  mitype_t volume_data_type=MI_TYPE_UBYTE;
  
  switch(this->GetComponentType())
  {
    case UCHAR:
      volume_data_type=MI_TYPE_UBYTE;
      get_buffer_min_max<unsigned char>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case CHAR:
      volume_data_type=MI_TYPE_BYTE;
      get_buffer_min_max<char>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case USHORT:
      volume_data_type=MI_TYPE_USHORT;
      get_buffer_min_max<unsigned short>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case SHORT:
      volume_data_type=MI_TYPE_SHORT;
      get_buffer_min_max<short>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case UINT:  
      volume_data_type=MI_TYPE_UINT;
      get_buffer_min_max<unsigned int>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case INT:
      volume_data_type=MI_TYPE_INT;
      get_buffer_min_max<int>(buffer,buffer_length,buffer_min,buffer_max);
      break;
//     case ULONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_ULONG;
//       break;
//     case LONG://TODO: make sure we are cross-platform here!
//       volume_data_type=MI_TYPE_LONG;
//       break;
    case FLOAT://TODO: make sure we are cross-platform here!
      volume_data_type=MI_TYPE_FLOAT;
      get_buffer_min_max<float>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    case DOUBLE://TODO: make sure we are cross-platform here!
      volume_data_type=MI_TYPE_DOUBLE;
      get_buffer_min_max<double>(buffer,buffer_length,buffer_min,buffer_max);
      break;
    default:
      itkDebugMacro(<<"Could read datatype " << this->GetComponentType() );
      return;
  }
  this->WriteImageInformation();
  
  //by default valid range will be equal to range, to avoid scaling
  miset_volume_valid_range(m_volume,buffer_max,buffer_min);
  miset_volume_range(m_volume,buffer_max,buffer_min);
  
  
  
  if ( miset_real_value_hyperslab(m_volume, volume_data_type, start, count, const_cast<void*>( buffer) ) < 0 )
    {
    itkDebugMacro(" Can not set real value hyperslab!!\n");
    }
}


} // end namespace itk
