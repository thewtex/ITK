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

#ifndef __itkFEMNoisyLoadLandmark_h
#define __itkFEMNoisyLoadLandmark_h

#include "itkFEMLoadLandmark.h"

namespace itk
{
namespace fem
{
/**
 * \class FEMNoisyLoadLandmark
 * \brief This landmark is derived from the motion of a specific landmark, but
 * allows the existance of noise or outliers
 *
 * \author Yixun Liu
 *
 * \ingroup ITKFEM
 */
class NoisyLoadLandmark : public LoadLandmark
{
public:
  /** Standard class typedefs. */
  typedef NoisyLoadLandmark             Self;
  typedef LoadLandmark                  Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Some convenient typedefs */
  typedef  Element::VectorType  VectorType;
  typedef  Element::MatrixType  MatrixType;

  /** Method for creation through the object factory. */
  itkSimpleNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NoisyLoadLandmark, LoadLandmark);

  /** Outlier or not */
  void SetOutlier(bool outlier)
  {
      m_IsOutlier = outlier;
  }
  bool IsOutlier() {return m_IsOutlier;}

  /** Set/Get Error norm */
  void SetErrorNorm(float errorNorm)
  {
      m_ErrorNorm = errorNorm;
  }

  float GetErrorNorm()
  {
      return m_ErrorNorm;
  }

  /** Set/Get Confidence */
  void SetConfidence(float confidence)
  {
      m_Confidence = confidence;
  }

  float GetConfidence()
  {
      return m_Confidence;
  }

  /** Set/Get real displacement */
  void SetRealDisplacement(VectorType displacement)
  {
      m_RealDisplacement = displacement;
  }

  VectorType GetRealDisplacement()
  {
      return m_RealDisplacement;
  }

  /** Set/Get simulated displacement */
  void SetSimulatedDisplacement(VectorType displacement)
  {
      m_SimulatedDisplacement = displacement;
  }

  VectorType GetSimulatedDisplacement()
  {
      return m_SimulatedDisplacement;
  }

  /** Set/Get Shape function */
  void SetShape(VectorType shape)
  {
      m_Shape = shape;
  }

  VectorType GetShape()
  {
      return m_Shape;
  }

  /** Set/Get flag for outside of the mesh */
  void SetIsOutOfMesh(bool out)
  {
      m_IsOutOfMesh = out;
  }

  bool IsOutOfMesh()
  {
      return m_IsOutOfMesh;
  }

  /** Set/Get Structure tensor */
  void SetStructureTensor(MatrixType& structureTensor)
  {
    m_StructureTensor = structureTensor;
    m_HasStructureTensor = true;
  }
  MatrixType& GetStructureTensor()
  {
      return m_StructureTensor;
  }

  bool HasStructureTensor()
  {
      return m_HasStructureTensor;
  }

  /** Set/Get Landmark tensor */
  void SetLandmarkTensor(MatrixType& landmarkTensor)
  {
    m_LandmarkTensor = landmarkTensor;
  }
  MatrixType& GetLandmarkTensor()
  {
      return m_LandmarkTensor;
  }

protected:

   /**
   * Default constructors
   */
  NoisyLoadLandmark()
  {
      m_IsOutlier = false;
      m_Confidence = 1.0;
      m_HasStructureTensor = false;
      m_IsOutOfMesh = false;
  }

  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /**
  * Confidence of the landmark
  */
  float m_Confidence;

 /**
  * Real displacement of the landmark
  */
   VectorType m_RealDisplacement;

  /**
  * Simulated displacement of the landmark
  */
   VectorType m_SimulatedDisplacement;

   /**
   * Shape function vector
   */
   VectorType m_Shape;

   /**
  * Magnitude of the error
  */
   float m_ErrorNorm;

   /**
   * Outlier or not
   */
   bool m_IsOutlier;

   /**
   * Outside of mesh
   */
   bool m_IsOutOfMesh;

   /**
   * Has structure tensor or not
   */
   bool m_HasStructureTensor;

  /**
  * Structure tensor
  */
  MatrixType m_StructureTensor;

  /**
  * Landmark tensor, which can be the stiffness tensor or the product
  * of the stiffness tensor and the structure tensor
  */
  MatrixType m_LandmarkTensor;

};

}
}  // end namespace itk::fem

#endif // #ifndef __itkNoisyFEMLoadLandmark_h
