#!/bin/bash
cd /tmp && \
git clone https://github.com/taosdata/TDengine.git -b ver-${TDENGINE_VERSION} --depth=1 \
cd TDengine && \
mkdir release && cd release && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . && make install && \
systemctl start taosd
