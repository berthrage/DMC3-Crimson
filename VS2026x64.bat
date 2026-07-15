@echo off
git submodule init
git submodule update
mkdir "VS2026 x64"
cd "VS2026 x64"
cmake .. -G "Visual Studio 18 2026" -A x64
cd ..
pause
