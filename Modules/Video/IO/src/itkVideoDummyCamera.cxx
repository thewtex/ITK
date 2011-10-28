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
#include "itkVideoDummyCamera.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageIOFactory.h"
#include <iomanip>

namespace itk
{

///////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Print
//

//
// Constructor
//
VideoDummyCamera::VideoDummyCamera()
{
  this->ResetMembers();
}

//
// Destructor
//
VideoDummyCamera::~VideoDummyCamera()
{
  this->FinishReadingOrWriting();
}

//
// PrintSelf
//
void VideoDummyCamera::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Background Pixel Value (" <<m_BackgroundValue.size() <<"):";
  os << std::hex << std::setfill('0'); // << std::setw(2) << std::nouppercase;
  for(unsigned int i=0; i<m_BackgroundValue.size(); i++)
    {
    os << " "  << std::setw(2) << (unsigned int)(m_BackgroundValue[i]);
    }
  os << std::dec << std::setfill(' '); // << std::nouppercase;
  os << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// Read related methods
//

//
// CanReadFile
//
bool VideoDummyCamera::CanReadFile(const char* filename)
{
  std::string strFileName = filename;
  std::string cameraInterface;

  // Make sure this is a camera:// URL
  if ( this->FileNameIsCamera(strFileName) )
    {
    // Check if this is the requested camera interface
    cameraInterface = this->GetCameraInterfaceFromURL( strFileName );
    if ("dummy" == cameraInterface)
      {
      return true;
      }
    }
  return false;
}

//
// ReadImageInformation
//
void VideoDummyCamera::ReadImageInformation()
{
  ParameterMapType::const_iterator parmIt;
  char ** strtoulEndPointer;

  // First, call the parent implementation:
  Superclass::ReadImageInformation();

  // All other initialization depends on the specific dummy camera chosen:
  // ---------------------------------------------------------------------

  // solidgray camera:
  if ("solidgray" == m_CameraIdentifierName)
    {
    // Specify Grayscale, 8-bit pixels
    m_NumberOfComponents = 1;
    m_ComponentType = UCHAR;
    m_PixelType = SCALAR;
    m_BackgroundValue.resize(1);

    // Check if the grayscale pixel value was specified
    parmIt = m_ParameterValues.find("gray");
    if ( m_ParameterValues.end() != parmIt )
      {
      // Use specified pixel value, after converting from a hex string
      m_BackgroundValue[0] = strtoul( parmIt->second.c_str(),
                                      strtoulEndPointer, 16);
      }
    else
      {
      // Use default pixel value
      m_BackgroundValue[0] = 0;
      }
    }

  // solidcolor camera:
  else if("solidcolor" == m_CameraIdentifierName)
    {
    // Specify RGB, 24-bit pixels
    m_NumberOfComponents = 3;
    m_ComponentType = UCHAR;
    m_PixelType = RGB;
    m_BackgroundValue.resize(3);

    // Check if the color pixel value was specified
    parmIt = m_ParameterValues.find("color");
    if ( m_ParameterValues.end() != parmIt )
      {
      // Use specified pixel value, after converting from 3 hex strings
      std::string redStr   = parmIt->second.substr(0,2);
      std::string greenStr = parmIt->second.substr(2,2);
      std::string blueStr  = parmIt->second.substr(4,2);
      m_BackgroundValue[0] = strtoul( redStr.c_str(),   strtoulEndPointer, 16);
      m_BackgroundValue[1] = strtoul( greenStr.c_str(), strtoulEndPointer, 16);
      m_BackgroundValue[2] = strtoul( blueStr.c_str(),  strtoulEndPointer, 16);
      }
    else
      {
      // Use default pixel value
      m_BackgroundValue[0] = 0;
      m_BackgroundValue[1] = 0;
      m_BackgroundValue[2] = 0;
      }
    }
  else
    {
    itkExceptionMacro(<< "Unknown camera identifier: " << m_CameraIdentifierName);
    }


  this->Modified();
}

//
// Read
//
void VideoDummyCamera::Read(void *buffer)
{
  // Make sure we've already called ReadImageInformation (dimensions are
  // non-zero)
  if (m_Dimensions[0] == 0)
    {
    itkExceptionMacro(<< "Cannot read frame with zero dimension.  "
                      << "May need to call ReadImageInformation");
    }

  // Acquire the image (in this case we generate it directly)
  SizeType PixelSize = this->GetPixelSize();
  SizeType BufferSize = this->GetImageSizeInBytes();
  // = GetImageSizeInComponents * ComponentSize; compare to GetImageSizeInPixels
  unsigned char *bufferPointer = reinterpret_cast<unsigned char*>(buffer);
  for(int i=0; i < BufferSize; i+=PixelSize)
    {
    //GetImageSizeInComponents  GetImageSizeInBytes  GetImageSizeInPixels
    memcpy( reinterpret_cast<void*>(bufferPointer+i),
            reinterpret_cast<void*>(&m_BackgroundValue[0]), PixelSize);
    }

  // Update VideoIO Properties as appropriate
  this->m_CurrentFrame += 1;
  //this->m_PositionInMSec =
  //this->m_FramesPerSecond = new value (if changed)
  //this->m_Ratio =

  // Put the new frame into the output buffer
  // ...already done in this case...
}

} // end namespace itk
