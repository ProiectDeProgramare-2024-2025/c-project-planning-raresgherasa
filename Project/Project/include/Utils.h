#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <limits>

#define RESET_COLOR "\x1b[0m"
#define BRIGHT_BLACK "\x1b[90m"
#define BRIGHT_RED "\x1b[91m"
#define BRIGHT_GREEN "\x1b[92m"
#define BRIGHT_YELLOW "\x1b[93m"
#define BRIGHT_BLUE "\x1b[94m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_CYAN "\x1b[96m"
#define BRIGHT_WHITE "\x1b[97m"

void clearScreen();
void consumeNewline();
int getValidatedIntegerInput(const std::string& prompt, int min, int max);
char getValidatedCharAnswer(const std::string& prompt, bool fiftyUsed);

#endif
