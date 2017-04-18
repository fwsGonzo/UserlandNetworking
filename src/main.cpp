#include <net/inet4.hpp>
#include "usernet.hpp"
#include "network.hpp" // generate_packet
#include "testsuite.hpp"

void outgoing(net::Packet_ptr packet);

int main(void)
{
  extern void __arch_init();
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

  network.negotiate_dhcp(1.0,
    [] (bool timeout) {
      printf("DHCP callback  timeout=%d\n", timeout);
    });

  // generate
  auto* packet = generate_packet(1024);
  for (int i = 0; i < 1024; i++) packet->data[i] = i & 0xff;
  printf("Generate custom packet %p with len=%u\n",
        packet, packet->driver.len);

  // feed network a raw packet that starts at packet_t::len
  network_driver.feed(packet);

  // begin event loop
  OS::event_loop();
}

void outgoing(net::Packet_ptr packet)
{
  printf("Packet received from network stack (len=%u)\n", packet->size());
}
