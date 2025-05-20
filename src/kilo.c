#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)
struct editorConfig {
    int screenRows;
    int screenCols;
    struct termios origin_termios;
};

struct editorConfig Conf;

void die(const char *s) {
    write(STDERR_FILENO, "\x1b[2J", 4);
    write(STDERR_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &Conf.origin_termios) == -1)
    die("tcsetattr error");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &Conf.origin_termios) == -1)
        die("tcsetattr error");
    atexit(disableRawMode);

    struct termios raw = Conf.origin_termios;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr error");
}

char editorReadKey() { 
    int nread; 
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    return c;
}

int getWindowSize(int *row, int *col){
    struct winsize ws;

    if(ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }else {
        *col = ws.ws_col;
        *row = ws.ws_row;
        return 0;
    }
}

void editorDrawRows() {
    int y;
    for(y = 0; y < Conf.screenRows; y++) {
        write(STDERR_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen() {
    write(STDERR_FILENO, "\x1b[2J", 4);
    write(STDERR_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDERR_FILENO, "\x1b[H", 3);
}

void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDERR_FILENO, "\x1b[2J", 4);
            write(STDERR_FILENO, "\x1b[H", 3);
            exit(0);    
        break;
    }
}

void initEditor() {
    if(getWindowSize(&Conf.screenRows, &Conf.screenCols) == -1) die("getWindowSize");
}

int main(int argc, char **args) {

    enableRawMode();
    initEditor();


    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
