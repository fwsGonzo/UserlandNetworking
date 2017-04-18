#pragma once

#include <kernel/os.hpp>
#include <unistd.h>
inline void OS::event_loop()
{
  while (true)
  {
    Timers::timers_handler();
    if (Timers::active() == 0) {
      printf("No active timers left\n");
    }
    pause();
  }
}
