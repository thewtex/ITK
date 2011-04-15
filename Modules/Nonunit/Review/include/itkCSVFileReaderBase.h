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

#ifndef __itkCSVFileReaderBase_h
#define __itkCSVFileReaderBase_h

#include "itkLightProcessObject.h"
#include <vcl_limits.h>
#include "itkMacro.h"
#include <fstream>

namespace itk
{
/** \class CSVFileReaderBase is base abstract class for reading csv files.
 * It contains the methods GetDataDimension() which is used to count the
 * the number of rows and columns in the file data set. Only after this
 * method is called should calls to the GetNextField() method be made.
 * The method ConvertStringToValueType() can be used to convert a string
 * obtained from GetNextField() to some desired numeric type. The Parse()
 * method is made to be pure virtual as a different parsing method would
 * be needed for parsing into different types of objects.
 *
 * The user would need to specify if there are row and column headers in
 * the file. Also, if there are headers that contain a field delimiter
 * character, the user can set the option of turning the string delimiter
 * character on. In the csv file, row and column header strings do not
 * necessarily need to be enclosed in "" as long as you specify that they
 * exist.
 *
 * The PrepareForParsing() method does not need to be called explicitly as
 * it is called in the GetDataDimension() method.
 */

class ITK_EXPORT CSVFileReaderBase:public LightProcessObject
{
public:
 /** Standard class typedefs */
 typedef CSVFileReaderBase         Self;
 typedef LightProcessObject        Superclass;

 /** Run-time type information (and related methods) */
 itkTypeMacro(Self,Superclass);

 /** Set Macros for members */
 itkSetStringMacro(FileName);
 itkSetMacro(FieldDelimiterCharacter,char);
 itkSetMacro(StringDelimiterCharacter,char);
 itkSetMacro(UseStringDelimiterCharacter,bool);
 itkSetMacro(HasRowHeaders,bool);
 itkSetMacro(HasColumnHeaders,bool);
 itkBooleanMacro(HasRowHeaders);
 itkBooleanMacro(HasColumnHeaders);
 itkBooleanMacro(UseStringDelimiterCharacter);

 /** Check that all essential components are present and plugged in. */
 void PrepareForParsing ();

 /** Counts the number of rows and columns in a file */
 void GetDataDimension (size_t & rows, size_t & columns);

 /** Gets the next entry in the file. Returns a string. This function
   * must be called after GetDataDimension() */
 void GetNextField(std::string & );

 /** Converting a string to other numeric value types */
 template <class TData>
 TData ConvertStringToValueType (const std::string str)
 {
  TData value;
  std::istringstream isstream(str);
  isstream >> value;

  size_t isstreamtellg = static_cast<size_t> ( isstream.tellg() );

  return (!isstream.fail() || isstreamtellg == str.length() ) ?
    value : vcl_numeric_limits<TData>::quiet_NaN();
 }

 /** Parse the data */
 virtual void Parse ()=0;

 /** Print the reader */
 void PrintSelf(std::ostream & os, Indent indent) const;

 CSVFileReaderBase ();
 virtual ~CSVFileReaderBase () {}

protected:
 std::string                  m_FileName;
 char                         m_FieldDelimiterCharacter;
 char                         m_StringDelimiterCharacter;
 bool                         m_UseStringDelimiterCharacter;
 bool                         m_HasRowHeaders;
 bool                         m_HasColumnHeaders;
 std::ifstream                m_InputStream;
 int                          m_EndOfColumnHeadersLine;
 std::string                  m_Line;

private:
 CSVFileReaderBase(const Self &);       //purposely not implemented
 void operator=(const Self &);    	//purposely not implemented
};

} //end namespace itk

#endif
