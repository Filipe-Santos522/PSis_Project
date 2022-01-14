/* Server source code file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 * 
 * Note: Some functions given by the professors were used.
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Serverfunc.h"
#include "sock_dg_inet.h"
#define MAX_NUMBER_OF_PLAYERS 10

int main(int argc, char* argv[]){
    message m;
    struct sockaddr_in client_addr;
    struct sockaddr_in *Players= malloc(MAX_NUMBER_OF_PLAYERS* sizeof(client_addr)); // List of player addresses
    int Num_players=0; // Number of players
    int Curr_Player=0; // Player that currently has the ball
    int sock_fd=Socket_creation(); // Create socket
    Socket_identification(sock_fd); // Bind socket

    //Listening to messages
    while (1){
        Receive_message(sock_fd, &m, &client_addr); // Receive message from client
        printf("received message\n");

        switch (m.type)
        {
        case 1: // If connect message:
            /* add client to list */
            Players[Num_players]=client_addr;
            Num_players++;
            if(Num_players==1){ // If first player connecting
                m.type=3;
                place_ball_random(&m.ball);
                Send_Reply(sock_fd, &m, &Players[0]);
                printf("sent message");
                Curr_Player=0;
            }else{ // If not first player, send only the ball
                m.type=6;
                Send_Reply(sock_fd, &m, &Players[Num_players-1]);
            }

            break;
        
        case 2:
            /* release ball - send ball to random client */
            m.type=3; // Change message type
            if(Num_players==1){
                Send_Reply(sock_fd, &m, &Players[0]);
            }else if(Num_players>1){ // Randomize player to send ball if there is more than one
                int i= rand()%(Num_players-1);
                if(i>=Curr_Player){
                    i++;
                }
                Curr_Player=i;
                Send_Reply(sock_fd, &m, &Players[i]); // Send ball to randomized player
            }else{
                printf("error\n");
                exit(1);
            }
            break;
        case 4:
            /* move ball - update ball on screen */
            m.type=4; // Change message tpye
            for(int j=0; j<Num_players; j++){ // Send ball to all clients
                if(j!=Curr_Player)
                    Send_Reply(sock_fd, &m, &Players[j]);
            }
            break;
        case 5:
            /* disconnect - remove user from list */
            Players[Curr_Player]=Players[Num_players-1];
            Num_players--;
            m.type=3;
            if(Num_players==1){
                Send_Reply(sock_fd, &m, &Players[0]);
            }else if(Num_players>1){
                int i= rand()%(Num_players-1);
                Curr_Player=i;
                Send_Reply(sock_fd, &m, &Players[i]);
            }
            break;
        default:
            perror("invalid message type");
            exit(-1);
            break;
        }
    }

    free(Players);
}