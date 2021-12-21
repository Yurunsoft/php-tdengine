#!/bin/bash
cd /tmp && \
wget https://github.com/taosdata/TDengine/archive/refs/tags/ver-${TDENGINE_VERSION}.tar.gz -O tdengine.tar.gz && \
mkdir -p tdengine && \
tar -xf tdengine.tar.gz -C tdengine --strip-components=1 && \
cd tdengine && \
mkdir release && cd release && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . && make install && \
systemctl start taosd
