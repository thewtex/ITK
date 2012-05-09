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

// a testing object type that is created to test DOM-based reading/writing

#ifndef __itkTestObject_h
#define __itkTestObject_h

#include "itkLightProcessObject.h"
#include "itkArray.h"
#include <vector>
#include <string>
#include <iostream>

namespace itk
{

class TestObject : public LightProcessObject
{
public:
  /** Standard class typedefs. */
  typedef TestObject                  Self;
  typedef LightProcessObject          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TestObject, LightProcessObject);

  itk::Array<double>& GetData1() { return m_Data1; }
  itk::Array<double>& GetData2() { return m_Data2; }
  std::vector<float>& GetFparams() { return m_Fparams; }
  bool& GetActive() { return m_Active; }
  std::string& GetNotes() { return m_Notes; }
  int& GetCount() { return m_Count; }
  //
  const itk::Array<double>& GetData1() const { return m_Data1; }
  const itk::Array<double>& GetData2() const { return m_Data2; }
  const std::vector<float>& GetFparams() const { return m_Fparams; }
  const bool& GetActive() const { return m_Active; }
  const std::string& GetNotes() const { return m_Notes; }
  const int& GetCount() const { return m_Count; }
  //
  void Print( std::ostream& os )
  {
    os << "TestObject: " << std::endl;
    os << "  notes = " << m_Notes << std::endl;
    os << "  active = " << m_Active << std::endl;
    os << "  count = " << m_Count << std::endl;
    //
    os << "  fparams = ";
    for ( unsigned int i = 0; i < m_Fparams.size(); i++ ) os << " " << m_Fparams[i];
    os << std::endl;
    //
    os << "  data1 = " << m_Data1 << std::endl;
    os << "  data2 = " << m_Data2 << std::endl;
  }

protected:
  TestObject() {}

private:
  TestObject(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  itk::Array<double>  m_Data1;
  itk::Array<double>  m_Data2;
  std::vector<float>  m_Fparams;
  bool                m_Active;
  std::string         m_Notes;
  int                 m_Count;
};

} // namespace itk

#endif // __itkTestObject_h
