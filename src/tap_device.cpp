#include <net/inet4>
#include <timers>
#include "drivers/tap_driver.hpp"
#include "drivers/usernet.hpp"

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

/*
  using namespace std::chrono;
  Timers::periodic(1s, 5s,
    [&network] (int) {
      printf("Active timers: %zu\n", Timers::active());
      printf("%s\n", network.tcp().to_string().c_str());
    });
*/

  // load balancers
  extern void pirahna_start(net::Inet4&);
  //pirahna_start(network);
  extern void balancer_start(net::Inet4&);
  balancer_start(network);

  extern void acorn_start(net::Inet4&);
  //acorn_start(network);

  extern void common_services(net::Inet4&);
  //common_services(network);

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
