/* ----------------------------- MNI Header -----------------------------------
@NAME       : itkMincTransformIO.h
@DESCRIPTION: ITK <-> MINC xfm adapter
@COPYRIGHT  :
              Copyright 2012 Vladimir Fonov, McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifndef __itkMincTransformIO_h
#define __itkMincTransformIO_h
#include "itkTransformIOBase.h"
#include <string>
#include <vector>
//minc header
#include <volume_io.h>

namespace itk
{
  /** \class MincTransformIO
  *  \brief Read&Write transforms in Minc  XFM Format
  *
  * \ingroup ITKIOMINC
  */
  class MincTransformIO:public TransformIOBase
  {
  public:
    typedef MincTransformIO               Self;
    typedef TransformIOBase               Superclass;
    typedef SmartPointer< Self >          Pointer;
    typedef TransformBase                 TransformType;
    typedef Superclass::TransformPointer  TransformPointer;
    typedef Superclass::TransformListType TransformListType;
    typedef TransformType::ParametersType ParametersType;

    /** Run-time type information (and related methods). */
    itkTypeMacro(MincTransformIO, TransformIOBase);
    itkNewMacro(Self);

    /** Determine the file type. Returns true if this ImageIO can read the
    * file specified. */
    virtual bool CanReadFile(const char *);

    /** Determine the file type. Returns true if this ImageIO can read the
    * file specified. */
    virtual bool CanWriteFile(const char *);

    /** Reads the data from disk into the memory buffer provided. */
    virtual void Read();

    virtual void Write();

  protected:

    MincTransformIO();
    virtual ~MincTransformIO();
    VIO_General_transform m_XFM;
    bool                  m_XFM_initialized;

  private:
    void _cleanup(void);
    void WriteOneTransform(const int transformIndex,const TransformType *transform,std::vector<VIO_General_transform> &m_XFM);
    void ReadOneTransform(VIO_General_transform *xfm);
  };
}
#endif // __itkMincTransformIO_h
