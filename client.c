#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Serverfunc.h"
#include "sock_dg_inet.h"
#include "single-pong.h"

int main(int argc, char* argv){
    int nbytes;
    message m;
    m.type = 1;
    m.ball = NULL;
    struct sockaddr_in client_addr;
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int sock_fd=Socket_creation();

    Send_Reply(sock_fd, &m, &server_addr);
    
}