/* ----------------------------- MNI Header -----------------------------------
@NAME       :
@DESCRIPTION:
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
#include "itkMincImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkMincImageIO.h"
#include "itkVersion.h"

namespace itk
{
  void MincImageIOFactory::PrintSelf(std::ostream&, Indent) const
  {
  }

  MincImageIOFactory::MincImageIOFactory()
  {
    this->RegisterOverride("itkImageIOBase",
                          "itkMincImageIO",
                          "Minc Image IO",
                          1,
                          CreateObjectFunction<MincImageIO>::New());
  }

  MincImageIOFactory::~MincImageIOFactory()
  {
  }

  const char*
  MincImageIOFactory::GetITKSourceVersion(void) const
  {
    return ITK_SOURCE_VERSION;
  }

  const char*
  MincImageIOFactory::GetDescription() const
  {
    return "Minc ImageIO Factory, allows the loading of Minc images into insight";
  }

  static bool MincImageIOFactoryHasBeenRegistered=false;

  void RegisterMincIO(void)
  {
    if( ! MincImageIOFactoryHasBeenRegistered )
    {
      MincImageIOFactoryHasBeenRegistered = true;
      MincImageIOFactory::RegisterOneFactory();
    }
  }

#if ( ITK_VERSION_MAJOR > 3 )
// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.


  void MincImageIOFactoryRegister__Private(void)
  {
    RegisterMincIO();
  }

#endif   //( ITK_VERSION_MAJOR > 3 )
} // end namespace itk
