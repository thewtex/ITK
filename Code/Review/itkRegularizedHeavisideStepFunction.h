/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRegularizedHeavisideStepFunction.h
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkRegularizedHeavisideStepFunction_h
#define __itkRegularizedHeavisideStepFunction_h

#include "itkHeavisideStepFunctionBase.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

namespace itk
{
/** \class RegularizedHeavisideStepFunction
 *
 * \brief Base class of the Regularized (smoothed) Heaviside functions.
 *
 * \author Mosaliganti K., Smith B., Gelas A., Gouaillard A., Megason S.
 *
 *  This code was taken from the Insight Journal paper:
 *
 *      "Cell Tracking using Coupled Active Surfaces for Nuclei and Membranes"
 *      http://www.insight-journal.org/browse/publication/642
 *      http://hdl.handle.net/10380/3055
 *
 *  That is based on the papers:
 *
 *      "Level Set Segmentation: Active Contours without edge"
 *      http://www.insight-journal.org/browse/publication/322
 *      http://hdl.handle.net/1926/1532
 *
 *      and
 *
 *      "Level set segmentation using coupled active surfaces"
 *      http://www.insight-journal.org/browse/publication/323
 *      http://hdl.handle.net/1926/1533
 *
 *
 */
template< class TInput = float, class TOutput = double >
class RegularizedHeavisideStepFunction:public HeavisideStepFunctionBase< TInput, TOutput >
{
public:
  typedef RegularizedHeavisideStepFunction             Self;
  typedef HeavisideStepFunctionBase< TInput, TOutput > Superclass;
  typedef SmartPointer< Self >                         Pointer;
  typedef SmartPointer< const Self >                   ConstPointer;

  typedef typename Superclass::InputType  InputType;
  typedef typename Superclass::OutputType OutputType;

  typedef typename NumericTraits< InputType >::RealType RealType;

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate(const InputType & input) const = 0;

  /** Evaluate the derivative at the specified input position */
  virtual OutputType EvaluateDerivative(const InputType & input) const = 0;

  void SetEpsilon(const RealType & ieps)
  {
    this->m_Epsilon = ieps;

    if ( ieps > vnl_math::eps )
      {
      m_OneOverEpsilon = 1.0 / ieps;
      }
    else
      {
      itkGenericExceptionMacro("ERROR: Epsilon needs to be greater than " << vnl_math::eps);
      }
  }

  RealType GetEpsilon() const
  {
    return this->m_Epsilon;
  }

  RealType GetOneOverEpsilon() const
  {
    return this->m_OneOverEpsilon;
  }

protected:
  RegularizedHeavisideStepFunction()
  {
    this->m_Epsilon = 1.0;
    this->m_OneOverEpsilon = 1.0;
  }

  virtual ~RegularizedHeavisideStepFunction() {}
private:
  RegularizedHeavisideStepFunction(const Self &); //purposely not implemented
  void operator=(const Self &);                   //purposely not implemented

  RealType m_Epsilon;
  RealType m_OneOverEpsilon;
};
}

#endif
