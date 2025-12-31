#include "keyboard_input.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

namespace scradle {

void setNonBlockingInput(bool enable) {
    static struct termios oldt, newt;
    static int oldf;

    if (enable) {
        // Get current terminal settings
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        // Disable canonical mode and echo
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // Set stdin to non-blocking
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    } else {
        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
    }
}

char checkKeyPress() {
    char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) > 0) {
        return ch;
    }
    return 0;
}

}  // namespace scradle
