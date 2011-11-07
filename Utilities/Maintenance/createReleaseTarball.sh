#!/bin/bash

# Create ITK release tarballs.
#
# Pass in the version to create the tarball from, e.g.
#  4.0a03
#  4.0b02
#  4.0rc01
#  4.0.0
#
# This script:
# - Clones a fresh copy of ITK with the latest tag.
# - Builds it and downloads testing data.
# - Creates the tarballs (tarball for Unix, zip file for Windows).
#
# Dependencies: standard unix utils.
# xz utils on Unix
# MSYS on Windows, builds with 'make', 7-zip 7z.exe in PATH

if test $# -lt 1; then
  echo "pass in the desired version"
  exit 1
fi
version="$1"
prefix=InsightToolkit-${version}
source_dir="${PWD}/${prefix}"
build_dir="${PWD}/${prefix}_build"
os_name=$(uname -s)
if test "$os_name" = "Linux" -o "$os_name" = "Darwin"; then
  unix=true
else
  unix=false
fi


#-------------------------------------------------
# - Clones a fresh copy of ITK with the latest tag.
git clone git://itk.org/ITK.git $prefix
cd $prefix
git checkout --quiet --detach v${version}
if test $? -ne 0; then
  echo "Could not checkout version $version."
  exit 1
fi
cd ..

#-----------------------------------------
# - Builds ITK and downloads testing data.
mkdir $build_dir
cd $build_dir
if $unix; then
  generator="Unix Makefiles"
else
  generator="MSYS Makefiles"
fi
# Configure CMake so the ExternalData gets downloaded into the source directory.
external_data_dir="${source_dir}/.ExternalData"
mkdir "$external_data_dir"
cmake -G "$generator" \
  -D "ExternalData_OBJECT_STORES:STRING=${external_data_dir}" \
  -D BUILD_EXAMPLES:BOOL=ON \
  -D BUILD_TESTING:BOOL=ON \
  "$source_dir"
if test $? -ne 0; then
  echo 'Configuration failed.'
  exit 1
fi
make -j4 ITKData
if test $? -ne 0; then
  echo 'make failed.'
  exit 1
fi
# During creation of the RC's, it may not be necessary for the RC's to pass.
#ctest -j4
#if test $? -ne 0; then
  #echo 'ctest failed.'
  #exit 1
#fi

# Verify all objects available.
cd "$source_dir"
git ls-files -- '*.md5' | while read line; do
  obj=$(cat "$line")
  if test -f ~/.ExternalData/MD5/$obj; then
    echo "no $obj".
    exit 1
  fi
done

#-----------------------------------------------------------------
# - Creates the tarballs (tarball for Unix, zip file for Windows).
cd "$source_dir"
# Remove git related files.
find . -name '.git*' | xargs rm -rf
cd ..

if $unix; then
  echo "Creating ${prefix}.tar..."
  tar -cf ${prefix}.tar "$prefix"
  # gets eaten by gzip
  cp ${prefix}.tar ${prefix}.2.tar
  echo "Creating ${prefix}.tar.gz..."
  gzip -9 ${prefix}.tar
  echo "Creating ${prefix}.tar.xz..."
  mv ${prefix}.2.tar ${prefix}.tar
  xz -9   ${prefix}.tar
else
  echo "Creating ${prefix}.zip..."
  7z a ${prefix}.zip $prefix
fi

#---------
# Clean up
rm -rf "$source_dir" "$build_dir"
