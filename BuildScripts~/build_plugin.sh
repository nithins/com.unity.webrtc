#!/bin/bash

export LIBWEBRTC_DOWNLOAD_URL=https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M83/webrtc-linux.zip
export GLEW_DOWNLOAD_URL=https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip
export SOLUTION_DIR=$(pwd)/Plugin~

# Download LibWebRTC 
curl -L $LIBWEBRTC_DOWNLOAD_URL > webrtc.zip
unzip -d $SOLUTION_DIR/webrtc webrtc.zip 

# Download GLEW 
curl -L $GLEW_DOWNLOAD_URL > glew.zip
unzip -d $SOLUTION_DIR glew.zip 
mv glew-2.1.0 glew

# Install libc++, libc++abi googletest clang glut
# TODO:: Remove this install process from here and recreate an image to build the plugin.
sudo apt update
sudo apt install -y libc++-dev libc++abi-dev googletest clang freeglut3-dev

# Install googletest
cd /usr/src/googletest
sudo cmake -Dcxx_no_rtti=ON \
           -DCMAKE_C_COMPILER="clang" \
           -DCMAKE_CXX_COMPILER="clang++" \
           -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
           CMakeLists.txt
sudo make
sudo cp googlemock/*.a "/usr/lib"
sudo cp googlemock/gtest/*.a "/usr/lib"

# Build UnityRenderStreaming Plugin 
cd "$SOLUTION_DIR"
cmake -DCMAKE_C_COMPILER="clang" \
      -DCMAKE_CXX_COMPILER="clang++" \
      .
make

# Run UnitTest
"$SOLUTION_DIR/WebRTCPluginTest/WebRTCPluginTest"