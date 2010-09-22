/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkXMLFileOutputWindow.h
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkXMLFileOutputWindow_h
#define __itkXMLFileOutputWindow_h

#include "itkFileOutputWindow.h"

namespace itk
{
/** \class XMLFileOutputWindow
 * \brief Messages sent from the system are sent to a file with each message
 *        enclosed by XML tags.
 *
 * Writes debug/warning/error output to an XML file.  Uses predefined XML
 * tags for each text display method.  The text is processed to replace
 * XML markup characters.
 *
 *   DisplayText - <Text>
 *
 *   DisplayErrorText - <Error>
 *
 *   DisplayWarningText - <Warning>
 *
 *   DisplayGenericOutputText - <GenericOutput>
 *
 *   DisplayDebugText - <Debug>
 *
 * The method DisplayTag outputs the text unprocessed.  To use this
 * class, instantiate it and then call SetInstance(this).
 *
 * \ingroup OSSystemObjects
 */
class ITKCommon_EXPORT XMLFileOutputWindow:public FileOutputWindow
{
public:
  /** Standard class typedefs. */
  typedef XMLFileOutputWindow        Self;
  typedef FileOutputWindow           Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(XMLFileOutputWindow, FileOutputWindow);

  /** Send a string to the XML file. */
  virtual void DisplayText(const char *);

  /** Send an error string to the XML file. */
  virtual void DisplayErrorText(const char *);

  /** Send a warning string to the XML file. */
  virtual void DisplayWarningText(const char *);

  /** Send a generic output string to the XML file. */
  virtual void DisplayGenericOutputText(const char *);

  /** Send a debug string to the XML file. */
  virtual void DisplayDebugText(const char *);

  /**  Put the text into the log file without processing it. */
  virtual void DisplayTag(const char *);

protected:
  XMLFileOutputWindow();
  virtual ~XMLFileOutputWindow();
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

  void Initialize();

  virtual void DisplayXML(const char *, const char *);

private:
  XMLFileOutputWindow(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented
};
} // end namespace itk

#endif
