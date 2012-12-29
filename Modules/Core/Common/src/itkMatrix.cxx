
#include "itkMatrix.h"

namespace itk
{

//Paritally specialized versions for performance of common cases
template <>
Vector< double, 2 >
Matrix< double, 2, 2 >
::operator*(const Vector< double, 2 > & vect) const
{
  Vector< double, 2 > result;
  double * const rr=&result[0];
  double const * const vv=&vect[0];
  double const * const A=m_Matrix[0]; //Get a pointer to the first element of array
  rr[0]  = A[0] * vv[0] + A[1] * vv[1];
  rr[1]  = A[2] * vv[0] + A[3] * vv[1];
  return result;
}


//Paritally specialized versions for performance of common cases
template <>
Vector< double, 3 >
Matrix< double, 3, 3 >
::operator*(const Vector< double, 3 > & vect) const
{
  Vector< double, 3 > result;
  double * const rr=&result[0];
  double const * const vv=&vect[0];
  double const * const A=m_Matrix[0]; //Get a pointer to the first element of array
  rr[0]  = A[0] * vv[0] + A[1] * vv[1] + A[2] * vv[2];
  rr[1]  = A[3] * vv[0] + A[4] * vv[1] + A[5] * vv[2];
  rr[2]  = A[6] * vv[0] + A[7] * vv[1] + A[8] * vv[2];
  return result;
}

}
