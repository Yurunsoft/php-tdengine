#!/bin/bash
cd /tmp && \
git clone --recurse -b ver-${TDENGINE_VERSION} --depth=1 https://github.com/taosdata/TDengine.git && \
cd TDengine && \
mkdir debug && cd debug && cmake .. -DBUILD_JDBC=false -DBUILD_TOOLS=false && make -j && make install && \
systemctl start taosd
