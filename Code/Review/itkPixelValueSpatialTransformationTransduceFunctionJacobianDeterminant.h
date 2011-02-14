/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCompositeTransformTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPixelValueSpatialTransformationTransduceFunctionJacobianDeterminant_h
#define __itkPixelValueSpatialTransformationTransduceFunctionJacobianDeterminant_h

#include <iostream>

#include "itkAffineTransform.h"
#include "itkPixelValueSpatialTransformationTransduceFunctionBase.h"
#include "vnl/algo/vnl_determinant.h"

/**
 * \class PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant
 * \brief Sample Jacobian Determinant transducer function object
 *
 * Work in progress - early example use of
 *  PixelValueSpatialTransformationTransduceFunctionBase class.
 *
 * Functor that returns an input value multiplied by the jacobian determinant
 * of an Affine transform's matrix.
 *
 * The user can set a transform via SetTransform. By default, the
 * transform is set to and identity AffineTransform in the ctor. After setting
 * the transform and before use, Prepare must be called to perform any required
 * initialization. When used in ResampleImageFilter, the transform is set
 * automatically, after which Prepare is called.
 *
 * TODO:
 *  - decide on details of determinant calc.
 *  - currently assumes assigned transform is an affine
 *  - which types of transforms should this work for?
 *
 * \ingroup Functions
 * \sa PixelValueSpatialTransformationTransduceFunctionBase
 */

namespace itk
{

/* Derived class for this test */
template< class TInput,
          class TOutput,
          class TPoint,
          unsigned int NInputDimensions,
          unsigned int NOutputDimensions >
class ITK_EXPORT PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant:
  public PixelValueSpatialTransformationTransduceFunctionBase
    <TInput,TPoint,NInputDimensions,NOutputDimensions>
{
public:
  /** Standard class typedefs. */
  typedef PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant Self;
  typedef PixelValueSpatialTransformationTransduceFunctionBase< TInput,
                                                                TPoint,
                                                                NInputDimensions,
                                                                NOutputDimensions>
                                                                Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant, Superclass);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  /** Affine type for transform */
  typedef itk::AffineTransform< typename TPoint::CoordRepType,
                                  NOutputDimensions> AffineTransformType;

  /** Parameter types */
  typedef typename Superclass::InputType  InputType;
  typedef typename Superclass::OutputType OutputType;
  typedef typename Superclass::PointType  PointType;

  /** Prepare the transducer for use. Must be called when a new transform
   * is set. */
  virtual void Prepare( void )
    {
    if( this->m_Transform.IsNull() )
      {
      itkExceptionMacro("Prepare: m_Transform is NULL.");
      }
    //TODO - sort out:
    //2x2 Affine txfm is returning a 4x2 jacobian when called as
    // affineTransform->GetJacobian( point );
    typename AffineTransformType::ConstPointer affine =
      dynamic_cast<const AffineTransformType*> (this->m_Transform.GetPointer());
    if( affine.IsNull() )
      {
      itkExceptionMacro("Failed casting transform to affine type." <<
                        "m_Transform class: "
                        << this->m_Transform->GetNameOfClass(); )
      }
      m_Determinant = vnl_determinant( affine->GetMatrix().GetVnlMatrix() );
    }

  /** Function operator - return input */
  inline
  const OutputType& operator()
    (const PointType & point, const InputType & input) const
    {
    m_FunctorOutput = m_Determinant * input;
    return m_FunctorOutput;
    }

protected:
  PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant()
    {
    /* Create a default identity Affine transform */
    typename AffineTransformType::Pointer aff = AffineTransformType::New();
    aff->SetIdentity();
    this->SetTransform( aff );
    /* Prepare for use once transform's been set */
    this->Prepare();
    }
  ~PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant(){}

  /* Holds output of function operator to enable return-by-reference */
  mutable OutputType  m_FunctorOutput;

  typename AffineTransformType::MatrixValueType   m_Determinant;

  virtual void PrintSelf(std::ostream & os, Indent indent) const
    {
    Superclass::PrintSelf(os, indent);
    }

private:
  PixelValueSpatialTransformationTransduceFunctionJacobianDeterminant(const Self &);
    //purposely not implemented
  void operator=(const Self &); //purposely not implemented

};

}

#endif
