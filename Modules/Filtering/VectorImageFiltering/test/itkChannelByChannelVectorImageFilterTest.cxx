/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkChannelByChannelVectorImageFilter.h"
#include "itkVectorImage.h"
#include "itkImage.h"
#include "itkMeanImageFilter.h"

int itkChannelByChannelVectorImageFilterTest(int argc, char * argv[])
{
  const unsigned int Dimension = 2;
  typedef unsigned char                              PixelType;
  typedef itk::Image<PixelType, Dimension>           ImageType;
  typedef itk::VectorImage<PixelType, Dimension>     VectorImageType;
  typedef itk::MeanImageFilter<ImageType, ImageType> MeanFilterType;

  typedef itk::ChannelByChannelVectorImageFilter<VectorImageType, VectorImageType, MeanFilterType>
  ChannelByChannelVectorImageFilterType;

  // Instantiating object
  ChannelByChannelVectorImageFilterType::Pointer filter = ChannelByChannelVectorImageFilterType::New();

  std::cout << filter << std::endl;

  return EXIT_SUCCESS;
}
