#include "test_system.hpp"

TestSystem test_system;

void TestSystem::outgoing_packets(net::Packet_ptr pkt)
{
  stacklevel++;
  if (stacklevel > 1000) {
    printf("Stack blew up!\n"); return;
  }

  current_handler(*network, std::move(pkt));

  stacklevel--;
}
