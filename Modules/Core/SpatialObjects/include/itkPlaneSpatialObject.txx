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
#ifndef __itkPlaneSpatialObject_txx
#define __itkPlaneSpatialObject_txx

#include "itkPlaneSpatialObject.h"

namespace itk
{
/** Constructor */
template< unsigned int TDimension >
PlaneSpatialObject< TDimension >
::PlaneSpatialObject()
{
  this->SetTypeName("PlaneSpatialObject");
  this->SetDimension(TDimension);
  m_Point.Fill(0);
  m_Normal.Fill(0);
}

/** Destructor */
template< unsigned int TDimension >
PlaneSpatialObject< TDimension >
::~PlaneSpatialObject()
{}

/** Test whether a point is inside or outside the object
 *  For computational speed purposes, it is faster if the method does not
 *  check the name of the class and the current depth */
template< unsigned int TDimension >
bool
PlaneSpatialObject< TDimension >
::IsInside(const PointType & point) const
{
  return IsInside(point, 0, NULL);
}

/** Test if the given point is inside the blob */
template< unsigned int TDimension >
bool
PlaneSpatialObject< TDimension >
::IsInside(const PointType & point, unsigned int depth, char *name) const
{
  itkDebugMacro("Checking if the point " << point << "is on the plane...");

  // The distance from point q to the plane defined by p and n is |dot((q-p), n)|/|n|

  // Convert the input point and plane point to a vector
  VectorType inputPointAsVector;
  VectorType planePointAsVector;
  for(unsigned int dimensionIndex = 0; dimensionIndex < point.GetPointDimension(); ++dimensionIndex)
    {
    inputPointAsVector[dimensionIndex] = point[dimensionIndex];
    planePointAsVector[dimensionIndex] = m_Point[dimensionIndex];
    }
  double distanceToPlane = vnl_math_abs((inputPointAsVector-planePointAsVector)*m_Normal)/(m_Normal.GetNorm());
  if(distanceToPlane < itk::NumericTraits< double >::epsilon() )
    {
    return true;
    }
  else
    {
    return false;
    }
}

/** Compute the bounds of the Plane */
template< unsigned int TDimension >
bool
PlaneSpatialObject< TDimension >
::ComputeLocalBoundingBox(void) const
{
  itkDebugMacro("Computing bounding box...");
  std::cerr << "This function is not yet implemented!" << std::endl;
  exit(-1);
  return false;
}

/** Returns if the Plane is evaluable at a specified point */
template< unsigned int TDimension >
bool
PlaneSpatialObject< TDimension >
::IsEvaluableAt(const PointType & point,
                unsigned int depth, char *name) const
{
  itkDebugMacro("Checking if the Plane is evaluable at " << point);
  std::cerr << "This function is not yet implemented!" << std::endl;
  exit(-1);
  return false;
}

/** Returns the value at a specified point */
template< unsigned int TDimension >
bool
PlaneSpatialObject< TDimension >
::ValueAt(const PointType & point, double & value, unsigned int depth,
          char *name) const
{
  itkDebugMacro("Getting the value of the tube at " << point);
  if ( IsInside(point, 0, name) )
    {
    value = this->GetDefaultInsideValue();
    return true;
    }
  else
    {
    if ( Superclass::IsEvaluableAt(point, depth, name) )
      {
      Superclass::ValueAt(point, value, depth, name);
      return true;
      }
    else
      {
      value = this->GetDefaultOutsideValue();
      return false;
      }
    }
  return false;
}

/** Print Self function */
template< unsigned int TDimension >
void
PlaneSpatialObject< TDimension >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Point: " << m_Point << std::endl;
  os << indent << "Normal: " << m_Normal << std::endl;
}
} // end namespace itk

#endif
