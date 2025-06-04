#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
private:
    std::string name;
    float score;

public:
    Player(const std::string& n = "", float s = 0.0f);
    std::string getName() const;
    float getScore() const;
};

#endif
