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
#ifndef __itkTransformFileReaderTemplate_h
#define __itkTransformFileReaderTemplate_h

#include "itkTransformIOBaseTemplate.h"

namespace itk
{
  /** \class TransformFileReaderTemplate
   *
   * \brief TODO
   * \ingroup ITKIOTransformBase
   *
   * \wiki
   * \wikiexample{IO/TransformFileReaderTemplate,Read a transform from a file}
   * \endwiki
   */
template<class ScalarType>
class ITK_EXPORT TransformFileReaderTemplate:public LightProcessObject
{
public:

  /** SmartPointer typedef support */
  typedef TransformFileReaderTemplate         Self;
  typedef SmartPointer< Self >                Pointer;
  typedef TransformBaseTemplate<ScalarType>   TransformType;

  typedef typename TransformType::ParametersType                           ParametersType;
  typedef typename TransformIOBaseTemplate<ScalarType>::TransformPointer   TransformPointer;
  typedef typename TransformIOBaseTemplate<ScalarType>::TransformListType  TransformListType;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  typedef Object Superclass;
  itkTypeMacro(TransformFileReaderTemplate, LightProcessObject);

  /** Set the filename  */
  itkSetStringMacro(FileName);

  /** Get the filename */
  itkGetStringMacro(FileName);

  /** Read the transform */
  virtual void Update();

  /** Get the list of transform */
  TransformListType * GetTransformList() { return &m_TransformList; }

protected:
  typename TransformIOBaseTemplate<ScalarType>::Pointer m_TransformIO;
  TransformFileReaderTemplate(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented
  void PrintSelf(std::ostream & os, Indent indent) const;

  std::string m_FileName;

  TransformFileReaderTemplate();
  virtual ~TransformFileReaderTemplate();
  void CreateTransform(TransformPointer & ptr, const std::string & ClassName);

  TransformListType m_TransformList;
};
} // namespace itk

#ifdef ITK_IO_FACTORY_REGISTER_MANAGER
#include "itkTransformIOFactoryTemplateRegisterManager.h"
#endif

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransformFileReaderTemplate.hxx"
#endif

#endif // __itkTransformFileReaderTemplate_h
