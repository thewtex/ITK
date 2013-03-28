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
#ifndef __itkTransformFileWriterTemplate_h
#define __itkTransformFileWriterTemplate_h

#include "itkTransformIOBaseTemplate.h"
#include <iostream>
#include <fstream>

namespace itk
{
  /** \class TransformFileWriterTemplate
   *
   * \brief TODO
   * \ingroup ITKIOTransformBase
   *
   * \wiki
   * \wikiexample{IO/TransformFileWriterTemplate,Write a transform to a file}
   * \endwiki
   */
template<class ScalarType>
class ITK_EXPORT TransformFileWriterTemplate:public LightProcessObject
{
public:

  /** SmartPointer typedef support */
  typedef TransformFileWriterTemplate  Self;
  typedef LightProcessObject           Superclass;
  typedef SmartPointer< Self >         Pointer;

  typedef TransformBaseTemplate<ScalarType>                                    TransformType;
  typedef typename TransformIOBaseTemplate<ScalarType>::ConstTransformPointer  ConstTransformPointer;
  typedef typename TransformIOBaseTemplate<ScalarType>::ConstTransformListType ConstTransformListType;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TransformFileWriterTemplate, LightProcessObject);

  /** Set the filename  */
  itkSetStringMacro(FileName);

  /** Get the filename */
  itkGetStringMacro(FileName);

  /** Set/Get the write mode (append/overwrite) for the Filter */
  void SetAppendOff();

  void SetAppendOn();

  void SetAppendMode(bool mode);

  bool GetAppendMode();

  /** Set/Get the input transform to write */
  void SetInput(const TransformType *transform);

  const TransformType * GetInput();

  /** Add a transform to be written */
  void AddTransform(const TransformType *transform);

  /** Set/Get the precision of the writing */
  itkSetMacro(Precision, unsigned int);
  itkGetConstMacro(Precision, unsigned int);

  /** Write out the transform */
  void Update();

protected:
  TransformFileWriterTemplate(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented
  void PrintSelf(std::ostream & os, Indent indent) const;

  TransformFileWriterTemplate();
  virtual ~TransformFileWriterTemplate();

private:
  void OpenStream(std::ofstream & out, bool binary);

  std::string            m_FileName;
  ConstTransformListType m_TransformList;
  unsigned int           m_Precision;
  bool                   m_AppendMode;
  };
} // namespace itk

#ifdef ITK_IO_FACTORY_REGISTER_MANAGER
#include "itkTransformIOFactoryTemplateRegisterManager.h"
#endif

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransformFileWriterTemplate.hxx"
#endif

#endif // __itkTransformFileWriterTemplate_h
