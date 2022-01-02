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


int main(int argc, char** argv){
    if(argc!=2){
        printf("Error in arguments\n");
        exit(1);
    }
    int nbytes;
    struct sockaddr_in client_addr;
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int sock_fd=Socket_creation();
    ball_position_t x;
    message m;
    m.type = 1; /* Set message type to "connect"*/
    Send_Reply(sock_fd, &m, &server_addr); /* Send connect message */

    paddle_position_t paddle;
    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    message_win = newwin(5, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    new_paddle(&paddle, PADLE_SIZE);
    draw_paddle(my_win, &paddle, true);


    int key;
    while (1){
        Receive_message(sock_fd, &m, &server_addr);
        switch (m.type){
            case 3:
                key = -1;
                m.type = 4;
                while(key != 113 && key != 114){
                    key = wgetch(my_win);
                    make_play(key, my_win, &paddle, &m.ball); 
                    mvwprintw(message_win, 1,1,"%c key pressed", key);
                    wrefresh(message_win);
                    Send_Reply(sock_fd, &m, &server_addr);
                }

                /* Check which key was pressed to stop playing*/
                if (key == 113){
                    m.type = 5; /* Change message type to "disconnect"*/
                    Send_Reply(sock_fd, &m, &server_addr); /* Send "disconnect" message*/
                    break;
                }
                else if (key == 114){
                    m.type = 2; /* Change message type to "release ball"*/
                    Send_Reply(sock_fd, &m, &server_addr); /* Send "release ball" message*/
                }
                break;
            
            case 4:
                /* Update the ball position on the screen (without paddle)*/
                update_ball_on_screen(my_win, &m.ball);
                break;

            default:
                perror("invalid message type");
                exit(-1);
                break;
        }
    }
    close(sock_fd);
    return 1;
}