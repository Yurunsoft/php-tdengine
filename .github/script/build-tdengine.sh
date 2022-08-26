#!/bin/bash
cd /tmp && \
git clone --recurse -b ver-${TDENGINE_VERSION} --depth=1 https://github.com/taosdata/TDengine.git && \
cd TDengine && \
mkdir release && cd release && cmake -DCMAKE_C_FLAGS="-Werror=return-type" -DCMAKE_BUILD_TYPE=Release .. && cmake --build . && make install && \
systemctl start taosd
