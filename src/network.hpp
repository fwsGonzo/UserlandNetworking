/**
 * Stuff
**/
#pragma once
#include "drivers/usernet.hpp"
#include <net/inet4>

inline UserNet& get_driver(net::Inet4& netw) {
  return (UserNet&) netw.nic();
}
inline UserNet& get_driver(net::Inet4* netw) {
  return (UserNet&) netw->nic();
}

struct packet_t
{
  UserNet::driver_hdr driver;
  char                data[0];
} __attribute__((packed));

// generating packet buffers is unfortunately not trivial:
// 1. add room for driver header (packet_t) + packet (net::Packet)
// 2. step ahead of net::Packet which is where the driver portion starts
// this is done because we want to avoid allocating net::Packet
inline packet_t* generate_packet(const uint16_t LEN)
{
  auto* data = new char[sizeof(net::Packet) + sizeof(packet_t) + LEN];
  packet_t* packet = (packet_t*) &data[sizeof(net::Packet)];
  packet->driver.len = LEN;
  return packet;
}

extern MAC::Addr const TEST_MAC_ADDRESS;

typedef delegate<void(net::tcp::Packet&)> tcp_callback_t;

extern void
make_tcp_packet(net::Packet& pkt, tcp_callback_t tcp_callback);
extern void
tcp_send_packet(net::Inet4& network, tcp_callback_t tcp_callback);

extern void
simulate_arp_response(net::Inet4& network, net::Packet_ptr packet);
