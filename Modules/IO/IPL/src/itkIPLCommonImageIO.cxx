/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itksys/SystemTools.hxx"
#include "itkIPLCommonImageIO.h"
#include "itkByteSwapper.h"
#include "itkAnatomicalOrientation.h"
#include "itkDirectory.h"
#include "itkMetaDataObject.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cassert>
#include <vector>

// From uiig library "The University of Iowa Imaging Group-UIIG"

namespace itk
{
// Default constructor
IPLCommonImageIO::IPLCommonImageIO()
  : m_SystemByteOrder(ByteSwapper<int>::SystemIsBigEndian() ? IOByteOrderEnum::BigEndian
                                                            : IOByteOrderEnum::LittleEndian)
  , m_FilenameList(new IPLFileNameList)
{
  this->SetComponentType(IOComponentEnum::SHORT);
}

IPLCommonImageIO::~IPLCommonImageIO()
{
  delete m_ImageHeader;
  delete m_FilenameList;
}

void
IPLCommonImageIO::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

bool
IPLCommonImageIO::CanWriteFile(const char *)
{
  return false;
}

unsigned int
IPLCommonImageIO::GetComponentSize() const
{
  return sizeof(short);
}

void
IPLCommonImageIO::Read(void * buffer)
{
  auto * img_buffer = static_cast<short *>(buffer);
  auto   it = m_FilenameList->begin();
  auto   itend = m_FilenameList->end();

  for (; it != itend; ++it)
  {
    const std::string curfilename = (*it)->GetImageFileName();
    std::ifstream     f;
    this->OpenFileForReading(f, curfilename);

    f.seekg((*it)->GetSliceOffset(), std::ios::beg);
    if (!this->ReadBufferAsBinary(f, img_buffer, m_FilenameList->GetXDim() * m_FilenameList->GetYDim() * sizeof(short)))
    {
      f.close();
      RAISE_EXCEPTION();
    }
    f.close();
    // ByteSwapper::SwapRangeFromSystemToBigEndian is set up based on
    // the FILE endian-ness, not as the name would lead you to believe.
    // So, on LittleEndian systems, SwapFromSystemToBigEndian will swap.
    // On BigEndian systems, SwapFromSystemToBigEndian will do nothing.
    itk::ByteSwapper<short>::SwapRangeFromSystemToBigEndian(img_buffer,
                                                            m_FilenameList->GetXDim() * m_FilenameList->GetYDim());
    img_buffer += m_FilenameList->GetXDim() * m_FilenameList->GetYDim();
  }
}

GEImageHeader *
IPLCommonImageIO::ReadHeader(const char *)
{
  //
  // must be redefined in a child class
  //
  return nullptr;
}

bool
IPLCommonImageIO::CanReadFile(const char *)
{
  //
  // must be redefined in child class or you'll never read anything ;-)
  //
  return false;
}

void
IPLCommonImageIO::ReadImageInformation()
{
  std::string FileNameToRead = this->GetFileName();
  //
  // GE images are stored in separate files per slice.
  // char imagePath[IOCommon::ITK_MAXPATHLEN+1];
  // TODO -- use std::string instead of C strings
  char              imageMask[IOCommon::ITK_MAXPATHLEN + 1];
  char              imagePath[IOCommon::ITK_MAXPATHLEN + 1];
  const std::string _imagePath = itksys::SystemTools::CollapseFullPath(FileNameToRead);

  FileNameToRead = _imagePath;

  this->m_ImageHeader = this->ReadHeader(FileNameToRead.c_str());
  //
  // if anything fails in the header read, just let
  // exceptions propagate up.

  bool              isCT = false;
  const std::string modality = m_ImageHeader->modality;
  if (modality == "CT")
  {
    isCT = true;
  }

  AddElementToList(m_ImageHeader->filename,
                   m_ImageHeader->sliceLocation,
                   m_ImageHeader->offset,
                   m_ImageHeader->imageXsize,
                   m_ImageHeader->imageYsize,
                   m_ImageHeader->imageXres,
                   m_ImageHeader->imageYres,
                   m_ImageHeader->seriesNumber,
                   (isCT) ? m_ImageHeader->examNumber
                          : m_ImageHeader->echoNumber); // If CT use examNumber, otherwise use echoNumber.

  // Add header info to metadictionary

  itk::MetaDataDictionary & thisDic = this->GetMetaDataDictionary();
  const std::string         classname(this->GetNameOfClass());
  itk::EncapsulateMetaData<std::string>(thisDic, ITK_InputFilterName, classname);
  itk::EncapsulateMetaData<std::string>(thisDic, ITK_OnDiskStorageTypeName, std::string("SHORT"));
  itk::EncapsulateMetaData<short>(thisDic, ITK_OnDiskBitPerPixel, static_cast<short>(16));

  //
  // has to be set before setting dir cosines,
  // otherwise the vector doesn't get allocated
  this->SetNumberOfDimensions(3);

  itk::EncapsulateMetaData<std::string>(thisDic, ITK_PatientID, std::string(m_ImageHeader->patientId));
  itk::EncapsulateMetaData<std::string>(thisDic, ITK_ExperimentDate, std::string(m_ImageHeader->date));

  if (_imagePath.empty())
  {
    RAISE_EXCEPTION();
  }
  strncpy(imagePath, _imagePath.c_str(), sizeof(imagePath));
  imagePath[IOCommon::ITK_MAXPATHLEN] = '\0';
  strncpy(imageMask, imagePath, sizeof(imageMask));
  imageMask[IOCommon::ITK_MAXPATHLEN] = '\0';

  char * lastslash = strrchr(imagePath, '/');
  if (lastslash == nullptr)
  {
    strcpy(imagePath, ".");
  }
  else
  {
    *lastslash = '\0';
  }
  auto Dir = itk::Directory::New();
  if (Dir->Load(imagePath) == 0)
  {
    RAISE_EXCEPTION();
  }

  for (std::vector<std::string>::size_type i = 0, numfiles = Dir->GetNumberOfFiles(); i < numfiles; ++i)
  {
    const char * curFname = Dir->GetFile(static_cast<unsigned int>(i));

    if (curFname == nullptr)
    {
      break;
    }
    const std::string fullPath = std::string(imagePath) + "/" + curFname;
    if (FileNameToRead == fullPath) // strcmp(curFname,FileNameToRead) ==
                                    // 0)
    {
      continue;
    }
    GEImageHeader * curImageHeader = nullptr;
    try
    {
      curImageHeader = this->ReadHeader(fullPath.c_str());
    }
    catch (const itk::ExceptionObject &)
    {
      // ReadGE4XHeader throws an exception on any error.
      // So if, for example we run into a subdirectory, it would
      // throw an exception, and we'd just want to skip it.
      continue;
    }
    if ((((isCT) ? curImageHeader->examNumber : curImageHeader->echoNumber) == m_FilenameList->GetKey2()) &&
        (curImageHeader->seriesNumber == m_FilenameList->GetKey1()))
    {
      AddElementToList(curImageHeader->filename,
                       curImageHeader->sliceLocation,
                       curImageHeader->offset,
                       curImageHeader->imageXsize,
                       curImageHeader->imageYsize,
                       curImageHeader->imageXres,
                       curImageHeader->imageYres,
                       curImageHeader->seriesNumber,
                       (isCT) ? curImageHeader->examNumber
                              : curImageHeader->echoNumber); // If CT use examNumber, otherwise use echoNumber.
    }
    delete curImageHeader;
  }

  // sort image list
  m_FilenameList->sortImageList();

  //
  //
  // set the image properties
  this->SetDimensions(0, m_ImageHeader->imageXsize);
  this->SetDimensions(1, m_ImageHeader->imageYsize);
  this->SetDimensions(2, static_cast<unsigned int>(m_FilenameList->NumFiles()));
  this->SetSpacing(0, m_ImageHeader->imageXres);
  this->SetSpacing(1, m_ImageHeader->imageYres);
  this->SetSpacing(2, m_ImageHeader->sliceThickness + m_ImageHeader->sliceGap);

  //
  // set direction cosines
  AnatomicalOrientation::DirectionType dir =
    AnatomicalOrientation(m_ImageHeader->coordinateOrientation).GetAsDirection();
  std::vector<double> dirx(3, 0);
  dirx[0] = dir[0][0];
  dirx[1] = dir[1][0];
  dirx[2] = dir[2][0];
  std::vector<double> diry(3, 0);
  diry[0] = dir[0][1];
  diry[1] = dir[1][1];
  diry[2] = dir[2][1];
  std::vector<double> dirz(3, 0);
  dirz[0] = dir[0][2];
  dirz[1] = dir[1][2];
  dirz[2] = dir[2][2];

  this->SetDirection(0, dirx);
  this->SetDirection(1, diry);
  this->SetDirection(2, dirz);

  this->ModifyImageInformation();
}

void
IPLCommonImageIO::SortImageListByNameAscend()
{
  m_FilenameList->SetSortOrder(IPLFileNameList::SortByNameAscend);
}

void
IPLCommonImageIO::SortImageListByNameDescend()
{
  m_FilenameList->SetSortOrder(IPLFileNameList::SortByNameDescend);
}

/**
 *
 */
void
IPLCommonImageIO::WriteImageInformation()
{
  RAISE_EXCEPTION();
}

/**
 *
 */
void
IPLCommonImageIO::Write(const void *)
{
  RAISE_EXCEPTION();
}

int
IPLCommonImageIO::GetStringAt(std::ifstream & f, std::streamoff Offset, char * buf, size_t amount, bool throw_exception)
{
  f.seekg(Offset, std::ios::beg);
  if (f.fail())
  {
    if (throw_exception)
    {
      RAISE_EXCEPTION();
    }
    else
    {
      return -1;
    }
  }
  if (!this->ReadBufferAsBinary(f, (void *)buf, amount))
  {
    if (throw_exception)
    {
      RAISE_EXCEPTION();
    }
    else
    {
      return -1;
    }
  }
  return 0;
}

int
IPLCommonImageIO::GetIntAt(std::ifstream & f, std::streamoff Offset, int * ip, bool throw_exception)
{
  int tmp = 0;

  if (this->GetStringAt(f, Offset, reinterpret_cast<char *>(&tmp), sizeof(int), throw_exception) == 0)
  {
    *ip = this->hdr2Int(reinterpret_cast<char *>(&tmp));
  }
  else
  {
    *ip = 0;
  }
  return 0;
}

int
IPLCommonImageIO::GetShortAt(std::ifstream & f, std::streamoff Offset, short * ip, bool throw_exception)
{
  short tmp = 0;

  if (this->GetStringAt(f, Offset, reinterpret_cast<char *>(&tmp), sizeof(short), throw_exception) == 0)
  {
    *ip = this->hdr2Short(reinterpret_cast<char *>(&tmp));
  }
  else
  {
    *ip = 0;
  }
  return 0;
}

int
IPLCommonImageIO::GetFloatAt(std::ifstream & f, std::streamoff Offset, float * ip, bool throw_exception)
{
  float tmp = NAN;

  if (this->GetStringAt(f, Offset, reinterpret_cast<char *>(&tmp), sizeof(float), throw_exception) == 0)
  {
    *ip = this->hdr2Float(reinterpret_cast<char *>(&tmp));
  }
  else
  {
    *ip = 0.0f;
  }
  return 0;
}

int
IPLCommonImageIO::GetDoubleAt(std::ifstream & f, std::streamoff Offset, double * ip, bool throw_exception)
{
  double tmp = NAN;

  if (this->GetStringAt(f, Offset, reinterpret_cast<char *>(&tmp), sizeof(double), throw_exception) == 0)
  {
    *ip = this->hdr2Double(reinterpret_cast<char *>(&tmp));
  }
  else
  {
    *ip = 0.0;
  }
  return 0;
}

short
IPLCommonImageIO::hdr2Short(char * hdr)
{
  short shortValue = 0;

  memcpy(&shortValue, hdr, sizeof(short));
  ByteSwapper<short>::SwapFromSystemToBigEndian(&shortValue);
  return (shortValue);
}

int
IPLCommonImageIO::hdr2Int(char * hdr)
{
  int intValue = 0;

  memcpy(&intValue, hdr, sizeof(int));
  ByteSwapper<int>::SwapFromSystemToBigEndian(&intValue);
  return (intValue);
}

float
IPLCommonImageIO::hdr2Float(char * hdr)
{
  float floatValue = NAN;

  memcpy(&floatValue, hdr, 4);
  ByteSwapper<float>::SwapFromSystemToBigEndian(&floatValue);

  return (floatValue);
}

double
IPLCommonImageIO::hdr2Double(char * hdr)
{
  double doubleValue = NAN;

  memcpy(&doubleValue, hdr, sizeof(double));
  ByteSwapper<double>::SwapFromSystemToBigEndian(&doubleValue);

  return (doubleValue);
}

int
IPLCommonImageIO::AddElementToList(const char * const filename,
                                   const float        sliceLocation,
                                   const int          offset,
                                   const int          XDim,
                                   const int          YDim,
                                   const float        XRes,
                                   const float        YRes,
                                   const int          Key1,
                                   const int          Key2)
{
  if (m_FilenameList->NumFiles() == 0)
  {
    m_FilenameList->SetXDim(XDim);
    m_FilenameList->SetYDim(YDim);
    m_FilenameList->SetXRes(XRes);
    m_FilenameList->SetYRes(YRes);
    m_FilenameList->SetKey1(Key1);
    m_FilenameList->SetKey2(Key2);
  }
  else if (XDim != m_FilenameList->GetXDim() || YDim != m_FilenameList->GetYDim())
  {
    return 0;
  }
  else if (itk::Math::NotAlmostEquals(XRes, m_FilenameList->GetXRes()) ||
           itk::Math::NotAlmostEquals(YRes, m_FilenameList->GetYRes()))
  {
    return 0;
  }
  else if (m_FilenameList->GetKey1() != Key1 || m_FilenameList->GetKey2() != Key2)
  {
    return 1; // It is OK for keys to not match,  Just don't add.
  }
  m_FilenameList->AddElementToList(filename, sliceLocation, offset, XDim, YDim, XRes, YRes, 0, Key1, Key2);
  return 1;
}

void
IPLCommonImageIO::sortImageListAscend()
{
  m_FilenameList->sortImageListAscend();
}

void
IPLCommonImageIO::sortImageListDescend()
{
  m_FilenameList->sortImageListDescend();
}

int
IPLCommonImageIO::statTimeToAscii(void * clock, char * timeString, int len)
{

  auto               tclock = static_cast<time_t>(*(static_cast<int *>(clock)));
  const char * const asciiTime = ctime(&tclock);

  strncpy(timeString, asciiTime, len);
  timeString[len - 1] = '\0';
  char * newline = nullptr;
  if ((newline = strrchr(timeString, '\n')) != nullptr || (newline = strrchr(timeString, '\r')))
  {
    *newline = '\0';
  }
  return 1;
}
} // end namespace itk
