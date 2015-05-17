#include <stdio.h>
#include <string.h>
#include <libserialport.h>

#define DEBUG
#define TRACE

int main(int argc, char* argv) {
  struct sp_port **portlist;
  struct sp_port *port;
  enum sp_return ret;
  int index;

  ret = sp_list_ports(&portlist);

  for (index = 0; portlist[index] != NULL; index++) {
    if(strcmp(sp_get_port_name(portlist[index]), "/dev/ttyUSB0") == 0) {
      sp_copy_port(portlist[index], &port);
      break;
    }
  }
  if (port != NULL) {
    struct sp_port_config *config;

    printf("%s\n", sp_get_port_name(port));
    ret = sp_new_config(&config);
    printf("%p\n", port);
    printf("%p\n", config);
    printf("%i\n", ret);
    ret = sp_get_config(port, config);
    printf("%i\n", ret);

    ret = sp_open(port, SP_MODE_READ);
    printf("%i\n", ret);
  }
}
