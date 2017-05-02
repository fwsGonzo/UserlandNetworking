#include <net/inet4.hpp>
#include <kernel/timers.hpp>
#include <unistd.h>
#include "usernet.hpp"
#include "http.hpp"

static void packet_sent(net::Packet_ptr);

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

  while (__AFL_LOOP(1000)) {
    int n = 0;
    if (ioctl(stdin, I_NREAD, &n) == 0) {
      if (n > 0) {
        
      } else {
        printf("empty buffer?\n");
      }
    }
    Timers::timers_handler();
    pause();
  }
}

// send packet to Linux
void packet_sent(net::Packet_ptr packet)
{
  //printf("write %d\n", packet->size());
  write(stdout, packet->layer_begin(), packet->size());
}
