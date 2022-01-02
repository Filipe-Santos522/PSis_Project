#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Serverfunc.h"
#include "single-pong.h"
#include "sock_dg_inet.h"

int main(int argc, char* argv[]){
    int nbytes;
    message ball;
    struct sockaddr_in client_addr;

    int sock_fd=Socket_creation();
    Socket_identification(sock_fd);

    //RECEIVE
    nbytes = Receive_message(sock_fd, &ball, sizeof(ball), 0, ( struct sockaddr *)&client_addr);

    //SEND
    Send_Reply(sock_fd, &ball, &client_addr);
    //LIST OF PLAYERS

}