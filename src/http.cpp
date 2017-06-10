#include "http.hpp"
#include <http>

void http_server(net::Inet4& network, uint16_t port)
{
  using namespace http;
  static std::unique_ptr<Server> server = std::make_unique<Server>(network.tcp());

  server->on_request(
    [] (Request_ptr req, auto writer)
    {
      //printf("Received request:\n%s\n", req->to_string().c_str());
      (void) req;
      // set content type
      writer->header().set_field(header::Content_Type, "image/jpeg");

      // write body
      static const int MB = 160*1024*1024;
      auto buf = std::shared_ptr<uint8_t> (new uint8_t[MB], std::default_delete<uint8_t[]> ());
      writer->write(std::move(buf), MB);
    });

  server->listen(port);
}
