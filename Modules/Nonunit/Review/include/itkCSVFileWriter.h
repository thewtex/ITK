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

#ifndef __itkCSVFileWriter_h
#define __itkCSVFileWriter_h

#include "itkLightProcessObject.h"
#include "itkMacro.h"
#include "itkArray2D.h"
#include "itkVector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_fixed.h"
#include "itkMatrix.h"

namespace itk
{
/* \class CSVFileWriter writes the data from an object into
 * a csv file. The writer uses the datablock member vnl_matrix and vnl_matrix_fixed.
 * As of now the objects itkMatrix, itkArray2D, and any vnl_matrix based objects are
 * supported.
 *
 * Also, writing row and column headers is not currently supported.
 */
template <class TValueType, unsigned int NRows = 0, unsigned int NColumns = 0>
class CSVFileWriter:public LightProcessObject
{
public:
 /** Standard class typedefs */
 typedef CSVFileWriter             Self;
 typedef LightProcessObject        Superclass;
 typedef SmartPointer <Self>       Pointer;
 typedef SmartPointer <const Self> ConstPointer;

 /** Method for creation through the object factory. */
 itkNewMacro(Self);

 /** Run-time type information (and related methods). */
 itkTypeMacro(Self,Superclass);

 /* Specify the name of the output file */
 itkSetStringMacro(FileName);

 // Matrix types
 typedef vnl_matrix<TValueType>                        vnlMatrixType;
 typedef vnl_matrix_fixed<TValueType, NRows, NColumns> vnlFixedMatrixType;
 typedef itk::Matrix<TValueType,NRows,NColumns>        itkMatrixType;

 typedef TValueType InputObjectType;

 /** Set the input object if the matrix is of vnl_matrix type or Array2D */
 void SetInput (vnlMatrixType* obj);

 /** Set the input object if the matrix is of vnl_matrix_fixed type */
 void SetInput (vnlFixedMatrixType* obj);

 /** Set the input object if the matrix is of itkMatrixType*/
 void SetInput (itkMatrixType* obj);

 virtual void Write ();

 CSVFileWriter();
 virtual ~CSVFileWriter () {}

 void PrintSelf (std::ostream &os, Indent indent) const;

private:
 std::string               m_FileName;
 TValueType               *m_InputObject;
 char                      m_FieldDelimiterCharacter;
 unsigned int              m_rows;
 unsigned int              m_cols;

 CSVFileWriter(const Self &);  //purposely not implemented
 void operator=(const Self &); //purposely not implemented
};

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCSVFileWriter.txx"
#endif

#endif
