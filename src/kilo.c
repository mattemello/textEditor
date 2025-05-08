#include <termios.h>
#include <unistd.h>

void enableRawMode() {
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO);
    
}

int main() {
    return 0;
}
