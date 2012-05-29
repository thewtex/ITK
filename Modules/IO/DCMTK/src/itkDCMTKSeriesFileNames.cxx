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

#include "itkDCMTKSeriesFileNames.h"
#include "itksys/SystemTools.hxx"
#include "itkProgressReporter.h"

namespace itk
{
DCMTKSeriesFileNames::DCMTKSeriesFileNames()
{
  m_InputDirectory = "";
  m_OutputDirectory = "";
  m_UseSeriesDetails = true;
  m_Recursive = false;
  m_LoadSequences = false;
  m_LoadPrivateTags = false;
}

DCMTKSeriesFileNames::~DCMTKSeriesFileNames()
{
}

void DCMTKSeriesFileNames::SetInputDirectory(const char *name)
{
  if ( !name )
    {
    itkExceptionMacro(<< "SetInputDirectory() received a NULL string");
    }
  std::string fname = name;
  this->SetInputDirectory(fname);
}

void DCMTKSeriesFileNames::SetInputDirectory(std::string const & name)
{
  if ( name == "" )
    {
    itkWarningMacro(<< "You need to specify a directory where "
                       "the DICOM files are located");
    return;
    }
  if ( m_InputDirectory == name )
    {
    return;
    }
  if ( !itksys::SystemTools::FileIsDirectory( name.c_str() ) )
    {
    itkWarningMacro(<< name << " is not a directory");
    return;
    }
  m_InputDirectory = name;
  //as a side effect it also execute
  this->Modified();
}

const SerieUIDContainer & DCMTKSeriesFileNames::GetSeriesUIDs()
{
}

const FilenamesContainer & DCMTKSeriesFileNames::GetFileNames(const std::string serie)
{
  m_InputFileNames.clear();
  return m_InputFileNames;
}

const FilenamesContainer & DCMTKSeriesFileNames::GetInputFileNames()
{
  // Do not specify any UID
  return GetFileNames("");
}

const FilenamesContainer & DCMTKSeriesFileNames::GetOutputFileNames()
{

}

void DCMTKSeriesFileNames::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  unsigned int i;
  os << indent << "InputDirectory: " << m_InputDirectory << std::endl;
  os << indent << "LoadSequences:" << m_LoadSequences << std::endl;
  os << indent << "LoadPrivateTags:" << m_LoadPrivateTags << std::endl;
  if ( m_Recursive )
    {
    os << indent << "Recursive: True" << std::endl;
    }
  else
    {
    os << indent << "Recursive: False" << std::endl;
    }

  for ( i = 0; i < m_InputFileNames.size(); i++ )
    {
    os << indent << "InputFilenames[" << i << "]: " << m_InputFileNames[i] << std::endl;
    }

}

void DCMTKSeriesFileNames::SetUseSeriesDetails(bool useSeriesDetails)
{
  m_UseSeriesDetails = useSeriesDetails;
//  m_SerieHelper->SetUseSeriesDetails(m_UseSeriesDetails);
//  m_SerieHelper->CreateDefaultUniqueSeriesIdentifier();
}
} //namespace ITK
