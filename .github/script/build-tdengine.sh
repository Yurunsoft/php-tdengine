#!/bin/bash
cd /tmp && \
git clone --recurse -b ver-${TDENGINE_VERSION} --depth=1 https://github.com/taosdata/TDengine.git && \
cd TDengine && \
mkdir release && cd release && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -Werror=return-type && make install && \
systemctl start taosd
