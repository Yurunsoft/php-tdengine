#!/bin/bash

cd D:\

Invoke-Webrequest "https://github.com/taosdata/TDengine/archive/refs/tags/ver-$env:TDENGINE_VERSION.zip" -Outfile "tdengine.zip"

Expand-Archive "tdengine.zip" "D:\tdengine"

cd "D:\tdengine"

mkdir release

cd release

&"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64

cmake .. -G "NMake Makefiles"

nmake
