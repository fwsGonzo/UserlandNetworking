#include <balancer.hpp>
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
}
