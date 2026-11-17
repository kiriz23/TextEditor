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
#include <errno.h>

/*** Defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** Data ***/

struct termios orig_termios;




/*** Terminal ***/

void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

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


char editorReadKey(){
    int nread;
    char c;

    while((nread=read(STDIN_FILENO, &c,1))!=1){
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}


/*** Input ***/

void editorProcessKeypress(){
    char c = editorReadKey();

    switch (c)
    {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }
}


/*** Output ***/

void editorDrawRows(){
    int y;
    for(y=0; y<=24; y++){
        write(STDOUT_FILENO,"~\r\n",3);
    }
}


void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  editorDrawRows();
  
  write(STDOUT_FILENO, "\x1b[H", 3);
}


/*** Init ***/

int main(){
    enableRawMode();

    while (1){
        editorRefreshScreen();
        editorProcessKeypress();  
    }

    return 0;
}

