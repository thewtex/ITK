/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkColorTableTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkColorTable.h"

template<class T> void ColorTableTest(const char *name)
{
  typedef itk::ColorTable<T> ColorTableType;
  typename ColorTableType::Pointer colors = ColorTableType::New();

  std::cout << "---------- Testing for type: :" << name
            << std::endl;
  colors->UseRandomColors(16);
  std::cout << "Random Colors" << std::endl;
  colors->Print(std::cout);
  std::cout << std::endl;

  colors->UseHeatColors(16);
  std::cout << "Heat Colors" << std::endl;
  colors->Print(std::cout);
  std::cout << std::endl;

  colors->UseGrayColors(16);
  std::cout << "Gray Colors" << std::endl;
  colors->Print(std::cout);
  std::cout << std::endl;

  colors->UseDiscreteColors();
  std::cout << "Discrete Colors" << std::endl;
  colors->Print(std::cout);
  std::cout << std::endl;
}

int itkColorTableTest(int, char* [] )
{
  ColorTableTest<unsigned char> ("unsigned char");
  ColorTableTest<char>          ("char");
  ColorTableTest<unsigned short>("unsigned short");
  ColorTableTest<short>         ("short");
  ColorTableTest<unsigned int>  ("unsigned int");
  ColorTableTest<int>           ("int");
  ColorTableTest<unsigned long> ("unsigned long");
  ColorTableTest<long>          ("long");
  ColorTableTest<float>         ("float");
  // Please note, that UseGrayColors and UseRandomColors for type double
  // overflow
  // ColorTableTest<double>        ("double");
  return EXIT_SUCCESS;
}
