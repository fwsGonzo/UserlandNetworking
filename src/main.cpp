#include <net/inet4.hpp>
#include "usernet.hpp"
static char statman_data[8192];

void outgoing(net::Packet_ptr packet)
{
  printf("Packet received from network stack (len=%u)\n", packet->size());
}

struct packet_t
{
  uint16_t len;
  char     data[0];
};

static packet_t* generate_packet(const uint16_t LEN)
{
  auto* data = new char[sizeof(net::Packet) + sizeof(packet_t) + LEN];
  packet_t* packet = (packet_t*) &data[sizeof(net::Packet)];
  packet->len = LEN;
  return packet;
}

int main(void)
{
  // needed for stats
  Statman::init((uintptr_t) statman_data, sizeof(statman_data));

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

  // generate
  auto* packet = generate_packet(1024);
  for (int i = 0; i < 1024; i++) packet->data[i] = i & 0xff;
  printf("Generate custom packet with len=%u\n", packet->len);

  // feed network a raw packet that starts at packet_t::len
  network_driver.feed(&packet->len, packet->len);
}
