#include <iostream>

#include <net/inet4>
#include <net/ethernet/header.hpp>

static net::BufferStore bufstore(256, 1500);

void fuzz_ip4(net::Inet4& network,
              const void* data, uint16_t packet_len)
{


	std::cerr << "packet_len: " << packet_len << std::endl;

  auto buffer = bufstore.get_buffer();
  // wrap in packet, pass to Link-layer
  auto* ptr = (net::Packet*) buffer.addr;
  new (ptr) net::Packet(
      0,
      packet_len,
      0 + packet_len,
      buffer.bufstore);
  // copy data over
  memcpy(ptr->layer_begin(), data, packet_len);
  // go past the Ethernet layer (into IP4)
  ptr->increment_layer_begin(sizeof(net::ethernet::Header));

  //network.

  // send packet directly to IP4 instance
  network.ip_obj().receive(net::Packet_ptr(ptr));
}
