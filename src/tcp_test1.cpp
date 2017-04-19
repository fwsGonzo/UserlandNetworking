#include <net/inet4.hpp>
#include "network.hpp" // generate_packet
#include "usernet.hpp"

typedef delegate<void(net::tcp::Packet&)> tcp_callback_t;

static void make_tcp_packet(net::Packet& pkt, tcp_callback_t tcp_callback)
{
  auto* eth_hdr = (net::ethernet::Header*) pkt.data_end();
  eth_hdr->set_dest(MAC::BROADCAST);
  eth_hdr->set_src(MAC::EMPTY);
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
  pkt.increment_layer_begin(-(int) sizeof(net::ethernet::Header));
}
static inline void
tcp_send_packet(net::Inet4& network, tcp_callback_t tcp_callback)
{
  auto  packet = network.create_packet();
  auto& driver = (UserNet&) network.nic();
  make_tcp_packet(*packet, tcp_callback);
  driver.feed(std::move(packet));
}

void tcp_test1(net::Inet4& network)
{
  // bind & listen on TCP port 100
  auto& listener = network.tcp().listen(100);
  listener.on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
    });
  // ip address of stack
  auto addr = network.ip_addr();

  // send some garbage to [NetworkIP]:100
  tcp_send_packet(network,
  [addr] (auto& tcp) {
    tcp.set_dst_port(100);
    tcp.set_ip_dst(addr);
    tcp.fill((uint8_t*) "HELLO WORLD!", 13);
  });
}
