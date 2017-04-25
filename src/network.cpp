#include "network.hpp"

const MAC::Addr TEST_MAC_ADDRESS {0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

void make_tcp_packet(net::Packet& pkt, tcp_callback_t tcp_callback)
{
  auto* eth_hdr = (net::ethernet::Header*) pkt.data_end();
  eth_hdr->set_dest(UserNet::MAC_ADDRESS);
  eth_hdr->set_src(TEST_MAC_ADDRESS);
  eth_hdr->set_type(net::Ethertype::IP4);
  pkt.increment_layer_begin(sizeof(net::ethernet::Header));
  /// TCP stuff
  auto& tcp = (net::tcp::Packet&) pkt;
  tcp.init();
  tcp_callback(tcp);
  tcp.set_checksum(net::TCP::checksum(tcp));
  /// IP4 stuff
  auto& ip4 = (net::PacketIP4&) pkt;
  ip4.make_flight_ready();
  printf("[TCP] SEND packet: %s\n", tcp.to_string().c_str());
  pkt.increment_layer_begin(-(int) sizeof(net::ethernet::Header));
}

void tcp_send_packet(net::Inet4& network, tcp_callback_t tcp_callback)
{
  auto  packet = network.create_packet();
  auto& driver = (UserNet&) network.nic();
  make_tcp_packet(*packet, tcp_callback);
  driver.feed(std::move(packet));
}
