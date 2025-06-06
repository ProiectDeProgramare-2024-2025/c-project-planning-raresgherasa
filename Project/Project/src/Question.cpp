#include "Question.h"

Question::Question(const std::string& q, const std::vector<std::string>& opts, int correct)
    : questionText(q), options(opts), correctAnswer(correct), used(false) {
    if (options.size() != 4) {
        options.resize(4, "N/A");
    }
    if (correctAnswer < 0 || correctAnswer > 3) {
        correctAnswer = 0;
    }
}

std::string Question::getQuestionText() const { return questionText; }
const std::vector<std::string>& Question::getOptions() const { return options; }
int Question::getCorrectAnswer() const { return correctAnswer; }
bool Question::isUsed() const { return used; }
void Question::setUsed(bool status) { used = status; }
