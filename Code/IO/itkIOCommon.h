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
#ifndef __itkIOCommon_h
#define __itkIOCommon_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkIntTypes.h"
#include "itkProcessObject.h"
#include "itkSpatialOrientation.h"

namespace itk
{
/** \class IOCommon
   * \brief Centralized funtionality for IO classes.
   *
   * This class provides encapsulated functionality to support the IO classes.
   *
   * \ingroup IOFilters
   *
   */
class ITKIO_EXPORT IOCommon
{
public:
  typedef enum {
    ITK_MAXPATHLEN = 2048, /**< Maximum length of a filename */
    MAX_FILENAMELIST_SIZE = 512
    } SysConstants;
  typedef enum {
    ITK_UCHAR,         // aka uint8_t
    ITK_CHAR,
    ITK_USHORT,        // aka uint16_t
    ITK_SHORT,
    ITK_UINT,          // aka uint32_t
    ITK_INT,
    ITK_ULONG,         // aka uint64_t
    ITK_LONG,
    ITK_FLOAT,
    ITK_DOUBLE
    } AtomicPixelType;   // enumerated constants for the different data types

  /** Convert the enumerated type to a string representation. */
  static std::string AtomicPixelTypeToString(const AtomicPixelType pixelType);

  /** Calculate the size, in bytes, that the atomic pixel type occupies. */
  static unsigned int ComputeSizeOfAtomicPixelType(const AtomicPixelType pixelType);
};

extern ITKIO_EXPORT const char *const ITK_OnDiskStorageTypeName;
extern ITKIO_EXPORT const char *const ITK_ImageFileBaseName;
extern ITKIO_EXPORT const char *const ITK_VoxelUnits;
extern ITKIO_EXPORT const char *const ITK_OnDiskBitPerPixel;
extern ITKIO_EXPORT const char *const SPM_ROI_SCALE;
extern ITKIO_EXPORT const char *const ITK_FileNotes;
extern ITKIO_EXPORT const char *const ITK_Origin;
extern ITKIO_EXPORT const char *const ITK_Spacing;
extern ITKIO_EXPORT const char *const ITK_FileOriginator;
extern ITKIO_EXPORT const char *const ITK_OriginationDate;
extern ITKIO_EXPORT const char *const ITK_PatientID;
extern ITKIO_EXPORT const char *const ITK_ExperimentDate;
extern ITKIO_EXPORT const char *const ITK_ExperimentTime;
extern ITKIO_EXPORT const char *const ITK_InputFilterName;
extern ITKIO_EXPORT const char *const ITK_NumberOfDimensions;
extern ITKIO_EXPORT const char *const ITK_ImageType;
extern ITKIO_EXPORT const char *const ITK_PatientName;
extern ITKIO_EXPORT const char *const ITK_ScanID;
extern ITKIO_EXPORT const char *const ROI_NAME;
extern ITKIO_EXPORT const char *const ROI_X_SIZE;
extern ITKIO_EXPORT const char *const ROI_X_RESOLUTION;
extern ITKIO_EXPORT const char *const ROI_Y_SIZE;
extern ITKIO_EXPORT const char *const ROI_Y_RESOLUTION;
extern ITKIO_EXPORT const char *const ROI_Z_SIZE;
extern ITKIO_EXPORT const char *const ROI_Z_RESOLUTION;
extern ITKIO_EXPORT const char *const ROI_NUM_SEGMENTS;
extern ITKIO_EXPORT const char *const ROI_PLANE;
extern ITKIO_EXPORT const char *const ROI_SCAN_ID;
} // end namespace itk

#endif // __itkIOCommon_h
