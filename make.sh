#!/bin/sh -e
__DIR__=$(cd "$(dirname "$0")";pwd)

COMPILE_PARAMS=""

if [ "$(uname | grep -i darwin)"x != ""x ]; then
  CPU_COUNT="$(sysctl -n machdep.cpu.core_count)"
else
  CPU_COUNT="$(/usr/bin/nproc)"
fi
if [ -z ${CPU_COUNT} ]; then
  CPU_COUNT=4
fi

cd "${__DIR__}"

if [ "$1" = "cmake" ] ;then
  phpize
  ./configure ${COMPILE_PARAMS}
  cmake .
  make -j ${CPU_COUNT}
  exit 0
fi

if [ "$1" = "clean" ] ;then
  make clean
  phpize --clean
  exit 0
fi

if [ "$1" = "install-module" ] ;then
  make tdengine
  __EXT_DIR__=$(php-config --extension-dir)
  cp lib/tdengine.so "${__EXT_DIR__}"
  echo "cp lib/tdengine.so ${__EXT_DIR__}"
  exit 0
fi

if [ "$1" = "help" ] ;then
  echo "./make.sh cmake"
  echo "./make.sh install-module"
  echo "./make.sh clean"
  echo "./make.sh debug"
  echo "./make.sh trace"
  echo "./make.sh library [dev]"
  echo "./make.sh"
  exit 0
fi

phpize
if [ "$1" = "debug" ] ;then
  ./configure ${COMPILE_PARAMS}
elif [ "$1" = "trace" ] ;then
  ./configure ${COMPILE_PARAMS}
else
  ./configure ${COMPILE_PARAMS}
fi
make clean
make -j ${CPU_COUNT}

if [ "$(whoami)" = "root" ]; then
  make install
else
  sudo make install
fi
