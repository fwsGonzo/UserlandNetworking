#include <net/inet4.hpp>
#include "usernet.hpp"
static char statman_data[8192];

void outgoing(net::Packet_ptr packet)
{
  printf("Packet received from network stack (len=%u)\n", packet->size());
}

int main(void)
{
  // needed for stats
  Statman::init((uintptr_t) statman_data, sizeof(statman_data));

  // the network driver
  UserNet    network_driver;
  network_driver.set_transmit_forward(outgoing);
  // the network stack
  net::Inet4 network((hw::Nic&) network_driver);
  network.network_config(
    { 10,  0,  0,  2},
    {255,255,255,  0},
    { 10,  0,  0,  1}, // GW
    {  8,  8,  8,  8}  // DNS
  );

  printf("Sending bogus to network!\n");
  char data[1024];
  for (int i = 0; i < sizeof(data); i++) data[i] = i & 0xff;

  network_driver.feed(data, sizeof(data));
}
