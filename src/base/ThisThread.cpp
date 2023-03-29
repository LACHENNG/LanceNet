// Author: Lang Chen @ nwpu
#include "ThisThread.h"

namespace ThisThread{

// definitions of externs in `ThisThread.h
__thread pid_t tl_cached_tid = INVALID_THREAD_ID;

__thread char tl_namebuf[THREAD_NAME_BUF_SIZE];

__thread char* tl_threadName = nullptr;

int ncalls_gettid = 0;

}
