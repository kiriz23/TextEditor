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
#include <sys/ioctl.h>

/*** Defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** Data ***/

struct editorConfig{
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

/*** Terminal ***/

void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}


void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("tcsetattr");
}


void enableRawMode(){
    struct termios raw;

    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    raw = E.orig_termios;
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


int getWindowSize(int *rows, int *cols){ // Can not work on all OS`s, needs to be changed
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) return -1;
    else{
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
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
    for(y=0; y<=E.screenrows; y++){
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

void initEditor(){
    if (getWindowSize(&E.screenrows,&E.screencols) == -1) die("getWindowSize");
}


int main(){
    enableRawMode();
    initEditor();

    while (1){
        editorRefreshScreen();
        editorProcessKeypress();  
    }

    return 0;
}

