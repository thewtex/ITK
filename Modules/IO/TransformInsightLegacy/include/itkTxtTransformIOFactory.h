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
#ifndef __itkTxtTransformIOFactory_h
#define __itkTxtTransformIOFactory_h

#include "itkObjectFactoryBase.h"
#include "itkTransformIOBase.h"

namespace itk
{
/** \class TxtTransformIOFactoryTemplate
   * \brief Create instances of TxtTransformIO objects using an object factory.
   * \ingroup ITKIOTransformInsightLegacy
   */
template<class ParametersValueType>
class ITK_EXPORT TxtTransformIOFactoryTemplate:public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef TxtTransformIOFactoryTemplate     Self;
  typedef ObjectFactoryBase                 Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char * GetITKSourceVersion(void) const;

  virtual const char * GetDescription(void) const;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TxtTransformIOFactoryTemplate, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    typename TxtTransformIOFactoryTemplate<ParametersValueType>::Pointer metaFactory =
        TxtTransformIOFactoryTemplate<ParametersValueType>::New();

    ObjectFactoryBase::RegisterFactoryInternal(metaFactory);
  }

protected:
  TxtTransformIOFactoryTemplate();
  ~TxtTransformIOFactoryTemplate();
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  TxtTransformIOFactoryTemplate(const Self &); //purposely not implemented
  void operator=(const Self &);        //purposely not implemented
};

/** This helps to meet backward compatibility */
typedef TxtTransformIOFactoryTemplate<double> TxtTransformIOFactory;

} // end namespace itk

extern template class itk::TxtTransformIOFactoryTemplate<float>;
extern template class itk::TxtTransformIOFactoryTemplate<double>;

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTxtTransformIOFactory.hxx"
#endif

#endif
