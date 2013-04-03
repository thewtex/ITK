#!/bin/bash
#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/
# Script to extract symbols that must be mangled from the targeted library
# (based on what it does by GDAL for internal libtiff) and to generate the
# mangling file.
#==========================================================================*/
usage() {
cat << EOF
  How to use this script:

1)  Use Linux, install

     - objdump

2) Run this script and pass as arguments

     a) The source three of the ITK Git repository
     b) An output temporary build directory

For example:
  generateTiffMangling /path/to/ITK/source /tmp/itk-tiff-build


An updated itk_tiff_mangle.h file is generated in the source dir

You can verify the mangling with the objdump
with -t options on internal itktiff library
EOF
}

if [ $# -ne 2 ] || [ "$1" == "--help" ]
then
  usage
  exit 1
fi

# Set internal variables
ITK_SRC_PATH=$1
ITK_BUILD_PATH=$2
OUT_FILE=$ITK_SRC_PATH/Modules/ThirdParty/TIFF/src/itktiff/itk_tiff_mangle.h

# Remove old mangling file and create a empty file to build correctly
rm $OUT_FILE 2>/dev/null
touch $OUT_FILE

rm -Rf $ITK_BUILD_PATH
mkdir $ITK_BUILD_PATH
cd $ITK_BUILD_PATH

# Configure and build the ITKIOTIFF module
# Building in Debug is important to avoid inlining
cmake $ITK_SRC_PATH \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DBUILD_SHARED_LIBS:BOOL=ON \
  -DBUILD_TESTING:BOOL=OFF \
  -DBUILD_EXAMPLES:BOOL=OFF \
  -DITK_BUILD_ALL_MODULES:BOOL=OFF \
  -DModule_ITKIOTIFF:BOOL=ON \
  -DITK_USE_SYSTEM_JPEG:BOOL=ON \
  -DITK_USE_SYSTEM_ZLIB:BOOL=ON \
  -DITKGroup_Core=OFF
make itktiff

TIFF_LIBRARY_FILEPATH=$(find $ITK_BUILD_PATH/lib/. -name "libitktiff*.so")


function manglingSymbol
{
   echo "$1" | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-50s%s\n", $1, $2, $3}'
}

function manglingSymbolList
{
   for symbol in $*
   do
      manglingSymbol $symbol >> $OUT_FILE
   done
}

# Generate the new mangling file
cat >> $OUT_FILE << EOF
/*
 * This file is generated automatically by generateTiffMangling.sh
 * DO NOT EDIT MANUALLY !
 */
#ifndef itk_tiff_mangle_h
#define itk_tiff_mangle_h

/*
 * This header file mangles all symbols of the libtiff library.
 * It is included in all files while building the tiff library.
 * Due to namespace pollution, no tiff headers should be included in .h files in ITK.
 */
EOF

symbols=`objdump -t $TIFF_LIBRARY_FILEPATH \
  | egrep  ".text|.rodata|.bss|.data" \
  | awk '{print $6}' \
  | grep -v .hidden \
  | grep -v __do_global \
  | grep -v _fini \
  | grep -v _init \
  | grep -v _end \
  | grep -v _edata \
  | grep -v __bss_start \
  | grep -v call_gmon_start \
  | grep -v frame_dummy \
  | grep -v __dso_handle \
  | grep -v __fstat \
  | grep -v ^fstat$ \
  | grep -v module. \
  | grep -v __PRETTY_FUNCTION__ \
  | grep -v reason \
  | grep -v dummy_EOI \
  | grep -v _fillmasks \
  | grep -v ycbcrcoeffs \
  | grep -v completed \
  | grep -v dotrange \
  | grep -v dtor_idx \
  | grep -v initialized \
  | grep -v oog_table \
  | grep -v whitepoint \
  | uniq \
  | sort`

manglingSymbolList $symbols

cat >> $OUT_FILE << EOF

#endif
EOF

echo "New mangling file generated"

exit
