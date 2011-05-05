set(DOCUMENTATION "This module contains the central classes of the ITK
toolkit.  They include, basic data structures \(such as Points, Vectors,
Images, Regions, Meshes\) the core of the process objects \(such as base
classes for image filers\) the pipeline infrastructure classes, the support
for multi-threading, and a collection of classes that isolate ITK from
platform specific features. It is anticipated that most other ITK modules will
depend on this one.")

itk_module(ITK-Common DEPENDS ITK-VNLInstantiation ITK-KWSys TEST_DEPENDS ITK-TestKernel ITK-Mesh DESCRIPTION "${DOCUMENTATION}")
#extra test dependency on ITK-Mesh is introduced by itkCellInterfaceTest.
