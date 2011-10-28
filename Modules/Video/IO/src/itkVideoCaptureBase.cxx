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
#include "itkVideoCaptureBase.h"

namespace itk
{

//
// PrintSelf
//
void VideoCaptureBase::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Camera Grabber Start Index:";
  for (unsigned int d=0; d<this->GetNumberOfDimensions(); d++)
    {
    os << " " << m_GrabberStartIndex[d];
    }
  os << std::endl;

  os << indent << "Camera Interface Name:  "  << m_CameraInterfaceName
     << std::endl;
  os << indent << "Camera Identifier Name:  " << m_CameraIdentifierName
     << std::endl;
  os << indent << "Parameter Values (" << m_ParameterValues.size() << "):"
     << std::endl;
  ParameterMapType::const_iterator pos;
  for(pos=m_ParameterValues.begin(); pos != m_ParameterValues.end(); pos++)
    {
    os << indent << "  " << pos->first << " = " << pos->second << std::endl;
    }
}

//
// ResetMembers -- reset member variables to empty state closed
//
void VideoCaptureBase::ResetMembers()
{
  Superclass::ResetMembers();

  // Set the start index to all zeros.
  m_GrabberStartIndex.clear();
  m_GrabberStartIndex.resize(m_NumberOfDimensions,0);

  this->Modified();
}

//
// InsureDimensionCount
//
void VideoCaptureBase::EnsureDimensionCount(unsigned int d)
{
  if (d > this->GetNumberOfDimensions())
    {
    unsigned int oldNumDimensions = m_NumberOfDimensions;

    // Call the parent-class implementation
    Superclass::EnsureDimensionCount(d);

    // Fill in remaining values with the value from the closest dimension
    m_GrabberStartIndex.resize( d, m_GrabberStartIndex.at(oldNumDimensions-1) );

    this->Modified();
    }
}


//
// SetFileName -- Parse the camera:// URL and save the results
//
void VideoCaptureBase::SetFileName(const std::string& cameraURL)
{
//  int numParameters = 0;
  bool atEndOfURL = false;
  size_t position;
  std::string truncatedCameraURL;

  ParameterValuePairType parameterValuePair;

  // Make sure this really is a camera:// URL:
  if (false == this->FileNameIsCamera(cameraURL))
    {
    itkExceptionMacro("Can only handle camera:// URLs, not " << cameraURL);
    }

  truncatedCameraURL = cameraURL.substr(9); // remove camera://

  // Extract the camera's interface name
  position = truncatedCameraURL.find_first_of('/');
  if (std::string::npos == position)
    {
    itkExceptionMacro("Improperly formatted camera:// URL: " << cameraURL);
    }
  this->m_CameraInterfaceName = truncatedCameraURL.substr(0,position);

  truncatedCameraURL = truncatedCameraURL.substr(position+1);

  // Extract the camera's identifier (i.e., it's name or number)
  position = truncatedCameraURL.find_first_of('?');
  if (std::string::npos == position)
    {
    // if there is no ? in the URL, then go all the way to the end
    position = truncatedCameraURL.length();
    atEndOfURL = true;
    }
  this->m_CameraIdentifierName = truncatedCameraURL.substr(0,position);

  // Loop through all the parameters (if any) included at the end of the URL
  while(!atEndOfURL)
    {
    truncatedCameraURL = truncatedCameraURL.substr(position+1);

    // Extract the next parameter's name
    position = truncatedCameraURL.find_first_of('=');
    if (std::string::npos == position)
      {
      itkExceptionMacro("Improperly formatted camera:// URL does not contain ="
                        << " after parameter name:  " << truncatedCameraURL
                        << "  from  " << cameraURL );
      }
    parameterValuePair.first = truncatedCameraURL.substr(0,position);

    truncatedCameraURL = truncatedCameraURL.substr(position+1);

    // Extract the next parameter's value
    position = truncatedCameraURL.find_first_of('&');
    if (std::string::npos == position)
      {
      // if there is no & left in the URL, then go all the way to the end
      position = truncatedCameraURL.length();
      atEndOfURL = true;
      }
    parameterValuePair.second = truncatedCameraURL.substr(0,position);

    this->m_ParameterValues.insert(parameterValuePair);
    }

  this->Modified();
}


//
// SetFileName -- Parse the camera:// URL and save the results
//
void VideoCaptureBase::SetFileName(const char* cameraURL)
{
  std::string strCameraURL = cameraURL;
  SetFileName(strCameraURL);
}


//
// GetCameraInterfaceFromURL -- return the camera interface name from a camera URL
//
  std::string VideoCaptureBase::GetCameraInterfaceFromURL( const std::string& cameraURL)
{
  size_t position;
  std::string truncatedCameraURL;

  // Make sure this really is a camera:// URL:
  if (false == this->FileNameIsCamera(cameraURL))
    {
    itkExceptionMacro("Can only handle camera:// URLs, not " << cameraURL);
    }

  truncatedCameraURL = cameraURL.substr(9); // remove camera://

  position = truncatedCameraURL.find_first_of('/');
  if (std::string::npos == position)
    {
    itkExceptionMacro("Improperly formatted camera:// URL: " << cameraURL);
    }

  return truncatedCameraURL.substr(0,position);
}


//
// ReadImageInformation
//
void VideoCaptureBase::ReadImageInformation()
{
  // Default parameter values for use when a grabber is active/open:
  // ---------------------------------------------------------------

  // Usually there is not an apriori number of frames for a grabber, so set it
  // to two less than 1/2 of the maximum possible for the variable type.
  // Using the above math allows for safe signed comparisons to +1 "stop values"
  const FrameOffsetType frameTotalActiveDefault = vnl_numeric_traits<
                                                  FrameOffsetType>::maxval/2-2;

  const TemporalRatioType framesPerSecondActiveDefault = 1.0;

  SizeValueType dimensionsActiveDefaultArray[] = {320, 240};
  std::vector<SizeValueType> dimensionsActiveDefault(
    dimensionsActiveDefaultArray,
    dimensionsActiveDefaultArray+2);  // copy the 2 elements


  // First, reset all the parameters to safe, "empty" default values
  this->ResetMembers();

  // Now, take care of the VideoIOBase parameters that only apply to video files
  // Note:  These parameters should probably be moved to a file-only subclass of
  //        VideoIOBase and then be removed all-together from here.
  // --------------------------------------------------------

  // No I-Frame issues to worry about
  m_IFrameInterval = 1;
  m_LastIFrame = m_FrameTotal-1;

  // Next, fill in usable defaults for the relevant parameters
  // ---------------------------------------------------------

  // By default, grabbers can run indefinately, so use max val:
  m_FrameTotal = frameTotalActiveDefault;

  // Default Dimensions
  for (unsigned int i = 0; i < this->GetNumberOfDimensions(); ++i)
    {
    m_Dimensions[i] = dimensionsActiveDefault.at(i); // .at checks out of range
    }

  m_FramesPerSecond = framesPerSecondActiveDefault;

  // Finally, parse our parameter map and adjust member variables as appropriate
  // ---------------------------------------------------------------------------

  // iterate through the entire map of parameters:
  ParameterMapType::const_iterator pos;
  for(pos=m_ParameterValues.begin(); pos != m_ParameterValues.end(); pos++)
    {
    // see if the current parameter name matches any that we know about:
    if (     "fps"       == pos->first){ m_FramesPerSecond      = atof(pos->second.c_str());}
    else if ("maxframes" == pos->first){ m_FrameTotal           = atol(pos->second.c_str());}
    else if ("xstart"    == pos->first){ m_GrabberStartIndex[0] = atoi(pos->second.c_str());}
    else if ("ystart"    == pos->first){ m_GrabberStartIndex[1] = atoi(pos->second.c_str());}
    else if ("width"     == pos->first){ m_Dimensions[0]        = atoi(pos->second.c_str());}
    else if ("height"    == pos->first){ m_Dimensions[1]        = atoi(pos->second.c_str());}
    else if ("xspacing"  == pos->first){ m_Spacing[0]           = atof(pos->second.c_str());}
    else if ("yspacing"  == pos->first){ m_Spacing[1]           = atof(pos->second.c_str());}
    // If present, the following parameters indicate grabbing 3D images:
    else if ("zstart"    == pos->first){ EnsureDimensionCount(3);
                                         m_GrabberStartIndex[2] = atoi(pos->second.c_str());}
    else if ("depth"     == pos->first){ EnsureDimensionCount(3);
                                         m_Dimensions[2]        = atoi(pos->second.c_str());}
    else if ("zspacing"  == pos->first){ EnsureDimensionCount(3);
                                         m_Spacing[2]           = atof(pos->second.c_str());}
    }

  this->Modified();
}

}
