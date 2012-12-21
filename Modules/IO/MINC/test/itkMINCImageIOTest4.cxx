/*=========================================================================
 *
 *  Copyright Vladimir S. FONOV
 *
 *  Based on itkHDF5ImageIOTest.cxx
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

#include "itkImage.h"
#include <iostream>

#include "itkMINCImageIO.h"
#include "itkMINCImageIOFactory.h"
#include "itkImageFileReader.h"

#include "itkImageMomentsCalculator.h"

template<class ImageType> int test_image_moments(const char *minc_image,double total, double mx,double my,double mz,double epsilon)
{
  itk::MINCImageIO::Pointer mincIO1 = itk::MINCImageIO::New();

  typedef itk::ImageFileReader< ImageType > ReaderType;

  typedef itk::ImageMomentsCalculator< ImageType > MomentsCalculatorType;

  typename ReaderType::Pointer reader = ReaderType::New();
  
  typename MomentsCalculatorType::Pointer calculator=MomentsCalculatorType::New();

  reader->SetImageIO( mincIO1 );

  reader->SetFileName( minc_image );

  reader->Update();
  calculator->SetImage(reader->GetOutput());
  calculator->Compute();

  std::cout.precision(10);
  std::cout<<"Image:"<<minc_image<<" sum="<<calculator->GetTotalMass()<<" COM="<<calculator->GetCenterOfGravity()<<std::endl;
  
  if(fabs(calculator->GetTotalMass()-total)>epsilon)
    {
    std::cerr<<"Total sum mismatch:"<<calculator->GetTotalMass()<<" difference="<<(calculator->GetTotalMass()-total)<<std::endl;
    return EXIT_FAILURE;
    }
    
  if(fabs(calculator->GetCenterOfGravity()[0]-mx)>epsilon)
    {
    std::cerr<<"Total mx mismatch:"<<calculator->GetCenterOfGravity()[0]<<std::endl;
    return EXIT_FAILURE;
    }
    
  if(fabs(calculator->GetCenterOfGravity()[1]-my)>epsilon)
    {
    std::cerr<<"Total my mismatch:"<<calculator->GetCenterOfGravity()[1]<<std::endl;
    return EXIT_FAILURE;
    }
    
  if(fabs(calculator->GetCenterOfGravity()[2]-mz)>epsilon)
    {
    std::cerr<<"Total mz mismatch:"<<calculator->GetCenterOfGravity()[2]<<std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


int itkMINCImageIOTest4( int argc, char * argv [] )
{

  if ( argc < 6 )
    {
    std::cerr << "Missing Arguments " << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputfile sum mx my mz " << std::endl;
    return EXIT_FAILURE;
    }

  double total=atof( argv[2] );

  double mx=atof( argv[3] );

  double my=atof( argv[4] );

  double mz=atof( argv[5] );

  double epsilon=1e-3;

  try
    {
      int ret=EXIT_SUCCESS;

      if(test_image_moments<itk::Image< double, 3 > >(argv[1],total,mx,my,mz,epsilon)!=EXIT_SUCCESS)
        {
          ret=EXIT_FAILURE;
        }
        
      if(test_image_moments<itk::Image< float, 3 > >(argv[1],total,mx,my,mz,epsilon)!=EXIT_SUCCESS)
        {
          ret=EXIT_FAILURE;
        }
      return ret;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
