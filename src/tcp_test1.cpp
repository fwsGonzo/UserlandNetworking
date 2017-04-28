#include <net/inet4.hpp>
#include "test_system.hpp"

static void outgoing_packets(net::Inet4&, net::Packet_ptr);
static void outgoing_arp_packet(net::Inet4&, net::Packet_ptr);
static void outgoing_tcp_packet(net::Inet4&, net::tcp::Packet&);

/// begin test ///
void tcp_test1(net::Inet4& network)
{
  test_system.init(network, outgoing_packets);

  // bind & listen on TCP port 100
  auto& listener = network.tcp().listen(100);
  listener.on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
    });
  // ip address of stack
  auto addr = network.ip_addr();

  // send first packet to [Network] port 100
  tcp_send_packet(network,
  [addr] (auto& tcp) {
    tcp.set_src_port(1);
    tcp.set_ip_src({10, 0, 0, 1});
    tcp.set_dst_port(100);
    tcp.set_ip_dst(addr);
    tcp.set_flag(net::tcp::SYN);
  });
}

void outgoing_tcp_packet(net::Inet4& network, net::tcp::Packet& pkt)
{
  printf("[TCP] RECV packet: %s\n", pkt.to_string().c_str());
  printf("RESET: %d\n", pkt.isset(net::tcp::RST));
  assert(0);
  if (pkt.isset(net::tcp::ACK)) {
    tcp_send_packet(network,
    [&src = pkt] (auto& tcp) {
      printf("*** Got ACK, sending bogus packet\n");
      tcp.set_ip_src({10, 0, 0, 1});
      tcp.set_dst_port(100);
      tcp.set_ip_dst(src.ip_src());
      tcp.set_seq(src.seq()+1);
      tcp.set_flag(net::tcp::SYN);
      //tcp.set_flag(net::tcp::ACK);
    });
  }
}

void outgoing_packets(net::Inet4& network, net::Packet_ptr pkt)
{
  auto* eth = (net::ethernet::Header*) pkt->layer_begin();
  printf("*** Packet recv type=%hx (len=%u)\n",
          eth->type(), pkt->size());

  switch(eth->type()) {
  case net::Ethertype::IP4:
      assert(eth->dest() == TEST_MAC_ADDRESS);
      pkt->increment_layer_begin(sizeof(net::ethernet::Header));
      // convert to TCP (assume they're all TCP in this test)
      outgoing_tcp_packet(network, (net::tcp::Packet&) *pkt);
      break;
  case net::Ethertype::ARP:
      pkt->increment_layer_begin(sizeof(net::ethernet::Header));
      simulate_arp_response(network, std::move(pkt));
      break;
  default:
      break;
  }
}
