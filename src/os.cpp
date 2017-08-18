#include <kernel/os.hpp>
#include <sys/time.h>
MHz OS::cpu_mhz_ = MHz(3500);
int64_t OS::micros_since_boot() noexcept
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

struct mallinfo
{
  // Total size of memory allocated with sbrk by malloc, in bytes.
  int arena;
  // Number of chunks not in use.
  // (The memory allocator internally gets chunks of memory from the
  // operating system, and then carves them up to satisfy individual
  // malloc requests; see The GNU Allocator.)
  int ordblks;
  // Unused.
  int smblks;
  // Total number of chunks allocated with mmap.
  int hblks;
  // Total size of memory allocated with mmap, in bytes.
  int hblkhd;
  // Unused and always 0.
  int usmblks;
  // Unused.
  int fsmblks;
  // Total size of memory occupied by chunks handed out by malloc.
  int uordblks;
  // Total size of memory occupied by free (not in use) chunks.
  int fordblks;
  // Size of the top-most releasable chunk that normally borders the end of the heap (i.e., the high end of the virtual address space’s data segment).
  int keepcost;
};
extern "C" struct mallinfo mallinfo(void);

std::string OS::version_str_ = "v1.0";
uintptr_t OS::heap_begin() noexcept {
  return 0;
}
uintptr_t OS::heap_end() noexcept {
  return 0;
}
uintptr_t OS::heap_usage() noexcept {
  auto info = mallinfo();
  return info.arena + info.hblkhd;
}

#include <kernel/rtc.hpp>
#include <time.h>
RTC::timestamp_t RTC::booted_at = time(0);
RTC::timestamp_t RTC::now() { return time(0); }
RTC::timestamp_t OS::boot_timestamp() {
  return RTC::boot_timestamp();
}

#include <smp>
int SMP::cpu_id() noexcept {
  return 0;
}
void SMP::global_lock() noexcept {}
void SMP::global_unlock() noexcept {}
void SMP::add_task(SMP::task_func, int) {}
void SMP::signal(int) {}

#include <service>
void Service::ready() {}
std::string Service::name() {
  return "Userland Networking";
}

#include <kernel/timers.hpp>
static void stop_timers() {}

#include <signal.h>
#include <unistd.h>
static timer_t timer_id;
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
  Statman::get().init((uintptr_t) statman_data, sizeof(statman_data));
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

#include <execinfo.h>
void print_backtrace()
{
  static const int NUM_ADDRS = 64;
  void*  addresses[NUM_ADDRS];

  int nptrs = backtrace(addresses, NUM_ADDRS);
  printf("backtrace() returned %d addresses\n", nptrs);

  /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     would produce similar output to the following: */

  char** strings = backtrace_symbols(addresses, nptrs);
  if (strings == NULL) {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }

  for (int j = 0; j < nptrs; j++)
      printf("#%02d: %8p %s\n", j, addresses[j], strings[j]);

  free(strings);
}

extern "C"
void panic(const char* why)
{
  printf("!! PANIC !!\nReason: %s\n", why);
  std::abort();
}
