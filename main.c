#include "common.h"

int main(void) {
    srand(time(0));

    int returnStatus = startupWindow();
    switch (returnStatus)
    {
    case 0: // Host
        // Discover
        char* pass_phrase = generate_pass_phrase();
        run_listener();
        // Start TUI
        startTUI(pass_phrase);
        bufferedWriting();
        stopTUI();

        break;
    case 1: // Join
        // Input Keyword
        // Connect
        // Start TUI
        break;

    default: // Exit
        exit(EXIT_SUCCESS);
        break;
    }

    return EXIT_SUCCESS;
}