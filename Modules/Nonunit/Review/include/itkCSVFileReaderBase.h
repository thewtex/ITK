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
/** \class CSVFileReaderBase is a base abstract class for reading csv files.
 * It contains the methods GetDataDimension() which is used to count
 * the number of rows and columns in the file data set. Only after this
 * method is called should calls to the GetNextField() method be made.
 * The method ConvertStringToValueType() can be used to convert a string
 * obtained from GetNextField() to some desired numeric type. The Parse()
 * method is what is used to do the full processing, that is, reading the file
 * and parsing the data into some object made to be pure virtual as a different
 * parsing method would be needed for parsing into different types of objects.
 *
 * The user would need to specify if there are row and column headers in the file
 * using the HasRowHeaders and HasColumnHeaders flags. Also, if row and/or column
 * headers are enclosed in "" or any other string delimiter character being used,
 * the user can set the option of turning the string delimiter character on with
 * the UseStringDelimiterCharacter flag. This is also especially useful if headers
 * such as "This,Header" contain commas or other field delimiter characters within them.
 *
 * If the csv file has row and column headers, they do not necessarily need to be enclosed in
 * "" or other string delimiter characters as long as you specify that row and/or column
 * headers exist in the file. Turning on the HasRowHeaders or HasColumnHeaders flags will
 * tell the reader to recognize them as headers.
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

 /** Set the name of the file to be read */
 itkSetStringMacro(FileName);

 /** Set the field delimiter character. It is possible to set
   * another character as a field delimiter character
   * such as ';' */
 itkSetMacro(FieldDelimiterCharacter,char);

 /** Set the UseStringDelimiterCharacter flag on if column or row
   * headers in the file are enclosed in "" or other characters.
   */
 itkSetMacro(UseStringDelimiterCharacter,bool);

 /** Set the string delimiter character if it is in use. */
 itkSetMacro(StringDelimiterCharacter,char);

 /** Set the HasRowHeaders flag to indicate existence of row headers in the file */
 itkSetMacro(HasRowHeaders,bool);

 /** Set the HasColumnHeaders flag to indicate existence of column headers in the file */
 itkSetMacro(HasColumnHeaders,bool);

 /** Boolean macros for setting HasRowHeaders, HasColumnHeaders and UseStringDelimiterCharacter
   * They can conveniently be set by appending On() or Off() to each of the variable names.
   */
 itkBooleanMacro(HasRowHeaders);
 itkBooleanMacro(HasColumnHeaders);
 itkBooleanMacro(UseStringDelimiterCharacter);

 /** Check that all essential components are present and plugged in. */
 void PrepareForParsing ();

 /** Counts the number of rows and columns in a file and prepares the file
   * iterative reading using the GetNextField() method.*/
 void GetDataDimension (size_t & rows, size_t & columns);

 /** Gets the next entry in the file. Returns a string. This function
   * must always only be called after GetDataDimension() */
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

 /** This method should be redefined in derived classes to parse the entire
   * file into some object. The GetNextField() method only gets one string
   * at a time. This method would store those strings or convert them using
   * ConvertStringToValueType() and store the numeric values into the object
   */
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
 void operator=(const Self &);          //purposely not implemented
};

} //end namespace itk

#endif
