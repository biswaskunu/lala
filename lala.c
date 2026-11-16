#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f)

// data
struct termios original_termios;

// error handeling
void die(const char *s) {
    perror(s);
    exit(1);
}

// terminal
void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
        die("tcsetattr error");
}
void enableRawMode(){
    if(tcgetattr(STDIN_FILENO, &original_termios) == -1)
        die("tcgetattr error");
    atexit(disableRawMode);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr error");
}
char editorReadKey(){
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

// input
void editorProcessKeyPress(){
    char c = editorReadKey();
    if (c == CTRL_KEY('q')) exit(0);
}

// main logic
int main(){
    enableRawMode();

    while (1) {
        editorProcessKeyPress();
    }
    return 0;
}