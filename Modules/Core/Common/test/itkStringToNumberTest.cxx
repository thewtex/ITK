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

#include <iostream>

#include "itkStringToNumber.h"
#include "itkNumberToString.h"
#include "itkNumericTraits.h"

template<typename T> void PrintValue2(const char *t, T)
{
  itk::StringToNumber<T> convertString;
  itk::NumberToString<T> convertNumber;

  T original, restored;
  std::string ascii;

// min
  original = itk::NumericTraits<T>::min();
  ascii = convertNumber(original);
  restored = convertString(ascii);
  if (original != restored)
    {
    std::cout << "ERROR: for " << t << " original != restored" << std::endl;
    std::cout << t << "(min) "
              << "raw: "
              << static_cast<typename itk::NumericTraits<T>::PrintType>(original)
              << " convertedToString: " << ascii
              << " convertedToNumber: " << static_cast<typename itk::NumericTraits<T>::PrintType>(restored)
              << std::endl;
    }

  original = itk::NumericTraits<T>::NonpositiveMin();
  ascii = convertNumber(original);
  restored = convertString(ascii);
  if (original != restored)
    {
    std::cout << "ERROR: for " << t << " original != restored" << std::endl;
    std::cout << t << "(NonpositiveMin) "
              << "raw: "
              << static_cast<typename itk::NumericTraits<T>::PrintType>(original)
              << " convertedToString: " << ascii
              << " convertedToNumber: " << static_cast<typename itk::NumericTraits<T>::PrintType>(restored)
              << std::endl;
    }

  original = itk::NumericTraits<T>::max();
  ascii = convertNumber(original);
  restored = convertString(ascii);
  if (original != restored)
    {
    std::cout << "ERROR: for " << t << " original != restored" << std::endl;
    std::cout << t << "(max) "
              << "raw: "
              << static_cast<typename itk::NumericTraits<T>::PrintType>(original)
              << " convertedToString: " << ascii
              << " convertedToNumber: " << static_cast<typename itk::NumericTraits<T>::PrintType>(restored)
              << std::endl;
    }

  original = itk::NumericTraits<T>::epsilon();
  ascii = convertNumber(original);
  restored = convertString(ascii);
  if (original != restored)
    {
    std::cout << "ERROR: for " << t << " original != restored" << std::endl;
    std::cout << t << "(epsilon) "
              << "raw: "
              << static_cast<typename itk::NumericTraits<T>::PrintType>(original)
              << " convertedToString: " << ascii
              << " convertedToNumber: " << static_cast<typename itk::NumericTraits<T>::PrintType>(restored)
              << std::endl;
    }
}
int itkStringToNumberTest(int, char* [] )
{
  PrintValue2("unsigned char", static_cast<unsigned char>(0));
  PrintValue2("char", static_cast<char>(0));
  PrintValue2("unsigned short", static_cast<unsigned short>(0));
  PrintValue2("short", static_cast<short>(0));
  PrintValue2("unsigned int", static_cast<unsigned int>(0));
  PrintValue2("int", static_cast<int>(0));
  PrintValue2("unsigned long", static_cast<unsigned long>(0));
  PrintValue2("long", static_cast<long>(0));
  PrintValue2("float", static_cast<float>(0));
  PrintValue2("double", static_cast<double>(0));
  PrintValue2("long double", static_cast<long double>(0));

  return EXIT_SUCCESS;

}
