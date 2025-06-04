#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

Game::Game(const std::string& filename) {
    std::random_device rd;
    rng.seed(rd());
    std::ifstream fileCheck(filename);
    if (!fileCheck.is_open()) {
        saveDefaultQuestions(filename);
    }
    fileCheck.close();
    loadQuestionsFromFile(filename);
}

void Game::loadQuestionsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << BRIGHT_RED "Could not open questions file. Using default questions.\n" RESET_COLOR;
        saveDefaultQuestions(filename);
        return;
    }

    questions.clear();
    std::string line;
    while (std::getline(file, line) && questions.size() < MAX_QUESTIONS) {
        if (line.empty()) continue;

        size_t prev_pos = 0;
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string::npos) continue;

        std::string qText = line.substr(prev_pos, comma_pos - prev_pos);
        prev_pos = comma_pos + 1;
        std::vector<std::string> opts(4);
        bool valid = true;

        for (int i = 0; i < 4; ++i) {
            comma_pos = line.find(',', prev_pos);
            if (comma_pos != std::string::npos) {
                opts[i] = static_cast<char>('A' + i) + std::string(". ") + line.substr(prev_pos, comma_pos - prev_pos);
                prev_pos = comma_pos + 1;
            } else if (i == 3 && prev_pos < line.length()) {
                opts[i] = static_cast<char>('A' + i) + std::string(". ") + line.substr(prev_pos);
                prev_pos = std::string::npos;
            } else {
                valid = false;
                break;
            }
        }

        if (!valid || prev_pos == std::string::npos || prev_pos >= line.length()) continue;

        try {
            int correctIdx = std::stoi(line.substr(prev_pos));
            if (correctIdx >= 0 && correctIdx <= 3) {
                questions.emplace_back(qText, opts, correctIdx);
            } else {
                std::cout << BRIGHT_YELLOW "Warning: Invalid correct answer index for question '" << qText << "'. Skipping.\n" RESET_COLOR;
            }
        } catch (const std::exception& e) {
            std::cout << BRIGHT_YELLOW "Warning: Invalid format for question '" << qText << "'. Skipping.\n" RESET_COLOR;
        }
    }
    file.close();
}

void Game::saveDefaultQuestions(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << BRIGHT_RED "Could not create questions file '" << filename << "'.\n" RESET_COLOR;
        return;
    }

    const std::vector<std::string> defaultQuestions = {
        "What is the capital of Brazil?,Rio de Janeiro,Brasilia,Sao Paulo,Salvador,1",
        "Which planet is known as the Red Planet?,Venus,Mars,Jupiter,Saturn,1",
        "What is 2 + 2?,22,4,44,2,1",
        "Who painted the Mona Lisa?,Van Gogh,Picasso,Da Vinci,Monet,2",
        "What is the largest ocean?,Atlantic,Indian,Arctic,Pacific,3",
        "What is the chemical symbol for water?,Wa,H2O,O2,H,1",
        "How many continents are there?,5,6,7,8,2",
        "What is the smallest country in the world?,Monaco,Vatican City,Nauru,San Marino,1",
        "Which of these is a primary color?,Green,Orange,Purple,Yellow,3",
        "What year did the Titanic sink?,1912,1905,1923,1918,0"
    };

    for (const auto& q : defaultQuestions) {
        file << q << "\n";
    }
    file.close();
}

void Game::resetQuestions() {
    for (auto& q : questions) {
        q.setUsed(false);
    }
}

int Game::getRandomQuestionIndex() {
    std::vector<size_t> availableIndices;
    for (size_t i = 0; i < questions.size(); ++i) {
        if (!questions[i].isUsed()) {
            availableIndices.push_back(i);
        }
    }
    if (availableIndices.empty()) return -1;
    std::uniform_int_distribution<> dis(0, availableIndices.size() - 1);
    return availableIndices[dis(rng)];
}

void Game::play() {
    float score = 0;
    int questionsAsked = 0;
    bool fiftyUsed = false;

    resetQuestions();
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Who Wants to Be a Millionaire ===\n\n" RESET_COLOR;

    while (questionsAsked < 15 && questionsAsked < static_cast<int>(questions.size())) {
        int qIndex = getRandomQuestionIndex();
        if (qIndex == -1) {
            std::cout << BRIGHT_YELLOW "Not enough unique questions available to play. Please add more questions.\n" RESET_COLOR;
            break;
        }

        char playerAnswer;
        bool answeredCorrectly = false;

        while (true) {
            clearScreen();
            std::cout << BRIGHT_YELLOW "Question " << questionsAsked + 1 << ":\n" RESET_COLOR;
            std::cout << BRIGHT_BLUE << questions[qIndex].getQuestionText() << "\n" RESET_COLOR;

            for (const auto& opt : questions[qIndex].getOptions()) {
                std::cout << BRIGHT_BLACK << opt[0] << ". " << BRIGHT_BLUE << opt.substr(3) << "\n" RESET_COLOR;
            }

            playerAnswer = getValidatedCharAnswer(
                fiftyUsed ? BRIGHT_BLACK "\nEnter your answer " BRIGHT_BLUE "(A-D)" BRIGHT_BLACK ": " RESET_COLOR
                          : BRIGHT_BLACK "\nEnter your answer " BRIGHT_BLUE "(A-D)" BRIGHT_BLACK " or " BRIGHT_BLUE "E " BRIGHT_BLACK "for 50/50 lifeline: " RESET_COLOR,
                fiftyUsed
            );

            if (playerAnswer == 'E' && !fiftyUsed) {
                fiftyUsed = true;
                clearScreen();
                std::cout << BRIGHT_YELLOW "Question " << questionsAsked + 1 << " (50/50 Lifeline Used):\n" RESET_COLOR;
                std::cout << BRIGHT_BLUE << questions[qIndex].getQuestionText() << "\n" RESET_COLOR;

                int correct = questions[qIndex].getCorrectAnswer();
                std::vector<int> wrongOptions;
                for (int i = 0; i < 4; ++i) {
                    if (i != correct) wrongOptions.push_back(i);
                }
                std::shuffle(wrongOptions.begin(), wrongOptions.end(), rng);
                int keepWrong = wrongOptions[0];
                std::vector<int> displayOptions = {correct, keepWrong};
                std::sort(displayOptions.begin(), displayOptions.end());

                for (int i : displayOptions) {
                    std::cout << BRIGHT_BLACK << questions[qIndex].getOptions()[i][0] << ". "
                              << BRIGHT_BLUE << questions[qIndex].getOptions()[i].substr(3) << "\n" RESET_COLOR;
                }
                std::cout << BRIGHT_YELLOW "\nLifeline used! Now choose from remaining options.\n" RESET_COLOR;
                std::cout << "Press Enter to continue...";
                std::string dummy;
                std::getline(std::cin, dummy);
                playerAnswer = getValidatedCharAnswer(
                    BRIGHT_BLACK "\nEnter your answer " BRIGHT_BLUE "(A-D)" BRIGHT_BLACK ": " RESET_COLOR,
                    true
                );
                int answerIndex = playerAnswer - 'A';
                if (answerIndex == questions[qIndex].getCorrectAnswer()) {
                    std::cout << BRIGHT_GREEN "\nCorrect!\n" RESET_COLOR;
                    score += 1.0;
                    questionsAsked++;
                    answeredCorrectly = true;
                } else {
                    std::cout << BRIGHT_RED "\nIncorrect! The correct answer was " << BRIGHT_GREEN
                              << static_cast<char>('A' + questions[qIndex].getCorrectAnswer()) << ". "
                              << questions[qIndex].getOptions()[questions[qIndex].getCorrectAnswer()].substr(3) << "\n" RESET_COLOR;
                    answeredCorrectly = false;
                }
                break;
            } else {
                int answerIndex = playerAnswer - 'A';
                if (answerIndex == questions[qIndex].getCorrectAnswer()) {
                    std::cout << BRIGHT_GREEN "\nCorrect!\n" RESET_COLOR;
                    score += 1.0;
                    questionsAsked++;
                    answeredCorrectly = true;
                } else {
                    std::cout << BRIGHT_RED "\nIncorrect! The correct answer was " << BRIGHT_GREEN
                              << static_cast<char>('A' + questions[qIndex].getCorrectAnswer()) << ". "
                              << questions[qIndex].getOptions()[questions[qIndex].getCorrectAnswer()].substr(3) << "\n" RESET_COLOR;
                    answeredCorrectly = false;
                }
                break;
            }
        }

        questions[qIndex].setUsed(true);

        if (!answeredCorrectly) {
            std::cout << BRIGHT_YELLOW "Game Over!\n" RESET_COLOR;
            break;
        }

        std::cout << "Press Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    clearScreen();
    std::cout << BRIGHT_CYAN "Game Over! Your final score: " << BRIGHT_GREEN << std::fixed
              << std::setprecision(1) << score << "\n" RESET_COLOR;

    std::string name;
    while (true) {
        std::cout << BRIGHT_BLACK "Enter your name " BRIGHT_BLUE "(max " << MAX_NAME - 1
                  << " characters)" BRIGHT_BLACK ": " RESET_COLOR;
        std::getline(std::cin, name);
        if (!name.empty() && name.length() < MAX_NAME) {
            break;
        }
        std::cout << BRIGHT_RED "Invalid name! Name cannot be empty and must be less than "
                  << MAX_NAME << " characters.\n" RESET_COLOR;
    }

    leaderboard.addPlayer(Player(name, score));
}

void Game::showLeaderboard() const {
    leaderboard.display();
}

void Game::showPlayerHistory() const {
    std::string name;
    while (true) {
        std::cout << BRIGHT_BLACK "Enter player name to search " BRIGHT_BLUE "(max " << MAX_NAME - 1
                  << " characters)" BRIGHT_BLACK ": " RESET_COLOR;
        std::getline(std::cin, name);
        if (!name.empty() && name.length() < MAX_NAME) {
            break;
        }
        std::cout << BRIGHT_RED "Invalid name! Name cannot be empty and must be less than "
                  << MAX_NAME << " characters.\n" RESET_COLOR;
    }
    leaderboard.displayPlayerHistory(name);
}

void Game::run() {
    int choice;
    do {
        clearScreen();
        std::cout << BRIGHT_YELLOW "=== Who Wants to Be a Millionaire ===\n" RESET_COLOR;
        std::cout << BRIGHT_MAGENTA "1. " BRIGHT_GREEN "Play Game\n" RESET_COLOR;
        std::cout << BRIGHT_MAGENTA "2. " BRIGHT_YELLOW "View Leaderboard\n" RESET_COLOR;
        std::cout << BRIGHT_MAGENTA "3. " BRIGHT_CYAN "View Player History\n" RESET_COLOR;
        std::cout << BRIGHT_MAGENTA "4. " BRIGHT_RED "Exit\n" RESET_COLOR;
        std::cout << BRIGHT_BLACK "-----------------------------------\n" RESET_COLOR;

        choice = getValidatedIntegerInput(BRIGHT_BLACK "Enter your choice " BRIGHT_BLUE "(1-4)" BRIGHT_BLACK ": " RESET_COLOR, 1, 4);

        switch (choice) {
            case 1:
                play();
                break;
            case 2:
                showLeaderboard();
                break;
            case 3:
                showPlayerHistory();
                break;
            case 4:
                clearScreen();
                std::cout << BRIGHT_YELLOW "Goodbye! Thanks for playing!\n" RESET_COLOR;
                break;
        }
    } while (choice != 4);
}
