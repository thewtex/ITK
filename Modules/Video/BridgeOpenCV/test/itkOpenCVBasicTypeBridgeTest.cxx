#include "itkOpenCVBasicTypeBridge.h"

int itkOpenCVBasicTypeBridgeTest( int, char* [] )
{
  typedef itk::Point< int, 2 >    ITKPoint2iType;
  typedef itk::Point< float, 2 >  ITKPoint2fType;
  typedef itk::Point< double, 2 > ITKPoint2dType;

  cv::Point   a( 1, 2 );
  cv::Point2f b( 0.1, 0.2 );
  cv::Point2d c( -0.4, 3.2 );

  ITKPoint2iType itkA = itk::OpenCVBasicTypeBridge< ITKPoint2iType, cv::Point >::FromOpenCVToITK( a );
  ITKPoint2fType itkB = itk::OpenCVBasicTypeBridge< ITKPoint2fType, cv::Point2f >::FromOpenCVToITK( b );
  ITKPoint2dType itkC = itk::OpenCVBasicTypeBridge< ITKPoint2dType, cv::Point2d >::FromOpenCVToITK( c );

  return EXIT_SUCCESS;
}
