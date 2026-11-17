/***************************************************************************//**
    @file       kilo.c
    @author     Kyrylo Krocha

*******************************************************************************/

/*** Includes ***/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** Data ***/

struct termios orig_termios;


/*** Terminal ***/

void die(const char *s){
    perror(s);
    exit(1);
}


void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}


void enableRawMode(){
    struct termios raw;

    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST); // No post processing of the output
    raw.c_cflag |= (CS8);    
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); //Flipping the bits in lflag to turn off parameters
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}


/*** Init ***/

int main(){
    enableRawMode();

    while (1){
        char c='\0';
        if(read(STDIN_FILENO, &c,1) == -1) die("read");
        if(iscntrl(c)){
            printf("%d/n", c);
        }else{
            printf("%d ('%c')\r\n", c, c);
        }
        if(c=='q') break;    
    }

    return 0;
}

