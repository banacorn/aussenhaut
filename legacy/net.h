#ifndef NET_H_
#define NET_H_

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "parser.h"

#define LINEBUFSIZE 16384
#define CMDBUFSIZE 512

String_ * read_message(int );
void send_message(int, String_ *);
void create_server(int , void (*)(int));

#endif // PARSER_H_
