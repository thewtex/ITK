/* ----------------------------- MNI Header -----------------------------------
@NAME       : itkMincTransformIOFactory.cxx
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
#include "itkVersion.h"
#include "itkMincTransformIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkMincTransformIO.h"
#include "itkTransformFactory.h"
//#include "itkDisplacementFieldTransform.h"

namespace itk
{
  void MincTransformIOFactory::PrintSelf(std::ostream &, Indent) const
  {}

  MincTransformIOFactory::MincTransformIOFactory()
  {
    this->RegisterOverride( "itkTransformIOBase",
                            "itkMincTransformIO",
                            "MINC XFM Transform IO",
                            1,
                            CreateObjectFunction< MincTransformIO >::New() );
  }

  MincTransformIOFactory::~MincTransformIOFactory()
  {}

  const char *
  MincTransformIOFactory::GetITKSourceVersion(void) const
  {
    return ITK_SOURCE_VERSION;
  }

  const char *
  MincTransformIOFactory::GetDescription() const
  {
    return "MINC XFM TransformIO Factory, allows the"
          " loading of Minc XFM transforms into insight";
  }

  // Undocumented API used to register during static initialization.
  // DO NOT CALL DIRECTLY.
  static bool MincTransformIOFactoryHasBeenRegistered;

  void MincTransformIOFactoryRegister__Private(void)
  {
    if( ! MincTransformIOFactoryHasBeenRegistered )
    {
      MincTransformIOFactoryHasBeenRegistered = true;
      MincTransformIOFactory::RegisterOneFactory();
      //TransformFactory< DisplacementFieldTransform<double,3> >::RegisterTransform ();
      // register additional transform type
    }
  }
} // end namespace itk
