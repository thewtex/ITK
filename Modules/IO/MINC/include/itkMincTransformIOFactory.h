/* ----------------------------- MNI Header -----------------------------------
@NAME       : itkMincTransformIOFactory.h
@DESCRIPTION: ITK <-> MINC adapter
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifndef __itkMincTransformIOFactory_h
#define __itkMincTransformIOFactory_h


#include "itkObjectFactoryBase.h"
#include "itkTransformIOBase.h"

namespace itk
{
/** \class MincTransformIOFactory
   * \brief Create instances of MincTransformIO objects using an object factory.
   *
   * \ingroup ITKIOMINC
   */
  class MincTransformIOFactory:public ObjectFactoryBase
  {
  public:
    /** Standard class typedefs. */
    typedef MincTransformIOFactory     Self;
    typedef ObjectFactoryBase          Superclass;
    typedef SmartPointer< Self >       Pointer;
    typedef SmartPointer< const Self > ConstPointer;

    /** Class methods used to interface with the registered factories. */
    virtual const char * GetITKSourceVersion(void) const;

    virtual const char * GetDescription(void) const;

    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(MincTransformIOFactory, ObjectFactoryBase);

    /** Register one factory of this type  */
    static void RegisterOneFactory(void)
    {
      MincTransformIOFactory::Pointer metaFactory = MincTransformIOFactory::New();

      ObjectFactoryBase::RegisterFactory(metaFactory);
    }

  protected:
    MincTransformIOFactory();
    ~MincTransformIOFactory();
    virtual void PrintSelf(std::ostream & os, Indent indent) const;

  private:
    MincTransformIOFactory(const Self &); //purposely not implemented
    void operator=(const Self &);        //purposely not implemented
  };
} // end namespace itk

#endif
