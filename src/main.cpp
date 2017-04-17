#include <net/inet4.hpp>
#include "usernet.hpp"
static char statman_data[8192];

int main(void)
{
  // needed for stats
  Statman::init((uintptr_t) statman_data, sizeof(statman_data));

  // the network driver
  UserNet    network_driver;
  // the network stack
  net::Inet4 network((hw::Nic&) network_driver);
  network.network_config(
    { 10,  0,  0,  2},
    {255,255,255,  0},
    { 10,  0,  0,  1}, // GW
    {  8,  8,  8,  8}  // DNS
  );

  printf("Hello world!\n");
}
