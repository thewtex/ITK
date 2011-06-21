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
#ifndef __QuickView_h
#define __QuickView_h

#include <vector>

#include <itkImage.h>
#include <itkRGBPixel.h>

class ImageInfo
{
public:
  typedef itk::Image<unsigned char, 2> ImageType;

  ImageInfo(ImageType *image, std::string description="")
  {
    m_Image = image;
    m_Description = description;
  }

  ImageType::Pointer m_Image;
  std::string        m_Description;
};

class RGBImageInfo
{
public:
  typedef itk::Image<itk::RGBPixel<unsigned char>, 2> ImageType;
  RGBImageInfo(ImageType *image, std::string description="")
  {
    m_Image = image;
    m_Description = description;
  }

  ImageType::Pointer m_Image;
  std::string        m_Description;
};

class QuickView
{

public:
  QuickView() {m_ShareCamera = true;}
  template<class TImage> void AddImage(
    TImage *,
    bool FlipVertical=true,
    std::string Description="");
  template<class TImage> void AddRGBImage(
    TImage *,
    bool FlipVertical=true,
    std::string Description="");
  void Visualize(bool interact=true);
  void ShareCameraOff() {m_ShareCamera = false;}
  void ShareCameraOn()  {m_ShareCamera = true;}
private:
  std::vector<ImageInfo>    Images;
  std::vector<RGBImageInfo> RGBImages;
  bool                      m_ShareCamera;
};

#endif
