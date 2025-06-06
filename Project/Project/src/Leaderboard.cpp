#include "Leaderboard.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

void Leaderboard::addPlayer(const Player& player) {
    if (players.size() < MAX_PLAYERS) {
        players.push_back(player);
        std::cout << BRIGHT_GREEN "Score added to leaderboard!\n" RESET_COLOR;
    } else {
        std::cout << BRIGHT_YELLOW "Leaderboard is full. Score not saved.\n" RESET_COLOR;
    }
}

void Leaderboard::display() const {
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Leaderboard ===\n\n" RESET_COLOR;
    if (players.empty()) {
        std::cout << BRIGHT_CYAN "No scores recorded yet. Play a game to add entries!\n" RESET_COLOR;
    } else {
        std::vector<Player> sortedPlayers = players;
        std::sort(sortedPlayers.begin(), sortedPlayers.end(),
            [](const Player& a, const Player& b) { return a.getScore() > b.getScore(); });

        std::cout << BRIGHT_BLACK "No. " BRIGHT_BLUE "Player Name" BRIGHT_BLACK " \t " BRIGHT_GREEN "Score\n" RESET_COLOR;
        std::cout << BRIGHT_BLACK "-----------------------------------\n" RESET_COLOR;
        for (size_t i = 0; i < sortedPlayers.size(); ++i) {
            std::string displayName = sortedPlayers[i].getName();
            if (displayName.length() > 15) {
                displayName = displayName.substr(0, 12) + "...";
            }
            std::cout << BRIGHT_BLACK << std::left << std::setw(3) << i + 1 << ". "
                      << BRIGHT_BLUE << std::left << std::setw(15) << displayName
                      << BRIGHT_BLACK "\t " BRIGHT_GREEN << std::fixed << std::setprecision(1)
                      << sortedPlayers[i].getScore() << "\n" RESET_COLOR;
        }
    }
    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void Leaderboard::displayPlayerHistory(const std::string& name) const {
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Player History ===\n\n" RESET_COLOR;
    std::cout << BRIGHT_CYAN "History for " << BRIGHT_BLUE << name << BRIGHT_CYAN ":\n" RESET_COLOR;
    bool found = false;
    for (const auto& player : players) {
        if (player.getName() == name) {
            std::cout << BRIGHT_BLACK " - Score: " << BRIGHT_GREEN << std::fixed
                      << std::setprecision(1) << player.getScore() << "\n" RESET_COLOR;
            found = true;
        }
    }
    if (!found) {
        std::cout << BRIGHT_CYAN "No history found for " << BRIGHT_BLUE << name << BRIGHT_CYAN ".\n" RESET_COLOR;
    }
    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}
