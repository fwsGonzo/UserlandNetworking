#include "network.hpp"

const MAC::Addr TEST_MAC_ADDRESS {0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
bool option_show_sent_packets = true;

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
  if (option_show_sent_packets)
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

#include <net/ip4/packet_arp.hpp>

void simulate_arp_response(net::Inet4& network, net::Packet_ptr packet) {
  auto& res = (net::PacketArp&) *packet;
  auto src_mac = res.source_mac();
  auto dst_mac = res.dest_mac();
  assert (dst_mac == MAC::BROADCAST);
  auto src_ip = res.source_ip();
  auto dst_ip = res.dest_ip();
  assert(dst_ip == net::ip4::Addr(10,0,0,1));

  res.init(TEST_MAC_ADDRESS, dst_ip, src_ip);
  res.set_dest_mac(src_mac);
  res.set_opcode(net::Arp::H_reply);
  // set ethernet stuff
  packet->increment_layer_begin(- (int)sizeof(net::ethernet::Header));
  auto* eth = (net::ethernet::Header*) packet->layer_begin();
  eth->set_dest(eth->src());
  eth->set_src(TEST_MAC_ADDRESS);
  // shipit
  get_driver(network).feed(std::move(packet));
}
