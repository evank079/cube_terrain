Description
------------
Cube Terrain is a 3d OpenGL program that creates a messerizing display of infinite cubes with unique, proceeduraly generate terrains in each cube.

Controls
------------------
W A S D - Rotetes up, left, down, right respectively at the next center point of the current cube

TO BUILD
------------

--LINUX BUILD--

If the build folder doesn't exist, first make it then cd into build:
mkdir build && cd build

Then once you are in the build directory:
cmake ..
cmake --build .

--WINDOWS BUILD--

Run these commands from the main directory (not the build folder):

cmake -B build-win \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++

cmake --build build-win


