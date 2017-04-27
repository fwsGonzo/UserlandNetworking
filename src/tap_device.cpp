#include <net/inet4.hpp>
#include <kernel/timers.hpp>
#include "tap_driver.hpp"
#include "usernet.hpp"
TAP_driver tap0;

static void packet_sent(net::Packet_ptr);
static void post_stats(int);
static void http_server(uint16_t port);

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
      conn->on_read(4096,
      [conn] (auto buf, size_t len)
      {
        //printf("READ: %.*s\n", (int) len, (const char*) buf.get());
        conn->write(buf, len);
      });
    });

  http_server(80);

  using namespace std::chrono;
  Timers::periodic(1s, 5s, post_stats);

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

void post_stats(int)
{
  printf("Active timers: %zu\n", Timers::active());
  printf("%s\n", test_system.network->tcp().to_string().c_str());
}

#include <http>
void http_server(uint16_t port)
{
  using namespace http;
  static std::unique_ptr<Server> server = std::make_unique<Server>(test_system.network->tcp());

  server->on_request(
    [] (Request_ptr req, auto writer)
    {
      //printf("Received request:\n%s\n", req->to_string().c_str());
      (void) req;
      // set content type
      writer->header().set_field(header::Content_Type, "image/jpeg");
      // write body
      static const int MB = 160*1024*1024;
      auto buf = std::unique_ptr<uint8_t[]> (new uint8_t[MB]);
      writer->write(std::move(buf), MB);
    });

  server->listen(port);
}
