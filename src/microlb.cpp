#include <balancer.hpp>

#include <os>
#include <timers>
#include <ctime>
#define PERIOD_SECS  5
static void print_stats(int);

static Balancer* balancer = nullptr;
#define NET_INCOMING  0
#define NET_OUTGOING  0

void balancer_start(net::Inet4& stack)
{
  std::vector<net::Socket> nodes {
    //{{10,20,17,191}, 80}, {{10,20,17,192}, 80},
    //{{10,20,17,193}, 80}, {{10,20,17,194}, 80}
    {{10,0,0,1}, 6001}, {{10,0,0,1}, 6002}, {{10,0,0,1}, 6003}, {{10,0,0,1}, 6004}
  };
  balancer = new Balancer(stack, 80, stack, nodes, 20);

  using namespace std::chrono;
  Timers::periodic(seconds(1), seconds(PERIOD_SECS), print_stats);
}


static std::string now()
{
  auto  tnow = time(0);
  auto* curtime = localtime(&tnow);

  char buff[48];
  int len = strftime(buff, sizeof(buff), "%c", curtime);
  return std::string(buff, len);
}

static void print_heap_info()
{
  static intptr_t last = 0;
  // show information on heap status, to discover leaks etc.
  auto heap_begin = OS::heap_begin();
  auto heap_end   = OS::heap_end();
  auto heap_usage = OS::heap_usage();
  intptr_t heap_size = heap_end - heap_begin;
  auto diff = heap_size - last;
  printf("Heap size %lu Kb  diff %ld (%ld Kb)  usage  %lu kB\n",
        heap_size / 1024, diff, diff / 1024, heap_usage / 1024);
  last = (int32_t) heap_size;
}

template <int N, typename T>
struct rolling_avg {
  std::deque<T> values;

  void push(T value) {
    if (values.size() >= N) values.pop_front();
    values.push_back(value);
  }
  double avg() const {
    double ps = 0.0;
    if (values.empty()) return ps;
    for (auto v : values) ps += v;
    return ps / values.size();
  }
};

static void print_stats(int)
{
  static int64_t last = 0;
  static rolling_avg<5, int64_t> avg_growth;
  static rolling_avg<5, int> avg_session;
  static rolling_avg<5, int> avg_poolsize;
  static rolling_avg<5, int> avg_poolconn;
  static rolling_avg<5, int> avg_waiting;

  const auto& nodes = balancer->nodes;

  auto totals = nodes.total_sessions();
  auto growth = totals - last;  last = totals;
  avg_growth.push(growth);

  avg_session.push(nodes.open_sessions());
  avg_poolsize.push(nodes.pool_size());
  avg_poolconn.push(nodes.pool_connections());
  avg_waiting.push(balancer->wait_queue());

  printf("*** [%s] ***\n", now().c_str());
  printf("Total %ld G %ld  Open %d  Wait %d\n",
         totals, growth, nodes.open_sessions(), balancer->wait_queue());
  printf("Avg.sessions=%.2f Avg.growth=%.2f Avg.waitq=%.2f\n",
          avg_session.avg(), avg_growth.avg(), avg_waiting.avg());
  printf("Avg.pool size=%.2f Avg.pool conns=%.2f\n",
          avg_poolsize.avg(), avg_poolconn.avg());
  // heap statistics
  print_heap_info();
}
