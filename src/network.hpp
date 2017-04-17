/**
 *
**/
#pragma once
#include <net/packet.hpp>

struct packet_t
{
  uint16_t len;
  char     data[0];
};

// generating packet buffers is unfortunately not trivial:
// 1. add room for driver header (packet_t) + packet (net::Packet)
// 2. step ahead of net::Packet which is where the driver portion starts
// this is done because we want to avoid allocating net::Packet
inline packet_t* generate_packet(const uint16_t LEN)
{
  auto* data = new char[sizeof(net::Packet) + sizeof(packet_t) + LEN];
  packet_t* packet = (packet_t*) &data[sizeof(net::Packet)];
  packet->len = LEN;
  return packet;
}
