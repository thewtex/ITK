
# file(READ ${dcmtkGenConfig} code)
# string(REPLACE "int i = strerror_r(0, buf, 100);"
# "buf = strerror_r(0, buf, 100)" code "${code}")
# file(WRITE ${dcmtkGenConfig} "${code}")

set(dcmtk3rdParty ${dcmtkSource}/CMake/3rdparty.cmake)

file(READ ${dcmtk3rdParty} code)
# string(REPLACE "# turn off library if it could not be found
# "
# "# turn off library if it could not be found
#     find_package(JPEG REQUIRED)
#     list(APPEND LIBTIFF_LIBS \${JPEG_LIBRARY})
# " code "${code}")

string(REPLACE "FIND_PACKAGE(TIFF)
" "" code "${code}")
string(REPLACE "FIND_PACKAGE(ZLIB)
" "" code "${code}")

file(WRITE ${dcmtk3rdParty} "${code}")

file(GLOB_RECURSE dcmtk_src "*.cc" "*.h")

foreach (filename ${dcmtk_src})
  #  message("patching ${filename}")
  file(READ ${filename} sourcecode)
  string(REPLACE "<zlib.h>"
    "\"itk_zlib.h\"" sourcecode "${sourcecode}")
  file(WRITE ${filename} "${sourcecode}")
endforeach(filename)
