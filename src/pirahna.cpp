#include <pirahna>
#include <util/config.hpp>

const Config& Config::get() noexcept {
  static std::string config =
R"V0G0N(
{
  "net" : [
    ["10.0.0.40", "255.255.255.0", "10.0.0.1"],
    ["10.0.0.42", "255.255.255.0", "10.0.0.1"],
    ["10.0.0.43", "255.255.255.0", "10.0.0.1"]
  ],

  "uplink": {
    "iface": 0,
    "url": "10.0.0.1:9090",
    "token": "kappa123",
    "reboot": true
  },

  "piranha" : {
    "created" : "2017-04-07T13:37:00Z",
    "algo" : "round_robin",
    "client_limit" : 1000,
    "mgmt" : {
      "iface" : 0,
      "port" : 6667
    },
    "out" : {
      "iface" : 0,
      "port" : 80
    },
    "in" : {
      "iface" : 0,
      "port" : 8888
    },
    "nodes" : [
      "10.0.0.1"
    ]
  }
}
)V0G0N";
  static Config conf(config.data(), config.data() + config.size());
  return conf;
}

void pirahna_start(net::Inet4& stack)
{
  auto conf = lb::Config::load();
  static lb::Commander commander(conf, stack, stack, stack);
}
