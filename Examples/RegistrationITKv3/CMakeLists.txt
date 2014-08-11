project(ImageRegistration)

foreach(example
    ITKv3ImageRegistration1
    ITKv3ImageRegistration2
    ITKv3ImageRegistration3
    ITKv3ImageRegistration4
    ITKv3ImageRegistration5
    ITKv3ImageRegistration6
    ITKv3ImageRegistration7
    ITKv3ImageRegistration8
    ITKv3ImageRegistration9
    ITKv3ImageRegistration10
    ITKv3ImageRegistration11
    ITKv3ImageRegistration12
    ITKv3ImageRegistration13
    ITKv3ImageRegistration14
    ITKv3ImageRegistration15
    ITKv3ImageRegistration16
    ITKv3ImageRegistration17
    ITKv3ImageRegistration18
    ITKv3ImageRegistration19
    ITKv3ImageRegistration20
    ITKv3DeformableRegistration1
    ITKv3DeformableRegistration2
    ITKv3DeformableRegistration3
    ITKv3DeformableRegistration4
    ITKv3DeformableRegistration5
    ITKv3DeformableRegistration6
    ITKv3DeformableRegistration7
    ITKv3DeformableRegistration8
    ITKv3DeformableRegistration11
    ITKv3DeformableRegistration12
    ITKv3DeformableRegistration13
    ITKv3DeformableRegistration14
    ITKv3DeformableRegistration15
    ITKv3DeformableRegistration16
    ITKv3DeformableRegistration17
    ITKv3MultiResImageRegistration1
    ITKv3MultiResImageRegistration2
    ITKv3MultiResImageRegistration3
    ITKv3ChangeInformationImageFilter
    ITKv3DisplacementFieldInitialization
    ITKv3LandmarkWarping2
    ITKv3BSplineWarping1
    ITKv3BSplineWarping2
    ITKv3ThinPlateSplineWarp
    ITKv3ModelToImageRegistration1
    ITKv3ModelToImageRegistration2
    ITKv3ImageRegistrationHistogramPlotter
    ITKv3MeanSquaresImageMetric1
    ITKv3DeformationFieldJacobian
    ITKv3IterativeClosestPoint1
    ITKv3IterativeClosestPoint2
    ITKv3IterativeClosestPoint3
    )
  string(SUBSTRING "${example}" 5 -1 source)
  add_example(${example} ${source}.cxx)
endforeach()

if( ITK_USE_FFTWD )
  foreach(example
      ITKv3DeformableRegistration9
      ITKv3DeformableRegistration10
      )
    string(SUBSTRING "${example}" 5 -1 source)
    add_example(${example} ${source}.cxx)
  endforeach()
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
