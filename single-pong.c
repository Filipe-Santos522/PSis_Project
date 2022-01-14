/* Pong functions source code file:
 * Original code given by the professors and adapted by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 */

#include <stdlib.h>
#include <ncurses.h>


#include "pong.h"
#include "single-pong.h"


// Function that creates a new paddle
void new_paddle (paddle_position_t * paddle, int legth){
    paddle->x = WINDOW_SIZE/2;
    paddle->y = WINDOW_SIZE-2;
    paddle->length = legth;
}

// Function that draws a paddle on the screen
void draw_paddle(WINDOW *win, paddle_position_t * paddle, int delete){
    int ch;
    if(delete){
        ch = '_';
    }else{
        ch = ' ';
    }
    int start_x = paddle->x - paddle->length;
    int end_x = paddle->x + paddle->length;
    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle->y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}

// Function that moves the paddle and handles collisions with the ball
void moove_paddle (paddle_position_t * paddle, int direction, ball_position_t *ball){
    int paddle_aux;

    if (direction == KEY_UP){
        paddle_aux=paddle->y-1;

        if (paddle->y  != 1 ){
            if (ball->y == paddle->y - 1 && ball->y != 1 && (ball->x - paddle->x < 3 && ball->x - paddle->x > -3)){
                ball->y--;
                ball->up_hor_down = -1;
                paddle->y--;
            }
            else if (paddle->y - 1 != ball->y || ball->x - paddle->x >= 3 || ball->x - paddle->x <= -3)
                paddle->y--;
        }
    }


    if (direction == KEY_DOWN){
        paddle_aux=paddle->y+1;

        if (paddle->y  != WINDOW_SIZE - 2){
            if (ball->y == paddle->y + 1 && ball->y != WINDOW_SIZE - 2 && (ball->x - paddle->x < 3 && ball->x - paddle->x > -3)){
                ball->y++;
                ball->up_hor_down = 1;
                paddle->y++;
            }
            else if (paddle->y + 1 != ball->y || ball->x - paddle->x >= 3 || ball->x - paddle->x <= -3)
                paddle->y++;
        }
        
    }
    
    if (direction == KEY_LEFT){
        paddle_aux=paddle->x-1;

        if (paddle->x  != 3){
            if (ball->y == paddle->y && ball->x != 1 &&  ball->x - paddle->x == -3){
                ball->x--;
                ball->left_ver_right = -1;
                paddle->x--;
            }
            else if (paddle->y != ball->y || ball->x - paddle->x <= -3)
                paddle->x--;
        }
        
    }
    
    if (direction == KEY_RIGHT){
        paddle_aux=paddle->x+1;
            
        if (paddle->x  != WINDOW_SIZE-4){
            if (ball->y == paddle->y && ball->x != WINDOW_SIZE-2 && ball->x - paddle->x == 3){
                ball->x++;
                ball->left_ver_right = 1;
                paddle->x++;
            }
            else if (paddle->y != ball->y || ball->x - paddle->x >= 3)
                paddle->x++;
        }
    }
}

// Function to place the ball in a random position
void place_ball_random(ball_position_t * ball){
    ball->x = rand() % WINDOW_SIZE ;
    ball->y = rand() % WINDOW_SIZE ;
    ball->c = 'o';
    ball->up_hor_down = rand() % 3 -1; //  -1 up, 1 - down
    ball->left_ver_right = rand() % 3 -1 ; // 0 vertical, -1 left, 1 right
}

// Function to move the ball and handle collisions with the paddle
void moove_ball(ball_position_t * ball, paddle_position_t paddle){
    
    int next_x = ball->x + ball->left_ver_right;
    int next_y = ball->y + ball->up_hor_down;

    int flag=0;
    int flag2=0;
    
    if ((paddle.x + 2 >= next_x && paddle.x - 2 <= next_x) && paddle.y == next_y){
        if(ball->up_hor_down==0){
            flag2=1;
        }else{
            flag = 1; 
        }
    }

    if( (next_y == 0 || next_y == WINDOW_SIZE-1 || flag == 1) && (next_x == 0 || next_x == WINDOW_SIZE-1 || flag2 == 1)){
        ball->up_hor_down *= -1;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
        return;
    }

    if( next_x == 0 || next_x == WINDOW_SIZE-1 || flag2 == 1 || (paddle.y==ball->y && (paddle.x + 2 >= next_x && paddle.x - 2 <= next_x))){
        ball->up_hor_down = rand() % 3 -1 ;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"left right win");
        wrefresh(message_win);
     }else{
        ball->x = next_x;
    }
    
    if( next_y == 0 || next_y == WINDOW_SIZE-1 || flag == 1 || (paddle.y==next_y && (paddle.x + 2 >= ball->x && paddle.x - 2 <= ball->x))){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
    }else{
        ball -> y = next_y;
    }
}


// Function to draw the ball on the screen
void draw_ball(WINDOW *win, ball_position_t * ball, int draw){
    int ch;
    if(draw){
        ch = ball->c;
    }else{
        ch = ' ';
    }
    wmove(win, ball->y, ball->x);
    waddch(win,ch);
    wrefresh(win);
}

// Function that updates the screen after a play is made. Used on the client with the ball
void make_play(int key, WINDOW* my_win, paddle_position_t * paddle, ball_position_t * ball){
    if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
        draw_paddle(my_win, paddle, false);
        moove_paddle (paddle, key, ball);
        draw_paddle(my_win, paddle, true);

        draw_ball(my_win, ball, false);
        moove_ball(ball, *paddle);
        draw_ball(my_win, ball, true);
    }
}

// Function that updates the ball for the clients that do not have the ball
/*void update_ball_on_screen(WINDOW* my_win, ball_position_t * ball, paddle_position_t paddle){
    draw_ball(my_win, ball, false);
    moove_ball(ball, paddle);
    draw_ball(my_win, ball, true);
}*/