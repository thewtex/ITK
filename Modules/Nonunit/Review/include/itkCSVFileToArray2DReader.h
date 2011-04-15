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

#ifndef __itkCSVFileToArray2DReader_h
#define __itkCSVFileToArray2DReader_h

#include "itkCSVFileReaderBase.h"
#include "itkCSVArray2DDataFrameObject.h"
#include "itkMacro.h"
#include "itkArray2D.h"
#include <vector>

namespace itk
{

/** \class CSVFileToArray2DReader is used for reading csv files. This reader should only
 * be used for parsing numeric data types. The Parse() method in this class parses the
 * data from the file into an Array2DDataFrameObject. As the Array2D object only holds
 * one data type, all the data, other than the row and column headers, will be cast to a
 * single data type.Row and column headers will be parsed into std vectors of strings.
 *
 * The user would need to specify the file name using the SetFileName() method.The field
 * delimiter character is set to ',' by default but other field delimiter characters may be
 * set using SetFieldDelimiterCharacter(). If there are row and column headers in the file,
 * the user must specify whether or not they exist using the HasRowHeaders and HasColumnHeaders
 * flags. Also, if row and/or column headers are enclosed in "" or any other string delimiter
 * character being used, the user can set the option of turning the string delimiter character
 * on with the UseStringDelimiterCharacter flag. This is also especially useful if headers
 * such as "This,Header" contain commas or other field delimiter characters within them.
 *
 * If the csv file has row and column headers, they do not necessarily need to be enclosed
 * in "" or other string delimiter characters as long as you specify that row and/or column
 * headers exist in the file. Turning on HasColumnHeaders flags will tell the reader that the
 * first line contains column headers and turning on HasRowHeaders will tell the reader to
 * recognize the first field in all subsequent lines as row headers. After all the inputs have
 * been set, the user then calls the Parse() method. The user can then get the results by
 * instantiating an Array2DDataFrameObject and setting it using the GetDataFrameObject() method.
 *
 * Below is an example of how this class can be used to read and parse the data from an input file:
 *
 * typedef itk::CSVFileToArray2DReader<double> ReaderType;
 * ReaderType::Pointer reader = ReaderType::New();
 *
 * std::string filename = "NameOfFile.csv";
 * reader->SetFileName ( filename );
 * reader->SetFieldDelimiterCharacter(',');
 * reader->SetStringDelimiterCharacter('"');
 * reader->HasColumnHeadersOn();
 * reader->HasRowHeadersOn();
 * reader->UseStringDelimiterCharacterOn();
 *
 * reader->Parse();
 *
 */
template <class TData>
class ITK_EXPORT CSVFileToArray2DReader:public CSVFileReaderBase
{
public:
 /** Standard class typedefs */
 typedef CSVFileToArray2DReader    Self;
 typedef CSVFileReaderBase         Superclass;
 typedef SmartPointer<Self>        Pointer;
 typedef SmartPointer<const Self>  ConstPointer;

 /** Standard New method. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods) */
 itkTypeMacro(Self,Superclass);

 /** DataFrame Object type */
 typedef typename itk::CSVArray2DDataFrameObject<TData> Array2DDataFrameObjectType;
 typedef typename Array2DDataFrameObjectType::Pointer   DataFrameObjectPointerType;

 typedef TData ValueType;

 /** This method can be used to get the data frame object once the data from the file has been parsed. */
 itkGetObjectMacro(DataFrameObject,Array2DDataFrameObjectType);

 /** Parses the data from the file. Gets all the fields and parses
   * row and column headers, if any, into std::vectors of strings and
   * parses the numeric data into an Array2D object. The vectors and
   * the Array2D object are part of the Array2DDataFrameobject.
   */
 void Parse();

 /** Print the reader */
 void PrintSelf(std::ostream & os, Indent indent) const;

 CSVFileToArray2DReader();
 virtual ~CSVFileToArray2DReader () {}

private:

 DataFrameObjectPointerType   m_DataFrameObject;

 CSVFileToArray2DReader(const Self &);  //purposely not implemented
 void operator=(const Self &);          //purposely not implemented
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileToArray2DReader.txx"
#endif

#endif
