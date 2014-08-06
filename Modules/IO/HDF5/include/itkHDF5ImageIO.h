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
/**
 *         The specification for this file format is taken from the
 *         web site http://analyzedirect.com/support/10.0Documents/Analyze_Resource_01.pdf
 * \author Hans J. Johnson
 *         The University of Iowa 2002
 */

#ifndef __itkHDF5ImageIO_h
#define __itkHDF5ImageIO_h
#include "ITKIOHDF5Export.h"


// itk namespace first suppresses
// kwstyle error for the H5 namespace below
namespace itk
{
}
namespace H5
{
class H5File;
class DataSpace;
class DataSet;
}

#include "itkStreamingImageIOBase.h"
#include <itkCommand.h>
#include "itk_H5Cpp.h"

namespace itk
{
class MetaDataObjectBase;
class MetaDataDictionary;
/** \class HDF5ImageIO
 *
 * \author Kent Williams
 * \brief Class that defines how to read HDF5 file format.
 * HDF5 IMAGE FILE FORMAT - As much information as I can determine from sourceforge.net/projects/HDF5lib
 *
 * \ingroup ITKIOHDF5
 *
 * HDF5 paths for elements in file
 * \li N is dimension of image
 * \li \/ITKVersion                   ITK Library Version string
 * \li \/HDFVersion                   HDF Version String
 * \li \/ITKImage                     Root Image Group
 * \li \/ITKImage\/\<name\>             name is arbitrary, but to parallel Transform I/O
 *                                    keep an image in a subgroup. The idea is to
 *                                    parallel transform file structure.
 * \li \/ITKImage\/\<name\>\/Origin     N-D point double
 * \li \/ITKImage\/\<name\>\/Directions N N-vectors double
 * \li \/ITKImage\/\<name\>\/Spacing    N-vector double
 * \li \/ITKImage\/\<name\>\/Dimensions N-vector ::itk::SizeValueType
 * \li \/ITKImage\/\<name\>\/VoxelType  String representing voxel type.
 *                             This can be inferred from the VoxelData
 *                             type info, but it makes the file more
 *                             user friendly with respect to HDF5 viewers.
 * \li \/ITKImage\/\<name\>\/VoxelData  multi-dim array of voxel data
 *                             in the case of non-scalar voxel type,
 *                             keep voxel components together, to make
 *                             loading possible without
 * \li \/ITKImage\/\<name\>\/MetaData   Group for storing metadata from
 *                             MetaDataDictionary
 * \li \/ITKImage\/\<name\>\/MetaData\/\<item-name\>
 *                             Dataset containing data for item-name
 *                             in the MetaDataDictionary
 * re-arrangement.
 *
 *
 */

class ITKIOHDF5_EXPORT HDF5ImageIO:public StreamingImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef HDF5ImageIO          Self;
  typedef ImageIOBase          Superclass;
  typedef SmartPointer< Self > Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(HDF5ImageIO, Superclass);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine if the file can be read with this ImageIO implementation.
   * \author Hans J Johnson
   * \param FileNameToRead The name of the file to test for reading.
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can read the file specified.
   */
  virtual bool CanReadFile(const char *FileNameToRead) ITK_OVERRIDE;

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation() ITK_OVERRIDE;

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void *buffer) ITK_OVERRIDE;

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine if the file can be written with this ImageIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \author Hans J. Johnson
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can write the file specified.
   */
  virtual bool CanWriteFile(const char *FileNameToWrite) ITK_OVERRIDE;

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation() ITK_OVERRIDE;

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void *buffer) ITK_OVERRIDE;

  itkGetConstMacro(H5RegionDim,const int);
  itkGetConstMacro(H5RegionStart,const hsize_t*);
  itkGetConstMacro(H5RegionSize,const hsize_t*);

protected:
  HDF5ImageIO();
  ~HDF5ImageIO();

  virtual SizeType GetHeaderSize(void) const ITK_OVERRIDE;

  virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

private:
  HDF5ImageIO(const Self &);   //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  void WriteString(const std::string &path,
                   const std::string &value);
  void WriteString(const std::string &path,
                   const char *s);
  std::string ReadString(const std::string &path);

  void WriteScalar(const std::string &path,
                   const bool &value);
  void WriteScalar(const std::string &path,
                   const long &value);
  void WriteScalar(const std::string &path,
                   const unsigned long &value);

  template <typename TScalar>
  void WriteScalar(const std::string &path,
                   const TScalar &value);

  template <typename TScalar>
  TScalar ReadScalar(const std::string &DataSetName);

  template <typename TScalar>
  void WriteVector(const std::string &path,
                   const std::vector<TScalar> &vec);

  template <typename TScalar>
  std::vector<TScalar> ReadVector(const std::string &DataSetName);

  void WriteDirections(const std::string &path,
                       const std::vector<std::vector<double> > &dir);

  std::vector<std::vector<double> > ReadDirections(const std::string &path);

  template <typename TType>
    bool WriteMeta(const std::string &name,
                   MetaDataObjectBase *metaObj);
  template <typename TType>
    bool WriteMetaArray(const std::string &name,
                   MetaDataObjectBase *metaObj);
  template <typename TType>
    void StoreMetaData(MetaDataDictionary *metaDict,
                       const std::string &HDFPath,
                       const std::string &name,
                       unsigned long numElements);
  void SetupStreaming(H5::DataSpace *imageSpace,
                      H5::DataSpace *slabSpace);
  H5::H5File  *m_H5File;
  H5::DataSet *m_VoxelDataSet;
  bool         m_ImageInformationWritten;

  int m_H5RegionDim;
  hsize_t m_H5RegionStart[4], m_H5RegionSize[4];
};

/** \class HDF5CommandRegionRecorder
 *
 * \author David Froger
 *
 * \brief Records debug informations about HDF5 read/write operation.
 *
 * \ingroup ITKIOHDF5
 *
 * HDF5 supports reading and writing in a subregion of an multidimensional
 * array stored in a file via the use of the selectHyperslab method.
 * The HDF5 reader/writer module in ITK make use of this HDF5 feature to
 * implement ITK streaming/pasting.
 *
 * HDF5ImageIO invokes an IterationEvent when it modifies the m_H5RegionStart
 * and m_H5RegionSize arrays. These arrays, which are passed to the HDF5
 * functions, define the subregion to read/write. Adding this class as an
 * observer of the IterationEvent allows to record the start of size of the
 * read/written regions, which provides usefull informations to debug and
 * test the streaming/pasting capability of this module.
 */
class HDF5CommandRegionRecorder : public itk::Command
{
public:
  /** Standard class typedefs. */
  typedef HDF5CommandRegionRecorder                    Self;
  typedef itk::Command                                 Superclass;
  typedef itk::SmartPointer<HDF5CommandRegionRecorder> Pointer;

  /** Type of the start or size of a region*/
  typedef std::vector<hsize_t> H5IndexType;

  /** Type of a list of H5IndexType*/
  typedef std::vector<H5IndexType> H5IndexHistoryType;

  /** Method for creation through the object factory. */
  itkNewMacro( HDF5CommandRegionRecorder );

protected:
  HDF5CommandRegionRecorder() {};

public:
  /** Record start and size of the current read/written region*/
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  /** Record start and size of the current read/written region*/
  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    const itk::HDF5ImageIO * filter = dynamic_cast< const itk::HDF5ImageIO * >( object );
    if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }

      H5IndexType H5RegionStart;
      H5IndexType H5RegionSize;

      for (int i=0; i<filter->GetH5RegionDim(); i++)
        {
        H5RegionStart.push_back( filter->GetH5RegionStart()[i] );
        H5RegionSize.push_back( filter->GetH5RegionSize()[i] );
        }

      m_StartHistory.push_back( H5RegionStart );
      m_SizeHistory.push_back( H5RegionSize );
  }

  /** Print history of all the read/written regions */
  void PrintHistory(const H5IndexHistoryType &startHistory,
    const H5IndexHistoryType &sizeHistory,
    std::ostream &out)
  {
    for (unsigned it = 0; it < sizeHistory.size(); it++)
      {
      out << "Iteration: " << it << " ";
      H5IndexType H5RegionStart = startHistory[it];
      H5IndexType H5RegionSize = sizeHistory[it];
      out << "start: [";
      for (unsigned i = 0; i < H5RegionStart.size(); i++)
        {
          out << H5RegionStart[i] << " ";
        }
      out << "] Size: [";
      for (unsigned i = 0; i < H5RegionSize.size(); i++)
        {
          out << H5RegionSize[i] << " ";
        }
        out << "]" << std::endl;
      }
  }

  /** Print self history of all the read/written regions */
  void PrintSelfHistory(std::ostream &out)
  {
    this->PrintHistory(this->m_StartHistory, this->m_SizeHistory, out);
  }

  /** Compare history of all the read/written regions with expected one
   * \return Returns true if this history is as expected.
   */
  bool CheckHistory(const H5IndexHistoryType &expectedStartHistory,
    const H5IndexHistoryType &expectedSizeHistory,
    std::ostream &out)
  {
    if (m_StartHistory == expectedStartHistory and m_SizeHistory == expectedSizeHistory)
      {
      out << "As expected, history is: " << std::endl;
      this->PrintHistory(m_StartHistory, m_SizeHistory, out);
      return true;
      }
    else
      {
      out << "Expected history:" << std::endl;
      this->PrintHistory(expectedStartHistory, expectedSizeHistory, out);
      out << "But got:" << std::endl;
      this->PrintHistory(m_StartHistory, m_SizeHistory, out);
      return false;
      }
  }

private:
  /** Historic of the start of the read/write regions */
  H5IndexHistoryType m_StartHistory;

  /** Historic of the size of the read/write regions */
  H5IndexHistoryType m_SizeHistory;
};

} // end namespace itk

#endif // __itkHDF5ImageIO_h
