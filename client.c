/* Client source code file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 * 
 * Note: Some functions given by the professors were used.
 */

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
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int sock_fd=Socket_creation(); // Create socket

    ball_position_t old_ball;
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

    new_paddle(&paddle, PADLE_SIZE); // Create player paddle


    int key;
    // Client loop
    while (1){ 
        Receive_message(sock_fd, &m, &server_addr); // Wait for message from server
        switch (m.type){
            case 3: // If message is send_ball:
                draw_ball(my_win, &m.ball, true); // Draw ball
                draw_paddle(my_win, &paddle, true); // Draw player's paddle
                key = -1;
                m.type = 4; // Change message type to move_ball
                while(key != 113 && key != 114){ // Get key from user
                    key = wgetch(my_win);
                    make_play(key, my_win, &paddle, &m.ball); // Update screen
                    mvwprintw(message_win, 1,1,"%c key pressed", key);
                    wrefresh(message_win);
                    Send_Reply(sock_fd, &m, &server_addr); // Send message to server to update ball position
                }
                old_ball=m.ball; // Save ball position for next iteration, so it can be erased from the screen

                /* Check which key was pressed to stop playing*/
                if (key == 113){ // If "quit"
                    m.type = 5; /* Change message type to "disconnect"*/
                    Send_Reply(sock_fd, &m, &server_addr); /* Send "disconnect" message*/
                    endwin(); // Terminate curses
                    exit(1);  // Terminate program
                    break;
                }
                else if (key == 114){ // If "release"
                    m.type = 2; /* Change message type to "release ball"*/
                    draw_paddle(my_win, &paddle, false);

                    Send_Reply(sock_fd, &m, &server_addr); /* Send "release ball" message*/
                }
                break;
            
            case 4:
                /* Update the ball position on the screen (without paddle)*/
                draw_ball(my_win, &old_ball, false);
                draw_ball(my_win, &m.ball, true);
                old_ball=m.ball;
                break;
            case 6: // Special message to save the first ball position so it can be erased after
                draw_ball(my_win, &m.ball, true);
                old_ball=m.ball;
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