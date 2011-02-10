set(ITKIO_SUPPORTS_ANALYZE_IMAGEIO TRUE CACHE INTERNAL  "Support the Analyze file format" )
set(ITKIO_SUPPORTS_BIORAD_IMAGEIO TRUE CACHE INTERNAL  "Support the Biorad file format" )
set(ITKIO_SUPPORTS_BMP_IMAGEIO TRUE CACHE INTERNAL  "Support the BMP file format" )
set(ITKIO_SUPPORTS_GDCM_IMAGEIO TRUE CACHE INTERNAL  "Support the GDCM file format" )
set(ITKIO_SUPPORTS_GIPL_IMAGEIO TRUE CACHE INTERNAL  "Support the GIPL file format" )
set(ITKIO_SUPPORTS_JPEG_IMAGEIO TRUE CACHE INTERNAL  "Support the JPEG file format" )
set(ITKIO_SUPPORTS_LSM_IMAGEIO TRUE CACHE INTERNAL  "Support the LSM file format" )
set(ITKIO_SUPPORTS_META_IMAGEIO TRUE CACHE INTERNAL  "Support the META file format" )
set(ITKIO_SUPPORTS_NIFTI_IMAGEIO TRUE CACHE INTERNAL  "Support the NIFTI file format" )
set(ITKIO_SUPPORTS_NRRD_IMAGEIO TRUE CACHE INTERNAL  "Support the NRRD file format" )
set(ITKIO_SUPPORTS_PNG_IMAGEIO TRUE CACHE INTERNAL  "Support the PNG file format" )
set(ITKIO_SUPPORTS_STIMULATE_IMAGEIO TRUE CACHE INTERNAL  "Support the STIMULATE file format" )
set(ITKIO_SUPPORTS_TIFF_IMAGEIO TRUE CACHE INTERNAL  "Support the TIFF file format" )
set(ITKIO_SUPPORTS_VTK_IMAGEIO TRUE CACHE INTERNAL  "Support the VTK file format" )

mark_as_advanced(ITKIO_SUPPORTS_ANALYZE_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_BIORAD_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_BMP_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_GDCM_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_GIPL_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_JPEG_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_LSM_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_META_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_NIFTI_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_NRRD_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_PNG_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_STIMULATE_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_TIFF_IMAGEIO)
mark_as_advanced(ITKIO_SUPPORTS_VTK_IMAGEIO)

# BioFormats options
mark_as_advanced(ITKIO_SUPPORTS_BIOFORMATS_IMAGEIO)
option(ITKIO_SUPPORTS_BIOFORMATS_IMAGEIO "Use BioFormats library" OFF)
if( ITKIO_SUPPORTS_BIOFORMATS_IMAGEIO )
message(
"IMPORTANT NOTE: You have enabled the use of BioFormats bindings. Although
these specific files are distributed according to a BSD-style license and the
Apache 2 license, it requires to be linked to the OME Bio-Formats Java library
at run-time to do anything useful. The OME Bio-Formats Java library is licensed
under the GPL v2 or later.  Therefore, if you wish to distribute this software
in binary form with Bio-Formats itself, your combined binary work must be
distributed under the terms of the GPL v2 license.
For more details, please see http://www.loci.wisc.edu/software/bio-formats"
)

  find_package( BioFormats )
  if( NOT BIOFORMATS_FOUND )
    message("You requested to use BioFormats but some components are missing")
  endif()
endif()
