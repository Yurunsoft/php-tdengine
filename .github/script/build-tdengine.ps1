#!/bin/bash

cd D:\

Invoke-Webrequest "https://github.com/taosdata/TDengine/archive/refs/tags/ver-$env:TDENGINE_VERSION.zip" -Outfile "tdengine.zip"

Expand-Archive "tdengine.zip" "D:\tdengine\"

cd "D:\tdengine\TDengine-ver-$env:TDENGINE_VERSION"

mkdir release

cd release

cmd /c "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64

cmd /c cmake .. -G "NMake Makefiles"

cmd /c nmake install
