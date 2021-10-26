#!/bin/bash -eu

export LIBWEBRTC_DOWNLOAD_URL=https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M89/webrtc-linux.zip
export SOLUTION_DIR=$(pwd)/Plugin~

# Download and unzip LibWebRTC 

ZIPFILE=webrtc.zip
UNZIPDIR=$SOLUTION_DIR/webrtc

if [ ! -d "$UNZIPDIR" ]; then
    if [ ! -f "$ZIPFILE" ]; then
        curl -L $LIBWEBRTC_DOWNLOAD_URL > $ZIPFILE
    fi
  unzip -d $UNZIPDIR webrtc.zip 
fi

# Install libc++, libc++abi clang glut
# TODO:: Remove this install process from here and recreate an image to build the plugin.
sudo apt update
sudo apt install -y clang-10 libc++-10-dev libc++abi-10-dev freeglut3-dev

# Build UnityRenderStreaming Plugin 
cd "$SOLUTION_DIR"
cmake . \
  -D CMAKE_C_COMPILER="clang-10" \
  -D CMAKE_CXX_COMPILER="clang++-10" \
  -D CMAKE_CXX_FLAGS="-stdlib=libc++" \
  -D CMAKE_BUILD_TYPE="Release" \
  -B build

cmake \
  --build build \
  --config Release \
  --target WebRTCPlugin
