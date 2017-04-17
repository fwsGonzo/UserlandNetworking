#include "usernet.hpp"

UserNet::UserNet()
  : Link(Link_protocol{{this, &UserNet::transmit}, mac()},
    256u, 2048 /* 256x half-page buffers */)
{

}

size_t UserNet::transmit_queue_available()
{
  return 128;
}

void UserNet::transmit(net::Packet_ptr)
{
  printf("transmit\n");
}

// create new packet from nothing
net::Packet_ptr UserNet::create_packet(int link_offset)
{
  auto buffer = bufstore().get_buffer();
  auto* ptr = (net::Packet*) buffer.addr;
  new (ptr) net::Packet(
        sizeof(driver_hdr) + link_offset,
        0,
        sizeof(driver_hdr) + packet_len(),
        buffer.bufstore);

  return net::Packet_ptr(ptr);
}
// wrap buffer-length (that should belong to bufferstore) in packet wrapper
std::unique_ptr<net::Packet> UserNet::recv_packet(uint8_t* data, uint16_t size)
{
  auto* ptr = (net::Packet*) (data - sizeof(net::Packet));
  new (ptr) net::Packet(
      sizeof(driver_hdr),
      size - sizeof(driver_hdr),
      sizeof(driver_hdr) + packet_len(),
      &bufstore());

  return net::Packet_ptr(ptr);
}

void UserNet::deactivate()
{
  std::terminate();
}

// nothing to do
void UserNet::move_to_this_cpu() {}
