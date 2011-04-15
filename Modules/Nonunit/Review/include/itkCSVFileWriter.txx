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
#ifndef __itkCSVFileWriter_txx
#define __itkCSVFileWriter_txx

#include "itkCSVFileWriter.h"
#include <fstream>


namespace itk
{
/* As of now, the csv file writer can write 2D itk objects such as
 * itkMatrix, itkArray2D and vnl_matrix to a csv file
 */

template <class TObjectType, unsigned int NRows, unsigned int NColumns>
void
CSVFileWriter<TObjectType,NRows,NColumns>
::Write ()
{
  if (m_InputObject == 0 )
    {
    itkExceptionMacro(<< "No input to writer!");
    }

  if (m_FileName == "")
    {
    itkExceptionMacro(<< "No filename was specified!");
    }

  std::ofstream outputStream(m_FileName.c_str(), std::ios::app);
  if ( outputStream.fail() )
    {
    std::string     message = "Can't open ";
    message += m_FileName;
    message += '\n';
    itkExceptionMacro(<< message );
    }

  for (unsigned int i = 0; i < m_rows; i++)
   {
   for (unsigned int j = 0; j < m_cols; j++)
     {
     outputStream << *(m_InputObject++);
     if (j < m_cols - 1)
       {
       outputStream << ",";
       }
     }
   outputStream << std::endl;
   }
  outputStream.close();
 }

template <class TObjectType, unsigned int NRows, unsigned int NColumns>
void
CSVFileWriter<TObjectType,NRows,NColumns>
::PrintSelf (std::ostream& os, Indent indent) const
 {
  os << indent << "File name: " << m_FileName << std::endl;
 }


} //end namespace itk

#endif
