#include <net/inet4.hpp>
#include <kernel/timers.hpp>
#include "drivers/usernet.hpp"
#include "http.hpp"
#include <unistd.h>
#include <iostream>

extern void fuzz_ip4(net::Inet4&, const void*, uint16_t len);
static void packet_sent(net::Packet_ptr packet);

// run network stack against Linux Tap device (virtual interface)
void stdin_device(net::Inet4& network)
{
  // set outgoing packet function on UserNet driver
  auto& driver = (UserNet&) network.nic();
  driver.set_transmit_forward(packet_sent);

  // bind & listen on TCP ECHO port 7
  auto& listener = network.tcp().listen(7);
  listener.on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
      conn->on_read(4096,
      [conn] (auto buf, size_t len)
      {
        //printf("READ: %.*s\n", (int) len, (const char*) buf.get());
        conn->write(buf, len);
      });
    });

  http_server(network, 80);

  using namespace std::chrono;
  Timers::periodic(1s, 5s,
    [&network] (int) {
      printf("Active timers: %zu\n", Timers::active());
      printf("%s\n", network.tcp().to_string().c_str());
    });

  while (!std::cin.eof())
  {
    // read data from stdin
    std::string stdin;
    std::cin >> stdin;
    // pass data to IP4 fuzzer
    fuzz_ip4(network, stdin.data(), stdin.size());
    // pass data to network
    //driver.write(stdin.data(), stdin.size());

    // handle timers n shit
    Timers::timers_handler();
    //pause();
  };
}

// send packet to Linux
void packet_sent(net::Packet_ptr packet)
{
  printf("write %d\n", packet->size());
  write(0, packet->layer_begin(), packet->size());
}
