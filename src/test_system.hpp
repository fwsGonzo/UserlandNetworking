#pragma once
#include "network.hpp" // generate_packet
#include "drivers/usernet.hpp"

struct TestSystem
{
  void outgoing_packets(net::Packet_ptr);

  typedef delegate<void(net::Inet4&, net::Packet_ptr)> forw_func;

  net::Inet4* network;
  forw_func   current_handler;
  int         stacklevel = 0;

  void init(net::Inet4& netw, forw_func func) {
    network = &netw;
    auto& driver = (UserNet&) network->nic();
    driver.set_transmit_forward({this, &TestSystem::outgoing_packets});
    current_handler = func;
  }
};
extern TestSystem test_system;
