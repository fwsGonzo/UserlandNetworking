#include <net/inet4>
#include <deque>
#include <net/http/websocket.hpp>
#include <net/http/ws_connector.hpp>
#include <http>

static std::deque<http::WebSocket_ptr> websockets;

static http::WebSocket_ptr& new_client(http::WebSocket_ptr socket)
{
  for (auto& client : websockets)
  if (client->is_alive() == false) {
    return client = std::move(socket);
  }

  websockets.push_back(std::move(socket));
  return websockets.back();
}

bool accept_client(net::Socket remote, std::string origin)
{
  /*
  printf("Verifying origin: \"%s\"\n"
         "Verifying remote: \"%s\"\n",
         origin.c_str(), remote.to_string().c_str());
  */
  (void) origin;
  return remote.address() == net::ip4::Addr(10,0,0,1);
}


void websocket_server(net::Inet<net::IP4>& inet, uint16_t port)
{
  // buffer used for testing
  static net::tcp::buffer_t BUFFER;
  static const int          BUFLEN = 1000;
  BUFFER = decltype(BUFFER)(new uint8_t[BUFLEN]);

  using namespace http;
  // Set up a HTTP server
  static Server httpd(inet.tcp());

  // Set up server connector
  static WS_server_connector ws_serve(
    [] (WebSocket_ptr ws)
    {
      if (ws == nullptr) return;
      auto& socket = new_client(std::move(ws));
      // if we are still connected, attempt was verified and the handshake was accepted
      if (socket->is_alive())
      {
        socket->on_read =
        [] (const char* data, size_t len) {
          (void) data;
          (void) len;
          printf("WebSocket on_read: %.*s\n", (int) len, data);
        };

        //socket->write("THIS IS A TEST CAN YOU HEAR THIS?");
        for (int i = 0; i < 1000; i++)
            socket->write(BUFFER, BUFLEN, http::WebSocket::BINARY);

        //socket->close();
      }
    },
    accept_client);
  httpd.on_request(ws_serve);
  httpd.listen(port);
  /// server ///
}
