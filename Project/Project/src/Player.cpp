#include "Player.h"

Player::Player(const std::string& n, float s) : name(n), score(s) {}

std::string Player::getName() const { return name; }
float Player::getScore() const { return score; }
