#ifndef NETWORK_H_
#define NETWORK_H_

#include "Cello.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define LINEBUFSIZE 16384
#define CMDBUFSIZE 512

struct Socket {
    int sin, sout, serr;
};
extern var Socket;

var read_message();
void send_message(var);
void create_server(struct Int*, struct Function*);
var replace_socket(struct Socket*);

#endif // NETWORK_H_
