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
#ifndef __itkHDF5TransformIOFactory_h
#define __itkHDF5TransformIOFactory_h


#include "itkObjectFactoryBase.h"
#include "itkHDF5TransformIO.h"

namespace itk
{
/** \class HDF5TransformIOFactoryTemplate
   * \brief Create instances of HDF5TransformIO objects using an object factory.
   *
   * \ingroup ITKIOTransformHDF5
   */
template<class ParametersValueType>
class ITK_EXPORT HDF5TransformIOFactoryTemplate:public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef HDF5TransformIOFactoryTemplate      Self;
  typedef ObjectFactoryBase                   Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char * GetITKSourceVersion(void) const;

  virtual const char * GetDescription(void) const;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(HDF5TransformIOFactoryTemplate, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    typename HDF5TransformIOFactoryTemplate<ParametersValueType>::Pointer metaFactory =
      HDF5TransformIOFactoryTemplate<ParametersValueType>::New();

    ObjectFactoryBase::RegisterFactoryInternal(metaFactory);
  }

protected:
  HDF5TransformIOFactoryTemplate();
  ~HDF5TransformIOFactoryTemplate();
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  HDF5TransformIOFactoryTemplate(const Self &); //purposely not implemented
  void operator=(const Self &);        //purposely not implemented
};

/** This helps to meet backward compatibility */
typedef HDF5TransformIOFactoryTemplate<double> HDF5TransformIOFactory;

} // end namespace itk

#ifndef itkHDF5TransformIOFactory_cxx
extern template class itk::HDF5TransformIOFactoryTemplate<float>;
extern template class itk::HDF5TransformIOFactoryTemplate<double>;
#endif

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHDF5TransformIOFactory.hxx"
#endif

#endif
