#!/bin/bash
cd /tmp && \
git clone --recurse -b ver-${TDENGINE_VERSION} --depth=1 https://github.com/taosdata/TDengine.git && \
cd TDengine && \
mkdir debug && cd debug && cmake .. && make && make install && \
echo "rpcForceTcp 1" >> /usr/local/etc/taos/taos.cfg
