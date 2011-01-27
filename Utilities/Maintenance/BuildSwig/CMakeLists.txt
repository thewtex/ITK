cmake_minimum_required(VERSION 2.8)
project(BuildSwig)

include(ExternalProject)
ExternalProject_add(PCRE
  URL http://downloads.sourceforge.net/project/pcre/pcre/8.12/pcre-8.12.tar.gz
  URL_MD5 fa69e4c5d8971544acd71d1f10d59193
  CONFIGURE_COMMAND ../PCRE/configure --prefix=${CMAKE_INSTALL_PREFIX}
  )
ExternalProject_add(Swig
  URL http://prdownloads.sourceforge.net/swig/swig-2.0.1.tar.gz
  URL_MD5 df4465a62ccc5f0120fee0890ea1a31f
  CONFIGURE_COMMAND ../Swig/configure --prefix=${CMAKE_INSTALL_PREFIX}
  --with-pcre-prefix=${CMAKE_INSTALL_PREFIX}
  DEPENDS PCRE
  )
