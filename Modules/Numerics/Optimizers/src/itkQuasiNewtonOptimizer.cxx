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
#ifndef _itkQuasiNewtonOptimizer_txx
#define _itkQuasiNewtonOptimizer_txx

#include "itkQuasiNewtonOptimizer.h"
#include "itkCommand.h"
#include "itkEventObject.h"
#include "itkMacro.h"

namespace itk
{
/**
 * Constructor
 */
QuasiNewtonOptimizer
::QuasiNewtonOptimizer()
{
  itkDebugMacro("Constructor");
}

void
QuasiNewtonOptimizer
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

/**
 * Advance one Step following the gradient direction
 */
void
QuasiNewtonOptimizer
::AdvanceOneStep(void)
{
  itkDebugMacro("AdvanceOneStep");

  double learningRate;

  if (this->GetCurrentIteration() == 0)
    {
    m_PreviousPosition = m_CurrentPosition;
    m_PreviousGradient = m_Gradient;
    }

  this->EstimateNewtonStep();

  /** Save for the next iteration */
  m_PreviousPosition = this->GetCurrentPosition();
  m_PreviousGradient = this->GetGradient();

  m_Gradient = m_NewtonAscent;

  /** Estimate learningRate */
  learningRate = this->EstimateLearningRate();
  learningRate = vnl_math_min(learningRate, 1.0);
  this->SetLearningRate( learningRate );
  //std::cout << " m_NewtonAscent = " << m_NewtonAscent << std::endl;
  //std::cout << " learningRate = " << learningRate << std::endl;

  this->Superclass::AdvanceOneStep();

}

/** Estimate Hessian step */
void QuasiNewtonOptimizer
::EstimateNewtonStep()
{
  int numPara = m_CurrentPosition.size();
  ParametersType sx1(numPara);
  ParametersType sx2(numPara);
  DerivativeType sg1(numPara);
  DerivativeType sg2(numPara);

  // Translate to the scaled space
  this->ScalePosition(m_PreviousPosition, sx1);
  this->ScalePosition(m_CurrentPosition,  sx2);
  this->ScaleDerivative(m_PreviousGradient, sg1);
  this->ScaleDerivative(m_Gradient,         sg2);

  // Estimate Hessian in the scaled space
  EstimateHessian(sx1, sx2, sg1, sg2);

  // Compute the Newton step
  ParametersType sdx(numPara);
  sdx = m_HessianInverse * sg2;

  // Translate the step back into the original space
  ParametersType dx(numPara);
  this->ScaleBackDerivative(sdx, dx);

  m_NewtonAscent = dx;

}

/** Estimate Hessian matrix */
void QuasiNewtonOptimizer
::EstimateHessian(ParametersType x1, ParametersType x2,
                  DerivativeType g1, DerivativeType g2)
{
  int numPara = x1.size();

  // Initialize Hessian to identity matrix
  if ( this->GetCurrentIteration() == 0 )
    {
    m_Hessian.SetSize(numPara, numPara);
    m_Hessian.Fill(0.0f);
    m_HessianInverse.SetSize(numPara, numPara);
    m_HessianInverse.Fill(0.0f);

    for (int i=0; i<numPara; i++)
      {
      m_Hessian[i][i] = 1.0; //identity matrix
      m_HessianInverse[i][i] = 1.0; //identity matrix
      }
    return;
    }

  HessianType oldHessian(numPara, numPara);
  oldHessian = m_Hessian;

  ParametersType dx(numPara);  //delta of position x: x_k+1 - x_k
  ParametersType dg(numPara);  //delta of gradient: g_k+1 - g_k
  ParametersType edg(numPara); //estimated delta of gradient: hessian_k * dx

  dx = x2 - x1;
  dg = g2 - g1;
  edg = oldHessian * dx;

  vnl_matrix<double> plus  = outer_product(dg, dg) / inner_product(dg, dx);
  vnl_matrix<double> minus = outer_product(edg, edg) / inner_product(edg, dx);
  vnl_matrix<double> newHessian = oldHessian + plus - minus;

  /*
  std::cout << "x1 = " << x1 << std::endl;
  std::cout << "x2 = " << x2 << std::endl;
  std::cout << "g1 = " << g1 << std::endl;
  std::cout << "g2 = " << g2 << std::endl;
  std::cout << "oldHessian = [" << (vnl_matrix<double>)oldHessian << "]" << std::endl;
  std::cout << "newHessian = [" << newHessian << "]" << std::endl;
  */

  m_Hessian         = newHessian;
  m_HessianInverse  = vnl_matrix_inverse<double>(newHessian);

}

/** Translate the parameters into the scaled space */
void QuasiNewtonOptimizer::ScalePosition(ParametersType p1, ParametersType &p2)
{
  double scale = 1.0;
  for (unsigned int i=0; i<p1.size(); i++)
    {
      scale = vcl_sqrt(this->GetScales()[i]);
      p2[i] = p1[i] * scale;
    }
}

/** Translate the parameters into the original space */
void QuasiNewtonOptimizer::ScaleBackPosition(ParametersType p1, ParametersType &p2)
{
  double scale = 1.0;
  for (unsigned int i=0; i<p1.size(); i++)
    {
      scale = vcl_sqrt(this->GetScales()[i]);
      p2[i] = p1[i] / scale;
    }
}

/** Translate the derivative into the scaled space */
void QuasiNewtonOptimizer::ScaleDerivative(DerivativeType g1, DerivativeType &g2)
{
  double scale = 1.0;
  for (unsigned int i=0; i<g1.size(); i++)
    {
      scale = vcl_sqrt(this->GetScales()[i]);
      g2[i] = g1[i] / scale;
    }
}


/** Translate the derivative into the original space */
void QuasiNewtonOptimizer::ScaleBackDerivative(DerivativeType g1, DerivativeType &g2)
{
  double scale = 1.0;
  for (unsigned int i=0; i<g1.size(); i++)
    {
      scale = vcl_sqrt(this->GetScales()[i]);
      g2[i] = g1[i] * scale;
    }
}

} // end namespace itk

#endif
