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

#include "itkCSVFileIO.h"
#include "itkArray2D.h"
#include "vnl/vnl_matrix.h"
#include "itkMatrix.h"
#include <iostream>


int itkCSVFileWriterTest (int argc, char *argv[])
{
 if (argc < 2)
 {
 std::cerr << "Usage: " << argv[0] << " Filename" << std::endl;
 return EXIT_FAILURE;
 }

 double nan = vcl_numeric_limits<double>::quiet_NaN();

 typedef itk::Array2D<double> ArrayType;
 ArrayType array(3,6);
 array[0][0] = nan; array[0][1] = 1e+09;   array[0][2] = 5;    array[0][3] = 9;
 array[0][4] = 6.1; array[0][5] = nan;
 array[1][0] = 99;  array[1][1] = 0;       array[1][2] = 3.75; array[1][3] = 0.008;
 array[1][4] = nan; array[1][5] = nan;
 array[2][0] = 1;   array[2][1] = 2.2;     array[2][2] = 9;    array[2][3] = 5.6;
 array[2][4] = nan; array[2][5] = 3e+10;

 typedef itk::CSVFileWriter<ArrayType> Array2DWriterType;
 Array2DWriterType::Pointer array_writer = Array2DWriterType::New();

 // should throw an exception as there is no input file nor any object to write out
 try
   {
   array_writer->Write();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 std::string filename = argv[1];
 array_writer->SetFileName( filename );
 // should throw an exception as there is no input object
 try
   {
   array_writer->Write();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 array_writer->SetInput( &array );
 try
   {
   array_writer->Write();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 std::cout << "The file has been written. " << std::endl;

 typedef vnl_matrix<double> vnlMatrixType;
 vnlMatrixType vnlmatrix(3,4);
 vnlmatrix[0][0] = nan; vnlmatrix[0][1] = 1e+09;   vnlmatrix[0][2] = 5;    vnlmatrix[0][3] = 9;
 vnlmatrix[1][0] = 99;  vnlmatrix[1][1] = 0;       vnlmatrix[1][2] = 3.75; vnlmatrix[1][3] = 0.008;
 vnlmatrix[2][0] = 1;   vnlmatrix[2][1] = 2.2;     vnlmatrix[2][2] = 9;    vnlmatrix[2][3] = 5.6;

 typedef itk::CSVFileWriter<vnlMatrixType> vnlMatrixWriterType;
 vnlMatrixWriterType::Pointer vnl_matrix_writer = vnlMatrixWriterType::New();
 vnl_matrix_writer->SetFileName( filename );
 vnl_matrix_writer->SetInput( &vnlmatrix );
 try
   {
   vnl_matrix_writer->Write();
   }
 catch (itk::ExceptionObject& exp)
   {
   std::cerr << "Exception caught!" << std::endl;
   std::cerr << exp << std::endl;
   }

 return EXIT_SUCCESS;
}
