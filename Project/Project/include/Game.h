#ifndef GAME_H
#define GAME_H

#include "Question.h"
#include "Leaderboard.h"
#include <vector>
#include <random>

class Game {
private:
    std::vector<Question> questions; // Relationship: Game contains Questions (composition)
    Leaderboard leaderboard;
    std::mt19937 rng;
    static const int MAX_QUESTIONS = 100;
    static const int MAX_NAME = 50;

    void loadQuestionsFromFile(const std::string& filename);
    void saveDefaultQuestions(const std::string& filename);
    void resetQuestions();
    int getRandomQuestionIndex();

public:
    Game(const std::string& filename);
    void play();
    void showLeaderboard() const;
    void showPlayerHistory() const;
    void run();
};

#endif
