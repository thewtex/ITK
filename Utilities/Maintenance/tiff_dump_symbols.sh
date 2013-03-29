#!/bin/sh
# Script to extract symbols that must be mangled from the targeted library
# (based on what it does by GDAL for internal libtiff)

ITK_SRC_PATH=/home/mickael/dev/src/ITK-msd
ITK_BUILD_PATH=/home/mickael/dev/build/ITK-msd
OUT_FILE=/home/mickael/dev/src/ITK-msd/itk_tiff_mangle.h

rm $ITK_SRC_PATH/Modules/ThirdParty/TIFF/src/itktiff/itk_tiff_mangle.h 2>/dev/null
touch $ITK_SRC_PATH/Modules/ThirdParty/TIFF/src/itktiff/itk_tiff_mangle.h

rm -Rf $ITK_BUILD_PATH
mkdir $ITK_BUILD_PATH
cd $ITK_BUILD_PATH
cmake $ITK_SRC_PATH -DCMAKE_BUILD_TYPE:STRING=Debug -DITK_BUILD_ALL_MODULES:BOOL=OFF -DModule_ITKIOTIFF:BOOL=ON -DITK_USE_SYSTEM_JPEG:BOOL=ON -DITK_USE_SYSTEM_ZLIB:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITKGroup_Core=OFF -DBUILD_SHARED_LIBS:BOOL=ON
make -j8

rm $OUT_FILE 2>/dev/null

echo "/* This is a generated file by tiff_dump_symbols.sh *DO NOT EDIT MANUALLY !* */" >> $OUT_FILE

echo "#ifndef itk_tiff_mangle_h\n" >> $OUT_FILE
echo "#define itk_tiff_mangle_h\n" >> $OUT_FILE

echo "/*This header file mangles all symbols exported from the tiff library.\n It is included in all files while building the tiff library. Due to\n namespace pollution, no tiff headers should be included in .h files in\n ITK.*/" >> $OUT_FILE

# grep functions
symbol_list=$(objdump -t lib/libitktiff-4.4.so  | grep .text | awk '{print $6}' | grep -v .text | grep -v __do_global | grep -v call_gmon_start | grep -v frame_dummy| grep -v __fstat |sort)
for symbol in $symbol_list
do
    echo $symbol | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-40s%s\n", $1, $2, $3}'  >> $OUT_FILE
done

rodata_symbol_list=$(objdump -t lib/libitktiff-4.4.so  | grep "\.rodata" |  awk '{print $6}' | grep -v "\.")
for symbol in $rodata_symbol_list
do
    echo $symbol | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-40s%s\n", $1, $2, $3}'  >> $OUT_FILE
done

data_symbol_list=$(objdump -t lib/libitktiff-4.4.so  | grep "\.data" | grep -v __dso_handle | awk '{print $6}' | grep -v "\.")
for symbol in $data_symbol_list
do
    echo $symbol | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-40s%s\n", $1, $2, $3}'  >> $OUT_FILE
done

bss_symbol_list=$(objdump -t lib/libitktiff-4.4.so  | grep "\.bss" | awk '{print $6}' | grep -v "\.")
for symbol in $bss_symbol_list
do
    echo $symbol | sed 's/.*/#define & itk_&/' | awk '{printf "%-8s%-40s%s\n", $1, $2, $3}'  >> $OUT_FILE
done

echo "\n#endif" >> $OUT_FILE
