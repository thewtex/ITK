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

#include "itkGDCMSeriesFileNames.h"
#include "itksys/SystemTools.hxx"
#include "itkProgressReporter.h"
#include "itkPrintHelper.h"
#include "gdcmSerieHelper.h"

namespace itk
{


GDCMSeriesFileNames::GDCMSeriesFileNames()
  : m_SerieHelper{ new gdcm::SerieHelper() }
{}

GDCMSeriesFileNames::~GDCMSeriesFileNames() = default;


void
GDCMSeriesFileNames::SetInputDirectory(const char * name)
{
  if (!name)
  {
    itkExceptionMacro("SetInputDirectory() received a nullptr string");
  }
  const std::string fname = name;
  this->SetInputDirectory(fname);
}

void
GDCMSeriesFileNames::AddSeriesRestriction(const std::string & tag)
{
  m_SerieHelper->AddRestriction(tag);
}

void
GDCMSeriesFileNames::SetInputDirectory(const std::string & name)
{
  if (name.empty())
  {
    itkWarningMacro("You need to specify a directory where "
                    "the DICOM files are located");
    return;
  }
  if (m_InputDirectory == name)
  {
    return;
  }
  if (!itksys::SystemTools::FileIsDirectory(name.c_str()))
  {
    itkWarningMacro(<< name << " is not a directory");
    return;
  }
  m_InputDirectory = name;
  m_SerieHelper->Clear();
  m_SerieHelper->SetUseSeriesDetails(m_UseSeriesDetails);
  m_SerieHelper->SetLoadMode((m_LoadSequences ? 0 : gdcm::LD_NOSEQ) | (m_LoadPrivateTags ? 0 : gdcm::LD_NOSHADOW));
  m_SerieHelper->SetDirectory(name, m_Recursive);
  // as a side effect it also execute
  this->Modified();
}

const GDCMSeriesFileNames::SeriesUIDContainerType &
GDCMSeriesFileNames::GetSeriesUIDs()
{
  m_SeriesUIDs.clear();
  // Accessing the first serie found (assume there is at least one)
  gdcm::FileList * flist = m_SerieHelper->GetFirstSingleSerieUIDFileSet();
  while (flist)
  {
    if (!flist->empty()) // make sure we have at least one serie
    {
      gdcm::File * file = (*flist)[0]; // for example take the first one

      // Create its unique series ID
      const std::string id = m_SerieHelper->CreateUniqueSeriesIdentifier(file).c_str();

      m_SeriesUIDs.emplace_back(id.c_str());
    }
    flist = m_SerieHelper->GetNextSingleSerieUIDFileSet();
  }
  if (m_SeriesUIDs.empty())
  {
    itkWarningMacro("No Series were found");
  }
  return m_SeriesUIDs;
}

const GDCMSeriesFileNames::FileNamesContainerType &
GDCMSeriesFileNames::GetFileNames(const std::string serie)
{
  m_InputFileNames.clear();
  // Accessing the first serie found (assume there is at least one)
  gdcm::FileList * flist = m_SerieHelper->GetFirstSingleSerieUIDFileSet();
  if (!flist)
  {
    itkWarningMacro("No Series can be found, make sure your restrictions are not too strong");
    return m_InputFileNames;
  }
  if (!serie.empty()) // user did not specify any sub selection based on UID
  {
    bool found = false;
    while (flist && !found)
    {
      if (!flist->empty()) // make sure we have at least one serie
      {
        gdcm::File *      file = (*flist)[0]; // for example take the first one
        const std::string id = m_SerieHelper->CreateUniqueSeriesIdentifier(file).c_str();

        if (id == serie)
        {
          found = true; // we found a match
          break;
        }
      }
      flist = m_SerieHelper->GetNextSingleSerieUIDFileSet();
    }
    if (!found)
    {
      itkWarningMacro("No Series were found");
      return m_InputFileNames;
    }
  }
  m_SerieHelper->OrderFileList(flist);

  if (!flist->empty())
  {
    ProgressReporter progress(this, 0, static_cast<itk::SizeValueType>(flist->size()), 10);
    for (auto & element : *flist)
    {
      gdcm::FileWithName * header = element;
      m_InputFileNames.push_back(header->filename);
      progress.CompletedPixel();
    }
  }
  else
  {
    itkDebugMacro("No files were found");
  }

  return m_InputFileNames;
}

const GDCMSeriesFileNames::FileNamesContainerType &
GDCMSeriesFileNames::GetInputFileNames()
{
  // Do not specify any UID
  return this->GetFileNames("");
}

const GDCMSeriesFileNames::FileNamesContainerType &
GDCMSeriesFileNames::GetOutputFileNames()
{
  // We are trying to extract the original filename and compose it with a path:

  // There are two different approaches if directory does not exist:
  // 1. Exit
  // 2. Mkdir
  // bool SystemTools::FileExists(const char* filename)
  // bool SystemTools::FileIsDirectory(const char* name)
  m_OutputFileNames.clear();

  if (m_OutputDirectory.empty())
  {
    itkDebugMacro("No output directory was specified");
    return m_OutputFileNames;
  }

  itksys::SystemTools::ConvertToUnixSlashes(m_OutputDirectory);
  if (m_OutputDirectory.back() != '/')
  {
    m_OutputDirectory += '/';
  }

  if (!m_InputFileNames.empty())
  {
    bool hasExtension = false;
    for (const auto & inputFileName : m_InputFileNames)
    {
      // look for extension ".dcm" and ".DCM"
      std::string::size_type dcmPos = inputFileName.rfind(".dcm");
      if ((dcmPos != std::string::npos) && (dcmPos == inputFileName.length() - 4))
      {
        hasExtension = true;
      }
      else
      {
        dcmPos = inputFileName.rfind(".DCM");
        if ((dcmPos != std::string::npos) && (dcmPos == inputFileName.length() - 4))
        {
          hasExtension = true;
        }
      }

      // look for extension ".dicom" and ".DICOM"
      std::string::size_type dicomPos = inputFileName.rfind(".dicom");
      if ((dicomPos != std::string::npos) && (dicomPos == inputFileName.length() - 6))
      {
        hasExtension = true;
      }
      else
      {
        dicomPos = inputFileName.rfind(".DICOM");
        if ((dicomPos != std::string::npos) && (dicomPos == inputFileName.length() - 6))
        {
          hasExtension = true;
        }
      }

      // construct a filename, adding an extension if necessary
      std::string filename = m_OutputDirectory + itksys::SystemTools::GetFilenameName(inputFileName);
      if (!hasExtension)
      {
        // input filename has no extension, add a ".dcm"
        filename += ".dcm";
      }

      // Add the file name to the output list
      m_OutputFileNames.push_back(filename);
    }
  }
  else
  {
    itkDebugMacro("No files were found.");
  }

  return m_OutputFileNames;
}

void
GDCMSeriesFileNames::PrintSelf(std::ostream & os, Indent indent) const
{
  using namespace print_helper;

  Superclass::PrintSelf(os, indent);

  os << indent << "InputDirectory: " << m_InputDirectory << std::endl;
  os << indent << "OutputDirectory: " << m_OutputDirectory << std::endl;

  os << indent << "InputFileNames: " << m_InputFileNames << std::endl;
  os << indent << "OutputFileNames: " << m_OutputFileNames << std::endl;

  os << indent << "SerieHelper: ";
  if (m_SerieHelper.get() != nullptr)
  {
    os << m_SerieHelper.get() << std::endl;
  }
  else
  {
    os << "(null)" << std::endl;
  }

  os << indent << "SeriesUIDs: " << m_SeriesUIDs << std::endl;

  itkPrintSelfBooleanMacro(UseSeriesDetails);
  itkPrintSelfBooleanMacro(Recursive);
  itkPrintSelfBooleanMacro(LoadSequences);
  itkPrintSelfBooleanMacro(LoadPrivateTags);
}

void
GDCMSeriesFileNames::SetUseSeriesDetails(bool useSeriesDetails)
{
  m_UseSeriesDetails = useSeriesDetails;
  m_SerieHelper->SetUseSeriesDetails(m_UseSeriesDetails);
  m_SerieHelper->CreateDefaultUniqueSeriesIdentifier();
}
} // namespace itk
