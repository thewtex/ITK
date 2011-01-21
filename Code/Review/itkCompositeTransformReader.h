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
#ifndef __itkCompositeTransformReader_h
#define __itkCompositeTransformReader_h

#include "itkLightProcessObject.h"
#include "itkCompositeTransform.h"
#include "itkTransformBase.h"
#include <vector>
#include <string>

namespace itk
{

/** \class CompositeTransformReader
 * \brief Reads a CompositeTransform object from file.
 *
 * This class reads a CompositeTransform from a file. To write files,
 * use CompositeTransformWriter. See CompositeTransformWriter for the file
 * format. The unique needs of writing and reading a CompositeTransform
 * motivated a pair of classes separate from TransformFileReader and
 * TransformFileWriter.
 *
 * The class is templated over same parameters as the CompositeTransform it is
 * to read.
 *
 * The user sets the filename to be read using SetFileName. This is the
 * master filename for the transform file. Files for individual component
 * transforms, as created by CompositeTransformWriter, are expected to be in
 * the same dir as the master file. The component transforms are each read
 * using TransformFileReader.
 *
 * The result is retrieved via GetCompositeTransform.
 *
 * \sa CompositeTranform, CompositeTranformWriter
 *
 */
template
<class TScalar = double, unsigned int NDimensions = 3>
class ITK_EXPORT CompositeTransformReader:public LightProcessObject
{
public:
  /** Standard class typedefs. */
  typedef CompositeTransformReader   Self;
  typedef LightProcessObject         Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef CompositeTransform<TScalar, NDimensions> CompositeTransformType;
  typedef typename CompositeTransformType::Pointer CompositeTransformPointer;
  typedef typename CompositeTransformType::TransformType ComponentTransformType;
  typedef typename ComponentTransformType::Pointer ComponentTransformPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(CompositeTransformReader, Superclass);

  /** Some convenient typedefs. */
  typedef std::vector< std::string >      FileNamesContainer;

  /** Set/Get filename. Full path */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Get the transform after reading */
  itkGetObjectMacro( CompositeTransform, CompositeTransformType );

  /**  Get the list of component filenames, with full path, read from file */
  itkGetConstReferenceMacro( ComponentFullPaths, FileNamesContainer );

  /** Read the transform */
  void Update();

protected:
  CompositeTransformReader();
  ~CompositeTransformReader();
  void PrintSelf(std::ostream & os, Indent indent) const;
  void OpenStream(std::ofstream & outputStream, bool binary);
  void CreateTransform(
    CompositeTransformPointer & ptr, const std::string & ClassName);

private:
  CompositeTransformReader(const Self &); //purposely not implemented
  void operator=(const Self &);    //purposely not implemented

  std::string                     m_FileName;
  CompositeTransformPointer       m_CompositeTransform;
  /** A list of component transform filenames that were read. */
  FileNamesContainer              m_ComponentFullPaths;

  /** trim spaces and newlines from start and end of a string */
  std::string trim(std::string const & source, char const *delims = " \t\r\n");
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCompositeTransformReader.txx"
#endif

#endif // __itkCompositeTransformReader_h
