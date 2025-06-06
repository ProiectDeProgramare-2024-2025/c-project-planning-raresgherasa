#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <vector>

class Question {
private:
    std::string questionText;
    std::vector<std::string> options; // Stores A. Option, B. Option, etc.
    int correctAnswer; // Index of correct option (0-3)
    bool used;

public:
    Question(const std::string& q = "", const std::vector<std::string>& opts = {}, int correct = 0);
    std::string getQuestionText() const;
    const std::vector<std::string>& getOptions() const;
    int getCorrectAnswer() const;
    bool isUsed() const;
    void setUsed(bool status);
};

#endif
