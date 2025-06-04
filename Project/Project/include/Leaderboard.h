#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "Player.h"
#include <vector>

class Leaderboard {
private:
    std::vector<Player> players; // Relationship: Leaderboard contains Players (composition)
    static const int MAX_PLAYERS = 100;

public:
    void addPlayer(const Player& player);
    void display() const;
    void displayPlayerHistory(const std::string& name) const;
};

#endif
