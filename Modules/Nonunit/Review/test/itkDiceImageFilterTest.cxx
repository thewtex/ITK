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
/*=========================================================================
 Arindam Bhattacharya
 The Ohio State University.
 =========================================================================*/


// This example introduces the use of the Dice Filter.This filter
// expects as input two binary images and then finds the Dice
// similarity co-efficient between them. It outputs a floating value
// (0-1) and an (grayscale)image where the intersection of the two
// input image black ( is grayscale value '0') the union is 'gray'
// (grayscale value '127').The formula for the calculations is given as
// 'as twice the shared information (intersection) over the sum of
// cardinalities'.The Dice distance which measures dissimilarities is
// given subtracting the value from 1.


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkDiceImageFilter.h"
#include "stdio.h"

int itkDiceImageFilterTest(int argc, char *argv[])
{
  typedef itk::Image<unsigned char, 2>        ImageType;
  typedef itk::itkDiceImageFilter<ImageType>  FilterType;

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

  if (argc !=3)
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << "[inputA] [inputB]" << std::endl;
    std::cerr << "The input files must be binary."<< std::endl;
    return EXIT_FAILURE;
    }
  reader->SetFileName(argv[1]);
  reader->Update();

  //read new image
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(argv[2]);
  reader2->Update();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInputImage1( reader->GetOutput() );
  filter->SetInputImage2( reader2->GetOutput() );
  filter->Update();
  std::cerr <<" The Dice coefficient is defined as, twice the shared information (intersection) over the sum of cardinalities between the two inputs"<<std::endl;
  std::cerr <<" The value output by the  Dice filter is : "
            << filter->GetDiceDistance() << std::endl;

    typedef  itk::ImageFileWriter< ImageType  > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("TestOutput.jpg");
    writer->SetInput(filter->GetOutput());
    writer->Update();

    return EXIT_SUCCESS;
}
