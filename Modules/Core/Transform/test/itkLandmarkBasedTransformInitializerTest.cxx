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
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkImage.h"
#include <math.h>
#include <iostream>

//
// The test specifies a bunch of fixed and moving landmarks and test if the
// fixed landmarks after transform by the computed transform coincides
// with the moving landmarks....

int itkLandmarkBasedTransformInitializerTest(int, char * [])
{

  const double nPI = 4.0 * vcl_atan( 1.0 );

  {
  // Test LandmarkBasedTransformInitializer for Rigid 3D landmark
  // based alignment
  std::cout << "Testing Landmark alignment with VersorRigid3DTransform" << std::endl;

  typedef  unsigned char  PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  FixedImageType::Pointer fixedImage   = FixedImageType::New();
  MovingImageType::Pointer movingImage = MovingImageType::New();

  // Create fixed and moving images of size 30 x 30 x 30
  //
  FixedImageType::RegionType fRegion;
  FixedImageType::SizeType   fSize;
  FixedImageType::IndexType  fIndex;
  fSize.Fill(30);
  fIndex.Fill(0);
  fRegion.SetSize( fSize );
  fRegion.SetIndex( fIndex );
  MovingImageType::RegionType mRegion;
  MovingImageType::SizeType   mSize;
  MovingImageType::IndexType  mIndex;
  mSize.Fill(30);
  mIndex.Fill(0);
  mRegion.SetSize( mSize );
  mRegion.SetIndex( mIndex );
  fixedImage->SetLargestPossibleRegion( fRegion );
  fixedImage->SetBufferedRegion( fRegion );
  fixedImage->SetRequestedRegion( fRegion );
  fixedImage->Allocate();
  movingImage->SetLargestPossibleRegion( mRegion );
  movingImage->SetBufferedRegion( mRegion );
  movingImage->SetRequestedRegion( mRegion );
  movingImage->Allocate();

  // Set the transform type..
  typedef itk::VersorRigid3DTransform< double > TransformType;
  TransformType::Pointer transform = TransformType::New();
  typedef itk::LandmarkBasedTransformInitializer< TransformType,
                                                  FixedImageType, MovingImageType > TransformInitializerType;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();

  // Set fixed and moving landmarks
  TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  TransformInitializerType::LandmarkPointContainer movingLandmarks;
  TransformInitializerType::LandmarkPointType point;
  TransformInitializerType::LandmarkPointType tmp;

  // Moving Landmarks = Fixed Landmarks rotated by 'angle' degrees and then
  //    translated by the 'translation'. Offset can be used to move the fixed
  //    landmarks around.
  double angle = 10 * nPI / 180.0;
  TransformInitializerType::LandmarkPointType translation;
  translation[0] = 6;
  translation[1] = 10;
  translation[2] = 7;
  TransformInitializerType::LandmarkPointType offset;
  offset[0] = 10;
  offset[1] = 1;
  offset[2] = 5;

  point[0]=2 + offset[0];
  point[1]=2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=2 + offset[0];
  point[1]=-2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=-2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);

  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetMovingLandmarks(movingLandmarks);

  initializer->SetTransform( transform );
  initializer->InitializeTransform();

  // Transform the landmarks now. For the given set of landmarks, since we computed the
  // moving landmarks explicitly from the rotation and translation specified, we should
  // get a transform that does not give any mismatch. In other words, if the fixed
  // landmarks are transformed by the transform computed by the
  // LandmarkBasedTransformInitializer, they should coincide exactly with the moving
  // landmarks. Note that we specified 4 landmarks, although three non-collinear
  // landmarks is sufficient to guarantee a solution.
  //
  TransformInitializerType::PointsContainerConstIterator
    fitr = fixedLandmarks.begin();
  TransformInitializerType::PointsContainerConstIterator
    mitr = movingLandmarks.begin();

  typedef TransformInitializerType::OutputVectorType  OutputVectorType;
  OutputVectorType error;
  OutputVectorType::RealValueType tolerance = 0.1;
  bool failed = false;

  while( mitr != movingLandmarks.end() )
    {
    std::cout << "  Fixed Landmark: " << *fitr << " Moving landmark " << *mitr
              << " Transformed fixed Landmark : " <<
      transform->TransformPoint( *fitr ) << std::endl;

    error = *mitr - transform->TransformPoint( *fitr);
    if( error.GetNorm() > tolerance )
      {
      failed = true;
      }

    ++mitr;
    ++fitr;
    }

  if( failed )
    {
    // Hang heads in shame
    std::cout << "  Fixed landmarks transformed by the transform did not match closely "
              << " enough with the moving landmarks.  The transform computed was: ";
    transform->Print(std::cout);
    std::cout << "  [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" << std::endl;
    }
  }

  {
  //Test landmark alignment using Rigid 2D transform in 2 dimensions
  std::cout << "Testing Landmark alignment with Rigid2DTransform" << std::endl;

  typedef  unsigned char  PixelType;
  const unsigned int Dimension = 2;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  FixedImageType::Pointer fixedImage   = FixedImageType::New();
  MovingImageType::Pointer movingImage = MovingImageType::New();

  // Create fixed and moving images of size 30 x 30
  //
  FixedImageType::RegionType fRegion;
  FixedImageType::SizeType   fSize;
  FixedImageType::IndexType  fIndex;
  fSize.Fill(30);
  fIndex.Fill(0);
  fRegion.SetSize( fSize );
  fRegion.SetIndex( fIndex );
  MovingImageType::RegionType mRegion;
  MovingImageType::SizeType   mSize;
  MovingImageType::IndexType  mIndex;
  mSize.Fill(30);
  mIndex.Fill(0);
  mRegion.SetSize( mSize );
  mRegion.SetIndex( mIndex );
  fixedImage->SetLargestPossibleRegion( fRegion );
  fixedImage->SetBufferedRegion( fRegion );
  fixedImage->SetRequestedRegion( fRegion );
  fixedImage->Allocate();
  movingImage->SetLargestPossibleRegion( mRegion );
  movingImage->SetBufferedRegion( mRegion );
  movingImage->SetRequestedRegion( mRegion );
  movingImage->Allocate();

  // Set the transform type..
  typedef itk::Rigid2DTransform< double > TransformType;
  TransformType::Pointer transform = TransformType::New();
  typedef itk::LandmarkBasedTransformInitializer< TransformType,
                                                  FixedImageType, MovingImageType > TransformInitializerType;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->DebugOn();

  // Set fixed and moving landmarks
  TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  TransformInitializerType::LandmarkPointContainer movingLandmarks;
  TransformInitializerType::LandmarkPointType point;
  TransformInitializerType::LandmarkPointType tmp;

  // Moving Landmarks = Fixed Landmarks rotated by 'angle' degrees and then
  //    translated by the 'translation'. Offset can be used to move the fixed
  //    landmarks around.
  double angle = 10 * nPI / 180.0;
  TransformInitializerType::LandmarkPointType translation;
  translation[0] = 6;
  translation[1] = 10;
  TransformInitializerType::LandmarkPointType offset;
  offset[0] = 10;
  offset[1] = 1;

  point[0]=2 + offset[0];
  point[1]=2 + offset[1];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  movingLandmarks.push_back(point);
  point[0]=2 + offset[0];
  point[1]=-2 + offset[1];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=2 + offset[1];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=-2 + offset[1];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  movingLandmarks.push_back(point);

  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetMovingLandmarks(movingLandmarks);


  initializer->SetTransform( transform );
  initializer->InitializeTransform();

  // Transform the landmarks now. For the given set of landmarks, since we computed the
  // moving landmarks explicitly from the rotation and translation specified, we should
  // get a transform that does not give any mismatch. In other words, if the fixed
  // landmarks are transformed by the transform computed by the
  // LandmarkBasedTransformInitializer, they should coincide exactly with the moving
  // landmarks. Note that we specified 4 landmarks, although two
  // landmarks is sufficient to guarantee a solution.
  //
  TransformInitializerType::PointsContainerConstIterator
    fitr = fixedLandmarks.begin();
  TransformInitializerType::PointsContainerConstIterator
    mitr = movingLandmarks.begin();

  typedef TransformInitializerType::OutputVectorType  OutputVectorType;
  OutputVectorType error;
  OutputVectorType::RealValueType tolerance = 0.1;
  bool failed = false;

  while( mitr != movingLandmarks.end() )
    {
    std::cout << "  Fixed Landmark: " << *fitr << " Moving landmark " << *mitr
              << " Transformed fixed Landmark : " <<
      transform->TransformPoint( *fitr ) << std::endl;

    error = *mitr - transform->TransformPoint( *fitr);
    if( error.GetNorm() > tolerance )
      {
      failed = true;
      }

    ++mitr;
    ++fitr;
    }

  if( failed )
    {
    // Hang heads in shame
    std::cout << "  Fixed landmarks transformed by the transform did not match closely "
              << " enough with the moving landmarks.  The transform computed was: ";
    transform->Print(std::cout);
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "  Landmark alignment using Rigid2D transform [PASSED]" << std::endl;
    }
  }

  {
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<PixelType,Dimension> ImageType;
  ImageType::Pointer fixedImage   = ImageType::New();
  ImageType::Pointer movingImage = ImageType::New();

  // Create fixed and moving images of size 30 x 30 x 30
  //
  ImageType::RegionType fRegion;
  ImageType::SizeType   fSize;
  ImageType::IndexType  fIndex;
  fSize.Fill(30);
  fIndex.Fill(0);
  fRegion.SetSize( fSize );
  fRegion.SetIndex( fIndex );
  ImageType::RegionType mRegion;
  ImageType::SizeType   mSize;
  ImageType::IndexType  mIndex;
  mSize.Fill(30);
  mIndex.Fill(0);
  mRegion.SetSize( mSize );
  mRegion.SetIndex( mIndex );
  fixedImage->SetLargestPossibleRegion( fRegion );
  fixedImage->SetBufferedRegion( fRegion );
  fixedImage->SetRequestedRegion( fRegion );
  fixedImage->Allocate();
  movingImage->SetLargestPossibleRegion( mRegion );
  movingImage->SetBufferedRegion( mRegion );
  movingImage->SetRequestedRegion( mRegion );
  movingImage->Allocate();

  typedef itk::AffineTransform<double,Dimension> TransformType;
  TransformType::Pointer transform = TransformType::New();
  typedef itk::LandmarkBasedTransformInitializer< TransformType,
                                                  ImageType, ImageType > TransformInitializerType;
  TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  TransformInitializerType::LandmarkPointContainer fixedLandmarks;
  TransformInitializerType::LandmarkPointContainer movingLandmarks;
#if 0
  //
  // this is a duplicate of the Slicer3 landmarks used
  // in Eun Young Kim's original testing.
  double fixedLandMarkInit[6][3] =
    {
      { -1.33671, -279.739, 176.001 },
      { 28.0989, -346.692, 183.367 },
      { -1.36713, -257.43, 155.36 },
      { -33.0851, -347.026, 180.865 },
      { -0.16083, -268.529, 148.96 },
      { -0.103873, -251.31, 122.973 }
    };
  double movingLandmarkInit[6][3] =
    {
      { -1.57264, -30.1293, 20.4644 },
      { 28.1456, -93.081, -5.36121 },
      { -1.57186, -0.250447, 12.5043 },
      { -33.0134, -92.073, -8.68572 },
      { -0.358551, -7.66854, 1.5261 },
      { 0.194046, 20.2889, -12.6472 }
    };
  for(unsigned i = 0; i < 6; i++)
    {
    TransformInitializerType::LandmarkPointType fixedPoint, movingPoint;
    for(unsigned j = 0; j < 3; j++)
      {
      fixedPoint[j] = fixedLandMarkInit[i][j];
      movingPoint[j] = movingLandmarkInit[i][j];
      }
    fixedLandmarks.push_back(fixedPoint);
    movingLandmarks.push_back(movingPoint);
    }
#else
  //
  // this uses the same landmark set used above for VersorRigid3D testing
  TransformInitializerType::LandmarkPointType point,tmp;
  // Moving Landmarks = Fixed Landmarks rotated by 'angle' degrees and then
  //    translated by the 'translation'. Offset can be used to move the fixed
  //    landmarks around.
  double angle = 10 * nPI / 180.0;
  TransformInitializerType::LandmarkPointType translation;
  translation[0] = 6;
  translation[1] = 10;
  translation[2] = 7;
  TransformInitializerType::LandmarkPointType offset;
  offset[0] = 10;
  offset[1] = 1;
  offset[2] = 5;

  point[0]=2 + offset[0];
  point[1]=2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=2 + offset[0];
  point[1]=-2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
  point[0]=-2 + offset[0];
  point[1]=-2 + offset[1];
  point[2]=0 + offset[2];
  fixedLandmarks.push_back(point);
  tmp = point;
  point[0] = vcl_cos(angle)*point[0] - vcl_sin(angle)*point[1] + translation[0];
  point[1] = vcl_sin(angle)*tmp[0] + vcl_cos(angle)*point[1] + translation[1];
  point[2] = point[2] + translation[2];
  movingLandmarks.push_back(point);
#endif
  initializer->SetFixedLandmarks(fixedLandmarks);
  initializer->SetMovingLandmarks(movingLandmarks);
  initializer->SetTransform(transform);
  initializer->InitializeTransform();

  std::cerr << "Transform " << transform << std::endl;

  TransformInitializerType::PointsContainerConstIterator
    fitr = fixedLandmarks.begin();
  TransformInitializerType::PointsContainerConstIterator
    mitr = movingLandmarks.begin();

  typedef TransformInitializerType::OutputVectorType  OutputVectorType;
  OutputVectorType error;
  OutputVectorType::RealValueType tolerance = 0.1;
  bool failed = false;

  while( mitr != movingLandmarks.end() )
    {
    std::cout << "  Fixed Landmark: " << *fitr << " Moving landmark " << *mitr
              << " Transformed fixed Landmark : " <<
      transform->TransformPoint( *fitr ) << std::endl;

    error = *mitr - transform->TransformPoint( *fitr);
    if( error.GetNorm() > tolerance )
      {
      failed = true;
      }

    ++mitr;
    ++fitr;
    }

  if( failed )
    {
    // Hang heads in shame
    std::cout << "  Fixed landmarks transformed by the transform did not match closely "
              << " enough with the moving landmarks.  The transform computed was: ";
    transform->Print(std::cout);
    std::cout << "  [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "  Landmark alignment using Affine transform [PASSED]" << std::endl;
    }
  }
  return EXIT_SUCCESS;
}
