#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios origin_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &origin_termios);
    atexit(disableRawMode);

    struct termios raw = origin_termios;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {

    enableRawMode();

    char c;

    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        if (iscntrl(c)) {
            printf("%d\n", c);
        }else{
            printf("%d ( %c )\n", c, c);
        }
    }

    return 0;
}
