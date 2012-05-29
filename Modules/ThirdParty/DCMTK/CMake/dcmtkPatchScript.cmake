set(dcmtk3rdParty ${dcmtkSource}/CMake/3rdparty.cmake)

#
# Newer versions of TIFF depend on JPEG
file(READ ${dcmtk3rdParty} code)
string(REPLACE "SET(LIBTIFF_LIBS \${TIFF_LIBRARY})
"
"SET(LIBTIFF_LIBS \${TIFF_LIBRARY})
    list(APPEND LIBTIFF_LIBS \${JPEG_LIBRARY})
" code "${code}")
#
# we pass in the TIFF and ZLIB library stuff,
# running find_package for those libraries
# screws up using the ITK versions
string(REPLACE "FIND_PACKAGE(TIFF)
" "" code "${code}")
string(REPLACE "FIND_PACKAGE(ZLIB)
" "" code "${code}")

file(WRITE ${dcmtk3rdParty} "${code}")

#
# in order to use the ITK versions of ZLIB and TIFF
# it is necessary to use the ITK symbol-mangled versions
file(GLOB_RECURSE dcmtk_src "*.cc" "*.h")

foreach (filename ${dcmtk_src})
  #  message("patching ${filename}")
  file(READ ${filename} sourcecode)
  string(REPLACE "<zlib.h>"
    "\"itk_zlib.h\"" sourcecode "${sourcecode}")
  string(REPLACE "<tiffio.h>"
    "\"itk_tiff.h\"" sourcecode "${sourcecode}")
  file(WRITE ${filename} "${sourcecode}")
endforeach(filename)
