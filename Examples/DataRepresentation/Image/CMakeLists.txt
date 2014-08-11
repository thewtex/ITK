project(ImageExamples)

foreach(example
    Image1
    Image2
    Image3
    Image4
    Image5
    VectorImage
    ImageAdaptor1
    ImageAdaptor2
    ImageAdaptor3
    ImageAdaptor4
    RGBImage
    )
  add_example(${example})
endforeach()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
