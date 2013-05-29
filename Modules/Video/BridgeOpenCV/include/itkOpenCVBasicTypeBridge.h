#ifndef __OpenCVBasicTypeBridge_h
#define __OpenCVBasicTypeBridge_h

#include "itkPoint.h"
#include "itkSize.h"
#include "itkIndex.h"
#include "itkMatrix.h"
//#include "itkQuadEdgeMeshPoint.h"

#include "cv.h"

namespace itk
{
template< class TITKData, class TOpenCVData >
struct OpenCVBasicTypeBridge
{};

template< class TCoordinate >
struct OpenCVBasicTypeBridge< itk::Point< TCoordinate, 2 >, cv::Point_< TCoordinate > >
{
  typedef itk::Point< TCoordinate, 2 >  ITKDataType;
  typedef cv::Point_< TCoordinate >     OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = iP.x;
    oP[1] = iP.y;

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( iP[0], iP[1] );
  }
};

template< class TCoordinate >
struct OpenCVBasicTypeBridge< itk::Point< TCoordinate, 3 >, cv::Point3_< TCoordinate > >
{
  typedef itk::Point< TCoordinate, 3 >  ITKDataType;
  typedef cv::Point3_< TCoordinate >    OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = iP.x;
    oP[1] = iP.y;
    oP[2] = iP.z;

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( iP[0], iP[1], iP[2] );
  }
};
/*
template< class TCoordinate >
struct OpenCVBasicTypeBridge< itk::QuadEdgeMeshPoint< TCoordinate, 3 >, cv::Point3_< TCoordinate > >
{
  typedef itk::QuadEdgeMeshPoint< TCoordinate, 3 >  ITKDataType;
  typedef cv::Point_< TCoordinate >                 OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = iP.x;
    oP[1] = iP.y;
    oP[2] = iP.z;

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( iP[0], iP[1], iP[2] );
  }
};
*/
template<>
struct OpenCVBasicTypeBridge< itk::Index< 2 >, cv::Point >
{
  typedef itk::Index< 2 >  ITKDataType;
  typedef cv::Point        OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = static_cast< itk::IndexValueType >( iP.x );
    oP[1] = static_cast< itk::IndexValueType >( iP.y );

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( static_cast< int >( iP[0] ), static_cast< int >( iP[1] ) );
  }
};

template<>
struct OpenCVBasicTypeBridge< itk::Index< 3 >, cv::Point3i >
{
  typedef itk::Index< 3 >  ITKDataType;
  typedef cv::Point3i      OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = static_cast< itk::IndexValueType >( iP.x );
    oP[1] = static_cast< itk::IndexValueType >( iP.y );
    oP[2] = static_cast< itk::IndexValueType >( iP.z );

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( static_cast< int >( iP[0] ), static_cast< int >( iP[1] ), static_cast< int >( iP[2] ) );
  }
};

template<>
struct OpenCVBasicTypeBridge< itk::Size< 2 >, cv::Size >
{
  typedef itk::Size< 2 >    ITKDataType;
  typedef cv::Size          OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    ITKDataType oP;
    oP[0] = static_cast< itk::SizeValueType >( iP.width );
    oP[1] = static_cast< itk::SizeValueType >( iP.height );

    return oP;
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    return OpenCVDataType( static_cast< int >( iP[0] ), static_cast< int >( iP[1] ) );
  }
};

template< class T, unsigned int NRows, unsigned int NColumns >
struct OpenCVBasicTypeBridge< itk::Matrix< T, NRows, NColumns >, cv::Matx< T, NRows, NColumns > >
{
  typedef itk::Matrix< T, NRows, NColumns > ITKDataType;
  typedef cv::Matx< T, NRows, NColumns >    OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    return ITKDataType( typename ITKDataType::InternalMatrixType().copy_in( iP.val ) );
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    OpenCVDataType oM;
    iP.GetVnlMatrix().copy_out( oM.val );
    return oM;
  }

};

template< class T, unsigned int VDimension >
struct OpenCVBasicTypeBridge< itk::Vector< T, VDimension >, cv::Vec< T, VDimension > >
{
  typedef itk::Vector< T, VDimension >  ITKDataType;
  typedef cv::Vec< T, VDimension >      OpenCVDataType;

  static ITKDataType FromOpenCVToITK( const OpenCVDataType& iP )
  {
    return ITKDataType( iP.val );
  }

  static OpenCVDataType FromITKToOpenCV( const ITKDataType & iP )
  {
    OpenCVDataType oM;
    std::copy( iP.Begin(), iP.End(), oM.val );
    return oM;
  }
};

}
#endif
