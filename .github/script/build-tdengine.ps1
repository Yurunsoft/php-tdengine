#!/bin/bash

cd D:\

Invoke-Webrequest "https://github.com/taosdata/TDengine/archive/refs/tags/ver-$env:TDENGINE_VERSION.zip" -Outfile "tdengine.zip"

Expand-Archive "tdengine.zip" "C:\tdengine"

cd "C:\tdengine"

mkdir release

cd release

cmake -DCMAKE_BUILD_TYPE=Release ..

cmake --build .

make install
