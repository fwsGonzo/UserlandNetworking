#include <kernel/os.hpp>
MHz OS::cpu_mhz_ = MHz(3500);

#include <smp>
int SMP::cpu_id() noexcept
{
  return 0;
}

#include <service>
void Service::ready() {}

#include <kernel/rtc.hpp>
#include <time.h>
RTC::timestamp_t RTC::booted_at = time(0);
RTC::timestamp_t RTC::now() { return time(0); }

#include <kernel/timers.hpp>
#include <unistd.h>
static void stop_timers() {
  printf("Timers stopped\n");
  std::terminate();
}
static void begin_timer(std::chrono::microseconds usec) {
  printf("Timer started: %lu usec\n", usec.count());
  usleep(usec.count());
}

void init_timers()
{
  Timers::init(begin_timer, stop_timers);
}
