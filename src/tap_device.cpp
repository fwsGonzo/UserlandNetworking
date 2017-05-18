#include <net/inet4.hpp>
#include <kernel/timers.hpp>
#include "drivers/tap_driver.hpp"
#include "drivers/usernet.hpp"
#include "http.hpp"

static void packet_sent(net::Packet_ptr);
static TAP_driver* current_tap_device = nullptr;

// run network stack against Linux Tap device (virtual interface)
void tap_device(net::Inet4& network)
{
  // set outgoing packet function on UserNet driver
  auto& driver = (UserNet&) network.nic();
  driver.set_transmit_forward(packet_sent);
  // create TAP device and hook up packet receive to UserNet driver
  TAP_driver tap0;
  tap0.on_read({driver, &UserNet::write});
  current_tap_device = &tap0;

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

  network.tcp().listen(100)
  .on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
      conn->on_read(4096,
      [conn] (auto, size_t)
      {
        //printf("READ: %.*s\n", (int) len, (const char*) buf.get());
        const size_t SIZE = 1;
        auto buf = net::tcp::buffer_t (new uint8_t[SIZE], std::default_delete<uint8_t[]> ());
        conn->write(buf, SIZE);
        conn->close();
      });
    });

  network.tcp().listen(101)
  .on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
    });

  http_server(network, 80);

  extern void websocket_server(net::Inet<net::IP4>& inet, uint16_t port);
  websocket_server(network, 8000);

  using namespace std::chrono;
  Timers::periodic(1s, 5s,
    [&network] (int) {
      printf("Active timers: %zu\n", Timers::active());
      printf("%s\n", network.tcp().to_string().c_str());
    });

  while (true) {
    Timers::timers_handler();
    tap0.wait();
  }
}

// send packet to Linux
void packet_sent(net::Packet_ptr packet)
{
  //printf("write %d\n", packet->size());
  current_tap_device->write(packet->layer_begin(), packet->size());
}
