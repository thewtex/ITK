set(DOCUMENTATION "This module contains the central classes of the ITK
toolkit.  They include, basic data structures \(such as Points, Vectors,
Images, Regions\) the core of the process objects \(such as base
classes for image filters\) the pipeline infrastructure classes, the support
for multi-threading, and a collection of classes that isolate ITK from
platform specific features. It is anticipated that most other ITK modules will
depend on this one.")

itk_module(ITK_Common
  DEPENDS
    ITK_VNLInstantiation
    ITK_KWSys
  TEST_DEPENDS
    ITK_TestKernel
    ITK_Mesh
    ITK_ImageIntensity
    ITK_IO_Base
  DESCRIPTION
    "${DOCUMENTATION}"
)

# Extra test dependency on ITK_Mesh is introduced by itkCellInterfaceTest.
# Extra test dependency on ITK_ImageIntensity is introduced by itkImageDuplicatorTest.
# Extra test dependency on ITK_IO_Base is introduced by itkImageRandomIteratorTest22.
