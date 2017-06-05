#include <net/inet4.hpp>
#include "network.hpp" // generate_packet
#include "testsuite.hpp"

static void outgoing(net::Packet_ptr packet);
extern void __arch_init();

int main(void)
{
  __arch_init();

  // the network driver
  UserNet network_driver;
  network_driver.set_transmit_forward(outgoing);
  // the network stack
  net::Inet4 network((hw::Nic&) network_driver);
  network.network_config(
    { 10,  0,  0,  2},
    {255,255,255,  0},
    { 10,  0,  0,  1}, // GW
    {  8,  8,  8,  8}  // DNS
  );
  // TAP device
  extern void tap_device(net::Inet4&);
  //tap_device(network);

  // AFL (or other) on stdin
  extern void stdin_device(net::Inet4&);
  stdin_device(network);

  extern void random_fuzzing(net::Inet4&);
  //random_fuzzing(network);

  extern void tcp_test1(net::Inet4&);
  //tcp_test1(network);

  // begin event loop
  OS::event_loop();
}

void outgoing(net::Packet_ptr packet)
{
  printf("Packet received from network stack (len=%u)\n", packet->size());
  printf("-> IGNORED\n");
}
