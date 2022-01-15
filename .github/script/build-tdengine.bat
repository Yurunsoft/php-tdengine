@ECHO OFF

d:
cd "D:\tdengine\TDengine-ver-%TDENGINE_VERSION%"

mkdir release

cd release

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64

cmake .. -G "NMake Makefiles" -DBUILD_JDBC=false -DTD_BUILD_HTTP=false -DTD_BUILD_LUA=false

nmake install
