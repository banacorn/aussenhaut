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
// var read_message(struct Socket*);
void send_message(var);
// void send_message(struct Socket*, var);

// String_ * read_message(int );
// void send_message(int, String_ *);
void create_server(struct Int*, struct Function*);

#endif // NETWORK_H_
