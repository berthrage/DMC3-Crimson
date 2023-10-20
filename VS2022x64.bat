@echo off
git submodule init
git submodule update
mkdir "VS2022 x64"
cd "VS2022 x64"
cmake .. -G "Visual Studio 17 2022" -A x64
cd ..
pause