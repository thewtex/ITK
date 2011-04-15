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
/** \class CSVFileToArray2DReader is used to parse csv files into
 * the Array2D data frame object. It is used to parse files that
 * contain only numeric data types. As the Array2D object only holds
 * one data type, all the data will be cast to a single data type.
 * If the file contains row and column headers, they will be parsed
 * into std vectors of strings.
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
 typedef typename itk::CSVArray2DDataFrameObject<TData> DataFrameObjectType;
 typedef typename DataFrameObjectType::Pointer          DFOPointerType;

 typedef TData ValueType;

 itkGetMacro(DataFrameObject,DFOPointerType);

 /** Function for parsing the data. */
 void Parse();

 /** Print the reader */
 void PrintSelf(std::ostream & os, Indent indent) const;

 CSVFileToArray2DReader();
 virtual ~CSVFileToArray2DReader () {}

private:

 DFOPointerType               m_DataFrameObject;

 CSVFileToArray2DReader(const Self &);      //purposely not implemented
 void operator=(const Self &);    	//purposely not implemented
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileToArray2DReader.txx"
#endif

#endif
