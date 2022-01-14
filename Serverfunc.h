/* Communication functions header file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "single-pong.h"


// Headers of functions declared in the source code file
int Socket_creation();
void Socket_identification(int);
void Send_Reply(int, struct message *, struct sockaddr_in*);
void Receive_message(int, struct message *, struct sockaddr_in*);