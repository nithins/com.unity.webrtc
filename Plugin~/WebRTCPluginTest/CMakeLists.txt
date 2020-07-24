cmake_minimum_required(VERSION 3.10)

project(webrtc-test)

# Common source files
file(GLOB sources
    *.cpp
    *.h
    ../WebRTCPlugin/*.h
    ../WebRTCPlugin/*.cpp
    ../WebRTCPlugin/GraphicsDevice/*.h
    ../WebRTCPlugin/GraphicsDevice/*.cpp
    ../WebRTCPlugin/Codec/*.h
    ../WebRTCPlugin/Codec/*.cpp
)

# Apple/Unix specific source files
if(MSVC)
    file(GLOB append_source
        NvCodec/*.h
        NvCodec/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.cpp
        ../WebRTCPlugin/GraphicsDevice/D3D11/*.h
        ../WebRTCPlugin/GraphicsDevice/D3D11/*.cpp
        ../WebRTCPlugin/GraphicsDevice/D3D12/*.h
        ../WebRTCPlugin/GraphicsDevice/D3D12/*.cpp
        ../WebRTCPlugin/GraphicsDevice/OpenGL/*.h
        ../WebRTCPlugin/GraphicsDevice/OpenGL/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.cpp
        ../WebRTCPlugin/Codec/SoftwareCodec/*.h
        ../WebRTCPlugin/Codec/SoftwareCodec/*.cpp
        ../WebRTCPlugin/Codec/NvCodec/*.cpp
        ../WebRTCPlugin/Codec/NvCodec/*.h
    )
    list (APPEND sources ${append_source})
elseif(APPLE)
    file(GLOB append_source
        ../WebRTCPlugin/GraphicsDevice/Metal/*.h
        ../WebRTCPlugin/GraphicsDevice/Metal/*.mm
        ../WebRTCPlugin/Codec/SoftwareCodec/*.h
        ../WebRTCPlugin/Codec/SoftwareCodec/*.cpp
        #Codec/VideoToolbox/VTEncoderMetal.mm
        #Codec/VideoToolbox/VTEncoderMetal.h
    )

    list (APPEND sources ${append_source})
elseif(UNIX)
    file(GLOB append_source
        NvCodec/*.h
        NvCodec/*.cpp
        ../WebRTCPlugin/GraphicsDevice/OpenGL/*.h
        ../WebRTCPlugin/GraphicsDevice/OpenGL/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/*.cpp
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.h
        ../WebRTCPlugin/GraphicsDevice/Vulkan/Cuda/*.cpp
        ../WebRTCPlugin/Codec/SoftwareCodec/*.h
        ../WebRTCPlugin/Codec/SoftwareCodec/*.cpp
        ../WebRTCPlugin/Codec/NvCodec/Util.cpp
        ../WebRTCPlugin/Codec/NvCodec/Util.h
        ../WebRTCPlugin/Codec/NvCodec/NvEncoderProxy.cpp
        ../WebRTCPlugin/Codec/NvCodec/NvEncoderProxy.h
    )
    list (APPEND sources ${append_source})
endif()

add_executable(WebRTCPluginTest ${sources} ../unity/include/IUnityGraphicsMetal.h)

if(APPLE OR UNIX)
  message(STATUS "macos or linux")
  target_compile_options(WebRTCPluginTest PRIVATE -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter -Wno-missing-field-initializers -Wno-long-long)
  target_compile_features(WebRTCPluginTest PRIVATE cxx_std_14)
endif()

# specify gtest root directory
if(MSVC)
  set(GTEST_ROOT "${CMAKE_SOURCE_DIR}/googletest")
  set(GMOCK_ROOT "${CMAKE_SOURCE_DIR}/googletest")
endif()

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
include(GoogleTest)

if(MSVC)

  # Use precompiled header
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yu /Yupch.h")

  target_compile_definitions(WebRTCPluginTest
      PRIVATE
        WEBRTC_WIN
        NOMINMAX
        GLEW_STATIC
  )
  target_link_libraries(WebRTCPluginTest
      PRIVATE
      ${WEBRTC_LIBRARIES}
      ${OPENGL_LIBRARIES}
      ${GLUT_LIBRARIES}
      ${Vulkan_LIBRARY}
      ${CUDA_CUDA_LIBRARY}
      ${GTEST_MAIN_LIBRARIES}
      ${GTEST_LIBRARIES}
      ${GMOCK_LIBRARIES}
      ${GLEW_STATIC_LIBRARIES}
      ${NVCODEC_LIBRARIES}
      d3d11
      d3d12
      dxgi
      winmm
      Secur32
      Msdmo
      Dmoguids
      wmcodecdspuuid
  )
  target_include_directories(WebRTCPluginTest
    PRIVATE
      ${CUDA_INCLUDE_DIRS}
      ${Vulkan_INCLUDE_DIR}
      ${GLEW_INCLUDE_DIR}
      ${GLUT_INCLUDE_DIR}
  )    
  set_target_properties( WebRTCPluginTest 
    PROPERTIES LINK_FLAGS 
      "/DEBUG"
  )

  # Creare precompiled header
  set_source_files_properties(pch.cpp 
    PROPERTIES COMPILE_FLAGS 
      "/Yc /Ycpch.h"
  )
elseif(APPLE)
  set(CMAKE_C_FLAGS "-x objective-c")
  set(CMAKE_CXX_FLAGS "-x objective-c++")

  target_compile_definitions(WebRTCPluginTest
    PRIVATE
      WEBRTC_MAC
      WEBRTC_POSIX
  )
  target_link_libraries(WebRTCPluginTest
    PRIVATE
      ${WEBRTC_LIBRARIES}
      ${OPENGL_LIBRARIES}
      ${FRAMEWORK_LIBS}
      ${GTEST_MAIN_LIBRARIES}
      ${GTEST_LIBRARIES}
  )

  target_include_directories(WebRTCPluginTest
    PRIVATE
      ..
  )
elseif(UNIX)
  target_compile_definitions(WebRTCPluginTest
    PRIVATE
      WEBRTC_LINUX
      WEBRTC_POSIX
  )

  target_link_libraries(WebRTCPluginTest
    PRIVATE
      ${WEBRTC_LIBRARIES}
      ${OPENGL_opengl_LIBRARY}
      ${GLUT_LIBRARIES}
      ${CMAKE_DL_LIBS}
      ${GTEST_MAIN_LIBRARIES}
      ${GTEST_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${Vulkan_LIBRARY}
      ${NVCODEC_LIBRARIES}
      ${GLEW_STATIC_LIBRARIES}
      cuda
  )
  target_include_directories(WebRTCPluginTest
    PRIVATE
      ..
      ${CUDA_INCLUDE_DIRS}
      ${GLEW_INCLUDE_DIR}
      ${GLUT_INCLUDE_DIR}
    )
endif()

gtest_add_tests(TARGET WebRTCPluginTest)

target_include_directories(WebRTCPluginTest
  PRIVATE
    .
    ${CMAKE_SOURCE_DIR}/WebRTCPlugin
    ${CMAKE_SOURCE_DIR}/unity/include
    ${WEBRTC_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIRS}
    ${NVCODEC_INCLUDE_DIR}
)
