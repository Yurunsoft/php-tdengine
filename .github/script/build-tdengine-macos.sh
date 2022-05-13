#!/bin/bash
cd /tmp && \
git clone https://github.com/taosdata/TDengine.git -b ver-${TDENGINE_VERSION} --depth=1 \
cd TDengine && \
mkdir debug && cd debug && cmake .. && make && make install && \
echo "rpcForceTcp 1" >> /usr/local/etc/taos/taos.cfg
