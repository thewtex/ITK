itk_module_test()

set(ITKIOMeshTests
  itkMeshFileReadWriteTest.cxx
  itkMeshFileReadWriteVectorAttributeTest.cxx
  itkPolylineReadWriteTest.cxx
)

CreateTestDriver(ITKIOMesh "${ITKIOMesh-Test_LIBRARIES}" "${ITKIOMeshTests}" )

itk_add_test(NAME itkMeshFileReadWriteTest00
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/input.vtk}
      ${ITK_TEST_OUTPUT_DIR}/output.vtk
)

itk_add_test(NAME itkMeshFileReadWriteTest01
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/sphere.fsb}
      ${ITK_TEST_OUTPUT_DIR}/sphere.vtk
)
itk_add_test(NAME itkMeshFileReadWriteTest02
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/sphere.fsb}
      ${ITK_TEST_OUTPUT_DIR}/sphere.fsb
)
itk_add_test(NAME itkMeshFileReadWriteTest03
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/sphere_thickness.vtk}
      ${ITK_TEST_OUTPUT_DIR}/sphere_thickness.vtk
)
itk_add_test(NAME itkMeshFileReadWriteTest04
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/thickness.fcv}
      ${ITK_TEST_OUTPUT_DIR}/thickness.fcv
)
itk_add_test(NAME itkMeshFileReadWriteTest05
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/pial.fsa}
      ${ITK_TEST_OUTPUT_DIR}/pial.fsa
)

itk_add_test(NAME itkMeshFileReadWriteTest06
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/cube.byu}
      ${ITK_TEST_OUTPUT_DIR}/cube.byu
)
itk_add_test(NAME itkMeshFileReadWriteTest07
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/sphere_thickness_b.vtk}
      ${ITK_TEST_OUTPUT_DIR}/sphere_thickness_a.vtk
1
)
itk_add_test(NAME itkMeshFileReadWriteTest08
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteVectorAttributeTest
      DATA{Baseline/sphere_normals.vtk}
      ${ITK_TEST_OUTPUT_DIR}/sphere_normals.vtk
)
itk_add_test(NAME itkMeshFileReadWriteTest09
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/octa.off}
      ${ITK_TEST_OUTPUT_DIR}/octa.off
)
itk_add_test(NAME itkMeshFileReadWriteTest10
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/box.obj}
      ${ITK_TEST_OUTPUT_DIR}/box.obj
)
itk_add_test(NAME itkMeshFileReadWriteTest11
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/pial.gii}
      ${ITK_TEST_OUTPUT_DIR}/pial.gii
)
itk_add_test(NAME itkMeshFileReadWriteTest12
      COMMAND ITKIOMeshTestDriver itkMeshFileReadWriteTest
      DATA{Baseline/aparc.gii}
      ${ITK_TEST_OUTPUT_DIR}/aparc.gii
)
itk_add_test(NAME itkPolylineReadWriteTest00
      COMMAND ITKIOMeshTestDriver itkPolylineReadWriteTest
      DATA{Baseline/fibers.vtk}
      ${ITK_TEST_OUTPUT_DIR}/fibers.vtk
)
itk_add_test(NAME itkPolylineReadWriteTest01
      COMMAND ITKIOMeshTestDriver itkPolylineReadWriteTest
      DATA{Baseline/fibers.vtk}
      ${ITK_TEST_OUTPUT_DIR}/fibers_b.vtk
      1
)
