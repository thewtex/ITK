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
#include "itkMetaDataObject.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkNumericTraits.h"
#include "itksys/SystemTools.hxx"
#include "H5Cpp.h"
#include "hdf5.h"

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
inline H5::PredType GetType(long *)
{
  return H5::PredType::NATIVE_LONG;
}
inline H5::PredType GetType(unsigned long *)
{
  return H5::PredType::NATIVE_ULONG;
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
::WriteString(H5::H5File *h5file,
                 const std::string &path,
                 const std::string &value)
{
  hsize_t numStrings(1);
  H5::DataSpace strSpace(1,&numStrings);
  H5::StrType strType(H5::PredType::C_S1,H5T_VARIABLE);
  H5::DataSet strSet = h5file->createDataSet(path,strType,strSpace);
  strSet.write(value,strType);
  strSet.close();
}

void
HDF5ImageIO
::WriteString(H5::H5File *h5file,
                 const std::string &path,
                 const char *s)
{
  std::string _s(s);
  WriteString(h5file,path,_s);
}

std::string
HDF5ImageIO
::ReadString(H5::H5File *h5file,
             const std::string &path)
{
  std::string rval;
  hsize_t numStrings(1);
  H5::DataSpace strSpace(1,&numStrings);
  H5::StrType strType(H5::PredType::C_S1,H5T_VARIABLE);
  H5::DataSet strSet = h5file->openDataSet(path);
  strSet.read(rval,strType,strSpace);
  strSet.close();
  return rval;
}

template <typename TScalar>
void
HDF5ImageIO
::WriteVector(H5::H5File *h5file,
              const std::string &path,
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
    h5file->createDataSet(path,
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
::ReadVector(H5::H5File *h5file,
               const std::string &DataSetName)
{
  std::vector<TScalar> vec;
  hsize_t dim;
  H5::DataSet vecSet = h5file->openDataSet(DataSetName);
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
::WriteDirections(H5::H5File *h5file,
                const std::string &path,
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
  H5::DataSet dirSet = h5file->createDataSet(path,
                                             H5::PredType::NATIVE_DOUBLE,
                                             dirSpace);
  dirSet.write(buf,H5::PredType::NATIVE_DOUBLE);
  dirSet.close();
  delete [] buf;
}

std::vector<std::vector<double> >
HDF5ImageIO
::ReadDirections(H5::H5File *h5file,
               const std::string &path)
{
  std::vector<std::vector<double> > rval;
  H5::DataSet dirSet = h5file->openDataSet(path);
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
//
// File layout strings


void
HDF5ImageIO
::ReadImageInformation()
{
  try
    {
    this->m_H5File = new H5::H5File(this->GetFileName(),
                                    H5F_ACC_RDONLY);
    std::string fileVersion =
      this->ReadString(this->m_H5File,ItkVersion);
    // not sure what to do with this initially
    std::string hdfVersion =
      this->ReadString(this->m_H5File,HDFVersion);

    std::string groupName(ImageGroup);
    H5::Group imageGroup(this->m_H5File->openGroup(groupName));
    groupName += "/0";
    H5::Group instanceGroup(this->m_H5File->openGroup(groupName));
    std::string OriginName(groupName);
    OriginName += Origin;
    this->m_Origin =
      this->ReadVector<double>(this->m_H5File,OriginName);

    std::string DirectionsName(groupName);
    DirectionsName += Directions;

    std::vector<std::vector<double> > directions =
      this->ReadDirections(this->m_H5File,
                           DirectionsName);

    int numDims = directions.size();
    this->SetNumberOfDimensions(numDims);

    for(int i = 0; i < numDims; i++)
      {
      this->SetDirection(i,directions[i]);
      }

    std::string SpacingName(groupName);
    SpacingName += Spacing;
    std::vector<double> spacing =
      this->ReadVector<double>(this->m_H5File,SpacingName);
    for(int i = 0; i < numDims; i++)
      {
      this->SetSpacing(i,spacing[i]);
      }

    std::string DimensionsName(groupName);
    DimensionsName += Dimensions;

    {
    std::vector<ImageIOBase::SizeValueType> Dims =
      this->ReadVector<ImageIOBase::SizeValueType>(this->m_H5File,DimensionsName);
    for(int i = 0; i < numDims; i++)
      {
      this->SetDimensions(i,Dims[i]);
      }
    }
    std::string VoxelTypeName(groupName);
    VoxelTypeName += VoxelType;
    std::string typeVal = this->ReadString(this->m_H5File,VoxelTypeName);


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
    imageSet.close();
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
  imageSet.read(buffer,voxelType);
  imageSet.close();
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
    this->WriteString(this->m_H5File,ItkVersion,
                                Version::GetITKVersion());

    this->WriteString(this->m_H5File,HDFVersion,
                                H5_VERS_INFO);
    std::string groupName(ImageGroup);
    H5::Group imageGroup(this->m_H5File->createGroup(groupName));
    groupName += "/0";
    H5::Group instanceGroup(this->m_H5File->createGroup(groupName));
    std::string OriginName(groupName);
    OriginName += Origin;
    this->WriteVector(this->m_H5File,OriginName,
                                this->m_Origin);
    std::string DirectionsName(groupName);
    DirectionsName += Directions;
    this->WriteDirections(this->m_H5File,
                                    DirectionsName,
                                    this->m_Direction);
    std::string SpacingName(groupName);
    SpacingName += Spacing;
    this->WriteVector(this->m_H5File,SpacingName,
                      this->m_Spacing);

    std::string DimensionsName(groupName);
    DimensionsName += Dimensions;
    this->WriteVector(this->m_H5File,DimensionsName,
                      this->m_Dimensions);
    std::string VoxelTypeName(groupName);
    VoxelTypeName += VoxelType;
    std::string typeVal(ComponentToString(this->GetComponentType()));
    this->WriteString(this->m_H5File,VoxelTypeName,typeVal);
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

  for(unsigned i(0), j(numDims-1); i < numDims; i++, j--)
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
  H5::DataSet imageSet = this->m_H5File->createDataSet(VoxelDataName,
                                                       dataType,
                                                       imageSpace);
  imageSet.write(buffer,dataType);
  imageSet.close();
  delete [] dims;
}

} // end namespace itk
