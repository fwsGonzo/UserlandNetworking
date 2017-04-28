#include <net/inet4.hpp>
#include "test_system.hpp"
#include "http.hpp"

static void outgoing_packets(net::Inet4&, net::Packet_ptr);

void random_fuzzing(net::Inet4& network)
{
  test_system.init(network, outgoing_packets);
  //network.negotiate_dhcp();

  http_server(network, 80);

  using namespace std::chrono;
  Timers::periodic(1ms, 5ms,
    [&network] (int) {
      tcp_send_packet(network,
        [addr = network.ip_addr()] (net::tcp::Packet& packet) {

          auto* start = packet.layer_begin();
          auto* end   = packet.data_end();

          for (auto* ptr = start; ptr < end; ptr++) {
            *ptr = rand() & 0xff;
          }

          // need to set protocol to allow transmit
          packet.set_protocol(net::Protocol::TCP);
          packet.set_ip_dst(addr);
          packet.set_flag(net::tcp::SYN);
        });
    });

  tcp_send_packet(network,
  [addr = network.ip_addr()] (auto& tcp) {
    tcp.set_src_port(1);
    tcp.set_ip_src({10, 0, 0, 1});
    tcp.set_dst_port(100);
    tcp.set_ip_dst(addr);
    tcp.set_flag(net::tcp::SYN);
  });
}

void outgoing_tcp_packet(net::Inet4&, net::tcp::Packet& pkt)
{
  printf("[TCP] RECV packet: %s\n", pkt.to_string().c_str());
}

void outgoing_packets(net::Inet4& network,
                      net::Packet_ptr pkt)
{
  auto* eth = (net::ethernet::Header*) pkt->layer_begin();
  printf("*** Packet recv type=%hx (len=%u)\n",
          eth->type(), pkt->size());

  switch(eth->type()) {
  case net::Ethertype::IP4:
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
