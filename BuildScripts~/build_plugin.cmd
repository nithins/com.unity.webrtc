@echo off

set LIBWEBRTC_DOWNLOAD_URL=https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M83/webrtc-win.zip
set GLEW_DOWNLOAD_URL=https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip
set GLUT_DOWNLOAD_URL=https://sourceforge.net/projects/freeglut/files/freeglut/3.2.1/freeglut-3.2.1.tar.gz/download
set SOLUTION_DIR=%cd%\Plugin~

echo -------------------
echo Download LibWebRTC 

curl -L %LIBWEBRTC_DOWNLOAD_URL% > webrtc.zip
7z x -aoa webrtc.zip -o%SOLUTION_DIR%\webrtc

echo -------------------
echo Download GLEW

cd %SOLUTION_DIR%
curl -L %GLEW_DOWNLOAD_URL% > glew.zip
7z x -aoa glew.zip -o%SOLUTION_DIR%
ren glew-2.1.0 glew

echo -------------------
echo Download GLUT

cd %SOLUTION_DIR%
curl -L %GLUT_DOWNLOAD_URL% > glut.tar.gz
7z x glut.tar.gz
7z x -aoa glut.tar -o%SOLUTION_DIR%
ren freeglut-3.2.1 glut

echo -------------------
echo Install googletest

cd %SOLUTION_DIR%
git clone https://github.com/google/googletest.git
cd googletest
git checkout 2fe3bd994b3189899d93f1d5a881e725e046fdc2
cmake . -G "Visual Studio 15 2017" -A x64 -B "build64"
cmake --build build64 --config Release
mkdir include\gtest
xcopy /e googletest\include\gtest include\gtest
mkdir include\gmock
xcopy /e googlemock\include\gmock include\gmock
mkdir lib
xcopy /e build64\googlemock\Release lib
xcopy /e build64\googlemock\gtest\Release lib

echo -------------------
echo Build GLUT

cd %SOLUTION_DIR%\glut
cmake -DFREEGLUT_BUILD_DEMOS=OFF .
cmake --build . --config Release
xcopy /e lib\Release lib

echo -------------------
echo Build com.unity.webrtc Plugin 

cd %SOLUTION_DIR%
cmake . -G "Visual Studio 15 2017" -A x64 -B "build64"
cmake --build build64 --config Release

echo -------------------
echo Copy freeglut.dll 
copy %SOLUTION_DIR%\bin\Release\freeglut.dll %SOLUTION_DIR%\build64\WebRTCPluginTest\Release\

echo -------------------
echo Test com.unity.webrtc Plugin 

%SOLUTION_DIR%\build64\WebRTCPluginTest\Release\WebRTCPluginTest.exe
if not %errorlevel% == 0 exit 1
echo -------------------