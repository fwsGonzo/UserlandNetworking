#include <net/inet4.hpp>
#include "network.hpp" // generate_packet
#include "usernet.hpp"

void make_tcp_packet(net::Packet& pkt)
{
  auto* eth_hdr = (net::ethernet::Header*) pkt.data_end();
  eth_hdr->set_dest(MAC::BROADCAST);
  eth_hdr->set_src(MAC::EMPTY);
  eth_hdr->set_type(net::Ethertype::IP4);
  pkt.increment_layer_begin(sizeof(net::ethernet::Header));
  /// IP4 stuff
  auto ip4 = std::static_pointer_cast<net::PacketIP4&> pkt;
  /// IP4 stuff
  pkt.increment_layer_begin(-(int) sizeof(net::ethernet::Header));
  pkt.increment_data_end(1024);
}

void tcp_test1(net::Inet4& network, UserNet& driver)
{
  // bind & listen on TCP port
  auto& listener = network.tcp().listen(100);
  listener.on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
    });

  auto packet = network.create_packet();
  make_tcp_packet(*packet);
  printf("Packet size: %d\n", packet->size());
  driver.feed(std::move(packet));
}
