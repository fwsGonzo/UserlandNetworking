#include <kernel/os.hpp>
#include <sys/time.h>
MHz OS::cpu_mhz_ = MHz(3500);
int64_t OS::micros_since_boot() noexcept
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}


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
#include <signal.h>
#include <unistd.h>
static timer_t timer_id;
static void stop_timers() {
  printf("Timers stopped\n");
}

extern "C" void alarm_handler(int sig)
{
  (void) sig;
}
static void begin_timer(std::chrono::microseconds usec)
{
  using namespace std::chrono;
  auto secs = duration_cast<seconds> (usec);

  struct itimerspec it;
  it.it_value.tv_sec  = secs.count();
  it.it_value.tv_nsec = 1000 * (usec.count() - secs.count() * 1000000);
  timer_settime(timer_id, 0, &it, nullptr);
}

#include <statman>
void __arch_init()
{
  // set affinity to CPU 1
#ifdef __linux__
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(1, &cpuset);
  sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
  // statman
  static char statman_data[8192];
  Statman::init((uintptr_t) statman_data, sizeof(statman_data));
  // setup Linux timer (with signal handler)
  struct sigevent sev;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo  = SIGALRM;
  timer_create(CLOCK_BOOTTIME, &sev, &timer_id);
  signal(SIGALRM, alarm_handler);
  // setup timer system
  Timers::init(begin_timer, stop_timers);
  Timers::ready();
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
