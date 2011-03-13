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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkConicShellInteriorExteriorSpatialFunction.h"
#include "vnl/vnl_matrix.h"

int itkConicShellInteriorExteriorSpatialFunctionTest(int, char* [] )
{
  std::cout << "itkConicShellInteriorExteriorSpatialFunctionTest test start" << std::endl;

  // Test will create an conic shell  (3-dimensional)
  const unsigned int dimension = 3;

  // Conic shell spatial function typedef
  typedef itk::ConicShellInteriorExteriorSpatialFunction<3>
  TConicFunctionType;

  // Point position typedef
  typedef TConicFunctionType::InputType      TConicFunctionInputType;
  typedef TConicFunctionType::InputValueType TConicFunctionInputValueType;
  // Point position real value typedef
  typedef TConicFunctionType::InputRealType TConicFunctionRealInputType;
  // Vector type for the direction of the central axis of the cone
  typedef TConicFunctionType::GradientType GradientType;
  // Vector type for all other vectors
  typedef TConicFunctionType::VectorType VectorType;
  // Create a conic spatial function for the source image
  TConicFunctionType::Pointer conicFunction = TConicFunctionType::New();

  // Define and set the direction of the conic shell
  GradientType originGradient;
  originGradient[0] = 3;
  originGradient[1] = 1;
  originGradient[2] = 1;

  // Define the test space, which encapsulates the truncated conic.
  int xExtent = 50;
  int yExtent = 50;
  int zExtent = 50;

  // Define and set the origin of the conic function
  TConicFunctionInputType center;
  center[0] = 0;
  center[1] = yExtent/2;
  center[2] = zExtent/2;

  TConicFunctionRealInputType  distanceMin;
  TConicFunctionRealInputType  distanceMax;
  TConicFunctionInputValueType epsilon;

  // we create a truncated cone of height 10, and angle at
  // the top 15degree
  distanceMin = 10;
  distanceMax = 20;
  double opertureDegree = 15.;
  epsilon = 1 - vcl_cos(opertureDegree*(vnl_math::pi)/180);

  conicFunction->SetOrigin( center );
  conicFunction->SetOriginGradient( originGradient );
  conicFunction->SetDistanceMin( distanceMin );
  conicFunction->SetDistanceMax( distanceMax );
  conicFunction->SetEpsilon( epsilon);
  conicFunction->SetPolarity( false );

  // position of a pixel in the test space
  double testPosition[dimension];

  bool functionValue;            // Value of pixel at a given position
  int  interiorPixelCounter = 0; // Count pixels inside conic shell

  // Evaluate all points in the spatial function and count the number of
  // pixels that are inside the conic shell.
  for(int x = 0; x < xExtent; x++)
    {
    for(int y = 0; y < yExtent; y++)
      {
      for(int z =0; z < zExtent; z++)
        {
        testPosition[0] = x;
        testPosition[1] = y;
        testPosition[2] = z;
        functionValue = conicFunction->Evaluate(testPosition);
        if(functionValue == 1)
          interiorPixelCounter++;
        }
      }
    }

  // normaly the point between distanceMin and distanceMax
  // in the direction of the gradient should be in the truncated conic
  VectorType gradientInVector;
  // this is not really pretty and comes from
  // the differenciation of covariant vectors
  // and normal vectors in ITK
  gradientInVector.SetVnlVector(originGradient.GetVnlVector() );
  gradientInVector.Normalize();
  TConicFunctionInputType
    testPoint = center + ( gradientInVector
                           * (distanceMin + distanceMax) / 2);
  functionValue = conicFunction->Evaluate(testPoint);

  // verification of volume

  TConicFunctionRealInputType volume;
  TConicFunctionRealInputType bigRadius;
  TConicFunctionRealInputType smallRadius;

  // Volume of conic basis
  bigRadius = vcl_tan(opertureDegree*(vnl_math::pi)/180)*distanceMax;
  smallRadius = vcl_tan(opertureDegree*(vnl_math::pi)/180)*distanceMin;
  volume = ( vnl_math::pi / 3 )*( distanceMax - distanceMin )
    *( bigRadius*bigRadius
       + smallRadius * smallRadius
       + bigRadius * smallRadius );

  // Percent difference in volume measurement and calculation
  double volumeError = (vcl_fabs(volume - interiorPixelCounter)/volume)*100;

  std::cout << conicFunction;

  // 5% error was randomly chosen as a successful conic shell  fill.
  // This should actually be some function of the image/conic shell  size.
  if( (volumeError <= 5 ) || (functionValue == 1) )
    {
    // function value = 1
    std::cout << "calculated conic shell volume = " << volume << std::endl
              << "measured conic shell volume = " << interiorPixelCounter << std::endl
              << "volume error = " << volumeError << "%" << std::endl
              << "function value = " << functionValue << std::endl
              << "origin location = (" << ( conicFunction->GetOrigin() )<< std::endl
              << "itkConicShellInteriorExteriorSpatialFunction ended successfully!" << std::endl;
    return EXIT_SUCCESS;
    }
  std::cerr << "calculated conic shell volume = " << volume << std::endl
            << "measured conic shell volume = " << interiorPixelCounter << std::endl
            << "volume error = " << volumeError << "%" << std::endl
            << "function value = " << functionValue << std::endl
            << "origin location = (" << ( conicFunction->GetOrigin() )<< std::endl
            << "itkConicShellInteriorExteriorSpatialFunction failed :(" << std::endl;
  return EXIT_FAILURE;
}
