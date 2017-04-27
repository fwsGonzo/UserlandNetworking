#pragma once
#include <delegate>

struct TAP_driver
{
  typedef delegate<void(void*, int)> on_read_func;
  void on_read(on_read_func func) { o_read = func; }
  void wait();

  int read (char *buf, int len);
  int write(const void* buf, int len);

  TAP_driver();
  ~TAP_driver();

private:
  int set_if_route(char *dev, const char* cidr = "10.0.0.0/24");
  int set_if_address(char *dev, const char* ip = "10.0.0.1");
  int set_if_up(char *dev);
  int alloc_tun(char *dev);

  on_read_func o_read;
  int   tun_fd;
  char* dev;
};
