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
#include "itkGE4ImageIO.h"
#include "itkByteSwapper.h"
#include "Ge4xHdr.h"
#include "itksys/SystemTools.hxx"
#include "itkBitCast.h"
#include <iostream>
#include <fstream>
// From uiig library "The University of Iowa Imaging Group-UIIG"

namespace itk
{
// Default constructor
GE4ImageIO::GE4ImageIO() = default;
GE4ImageIO::~GE4ImageIO() = default;

bool
GE4ImageIO::CanReadFile(const char * FileNameToRead)
{
  char tmpStr[64];

  std::ifstream f;
  try
  {
    this->OpenFileForReading(f, FileNameToRead);
  }
  catch (const ExceptionObject &)
  {
    return false;
  }

  // This is a weak heuristic but should only be true for GE4 files
  //
  // Get the Plane from the IMAGE Header.
  if (this->GetStringAt(f, SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_PLANENAME * 2, tmpStr, 16, false) == -1)
  {
    f.close();
    return false;
  }
  tmpStr[16] = '\0';
  // if none of these strings show up, most likely not GE4
  if (strstr(tmpStr, "CORONAL") == nullptr && strstr(tmpStr, "SAGITTAL") == nullptr &&
      strstr(tmpStr, "AXIAL") == nullptr && strstr(tmpStr, "OBLIQUE") == nullptr)
  {
    f.close();
    return false;
  }
  //
  // doesn't appear to be any signature in the header so I guess
  // I have to assume it's readable
  f.close();
  return true;
}

GEImageHeader *
GE4ImageIO::ReadHeader(const char * FileNameToRead)
{
  if (FileNameToRead == nullptr || strlen(FileNameToRead) == 0)
  {
    return nullptr;
  }
  //
  // need to check if this is a valid file before going further
  if (!this->CanReadFile(FileNameToRead))
  {
    RAISE_EXCEPTION();
  }
  auto * hdr = new GEImageHeader;
  // Set modality to UNKNOWN
  strcpy(hdr->modality, "UNK");

  char tmpStr[IOCommon::ITK_MAXPATHLEN + 1];

  //
  // save off the name of the current file...
  strncpy(hdr->filename, FileNameToRead, sizeof(hdr->filename) - 1);
  hdr->filename[sizeof(hdr->filename) - 1] = '\0';

  //
  // Next, can you open it?

  std::ifstream f;
  this->OpenFileForReading(f, FileNameToRead);

  this->GetStringAt(f, SIGNA_STHDR_START * 2 + SIGNA_STHDR_DATE_ASCII * 2, tmpStr, 10);
  tmpStr[10] = '\0';
  strncpy(hdr->date, tmpStr, sizeof(hdr->date) - 1);
  hdr->date[sizeof(hdr->date) - 1] = '\0';

  // Get Patient-Name from the STUDY Header
  this->GetStringAt(f, SIGNA_STHDR_START * 2 + SIGNA_STHDR_PATIENT_NAME * 2, tmpStr, 32);
  tmpStr[32] = '\0';
  strncpy(hdr->hospital, tmpStr, sizeof(hdr->hospital) - 1);
  hdr->hospital[sizeof(hdr->hospital) - 1] = '\0';

  /* Get Patient-Number from the STUDY Header */
  this->GetStringAt(f, SIGNA_STHDR_START * 2 + SIGNA_STHDR_PATIENT_ID * 2, tmpStr, 12);
  tmpStr[12] = '\0';
  strncpy(hdr->patientId, tmpStr, sizeof(hdr->patientId) - 1);
  hdr->patientId[sizeof(hdr->patientId) - 1] = '\0';

  /* Get the Exam-Number from the STUDY Header */
  this->GetStringAt(f, SIGNA_STHDR_START * 2 + SIGNA_STHDR_STUDY_NUM * 2, tmpStr, 6);
  tmpStr[6] = '\0';
  strncpy(hdr->scanId, tmpStr, sizeof(hdr->scanId) - 1);
  hdr->scanId[sizeof(hdr->scanId) - 1] = '\0';

  /* Get the FOV from the SERIES Header */
  f.seekg(SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_FOV * 2, std::ios::beg);
  IOCHECK();
  int intTmp = 0;
  f.read(reinterpret_cast<char *>(&intTmp), sizeof(intTmp));
  IOCHECK();
  const float tmpFloat = MvtSunf(intTmp);

  hdr->xFOV = tmpFloat;
  hdr->yFOV = hdr->xFOV;

  /* Get the Plane from the IMAGE Header */
  this->GetStringAt(f, SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_PLANENAME * 2, tmpStr, 16);
  tmpStr[16] = '\0';

  if (strstr(tmpStr, "CORONAL") != nullptr)
  {
    hdr->coordinateOrientation = AnatomicalOrientation(AnatomicalOrientation::CoordinateEnum::RightToLeft,
                                                       AnatomicalOrientation::CoordinateEnum::SuperiorToInferior,
                                                       AnatomicalOrientation::CoordinateEnum::PosteriorToAnterior);
  }
  else if (strstr(tmpStr, "SAGITTAL") != nullptr)
  {
    hdr->coordinateOrientation = AnatomicalOrientation(AnatomicalOrientation::CoordinateEnum::AnteriorToPosterior,
                                                       AnatomicalOrientation::CoordinateEnum::InferiorToSuperior,
                                                       AnatomicalOrientation::CoordinateEnum::RightToLeft);
  }
  else if (strstr(tmpStr, "AXIAL") != nullptr)
  {
    hdr->coordinateOrientation = AnatomicalOrientation(AnatomicalOrientation::CoordinateEnum::RightToLeft,
                                                       AnatomicalOrientation::CoordinateEnum::AnteriorToPosterior,
                                                       AnatomicalOrientation::CoordinateEnum::InferiorToSuperior);
  }
  else
  {
    hdr->coordinateOrientation = AnatomicalOrientation(AnatomicalOrientation::CoordinateEnum::RightToLeft,
                                                       AnatomicalOrientation::CoordinateEnum::SuperiorToInferior,
                                                       AnatomicalOrientation::CoordinateEnum::PosteriorToAnterior);
  }

  /* Get the Scan Matrix from the IMAGE Header */
  this->GetShortAt(f, SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_SCANMATRIXX * 2, &(hdr->acqXsize));
  this->GetShortAt(f, (SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_SCANMATRIXY * 2) + sizeof(short), &(hdr->acqYsize));

  /* Get Series-Number from SERIES Header */
  this->GetStringAt(f, SIGNA_SEHDR_START * 2 + SIGNA_SEHDR_SERIES_NUM * 2, tmpStr, 3);
  tmpStr[3] = '\0';
  hdr->seriesNumber = std::stoi(tmpStr);

  /* Get Image-Number from IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_IMAGE_NUM * 2, tmpStr, 3);
  tmpStr[3] = '\0';
  hdr->imageNumber = std::stoi(tmpStr);

  /* Get Images-Per-Slice from IMAGE Header */
  const int per_slice_status = this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_PHASENUM * 2, tmpStr, 3);
  tmpStr[3] = '\0';
  if (strlen(tmpStr) > 0 && per_slice_status >= 0)
  {
    hdr->imagesPerSlice = static_cast<short>(std::stoi(tmpStr));
  }
  else
  {
    hdr->imagesPerSlice = 0; // Use default of 0 to mimic previous atoi failure result.
  }

  /* Get the Slice Location from the IMAGE Header */
  // hack alert -- and this goes back to a hack in the original code
  // you read in an integer, but you DON'T byte swap it, and then pass
  // it into the MvtSunf function to get the floating point value.
  // to circumvent byte swapping in GetIntAt, use GetStringAt
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_SLICELOC * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->sliceLocation = MvtSunf(intTmp);

  this->GetStringAt(
    f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_SLICE_THICK * 2, reinterpret_cast<char *>(&intTmp), sizeof(intTmp));

  hdr->sliceThickness = MvtSunf(intTmp);

  /* Get the Slice Spacing from the IMAGE Header */
  this->GetStringAt(
    f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_SLICE_SPACING * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->sliceGap = MvtSunf(intTmp);

  /* Get TR from the IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_TR * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->TR = MvtSunf(intTmp);

  /* Get TE from the IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_TE * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->TE = MvtSunf(intTmp);

  /* Get TI from the IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_TI * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->TI = MvtSunf(intTmp);

  /* Get Number of Echos from the IMAGE Header */
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_NUMECHOS * 2, &(hdr->numberOfEchoes));

  /* Get Echo Number from the IMAGE Header */
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_ECHONUM * 2, &(hdr->echoNumber));

  /* Get PSD-Name from the IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_PSD_NAME * 2, tmpStr, 12);
  tmpStr[12] = '\0';

  /* Get X Pixel Dimension from the IMAGE Header */
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_X_DIM * 2, &(hdr->imageXsize));

  /* Get Y Pixel Dimension from the IMAGE Header */
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_Y_DIM * 2, &(hdr->imageYsize));

  /* Get Pixel Size from the IMAGE Header */
  this->GetStringAt(
    f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_PIXELSIZE * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->imageXres = MvtSunf(intTmp);
  hdr->imageYres = hdr->imageXres;

  /* Get NEX from the IMAGE Header */
  this->GetStringAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_NEX * 2, reinterpret_cast<char *>(&intTmp), sizeof(int));

  hdr->NEX = static_cast<short>(MvtSunf(intTmp));

  /* Get Flip Angle from the IMAGE Header */
  short tmpShort = 0;
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_FLIP * 2, &tmpShort);

  if (tmpShort > 0)
  {
    hdr->flipAngle = static_cast<int>(tmpShort);
  }
  else
  {
    hdr->flipAngle = 90;
  }

  // DEBUG: HACK -- what should pulse sequence be?  Is it valid for 4x filters
  // Just setting it to dummy value -- Hans
  // copy from ge5x strncpy (hdr->pulseSequence, &hdr[IM_HDR_START + IM_PSDNAME],
  // 31);
  strncpy(hdr->pulseSequence, "UNKNOWN_GE4x_PULSE_SEQUENCE", 31);
  hdr->pulseSequence[31] = '\0';

  /* Get the Number of Images from the IMAGE Header */
  this->GetShortAt(f, SIGNA_IHDR_START * 2 + SIGNA_IMHDR_NUMSLICES * 2, &(hdr->numberOfSlices));

  //    status = stat (imageFile, &statBuf);
  //    if (status == -1)
  //      {
  //  return (nullptr);
  //      }
  //
  //    hdr->offset = statBuf.st_size - (hdr->imageXsize * hdr->imageYsize * 2);
  //
  // find file length in line ...
  const SizeValueType file_length = itksys::SystemTools::FileLength(FileNameToRead);

  hdr->offset = file_length - (hdr->imageXsize * hdr->imageYsize * 2);
  return hdr;
}

float
GE4ImageIO::MvtSunf(int numb)
{
  constexpr auto signbit = 020000000000U;
  constexpr auto dmantissa = 077777777U;
  constexpr auto dexponent = 0177U;
  constexpr auto smantissa = 037777777U;
  constexpr auto smantlen = 23U;
  ByteSwapper<int>::SwapFromSystemToBigEndian(&numb);
  const unsigned int dg_exp = (numb >> 24) & dexponent;
  const unsigned int dg_sign = numb & signbit;
  unsigned int       dg_mantissa = (numb & dmantissa) << 8;
  int                sun_exp = 4 * (dg_exp - 64);
  while ((dg_mantissa & signbit) == 0 && dg_mantissa != 0)
  {
    --sun_exp;
    dg_mantissa = dg_mantissa << 1;
  }
  sun_exp += 126;
  if (sun_exp < 0)
  {
    sun_exp = 0;
  }
  else if (sun_exp > 255)
  {
    sun_exp = 255;
  }
  dg_mantissa = dg_mantissa << 1;
  const unsigned int sun_num{ dg_sign | (sun_exp << smantlen) | ((dg_mantissa >> 9) & smantissa) };
  return bit_cast<float>(sun_num);
}
} // end namespace itk
