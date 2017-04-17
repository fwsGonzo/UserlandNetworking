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
