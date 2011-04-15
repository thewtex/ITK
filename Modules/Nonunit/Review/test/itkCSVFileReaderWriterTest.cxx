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

#include "itkCSVFileReader.h"
#include "itkCSVFileWriter.h"
#include "itkCSVDataFrameObject.h"
#include "itkArray2D.h"

const double epsilon = 1e-10;

// function for comparing matrices
template <class T>
bool testMatrix (const itk::Array2D<T>& m1, const itk::Array2D<T>& m2)
{
 bool pass = true;

 if (m1.size() != m2.size() ) return false;

 for (unsigned int i = 0; i < m1.rows(); i++)
   {
   for (unsigned int j = 0; j < m1.cols(); j++)
     {
     if (vcl_fabs(m1[i][j] - m2[i][j]) > epsilon) pass = false;
     }
   }
 return pass;
}

int itkCSVFileReaderWriterTest (int argc, char *argv[])
{
 if (argc < 2)
 {
 std::cerr << "Usage: " << argv[0] << " Filename" << std::endl;
 return EXIT_FAILURE;
 }


 double nan = vcl_numeric_limits<double>::quiet_NaN();

 typedef itk::Array2D<double> MatrixType;
 const unsigned int ARows = 3;
 const unsigned int ACols = 6;
 MatrixType matrix(ARows,ACols);
 matrix[0][0] = nan; matrix[0][1] = 1e+09;   matrix[0][2] = 5;    matrix[0][3] = 9;
 matrix[0][4] = 6.1; matrix[0][5] = nan;
 matrix[1][0] = 99;  matrix[1][1] = 0;       matrix[1][2] = 3.75; matrix[1][3] = 0.008;
 matrix[1][4] = nan; matrix[1][5] = nan;
 matrix[2][0] = 1;   matrix[2][1] = 2.2;     matrix[2][2] = 9;    matrix[2][3] = 5.6;
 matrix[2][4] = nan; matrix[2][5] = 3e+10;

 // write out the array2D object
 typedef itk::CSVFileWriter<double, ARows, ACols> WriterType;
 WriterType::Pointer writer = WriterType::New();
 std::string filename = argv[1];
 writer->SetFileName( filename );
 writer->SetInput( &matrix );
 try
   {
   writer->Write();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }


 typedef itk::CSVFileReader<double > ReaderType;
 ReaderType::Pointer reader = ReaderType::New();
 reader->SetFileName( filename );
 reader->SetFieldDelimiterCharacter(',');
 reader->SetStringDelimiterCharacter('"');
 reader->HaveColumnHeadersOff();
 reader->HaveRowHeadersOff();
 reader->UseStringDelimiterCharacterOff();

 // read the file
 try
   {
   reader->Parse();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 MatrixType test_matrix = reader->GetMatrix();

 std::cout << "Actual matrix: " << std::endl;
 std::cout << matrix << std::endl;
 std::cout << "Test matrix: " << std::endl;
 std::cout << test_matrix;
 std::cout << "Sizes: " << matrix.size() << " " << test_matrix.size() << std::endl;

 if (!testMatrix(matrix,test_matrix))
   {
   std::cerr << "Matrices are not the same! Test Failed!" << std::endl;
   return EXIT_FAILURE;
   }

 return EXIT_SUCCESS;
}
