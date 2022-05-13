#!/bin/bash

cd D:\

git clone --recurse -b ver-$env:TDENGINE_VERSION --depth=1 https://github.com/taosdata/TDengine.git
