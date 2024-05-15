@echo off
set currentPath=%cd%

ECHO Cloning VCPKG from Git
ECHO =============================

git clone https://github.com/microsoft/vcpkg Dependencies/vcpkg

ECHO =============================
ECHO Initializing CMake project
ECHO =============================

cd Dependencies/vcpkg
bootstrap-vcpkg.sh
vcpkg install

ECHO =============================

ECHO Initializing CMake project
ECHO =============================
cd %currentPath%
cmake -DCMAKE_TOOLCHAIN_FILE=%currentPath%/Dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake -S %currentPath% -B %currentPath%/build

ECHO =============================
ECHO Install done