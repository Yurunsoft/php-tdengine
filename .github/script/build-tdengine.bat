@ECHO OFF

d:
cd "D:\tdengine\TDengine-ver-%TDENGINE_VERSION%"

mkdir release

cd release

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64

cmake .. -G "NMake Makefiles"

nmake install
