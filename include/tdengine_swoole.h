#pragma once

#ifdef HAVE_SWOOLE

#include "swoole_api.h"
#include "swoole_coroutine.h"
#include "swoole_socket.h"
#include "swoole_pipe.h"
#include "swoole_async.h"
#include "swoole_coroutine_system.h"
using namespace swoole;
using namespace swoole::coroutine;
using namespace swoole::network;
using swoole::Coroutine;
using swoole::Reactor;
using swoole::Pipe;

#endif
