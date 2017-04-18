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

#ifdef __MACH__
#include <stdlib.h>
#include <stddef.h>
#include <gsl/gsl_assert>
void* memalign(size_t alignment, size_t size) {
  void* ptr {nullptr};
  int res = posix_memalign(&ptr, alignment, size);
  Ensures(res == 0);
  return ptr;
}
void* aligned_alloc(size_t alignment, size_t size) {
  return memalign(alignment, size);
}
#endif
