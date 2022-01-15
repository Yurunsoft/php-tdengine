#!/bin/bash

cd D:\

Invoke-Webrequest "https://github.com/taosdata/TDengine/archive/refs/tags/ver-$env:TDENGINE_VERSION.zip" -Outfile "tdengine.zip"

Expand-Archive "tdengine.zip" "D:\tdengine\"
