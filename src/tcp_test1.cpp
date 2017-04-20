#include <net/inet4.hpp>
#include "network.hpp" // generate_packet
#include "usernet.hpp"

static struct TestSystem
{
  void outgoing_packets(net::Packet_ptr);
  void outgoing_arp_packet(net::Packet_ptr);
  void outgoing_tcp_packet(net::tcp::Packet&);

  net::Inet4* network;
  UserNet*    driver;
  int         stacklevel = 0;

  void init(net::Inet4& netw) {
    network = &netw;
    driver  = &(UserNet&) network->nic();
    driver->set_transmit_forward({this, &TestSystem::outgoing_packets});
  }
} test_system;

void tcp_test1(net::Inet4& network)
{
  test_system.init(network);

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
    tcp.set_ip_src({10, 0, 0, 1});
    tcp.set_dst_port(100);
    tcp.set_ip_dst(addr);
    tcp.fill((uint8_t*) "HELLO WORLD!", 13);
  });
}

void TestSystem::outgoing_tcp_packet(net::tcp::Packet& pkt)
{
  printf("[TCP] packet: %s\n", pkt.to_string().c_str());
  if (pkt.isset(net::tcp::ACK)) {
    tcp_send_packet(*network,
    [this, &src = pkt] (auto& tcp) {
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

#include <net/ip4/packet_arp.hpp>

void TestSystem::outgoing_arp_packet(net::Packet_ptr packet) {
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
  driver->feed(std::move(packet));
}

void TestSystem::outgoing_packets(net::Packet_ptr pkt)
{
  stacklevel++;
  auto* eth = (net::ethernet::Header*) pkt->layer_begin();
  printf("[%d] Packet recv type=%hx (len=%u)\n",
          stacklevel, eth->type(), pkt->size());

  switch(eth->type()) {
  case net::Ethertype::IP4:
      assert(eth->dest() == TEST_MAC_ADDRESS);
      pkt->increment_layer_begin(sizeof(net::ethernet::Header));
      // convert to TCP (assume they're all TCP in this test)
      outgoing_tcp_packet((net::tcp::Packet&) *pkt);
      break;
  case net::Ethertype::ARP:
      pkt->increment_layer_begin(sizeof(net::ethernet::Header));
      outgoing_arp_packet(std::move(pkt));
      break;
  default:
      break;
  }
  stacklevel--;
}
