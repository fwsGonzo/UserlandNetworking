#include <net/inet4.hpp>
#include "test_system.hpp"
#include "http.hpp"

static void outgoing_packets(net::Inet4&, net::Packet_ptr);

void random_fuzzing(net::Inet4& network)
{
  test_system.init(network, outgoing_packets);
  network.negotiate_dhcp();

  http_server(network, 80);

  using namespace std::chrono;
  Timers::periodic(1ms, 5ms,
    [&network] (int) {
      tcp_send_packet(network,
        [] (net::tcp::Packet& packet) {

          auto* start = packet.layer_begin();
          auto* end   = packet.data_end();

          for (auto* ptr = start; ptr < end; ptr++) {
            *ptr = rand() & 0xff;
          }
        });
    });
}

void outgoing_packets(net::Inet4& netw,
                      net::Packet_ptr pkt)
{
}
