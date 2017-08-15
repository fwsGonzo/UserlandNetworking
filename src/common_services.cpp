#include <net/inet4>
#include "http.hpp"

void common_services(net::Inet4& network)
{
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
}
