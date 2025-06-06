#include "Utils.h"
#include <iostream>
#include <cctype>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void consumeNewline() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidatedIntegerInput(const std::string& prompt, int min, int max) {
    int choice;
    while (true) {
        std::cout << prompt;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cout << BRIGHT_RED "Invalid input! Please enter a number.\n" RESET_COLOR;
            std::cin.clear();
            consumeNewline();
        } else if (choice >= min && choice <= max) {
            consumeNewline();
            return choice;
        } else {
            std::cout << BRIGHT_RED "Invalid choice! Please enter a number between " << min << " and " << max << ".\n" RESET_COLOR;
            consumeNewline();
        }
    }
}

char getValidatedCharAnswer(const std::string& prompt, bool fiftyUsed) {
    std::string buffer;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, buffer);
        if (buffer.length() == 1) {
            char inputChar = static_cast<char>(std::toupper(buffer[0]));
            if ((inputChar >= 'A' && inputChar <= 'D') || (inputChar == 'E' && !fiftyUsed)) {
                return inputChar;
            }
        }
        std::cout << BRIGHT_RED "Invalid input! Please enter A, B, C, D" << (fiftyUsed ? "" : ", or E") << ".\n" RESET_COLOR;
    }
}
