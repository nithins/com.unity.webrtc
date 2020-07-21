cmake_minimum_required(VERSION 3.10)

project(NvCodec)

set(NVCODEC_DIR ${CMAKE_SOURCE_DIR}/NvCodec)
set(NV_ENC_DIR ${NVCODEC_DIR}/NvEncoder)
set(NV_DEC_DIR ${NVCODEC_DIR}/NvDecoder)

set(NVCODEC_INCLUDE_DIR
  ${NVCODEC_DIR}
  ${NVCODEC_DIR}/include
  ${NV_ENC_DIR}
  ${NV_DEC_DIR}
  CACHE PATH
  "NVCODEC INCLUDE PATH"
  )

if(MSVC)
  file(GLOB sources
    ${NV_ENC_DIR}/*.h
    ${NV_ENC_DIR}/*.cpp
    ${NV_DEC_DIR}/*.h
    ${NV_DEC_DIR}/*.cpp
  )
elseif(UNIX)
  file(GLOB sources
    ${NV_ENC_DIR}/NvEncoder.cpp
    ${NV_ENC_DIR}/NvEncoder.h
    ${NV_ENC_DIR}/NvEncoderCuda.cpp
    ${NV_ENC_DIR}/NvEncoderCuda.h
    ${NV_ENC_DIR}/NvEncoderGL.cpp
    ${NV_ENC_DIR}/NvEncoderGL.h
    ${NV_ENC_DIR}/NvEncoderOutputInVidMemCuda.cpp
    ${NV_ENC_DIR}/NvEncoderOutputInVidMemCuda.h
    ${NV_DEC_DIR}/*.h
    ${NV_DEC_DIR}/*.cpp
  )
endif()

add_library(NvCodec STATIC ${sources})

target_include_directories(NvCodec
  PRIVATE
    ${CUDA_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${NVCODEC_INCLUDE_DIR}
)

#set(NV_APPENC_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/AppEncode/Common)
#set(NV_APPDEC_COMMON_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/AppDecode/Common)

#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#    set(NVCODEC_SAMPLES_INSTALL_DIR ${CMAKE_BINARY_DIR})
#else()
#    set(NVCODEC_SAMPLES_INSTALL_DIR ${CMAKE_BINARY_DIR})
#endif()

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CUVID_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/nvcuvid.lib)
    set(NVENCODEAPI_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/nvencodeapi.lib)
  else()
    set(CUVID_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/Win32/nvcuvid.lib)
    set(NVENCODEAPI_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/Win32/nvencodeapi.lib)
  endif()
else ()
  find_library(CUVID_LIB nvcuvid)
  find_library(NVENCODEAPI_LIB nvidia-encode)
endif()

set(NVCODEC_LIBRARIES 
  NvCodec
  ${CUVID_LIB}
  ${NVENCODEAPI_LIB}
  CACHE PATH 
  "NVCODEC LIBRARIES PATH"
)

#if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    find_package(PkgConfig REQUIRED)
#    pkg_check_modules(PC_AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
#    pkg_check_modules(PC_AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
#    pkg_check_modules(PC_AVUTIL REQUIRED IMPORTED_TARGET libavutil)
#    pkg_check_modules(PC_SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)

#    set(NV_FFMPEG_HDRS ${PC_AVCODEC_INCLUDE_DIRS})
#    find_library(AVCODEC_LIBRARY NAMES avcodec
#            HINTS
#            ${PC_AVCODEC_LIBDIR}
#            ${PC_AVCODEC_LIBRARY_DIRS}
#            )
#    find_library(AVFORMAT_LIBRARY NAMES avformat
#            HINTS
#            ${PC_AVFORMAT_LIBDIR}
#            ${PC_AVFORMAT_LIBRARY_DIRS}
#            )
#    find_library(AVUTIL_LIBRARY NAMES avutil
#            HINTS
#            ${PC_AVUTIL_LIBDIR}
#            ${PC_AVUTIL_LIBRARY_DIRS}
#            )
#    find_library(SWRESAMPLE_LIBRARY NAMES swresample
#            HINTS
#            ${PC_SWRESAMPLE_LIBDIR}
#            ${PC_SWRESAMPLE_LIBRARY_DIRS}
#            )
#    set(AVCODEC_LIB ${AVCODEC_LIBRARY})
#    set(AVFORMAT_LIB ${AVFORMAT_LIBRARY})
#    set(AVUTIL_LIB ${AVUTIL_LIBRARY})
#    set(SWRESAMPLE_LIB ${SWRESAMPLE_LIBRARY})
#endif()