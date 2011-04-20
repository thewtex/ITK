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
#include "itkVersion.h"
#include "itkHDF5ImageIO.h"
#include "itkIOCommon.h"
#include "itkByteSwapper.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkArray.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkNumericTraits.h"
#include "itksys/SystemTools.hxx"
#include "itk_H5Cpp.h"
#include "itk_hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace itk
{

HDF5ImageIO::HDF5ImageIO() : m_H5File(0)
{
}

HDF5ImageIO::~HDF5ImageIO()
{
  delete this->m_H5File;
}

void
HDF5ImageIO
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

//
// strings defining HDF file layout for image data.
namespace
{
const std::string ItkVersion("/ITKVersion");
const std::string HDFVersion("/HDFVersion");
const std::string ImageGroup("/ITKImage");
const std::string Origin("/Origin");
const std::string Directions("/Directions");
const std::string Spacing("/Spacing");
const std::string Dimensions("/Dimension");
const std::string VoxelType("/VoxelType");
const std::string VoxelData("/VoxelData");
const std::string MetaDataName("/MetaData");

inline H5::PredType GetType(double *)
{
  return H5::PredType::NATIVE_DOUBLE;
}
inline H5::PredType GetType(float *)
{
  return H5::PredType::NATIVE_FLOAT;
}
inline H5::PredType GetType(int *)
{
  return H5::PredType::NATIVE_INT;
}
inline H5::PredType GetType(unsigned int *)
{
  return H5::PredType::NATIVE_UINT;
}
inline H5::PredType GetType(short *)
{
  return H5::PredType::NATIVE_SHORT;
}
inline H5::PredType GetType(unsigned short *)
{
  return H5::PredType::NATIVE_USHORT;
}
inline H5::PredType GetType(long *)
{
  return H5::PredType::NATIVE_LONG;
}
inline H5::PredType GetType(unsigned long *)
{
  return H5::PredType::NATIVE_ULONG;
}
inline H5::PredType GetType(unsigned char *)
{
  return H5::PredType::NATIVE_UCHAR;
}
inline H5::PredType GetType(char *)
{
  return H5::PredType::NATIVE_CHAR;
}
inline H5::PredType GetType(bool *)
{
  return H5::PredType::NATIVE_HBOOL;
}

inline
ImageIOBase::IOComponentType
PredTypeToComponentType(H5::DataType &type)
{
  if(type ==  H5::PredType::NATIVE_UCHAR)
    {
    return ImageIOBase::UCHAR;
    }
  if(type ==  H5::PredType::NATIVE_CHAR)
    {
    return ImageIOBase::CHAR;
    }
  if(type ==  H5::PredType::NATIVE_USHORT)
    {
    return ImageIOBase::USHORT;
    }
  if(type ==  H5::PredType::NATIVE_SHORT)
    {
    return ImageIOBase::SHORT;
    }
  if(type ==  H5::PredType::NATIVE_UINT)
    {
    return ImageIOBase::UINT;
    }
  if(type ==  H5::PredType::NATIVE_INT)
    {
    return ImageIOBase::INT;
    }
  if(type ==  H5::PredType::NATIVE_ULONG)
    {
    return ImageIOBase::ULONG;
    }
  if(type ==  H5::PredType::NATIVE_LONG)
    {
    return ImageIOBase::LONG;
    }
  if(type ==  H5::PredType::NATIVE_FLOAT)
    {
    return ImageIOBase::FLOAT;
    }
  if(type ==  H5::PredType::NATIVE_DOUBLE)
    {
    return ImageIOBase::DOUBLE;
    }
  itkGenericExceptionMacro(<< "unsupported data type "
                           << type.fromClass());
  // never reached but silences warning
  return ImageIOBase::UCHAR;
}

H5::PredType
ComponentToPredType(ImageIOBase::IOComponentType cType)
{
  switch ( cType )
    {
    case ImageIOBase::UCHAR:
      return H5::PredType::NATIVE_UCHAR;
    case ImageIOBase::CHAR:
      return H5::PredType::NATIVE_CHAR;
    case ImageIOBase::USHORT:
      return H5::PredType::NATIVE_USHORT;
    case ImageIOBase::SHORT:
      return H5::PredType::NATIVE_SHORT;
    case ImageIOBase::UINT:
      return H5::PredType::NATIVE_UINT;
    case ImageIOBase::INT:
      return H5::PredType::NATIVE_INT;
    case ImageIOBase::ULONG:
      return H5::PredType::NATIVE_ULONG;
    case ImageIOBase::LONG:
      return H5::PredType::NATIVE_LONG;
    case ImageIOBase::FLOAT:
      return H5::PredType::NATIVE_FLOAT;
    case ImageIOBase::DOUBLE:
      return H5::PredType::NATIVE_DOUBLE;
    default:
      break;
    }
  itkGenericExceptionMacro(<< "unsupported IOComponentType"
                           << cType);
  // never reached but silences warning.
  return H5::PredType::NATIVE_UCHAR;
}

std::string
ComponentToString(ImageIOBase::IOComponentType cType)
{
  std::string rval;
  switch ( cType )
    {
    case ImageIOBase::UCHAR:
      rval = "UCHAR";
      break;
    case ImageIOBase::CHAR:
      rval = "CHAR";
      break;
    case ImageIOBase::USHORT:
      rval = "USHORT";
      break;
    case ImageIOBase::SHORT:
      rval = "SHORT";
      break;
    case ImageIOBase::UINT:
      rval = "UINT";
      break;
    case ImageIOBase::INT:
      rval = "INT";
      break;
    case ImageIOBase::ULONG:
      rval = "ULONG";
      break;
    case ImageIOBase::LONG:
      rval = "LONG";
      break;
    case ImageIOBase::FLOAT:
      rval = "FLOAT";
      break;
    case ImageIOBase::DOUBLE:
      rval = "DOUBLE";
      break;
    default:
      itkGenericExceptionMacro(<< "unsupported IOComponentType"
                               << cType);
    }
  return rval;
}

}

void
HDF5ImageIO
::WriteScalar(const std::string &path,
              const bool &value)
{
  hsize_t numScalars(1);
  H5::DataSpace scalarSpace(1,&numScalars);
  H5::PredType scalarType =
    H5::PredType::NATIVE_HBOOL;
  H5::DataSet scalarSet =
    this->m_H5File->createDataSet(path,
                          scalarType,
                          scalarSpace);
  //
  // HDF5 can't distinguish
  // between bool and int datasets
  // in a disk file. So add an attribute
  // labeling this as a bool
  const std::string isBoolName("isBool");
  H5::Attribute isBool =
    scalarSet.createAttribute(isBoolName,
                               scalarType,
                               scalarSpace);
  bool trueVal(true);
  isBool.write(scalarType,&trueVal);
  isBool.close();

  scalarSet.write(&value,scalarType);
  scalarSet.close();
}

template <typename TScalar>
void
HDF5ImageIO
::WriteScalar(const std::string &path,
              const TScalar &value)
{
  hsize_t numScalars(1);
  H5::DataSpace scalarSpace(1,&numScalars);
  H5::PredType scalarType =
    GetType(static_cast<TScalar *>(0));
  H5::DataSet scalarSet =
    this->m_H5File->createDataSet(path,
                          scalarType,
                          scalarSpace);
  scalarSet.write(&value,scalarType);
  scalarSet.close();
}

template <typename TScalar>
TScalar
HDF5ImageIO
::ReadScalar(const std::string &DataSetName)
{
  hsize_t dim;
  H5::DataSet scalarSet = this->m_H5File->openDataSet(DataSetName);
  H5::DataSpace Space = scalarSet.getSpace();

  if(Space.getSimpleExtentNdims() != 1)
    {
    itkExceptionMacro(<< "Wrong # of dims for TransformType "
                      << "in HDF5 File");
    }
  Space.getSimpleExtentDims(&dim,0);
  if(dim != 1)
    {
    itkExceptionMacro(<< "Elements > 1 for scalar type "
                      << "in HDF5 File");
    }
  TScalar scalar;
  H5::PredType scalarType =
    GetType(static_cast<TScalar *>(0));
  scalarSet.read(&scalar,scalarType);
  scalarSet.close();
  return scalar;
}


void
HDF5ImageIO
::WriteString(const std::string &path,
                 const std::string &value)
{
  hsize_t numStrings(1);
  H5::DataSpace strSpace(1,&numStrings);
  H5::StrType strType(H5::PredType::C_S1,H5T_VARIABLE);
  H5::DataSet strSet = this->m_H5File->createDataSet(path,strType,strSpace);
  strSet.write(value,strType);
  strSet.close();
}

void
HDF5ImageIO
::WriteString(const std::string &path,
                 const char *s)
{
  std::string _s(s);
  WriteString(path,_s);
}

std::string
HDF5ImageIO
::ReadString(const std::string &path)
{
  std::string rval;
  hsize_t numStrings(1);
  H5::DataSpace strSpace(1,&numStrings);
  H5::StrType strType(H5::PredType::C_S1,H5T_VARIABLE);
  H5::DataSet strSet = this->m_H5File->openDataSet(path);
  strSet.read(rval,strType,strSpace);
  strSet.close();
  return rval;
}

template <typename TScalar>
void
HDF5ImageIO
::WriteVector(const std::string &path,
              const std::vector<TScalar> &vec)
{
  hsize_t dim(vec.size());
  TScalar *buf(new TScalar[dim]);
  for(unsigned i(0); i < dim; i++)
    {
    buf[i] = vec[i];
    }

  H5::DataSpace vecSpace(1,&dim);
  H5::PredType vecType =
    GetType(static_cast<TScalar *>(0));
  H5::DataSet vecSet =
    this->m_H5File->createDataSet(path,
                          vecType,
                          vecSpace);
  H5::PredType scalarType =
    GetType(static_cast<TScalar *>(0));
  vecSet.write(buf,scalarType);
  vecSet.close();
  delete [] buf;

}

template <typename TScalar>
std::vector<TScalar>
HDF5ImageIO
::ReadVector(const std::string &DataSetName)
{
  std::vector<TScalar> vec;
  hsize_t dim;
  H5::DataSet vecSet = this->m_H5File->openDataSet(DataSetName);
  H5::DataSpace Space = vecSet.getSpace();

  if(Space.getSimpleExtentNdims() != 1)
    {
    itkExceptionMacro(<< "Wrong # of dims for TransformType "
                      << "in HDF5 File");
    }
  Space.getSimpleExtentDims(&dim,0);
  vec.resize(dim);
  TScalar *buf = new TScalar[dim];
  H5::PredType vecType =
    GetType(static_cast<TScalar *>(0));
  vecSet.read(buf,vecType);
  for(unsigned i(0); i < dim; i++)
    {
    vec[i] = buf[i];
    }
  delete [] buf;
  vecSet.close();
  return vec;
}

void
HDF5ImageIO
::WriteDirections(const std::string &path,
                const std::vector<std::vector<double> > &dir)
{
  hsize_t dim[2];
  dim[1] = dir.size();
  dim[0] = dir[0].size();
  double *buf(new double[dim[0] * dim[1]]);
  unsigned k(0);
  for(unsigned i(0); i < dim[1]; i++)
    {
    for(unsigned j(0); j < dim[0]; j++)
      {
      buf[k] = dir[i][j];
      k++;
      }
    }

  H5::DataSpace dirSpace(2,dim);
  H5::DataSet dirSet = this->m_H5File->createDataSet(path,
                                             H5::PredType::NATIVE_DOUBLE,
                                             dirSpace);
  dirSet.write(buf,H5::PredType::NATIVE_DOUBLE);
  dirSet.close();
  delete [] buf;
}

std::vector<std::vector<double> >
HDF5ImageIO
::ReadDirections(const std::string &path)
{
  std::vector<std::vector<double> > rval;
  H5::DataSet dirSet = this->m_H5File->openDataSet(path);
  H5::DataSpace dirSpace = dirSet.getSpace();
  hsize_t dim[2];
  if(dirSpace.getSimpleExtentNdims() != 2)
    {
    itkExceptionMacro(<< " Wrong # of dims for Image Directions "
                      << "in HDF5 File");
    }
  dirSpace.getSimpleExtentDims(dim,0);
  rval.resize(dim[1]);
  for(unsigned i = 0; i < dim[1]; i++)
    {
    rval[i].resize(dim[0]);
    }
  H5::FloatType dirType = dirSet.getFloatType();
  if(dirType.getSize() == sizeof(double))
    {
    double *buf = new double[dim[0] * dim[1]];
    dirSet.read(buf,H5::PredType::NATIVE_DOUBLE);
    int k = 0;
    for(unsigned i(0); i < dim[1]; i++)
      {
      for(unsigned j(0); j < dim[0]; j++)
        {
        rval[i][j] = buf[k];
        k++;
        }
      }
    delete [] buf;
    }
  else
    {
    float *buf = new float[dim[0] * dim[1]];
    dirSet.read(buf,H5::PredType::NATIVE_FLOAT);
    int k = 0;
    for(unsigned i(0); i < dim[1]; i++)
      {
      for(unsigned j(0); j < dim[0]; j++)
        {
        rval[i][j] = buf[k];
        k++;
        }
      }
    delete [] buf;
    }
  dirSet.close();
  return rval;
}

template <typename TType>
void
HDF5ImageIO
::StoreMetaData(MetaDataDictionary *metaDict,
                const std::string &HDFPath,
                const std::string &name,
                unsigned long numElements)
{
  if(numElements == 1)
    {
    TType val = this->ReadScalar<TType>(HDFPath);
    EncapsulateMetaData<TType>(*metaDict,name,val);
    }
  else
    {
    //
    // store as itk::Array -- consistent with how
    // metadatadictionary actually is used in ITK
    std::vector<TType> valVec =
      this->ReadVector<TType>(HDFPath);
    Array<TType> val(valVec.size());
    for(unsigned int i = 0; i < val.GetSize(); i++)
      {
      val[i] = valVec[i];
      }
    EncapsulateMetaData<Array<TType> >(*metaDict,name,val);
    }
}

bool
HDF5ImageIO
::CanWriteFile(const char *FileNameToWrite)
{
  const char *extensions[] =
    {
      ".hdf",".h4",".hdf4",".h5",".hdf5",".he4",".he5",".hd5",0,
    };
  std::string ext
    (itksys::SystemTools::GetFilenameLastExtension(FileNameToWrite));
  for(unsigned i = 0; extensions[i] != 0; i++)
    {
    if(ext == extensions[i])
      {
      return true;
      }
    }
  return false;
}

// This method will only test if the header looks like an
// HDF5 Header.  Some code is redundant with ReadImageInformation
// a StateMachine could provide a better implementation
bool
HDF5ImageIO
::CanReadFile(const char *FileNameToRead)
{
 // call standard method to determine HDF-ness
  bool rval;
  // HDF5 is so exception happy, we have to worry about
  // it throwing a wobbly here if the file doesn't exist
  // or has some other problem.
  try
    {
    rval = H5::H5File::isHdf5(FileNameToRead);
    }
  catch(...)
    {
    rval = false;
    }
  return rval;
}


void
HDF5ImageIO
::ReadImageInformation()
{
  try
    {
    this->m_H5File = new H5::H5File(this->GetFileName(),
                                    H5F_ACC_RDONLY);
    std::string fileVersion =
      this->ReadString(ItkVersion);
    // not sure what to do with this initially
    std::string hdfVersion =
      this->ReadString(HDFVersion);

    std::string groupName(ImageGroup);
    H5::Group imageGroup(this->m_H5File->openGroup(groupName));
    groupName += "/0";
    H5::Group instanceGroup(this->m_H5File->openGroup(groupName));
    std::string OriginName(groupName);
    OriginName += Origin;
    this->m_Origin =
      this->ReadVector<double>(OriginName);

    std::string DirectionsName(groupName);
    DirectionsName += Directions;

    std::vector<std::vector<double> > directions =
      this->ReadDirections(DirectionsName);

    int numDims = directions.size();
    this->SetNumberOfDimensions(numDims);

    for(int i = 0; i < numDims; i++)
      {
      this->SetDirection(i,directions[i]);
      }

    std::string SpacingName(groupName);
    SpacingName += Spacing;
    std::vector<double> spacing =
      this->ReadVector<double>(SpacingName);
    for(int i = 0; i < numDims; i++)
      {
      this->SetSpacing(i,spacing[i]);
      }

    std::string DimensionsName(groupName);
    DimensionsName += Dimensions;

    {
    std::vector<ImageIOBase::SizeValueType> Dims =
      this->ReadVector<ImageIOBase::SizeValueType>(DimensionsName);
    for(int i = 0; i < numDims; i++)
      {
      this->SetDimensions(i,Dims[i]);
      }
    }
    std::string VoxelTypeName(groupName);
    VoxelTypeName += VoxelType;
    std::string typeVal = this->ReadString(VoxelTypeName);


    std::string VoxelDataName(groupName);
    VoxelDataName += VoxelData;
    H5::DataSet imageSet = this->m_H5File->openDataSet(VoxelDataName);
    H5::DataSpace imageSpace = imageSet.getSpace();

    //
    // set the componentType
    H5::DataType imageVoxelType = imageSet.getDataType();
    this->m_ComponentType = PredTypeToComponentType(imageVoxelType);
    //
    // if this isn't a scalar image, deduce the # of components
    // by comparing the size of the Directions matrix with the
    // reported # of dimensions in the voxel dataset
    hsize_t nDims = imageSpace.getSimpleExtentNdims();
    hsize_t *Dims = new hsize_t[nDims];
    imageSpace.getSimpleExtentDims(Dims);
    if(nDims > this->GetNumberOfDimensions())
      {
      this->SetNumberOfComponents(Dims[nDims - 1]);
      }
    delete [] Dims;

    //
    // read out metadata
    MetaDataDictionary & metaDict = this->GetMetaDataDictionary();

    std::string MetaDataGroupName(groupName);
    MetaDataGroupName += MetaDataName;
    MetaDataGroupName += "/";
    H5::Group metaGroup(this->m_H5File->openGroup(MetaDataGroupName));
    for(unsigned int i(0); i < metaGroup.getNumObjs(); i++)
      {
      H5std_string name = metaGroup.getObjnameByIdx(i);

      std::string MetaDataName(MetaDataGroupName);
      MetaDataName += name;
      H5::DataSet metaDataSet = this->m_H5File->openDataSet(MetaDataName);
      H5::DataType metaDataType = metaDataSet.getDataType();
      H5::DataSpace metaDataSpace = metaDataSet.getSpace();
      hsize_t nDims = metaDataSpace.getSimpleExtentNdims();
      if(nDims > 1)
        {
        // ignore > 1D metadata
        continue;
        }
      hsize_t Dims;
      metaDataSpace.getSimpleExtentDims(&Dims);
      //
      // work around bool/int confusion on disk.
      if(metaDataType == H5::PredType::NATIVE_INT)
        {
        if(metaDataSet.doesAttrExist("isBool"))
          {
          // itk::Array<bool> apparently can't
          // happen because vnl_vector<bool> isn't
          // instantiated
          bool val = this->ReadScalar<bool>(MetaDataName);
          EncapsulateMetaData<bool>(metaDict,name,val);
          }
        else
          {
          this->StoreMetaData<int>(&metaDict,
                              MetaDataName,
                              name,
                              Dims);
          }
        }
      else if(metaDataType == H5::PredType::NATIVE_CHAR)
        {
        this->StoreMetaData<char>(&metaDict,
                                  MetaDataName,
                                  name,
                                  Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_UCHAR)
        {
        this->StoreMetaData<unsigned char>(&metaDict,
                                           MetaDataName,
                                           name,
                                           Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_SHORT)
        {
        this->StoreMetaData<short>(&metaDict,
                                   MetaDataName,
                                   name,
                                   Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_USHORT)
        {
        this->StoreMetaData<unsigned short>(&metaDict,
                                            MetaDataName,
                                            name,
                                            Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_UINT)
        {
        this->StoreMetaData<unsigned int>(&metaDict,
                                          MetaDataName,
                                          name,
                                          Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_LONG)
        {
        this->StoreMetaData<long>(&metaDict,
                                  MetaDataName,
                                  name,
                                  Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_ULONG)
        {
        this->StoreMetaData<unsigned long>(&metaDict,
                                           MetaDataName,
                                           name,
                                           Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_FLOAT)
        {
        this->StoreMetaData<float>(&metaDict,
                                   MetaDataName,
                                   name,
                                   Dims);
        }
      else if(metaDataType == H5::PredType::NATIVE_DOUBLE)
        {
        this->StoreMetaData<double>(&metaDict,
                                    MetaDataName,
                                    name,
                                    Dims);
        }
      else
        {
        H5::StrType strType(H5::PredType::C_S1,H5T_VARIABLE);
        if(metaDataType == strType)
          {
          std::string val = this->ReadString(MetaDataName);
          EncapsulateMetaData<std::string>(metaDict,name,val);
          }
        }
      }
    imageSet.close();
    }
  // catch failure caused by the H5File operations
  catch( H5::AttributeIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  catch( H5::FileIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSet operations
  catch( H5::DataSetIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSpace operations
  catch( H5::DataSpaceIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSpace operations
  catch( H5::DataTypeIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
}

void
HDF5ImageIO
::Read(void *buffer)
{
  // HDF5 dimensions listed slowest moving first, ITK are fastest
  // moving first.
  std::string VoxelDataName(ImageGroup);
  VoxelDataName += "/0";
  VoxelDataName += VoxelData;
  H5::DataSet imageSet = this->m_H5File->openDataSet(VoxelDataName);
  H5::DataType voxelType = imageSet.getDataType();
  H5::DataSpace imageSpace = imageSet.getSpace();

  //
  int numComponents = this->GetNumberOfComponents();

  // get size of chunks
  // NOTE: for the purposes of reading chunks
  // we're just treating the voxel's components
  // as an 'extra' dimension.
  const int dim(this->GetNumberOfDimensions() +
                (numComponents > 1 ? 1 : 0));
  const H5::DSetCreatPropList props(imageSet.getCreatePlist());
  hsize_t *chunkDims = new hsize_t[dim];
  props.getChunk(dim,chunkDims);
  //  imageSet.read(buffer,voxelType);

  //
  // chunk size should be w/slowest moving dim
  // collapsed
  hsize_t *readChunkDims = new hsize_t[dim-1];
  hsize_t *offset = new hsize_t[dim];
  // start at zero...
  for(int i = 0; i < dim; i++)
    {
    offset[i] = 0;
    }
  //
  // need size in bytes to advance buffer ptr
  hsize_t chunkSize = voxelType.getSize();
  //
  // chunk is dims past slowest moving
  for(int i = 0; i < dim-1; i++)
    {
    chunkSize *=
      (readChunkDims[i] = chunkDims[i+1]);
    }
  //
  // buffer pointer
  char *readPtr = reinterpret_cast<char *>(buffer);
  //
  // find slowest moving.
  // take into account non-scalar pixels --
  // dim will be 1 more than ImageIOBase::GetNumberOfDimensions
  const int slowest_dim =
    numComponents > 1 ?
    this->GetDimensions(dim-2)
    : this->GetDimensions(dim-1);

  for(int i = 0; i < slowest_dim; i++)
    {
    offset[0] = i; // HDF puts slowest moving 1st
    H5::DataSpace dspace(dim-1,readChunkDims);
    imageSpace.selectHyperslab(H5S_SELECT_SET,chunkDims,offset);
    imageSet.read(readPtr,voxelType,dspace,imageSpace);
    readPtr += chunkSize;
    }
  imageSet.close();
  delete [] chunkDims;
  delete [] readChunkDims;
  delete [] offset;
}

template <typename TType>
bool
HDF5ImageIO
::WriteMeta(const std::string &name,MetaDataObjectBase *metaObjBase)
{
  MetaDataObject<TType> *metaObj =
    dynamic_cast<MetaDataObject<TType> *>(metaObjBase);
  if(metaObj == 0)
    {
    return false;
    }
  TType val = metaObj->GetMetaDataObjectValue();
  this->WriteScalar(name,val);
  return true;
}

template <typename TType>
bool
HDF5ImageIO
::WriteMetaArray(const std::string &name,MetaDataObjectBase *metaObjBase)
{
  typedef MetaDataObject< Array<TType> > MetaDataArrayObject;
    MetaDataArrayObject *metaObj =
    dynamic_cast<MetaDataArrayObject  *>(metaObjBase);
  if(metaObj == 0)
    {
    return false;
    }
  Array<TType> val = metaObj->GetMetaDataObjectValue();
  std::vector<TType> vecVal(val.GetSize());
  for(unsigned i = 0; i < val.size(); i++)
    {
    vecVal[i] = val[i];
    }
  this->WriteVector(name,vecVal);
  return true;
}
/**
 * For HDF5 this does not write a file, it only fills in the
 * appropriate header information.
 */
void
HDF5ImageIO
::WriteImageInformation(void)
{
  try
    {
    this->m_H5File = new H5::H5File(this->GetFileName(),
                                    H5F_ACC_TRUNC);
    this->WriteString(ItkVersion,
                      Version::GetITKVersion());

    this->WriteString(HDFVersion,
                                H5_VERS_INFO);
    std::string groupName(ImageGroup);
    H5::Group imageGroup(this->m_H5File->createGroup(groupName));
    groupName += "/0";
    H5::Group instanceGroup(this->m_H5File->createGroup(groupName));
    std::string OriginName(groupName);
    OriginName += Origin;
    this->WriteVector(OriginName,this->m_Origin);
    std::string DirectionsName(groupName);
    DirectionsName += Directions;
    this->WriteDirections(DirectionsName,this->m_Direction);
    std::string SpacingName(groupName);
    SpacingName += Spacing;
    this->WriteVector(SpacingName,
                      this->m_Spacing);

    std::string DimensionsName(groupName);
    DimensionsName += Dimensions;
    this->WriteVector(DimensionsName,
                      this->m_Dimensions);

    std::string VoxelTypeName(groupName);
    VoxelTypeName += VoxelType;
    std::string typeVal(ComponentToString(this->GetComponentType()));
    this->WriteString(VoxelTypeName,typeVal);

    std::string MetaDataGroupName(groupName);
    MetaDataGroupName += MetaDataName;
    H5::Group metaDataGroup =
      this->m_H5File->createGroup(MetaDataGroupName);
    //
    // MetaData.
    MetaDataDictionary & metaDict = this->GetMetaDataDictionary();
    MetaDataDictionary::Iterator it = metaDict.Begin(),
      end = metaDict.End();
    for(; it != end; it++)
      {
      MetaDataObjectBase *metaObj =
        it->second.GetPointer();
      std::string objName(MetaDataGroupName);
      objName += "/";
      objName += it->first;
      // scalars
      if(this->WriteMeta<bool>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<char>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<unsigned char>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<short>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<unsigned short>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<int>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<unsigned int>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<long>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<unsigned long>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<float>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMeta<double>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<char>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<unsigned char>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<short>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<unsigned short>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<int>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<unsigned int>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<long>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<unsigned long>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<float>(objName,metaObj))
        {
        continue;
        }
      if(this->WriteMetaArray<double>(objName,metaObj))
        {
        continue;
        }
      //
      // C String Arrays
      {
      MetaDataObject<char *> *cstringObj =
        dynamic_cast<MetaDataObject<char *> *>(metaObj);
      MetaDataObject<const char *> *constCstringObj =
        dynamic_cast<MetaDataObject<const char *> *>(metaObj);
      if(cstringObj != 0 || constCstringObj != 0)
        {
        const char *val;
        if(cstringObj != 0)
          {
          val = cstringObj->GetMetaDataObjectValue();
          }
        else
          {
          val = constCstringObj->GetMetaDataObjectValue();
          }
        this->WriteString(objName,val);
        continue;
        }
      }
      //
      // std::string
      {
      MetaDataObject<std::string> *stdStringObj =
        dynamic_cast<MetaDataObject<std::string> *>(metaObj);
      if(stdStringObj != 0)
        {
        std::string val = stdStringObj->GetMetaDataObjectValue();
        this->WriteString(objName,val);
        continue;
        }
      }
      }
    }
  // catch failure caused by the H5File operations
  catch( H5::FileIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSet operations
  catch( H5::DataSetIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSpace operations
  catch( H5::DataSpaceIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
  // catch failure caused by the DataSpace operations
  catch( H5::DataTypeIException error )
    {
    itkExceptionMacro(<< error.getCDetailMsg());
    }
}

/**
 * Write the image Information before writing data
 */
void
HDF5ImageIO
::Write(const void *buffer)
{
  this->WriteImageInformation();
  // HDF5 dimensions listed slowest moving first, ITK are fastest
  // moving first.
  int numComponents = this->GetNumberOfComponents();
  int numDims = this->GetNumberOfDimensions();
  hsize_t *dims = new hsize_t[numDims + (numComponents == 1 ? 0 : 1)];

  for(int i(0), j(numDims-1); i < numDims; i++, j--)
    {
      dims[j] = this->m_Dimensions[i];
    }
  if(numComponents > 1)
    {
    dims[numDims] = numComponents;
    numDims++;
    }
  H5::DataSpace imageSpace(numDims,dims);
  H5::PredType dataType = ComponentToPredType(this->GetComponentType());
  std::string VoxelDataName(ImageGroup);
  VoxelDataName += "/0";
  VoxelDataName += VoxelData;
  // set up properties for chunked, compressed writes.
  // in this case, set the chunk size to be the N-1 dimension
  // region
  H5::DSetCreatPropList plist;
  plist.setDeflate(5);
  dims[0] = 1;
  plist.setChunk(numDims,dims);

  H5::DataSet imageSet = this->m_H5File->createDataSet(VoxelDataName,
                                                       dataType,
                                                       imageSpace,plist);
  imageSet.write(buffer,dataType);
  imageSet.close();
  delete [] dims;
}

} // end namespace itk
