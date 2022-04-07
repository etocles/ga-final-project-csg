@echo off

mkdir ga-win64
cd ga-win64
cmake -G "Visual Studio 16 2019" ../../src/ga1-core
cd ..
