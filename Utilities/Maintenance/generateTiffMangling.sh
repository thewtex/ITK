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
if [ -z "$1" ] || [ "$1" == "--help" ]
then
echo "      "
echo " How to use this script      "
echo "      "
echo "  0) Use Linux with objdump install"
echo "     WARNING: this script will remove your build tree"
echo "  1) Run the script:"
echo "       generateTiffMangling /path/to/ITK/source /path/to/ITK/build"
echo "  2) It should generate a new itk_tiff_mangle.h file."
echo "  3) You can verify the mangling with the objdump "
echo "      command with -t options on internal itktiff library"
echo "    "

else

#==========================================================================

function manglingSymbol
{
   echo "$1" | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-50s%s\n", $1, $2, $3}'
}

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
cmake $ITK_SRC_PATH -DCMAKE_BUILD_TYPE:STRING=Debug -DITK_BUILD_ALL_MODULES:BOOL=OFF -DModule_ITKIOTIFF:BOOL=ON -DITK_USE_SYSTEM_JPEG:BOOL=ON -DITK_USE_SYSTEM_ZLIB:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITKGroup_Core=OFF -DBUILD_SHARED_LIBS:BOOL=ON -G "Eclipse CDT4 - Unix Makefiles"
make -j8

TIFF_LIBRARY_FILEPATH=$(find $ITK_BUILD_PATH/lib/. -name "libitktiff*.so")

echo "/* This is a generated file by generateTiffMangling.sh *DO NOT EDIT MANUALLY !* */" >> $OUT_FILE

echo "#ifndef itk_tiff_mangle_h" >> $OUT_FILE
echo "#define itk_tiff_mangle_h" >> $OUT_FILE

echo "/* This header file mangles all symbols exported from the tiff library." >> $OUT_FILE
echo "   It is included in all files while building the tiff library. Due to namespace pollution," >> $OUT_FILE
echo "   no tiff headers should be included in .h files in ITK.*/" >> $OUT_FILE

#-----------------
# grep functions
symbol_list=$(objdump -t $TIFF_LIBRARY_FILEPATH  | grep .text | awk '{print $6}' | grep -v .text | grep -v __do_global | grep -v call_gmon_start | grep -v frame_dummy| grep -v __fstat | grep -v ^fstat$ |sort)

# mangling function
for symbol in $symbol_list
do
    manglingSymbol $symbol >> $OUT_FILE
done

#-----------------
rodata_symbol_list=$(objdump -t $TIFF_LIBRARY_FILEPATH  | grep "\.rodata" |  awk '{print $6}' | grep -v "\.")

for symbol in $rodata_symbol_list
do
    manglingSymbol $symbol >> $OUT_FILE
done

#-----------------
data_symbol_list=$(objdump -t $TIFF_LIBRARY_FILEPATH  | grep "\.data" | grep -v __dso_handle | awk '{print $6}' | grep -v "\.")

for symbol in $data_symbol_list
do
    manglingSymbol $symbol >> $OUT_FILE
done

#-----------------
bss_symbol_list=$(objdump -t $TIFF_LIBRARY_FILEPATH  | grep "\.bss" | awk '{print $6}' | grep -v "\.")

for symbol in $bss_symbol_list
do
    manglingSymbol $symbol >> $OUT_FILE
done


echo "#endif" >> $OUT_FILE

echo "New mangling file generated"

fi
