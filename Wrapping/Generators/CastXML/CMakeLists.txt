###############################################################################
option(
  ITK_USE_SYSTEM_CASTXML
  "Use system castxml. If OFF, castxml is built as external project."
  OFF
)
mark_as_advanced(ITK_USE_SYSTEM_CASTXML)
if(WIN32)
  set(exe .exe)
endif()
set(castxml_ep ${CMAKE_CURRENT_BINARY_DIR}/castxml/bin/castxml${exe})

if(ITK_USE_SYSTEM_CASTXML)
  # the path set for the EP build prevents find_program to do its job
  if("${CASTXML_EXECUTABLE}" STREQUAL "${castxml_ep}")
    unset(CASTXML_EXECUTABLE CACHE)
  endif()
  find_program(CASTXML_EXECUTABLE castxml)
  if(NOT CASTXML_EXECUTABLE)
    message(
      FATAL_ERROR
      "Could NOT find castxml. Turn ITK_USE_SYSTEM_CASTXML to OFF to build it with ITK."
    )
  endif()
else()
  include(ExternalProject)

  set(_download_castxml_binaries 0)
  set(_castxml_url)
  set(_castxml_hash)
  set(_castxml_version 2025.07.23)

  # If 64 bit Linux build host, use the CastXML binary
  if(
    CMAKE_HOST_SYSTEM_NAME
      MATCHES
      "Linux"
    AND
      CMAKE_HOST_SYSTEM_PROCESSOR
        STREQUAL
        "x86_64"
  )
    set(
      _castxml_hash
      0cb019e8a59bf1d8cdeb23a6b8320d764564a00d838b855cc77eec6aec9186ae
    )
    set(
      _castxml_url
      "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${_castxml_version}/castxml-ubuntu-22.04-x86_64.tar.gz"
    )
    set(_download_castxml_binaries 1)
  elseif(
    CMAKE_HOST_SYSTEM_NAME
      MATCHES
      "Linux"
    AND
      CMAKE_HOST_SYSTEM_PROCESSOR
        STREQUAL
        "aarch64"
  )
    # 254ad35ab06f1fde4a58de5b2f01f59410c64071112e96fd007ceb1b57473e67
    set(
      _castxml_hash
      2919668dc0d9d15daf978863bf3c850c31f36be2b34755632a429cde61008d82
    )
    # "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${_castxml_version}/castxml-almalinux-8-arm-aarch64.tar.gz"
    set(
      _castxml_url
      "https://itk.mypinata.cloud/ipfs/bafybeibpmtb3nmw7bnuyweqbt72y6y5fkqpofxcicyur2fpqlcr67s65wa/castxml-almalinux-8-arm-aarch64-2025-07-28.tar.gz"
    )
    set(_download_castxml_binaries 1)

    # If 64 bit Windows build host, use the CastXML binary
  elseif(
    CMAKE_HOST_SYSTEM_NAME
      MATCHES
      "Windows"
    AND
      CMAKE_HOST_SYSTEM_PROCESSOR
        STREQUAL
        "AMD64"
  )
    set(
      _castxml_hash
      af28ba87e0342033cc3d275a0e2cf484216d1ed252706728a29e05518b189c6c
    )
    set(
      _castxml_url
      "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${_castxml_version}/castxml-windows.zip"
    )
    set(_download_castxml_binaries 1)

    # If 64 bit Mac OS X build host ( >= 10.9, Mavericks), use the CastXML binary
  elseif(
    CMAKE_HOST_SYSTEM_NAME
      MATCHES
      "Darwin"
    AND
      CMAKE_HOST_SYSTEM_PROCESSOR
        STREQUAL
        "x86_64"
    AND
      (
        NOT
          CMAKE_HOST_SYSTEM_VERSION
            VERSION_LESS
            "13.0.0"
      )
  )
    set(
      _castxml_hash
      4fe0e3bf44d4d7b771256cbc6ccb945971f0ba0bb6095498bad54ef84bde6175
    )
    set(
      _castxml_url
      "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${_castxml_version}/castxml-macos-13-x86_64.tar.gz"
    )
    set(_download_castxml_binaries 1)
  elseif(
    CMAKE_HOST_SYSTEM_NAME
      MATCHES
      "Darwin"
    AND
      CMAKE_HOST_SYSTEM_PROCESSOR
        STREQUAL
        "arm64"
  )
    set(
      _castxml_hash
      2dcb380d3fb8c1aa6c247f00fbce99ebfe154d3de9f718ca388953dc169467ac
    )
    set(
      _castxml_url
      "https://github.com/CastXML/CastXMLSuperbuild/releases/download/v${_castxml_version}/castxml-macos-15-arm64.tar.gz"
    )
    set(_download_castxml_binaries 1)
  endif()

  # Download binaries
  if(_download_castxml_binaries AND NOT TARGET castxml)
    if(ITK_BINARY_DIR)
      itk_download_attempt_check(CastXML)
    endif()
    # Set the CMP0135 policy to NEW: set the timestamps of the
    # extracted files to their extraction timestamps. Used for
    # CMake-based projects such as CastXML.
    cmake_policy(PUSH)
    if(POLICY CMP0135)
      cmake_policy(SET CMP0135 NEW)
    endif()
    ExternalProject_Add(
      castxml
      URL
        ${_castxml_url}
      URL_HASH SHA256=${_castxml_hash}
      CONFIGURE_COMMAND
        ""
      BUILD_COMMAND
        ""
      INSTALL_COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/castxml-prefix/src/castxml"
        "${CMAKE_CURRENT_BINARY_DIR}/castxml"
    )
    cmake_policy(POP)
    set(CASTXML_EXECUTABLE ${castxml_ep})
    set(
      CASTXML_EXECUTABLE
      ${castxml_ep}
      CACHE FILEPATH
      "castxml executable."
      FORCE
    )
    # Build from source
  elseif(NOT TARGET castxml)
    set(compiler_information)
    if(NOT CMAKE_CROSSCOMPILING)
      set(
        CMAKE_CXX_COMPILER_LAUNCHER_FLAG
        -DCMAKE_CXX_COMPILER_LAUNCHER:FILEPATH=${CMAKE_CXX_COMPILER_LAUNCHER}
      )
      set(
        CMAKE_C_COMPILER_LAUNCHER_FLAG
        -DCMAKE_C_COMPILER_LAUNCHER:FILEPATH=${CMAKE_C_COMPILER_LAUNCHER}
      )
      set(
        compiler_information
        -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
        ${CMAKE_CXX_COMPILER_LAUNCHER_FLAG}
        -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
        ${CMAKE_C_COMPILER_LAUNCHER_FLAG}
      )
    endif()
    # might be set to on by default when llvm/clang 3.6 are released
    # option(ITK_USE_SYSTEM_LLVM "Use system llvm and clang. If OFF, llvm and clang are built as external projects." ON)
    set(ITK_USE_SYSTEM_LLVM OFF)
    if(ITK_USE_SYSTEM_LLVM)
      find_package(LLVM REQUIRED)
      set(castxml_deps)
    else()
      # check the version of clang compiler
      if(
        APPLE
        AND
          (
            (
              (
                CMAKE_CXX_COMPILER_ID
                  STREQUAL
                  "Clang"
              )
              AND
                CMAKE_CXX_COMPILER_VERSION
                  VERSION_LESS
                  "3.2"
            )
            OR
              (
                (
                  CMAKE_CXX_COMPILER_ID
                    STREQUAL
                    "AppleClang"
                )
                AND
                  CMAKE_CXX_COMPILER_VERSION
                    VERSION_LESS
                    "5.0"
              )
          )
      )
        message(
          FATAL_ERROR
          "Mac OS X 10.9 (Mavericks) or newer is required to wrap ITK."
        )
      endif()
      # If we are building ITK
      if(ITK_BINARY_DIR)
        itk_download_attempt_check(LLVM)
        itk_download_attempt_check(Clang)
      endif()
      set(llvm_version 18.1.3)
      set(llvm_folder 18.1.3)
      set(LLVM_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm/lib/cmake/llvm/)

      if(${CMAKE_VERSION} VERSION_LESS 3.24)
        set(download_extract_timestamp_flag)
      else()
        set(
          download_extract_timestamp_flag
          DOWNLOAD_EXTRACT_TIMESTAMP
          TRUE
        )
      endif()

      set(
        llvm_cmake_sha256
        acfecb615d41c5b1a0a31e15324994ca06f7a3f37d8958d719b20de0d217b71b
      )
      ExternalProject_Add(
        llvm-cmake
        URL
          "https://github.com/llvm/llvm-project/releases/download/llvmorg-${llvm_folder}/cmake-${llvm_version}.src.tar.xz"
        URL_HASH SHA256=${llvm_cmake_sha256}
        CONFIGURE_COMMAND
          ""
        BUILD_COMMAND
          ""
        INSTALL_COMMAND
          "" ${download_extract_timestamp_flag}
        LOG_BUILD 0
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm-prefix/src/cmake
      )

      set(
        llvm_sha256
        fa6db8951f5ef576ac6bad43d5e1ed83962754538c998fbfa0397cd4521abc00
      )
      ExternalProject_Add(
        llvm
        URL
          "https://github.com/llvm/llvm-project/releases/download/llvmorg-${llvm_folder}/llvm-${llvm_version}.src.tar.xz"
        URL_HASH SHA256=${llvm_sha256}
        DEPENDS
          llvm-cmake
        CMAKE_ARGS
          -Wno-dev
        CMAKE_GENERATOR "${CMAKE_GENERATOR}"
        CMAKE_CACHE_ARGS
          -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
          -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
          -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
          -DLLVM_STATIC_LINK_CXX_STDLIB:BOOL=ON
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
          -DLLVM_INSTALL_PREFIX:PATH=<INSTALL_DIR>
          -DLLVM_ENABLE_TERMINFO:BOOL=OFF -DLLVM_INCLUDE_TESTS:BOOL=OFF
          -DLLVM_INCLUDE_BENCHMARKS:BOOL=OFF -DLLVM_INCLUDE_DOCS:BOOL=OFF
          -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN:BOOL=ON
          ${download_extract_timestamp_flag}
        LOG_BUILD 0
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm
      )

      set(
        clang_sha256
        e43e1729713ac0241aa026fa2f98bb54e74a196a6fed60ab4819134a428eb6d8
      )
      ExternalProject_Add(
        clang
        URL
          "https://github.com/llvm/llvm-project/releases/download/llvmorg-${llvm_folder}/clang-${llvm_version}.src.tar.xz"
        URL_HASH SHA256=${clang_sha256}
        DEPENDS
          llvm
        SOURCE_DIR
          ${CMAKE_CURRENT_BINARY_DIR}/llvm-prefix/src/cfe-${llvm_version}
        CMAKE_ARGS
          -Wno-dev
        CMAKE_GENERATOR "${CMAKE_GENERATOR}"
        CMAKE_CACHE_ARGS
          -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
          -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
          -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
          -DCLANG_INCLUDE_DOCS:BOOL=OFF -DCLANG_INCLUDE_TESTS:BOOL=OFF
          -DLLVM_INCLUDE_TESTS:BOOL=OFF
          -DLLVM_CONFIG:PATH=${CMAKE_CURRENT_BINARY_DIR}/llvm/bin/llvm-config
          -DLLVM_DIR:PATH=${LLVM_DIR}
          -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN:BOOL=ON
          ${download_extract_timestamp_flag}
        LOG_BUILD 0
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/llvm
      )

      set(
        castxml_deps
        llvm
        clang
      )
    endif()

    # If we are building ITK
    if(ITK_BINARY_DIR)
      itk_download_attempt_check(CastXML)
    endif()
    ExternalProject_Add(
      castxml
      GIT_REPOSITORY https://github.com/CastXML/CastXML.git
      GIT_TAG v${_castxml_version}
      UPDATE_COMMAND
        ""
      DEPENDS
        ${castxml_deps}
      CMAKE_ARGS
        -Wno-dev
      CMAKE_GENERATOR "${CMAKE_GENERATOR}"
      CMAKE_CACHE_ARGS
        ${compiler_information}
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DLLVM_DIR:PATH=${LLVM_DIR}
      INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/castxml
    )
    set(CASTXML_EXECUTABLE ${castxml_ep})
    set(
      CASTXML_EXECUTABLE
      ${castxml_ep}
      CACHE FILEPATH
      "castxml executable."
      FORCE
    )
  endif()
endif()
mark_as_advanced(CASTXML_EXECUTABLE)

###############################################################################
# store the current dir, so it can be reused later
set(
  ITK_WRAP_CASTXML_SOURCE_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}"
  CACHE INTERNAL
  "castxml source dir"
)
