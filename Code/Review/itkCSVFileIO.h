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

#ifndef __itkCSVFileIO_h
#define __itkCSVFileIO_h

#include "itkLightProcessObject.h"
#include "itkMacro.h"
#include "itkArray2D.h"
#include <vector>

namespace itk
{
/** \class itkCSVFileParser is used to parse CSV files into some readable itk object.
  * This class reads the input file and parses into an STL vector.
  * An STL vector is being used as it can grow dynamically.
  * This way, the csv file only needs to be read once.
  *
  * We assume that the data in the .csv file has the same number of columns
  * in each row.
  * We also assume that the first row and first column in the file contain headers.
  *
  */

class CSVFileParser:public LightProcessObject
{
public:
 /** Standard class typedefs */
 typedef CSVFileParser       Self;
 typedef LightProcessObject  Superclass;

 /** Macros to set member variables */
 itkSetStringMacro(FileName);
 itkSetMacro(FieldDelimiterCharacter,char);
 itkSetMacro(StringDelimiterCharacter,char);
 itkSetMacro(HaveColumnHeaders,bool);
 itkSetMacro(HaveRowHeaders,bool);
 itkSetMacro(UseStringDelimiterCharacter,bool);
 itkBooleanMacro(HaveColumnHeaders);
 itkBooleanMacro(HaveRowHeaders);
 itkBooleanMacro(UseStringDelimiterCharacter)

 /** Typedefs for 2-D vectors */
 typedef std::vector <std::string>  StringVector;
 typedef std::vector <StringVector> StringVector2D;

 /** Function to check if  a line is whitespace */
 bool isLineWhiteSpace (std::string line);

 /** Function to the read and parse the data into an stl vector */
 void ReadData();

 CSVFileParser ();
 virtual ~CSVFileParser () {}

protected:
  StringVector2D   m_Table;
  bool             m_HaveRowHeaders;
  bool             m_HaveColumnHeaders;
  bool             m_UseStringDelimiterCharacter;

private:
 std::string   m_FileName;
 char          m_FieldDelimiterCharacter;
 char          m_StringDelimiterCharacter;
};

/** \class DataFrameObject converts the string data into some user-specified
  * numeric type. It also maintains vectors that contain the column
  * and row headers
  */

template <class TNumericData>
class CSVDataFrameObject:public CSVFileParser
{
public:
 // Standard class typedefs
 typedef CSVDataFrameObject Self;
 typedef CSVFileParser      Superclass;

 /** Method for creation through the object factory. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods). */
 itkTypeMacro(CSVFileParser, Superclass);

 typedef itk::Array2D <TNumericData>  MatrixType;
 typedef std::vector <TNumericData>   NumericVectorType;

 // Function to convert string to a numeric type
 TNumericData        StringToNumeric (const std::string);

 // Function for parsing the data and creating the vectors for the headers
 MatrixType        ParseData();

 // Functions for retrieving data (indices for rows or columns begin at 0)
 std::vector<TNumericData>   GetRow (std::string);
 std::vector<TNumericData>   GetRow (unsigned int);

 std::vector<TNumericData>   GetColumn (std::string);
 std::vector<TNumericData>   GetColumn (unsigned int);

 TNumericData         GetData (std::string, std::string);
 TNumericData         GetData (unsigned int, unsigned int);

 TNumericData         GetColumnData (std::string, unsigned int);
 TNumericData         GetRowData (std::string, unsigned int);

 StringVector         GetRowNames()
 { return this->RowHeaders; }

 StringVector         GetColumnNames()
 { return this->ColumnHeaders; }

 TNumericData operator()(unsigned int row, unsigned int col)
 {
  return GetData(row,col);
 }

 TNumericData operator()(std::string row, std::string col)
 {
  return GetData(row,col);
 }

 void          DisplayData();

 CSVDataFrameObject () {}
 virtual ~CSVDataFrameObject() {}

private:
 CSVDataFrameObject(const Self &); //purposely not implemented
 void operator=(const Self &);      //purposely not implemented

 MatrixType    DataMatrix;
 StringVector  ColumnHeaders;
 StringVector  RowHeaders;
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileIO.cxx"
#endif

#endif
