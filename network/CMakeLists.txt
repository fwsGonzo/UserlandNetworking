
set(NET_SOURCES
  IncludeOS/src/net/buffer_store.cpp
  IncludeOS/src/net/super_stack.cpp
  IncludeOS/src/net/inet4.cpp
  IncludeOS/src/net/inet_common.cpp

  IncludeOS/src/net/ethernet/ethernet.cpp
  IncludeOS/src/net/ip4/arp.cpp
  IncludeOS/src/net/ip4/ip4.cpp

  IncludeOS/src/net/tcp/tcp.cpp
  IncludeOS/src/net/tcp/connection.cpp
  IncludeOS/src/net/tcp/connection_states.cpp
  IncludeOS/src/net/tcp/write_queue.cpp
  IncludeOS/src/net/tcp/read_buffer.cpp
  IncludeOS/src/net/tcp/rttm.cpp
  IncludeOS/src/net/tcp/listener.cpp
  IncludeOS/src/net/ip4/icmp4.cpp
  IncludeOS/src/net/ip4/udp.cpp
  IncludeOS/src/net/ip4/udp_socket.cpp

  IncludeOS/src/net/dns/dns.cpp
  IncludeOS/src/net/dns/client.cpp
  IncludeOS/src/net/dhcp/dh4client.cpp
  IncludeOS/src/net/dhcp/dhcpd.cpp

  IncludeOS/src/net/http/header.cpp
  IncludeOS/src/net/http/header_fields.cpp
  IncludeOS/src/net/http/message.cpp
  IncludeOS/src/net/http/request.cpp
  IncludeOS/src/net/http/response.cpp
  IncludeOS/src/net/http/status_codes.cpp
  IncludeOS/src/net/http/time.cpp
  IncludeOS/src/net/http/version.cpp
  IncludeOS/src/net/http/mime_types.cpp
  IncludeOS/src/net/http/cookie.cpp
  IncludeOS/src/net/http/client_connection.cpp
  IncludeOS/src/net/http/client.cpp
  IncludeOS/src/net/http/server_connection.cpp
  IncludeOS/src/net/http/server.cpp
  IncludeOS/src/net/http/response_writer.cpp
  #IncludeOS/src/net/http/secure_server.cpp
  IncludeOS/src/net/ws/websocket.cpp
)

set(OS_SOURCES
    IncludeOS/src/fs/dirent.cpp
    IncludeOS/src/fs/disk.cpp
    IncludeOS/src/fs/fat.cpp
    IncludeOS/src/fs/fat_sync.cpp
    IncludeOS/src/fs/fat_async.cpp
    IncludeOS/src/fs/filesystem.cpp
    IncludeOS/src/fs/mbr.cpp
    IncludeOS/src/fs/path.cpp
    IncludeOS/src/kernel/timers.cpp
    IncludeOS/src/util/async.cpp
    IncludeOS/src/util/logger.cpp
    IncludeOS/src/util/sha1.cpp
    IncludeOS/src/util/statman.cpp
    IncludeOS/src/util/path_to_regex.cpp
    IncludeOS/src/util/percent_encoding.cpp
    IncludeOS/src/util/uri.cpp
  )

set(MOD_SOURCES
    IncludeOS/lib/mana/src/server.cpp
    IncludeOS/lib/mana/src/request.cpp
    IncludeOS/lib/mana/src/response.cpp
    IncludeOS/lib/mana/src/attributes/cookie_jar.cpp
    IncludeOS/lib/mana/src/middleware/butler.cpp
    IncludeOS/lib/mana/src/middleware/cookie_parser.cpp
    IncludeOS/lib/mana/src/middleware/director.cpp
    IncludeOS/lib/mana/src/middleware/parsley.cpp
    IncludeOS/lib/mana/src/components/dashboard/dashboard.cpp
  )

add_library(network STATIC ${NET_SOURCES} ${OS_SOURCES} ${MOD_SOURCES})
target_include_directories(network PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/IncludeOS/lib/mana/include
    ${CMAKE_CURRENT_SOURCE_DIR}/IncludeOS/mod/rapidjson/include)

add_library(http_parser STATIC "IncludeOS/mod/http-parser/http_parser.c")
set_target_properties(http_parser PROPERTIES LINKER_LANGUAGE C)
