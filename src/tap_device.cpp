#include <net/inet4.hpp>
#include <kernel/timers.hpp>
#include "tap_driver.hpp"
#include "usernet.hpp"
TAP_driver tap0;

void packet_sent(net::Packet_ptr);
static struct TestSystem
{
  net::Inet4* network;
  UserNet*    driver;

  void init(net::Inet4& netw) {
    network = &netw;
    driver  = &(UserNet&) network->nic();
    driver->set_transmit_forward(packet_sent);
  }
} test_system;

// run network stack against Linux Tap device (virtual interface)
void tap_device(net::Inet4& network)
{
  test_system.init(network);
  tap0.on_read({test_system.driver, &UserNet::write});

  // bind & listen on TCP ECHO port 7
  auto& listener = network.tcp().listen(7);
  listener.on_connect(
    [] (auto conn) {
      printf("New connection received: %s\n", conn->to_string().c_str());
      conn->on_read(1500,
      [conn] (auto buf, size_t len)
      {
        //printf("READ: %.*s\n", (int) len, (const char*) buf.get());
        conn->write(buf, len);
      });
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
  tap0.write(packet->layer_begin(), packet->size());
}
