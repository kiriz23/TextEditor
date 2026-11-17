/***************************************************************************//**
    @file       kilo.c
    @author     Kyrylo Krocha

*******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;


void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}


void enableRawMode(){
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST); // No post processing of the output
    raw.c_cflag |= (CS8);    
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); //Flipping the bits in lflag to turn off parameters

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main(){
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c,1)==1 && c!='q'){
        if(iscntrl(c)){
            printf("%d/n", c);
        }else{
            printf("%d ('%c')\r\n", c, c);
        }    
    }

    return 0;
}

