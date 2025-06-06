#include "Game.h"
#include "Utils.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string filename = "questions.txt";
    if (argc > 1) {
        filename = argv[1];
        std::cout << BRIGHT_CYAN "Using question file: " << filename << "\n" RESET_COLOR;
    } else {
        std::cout << BRIGHT_YELLOW "No question file specified. Using default: " << filename << "\n" RESET_COLOR;
    }

    Game game(filename);
    game.run();
    return 0;
}
