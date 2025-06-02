#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <random>

#define RESET_COLOR "\x1b[0m"
#define BRIGHT_BLACK "\x1b[90m"
#define BRIGHT_RED "\x1b[91m"
#define BRIGHT_GREEN "\x1b[92m"
#define BRIGHT_YELLOW "\x1b[93m"
#define BRIGHT_BLUE "\x1b[94m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_CYAN "\x1b[96m"
#define BRIGHT_WHITE "\x1b[97m"

#define MAX_QUESTIONS 100
#define MAX_NAME 50

struct Question {
    std::string question;
    std::string options[4];
    int correctAnswer;
    bool used;
};

struct Player {
    std::string name;
    float score;
};

Question questions[MAX_QUESTIONS];
int questionCount = 0;
Player leaderboard[100];
int leaderboardSize = 0;
std::mt19937 globalRng; // Global random number generator

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
            } else {
                std::cout << BRIGHT_RED "Invalid input! Please enter A, B, C, D" << (fiftyUsed ? "" : ", or E") << ".\n" RESET_COLOR;
            }
        } else {
            std::cout << BRIGHT_RED "Invalid input! Please enter only one character (A-D" << (fiftyUsed ? "" : ", or E") << ").\n" RESET_COLOR;
        }
    }
}

void loadQuestionsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << BRIGHT_RED "Could not open questions file. Using default questions.\n" RESET_COLOR;
        return;
    }

    questionCount = 0;
    std::string line;
    while (std::getline(file, line) && questionCount < MAX_QUESTIONS) {
        if (line.empty()) continue;

        size_t prev_pos = 0;
        size_t comma_pos = line.find(',');

        if (comma_pos != std::string::npos) {
            questions[questionCount].question = line.substr(prev_pos, comma_pos - prev_pos);
            prev_pos = comma_pos + 1;
        } else {
            continue;
        }

        for (int i = 0; i < 4; i++) {
            comma_pos = line.find(',', prev_pos);
            if (comma_pos != std::string::npos) {
                questions[questionCount].options[i] = static_cast<char>('A' + i) + std::string(". ") + line.substr(prev_pos, comma_pos - prev_pos);
                prev_pos = comma_pos + 1;
            } else {
                if (i == 3 && prev_pos < line.length()) {
                    questions[questionCount].options[i] = static_cast<char>('A' + i) + std::string(". ") + line.substr(prev_pos);
                    prev_pos = std::string::npos;
                } else {
                    questions[questionCount].question = "";
                    break;
                }
            }
        }

        if (questions[questionCount].question.empty()) continue;

        if (prev_pos != std::string::npos && prev_pos < line.length()) {
            std::string correctAnsStr = line.substr(prev_pos);
            try {
                int correctIdx = std::stoi(correctAnsStr);
                if (correctIdx >= 0 && correctIdx <= 3) {
                    questions[questionCount].correctAnswer = correctIdx;
                } else {
                    std::cout << BRIGHT_YELLOW "Warning: Invalid correct answer index for question '" << questions[questionCount].question << "'. Skipping.\n" RESET_COLOR;
                    continue;
                }
            } catch (const std::invalid_argument& e) {
                std::cout << BRIGHT_YELLOW "Warning: Invalid correct answer format for question '" << questions[questionCount].question << "'. Skipping.\n" RESET_COLOR;
                continue;
            } catch (const std::out_of_range& e) {
                std::cout << BRIGHT_YELLOW "Warning: Correct answer out of range for question '" << questions[questionCount].question << "'. Skipping.\n" RESET_COLOR;
                continue;
            }
        } else {
            continue;
        }

        questions[questionCount].used = false;
        questionCount++;
    }

    file.close();
}

void saveDefaultQuestions(const std::string& filename) {
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

void resetQuestions() {
    for (int i = 0; i < questionCount; i++) {
        questions[i].used = false;
    }
}

int getRandomQuestionIndex() {
    std::vector<int> availableIndices;
    for (int i = 0; i < questionCount; i++) {
        if (!questions[i].used) {
            availableIndices.push_back(i);
        }
    }

    if (availableIndices.empty()) return -1;

    std::uniform_int_distribution<> dis(0, availableIndices.size() - 1);
    return availableIndices[dis(globalRng)];
}

void playGame() {
    float score = 0;
    int questionsAsked = 0;
    bool fiftyUsed = false;

    resetQuestions();
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Who Wants to Be a Millionaire ===\n\n" RESET_COLOR;

    while (questionsAsked < 15 && questionsAsked < questionCount) {
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
            std::cout << BRIGHT_BLUE << questions[qIndex].question << "\n" RESET_COLOR;

            for (int i = 0; i < 4; i++) {
                std::cout << BRIGHT_BLACK << static_cast<char>('A' + i) << ". " << BRIGHT_BLUE << questions[qIndex].options[i].substr(3) << "\n" RESET_COLOR;
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
                std::cout << BRIGHT_BLUE << questions[qIndex].question << "\n" RESET_COLOR;

                int correct = questions[qIndex].correctAnswer;
                std::vector<int> wrongOptions;
                for (int i = 0; i < 4; ++i) {
                    if (i != correct) {
                        wrongOptions.push_back(i);
                    }
                }
                std::shuffle(wrongOptions.begin(), wrongOptions.end(), globalRng);
                int keepWrong = wrongOptions[0]; // Keep one wrong option

                // Sort indices to display options in original order
                std::vector<int> displayOptions = {correct, keepWrong};
                std::sort(displayOptions.begin(), displayOptions.end());

                for (int i : displayOptions) {
                    std::cout << BRIGHT_BLACK << static_cast<char>('A' + i) << ". " << BRIGHT_BLUE << questions[qIndex].options[i].substr(3) << "\n" RESET_COLOR;
                }
                std::cout << BRIGHT_YELLOW "\nLifeline used! Now choose from remaining options.\n" RESET_COLOR;
                std::cout << "Press Enter to continue...";
                std::string dummy;
                std::getline(std::cin, dummy);
                // Prompt for answer with fiftyUsed = true
                playerAnswer = getValidatedCharAnswer(
                    BRIGHT_BLACK "\nEnter your answer " BRIGHT_BLUE "(A-D)" BRIGHT_BLACK ": " RESET_COLOR,
                    true
                );
                int answerIndex = playerAnswer - 'A';
                if (answerIndex == questions[qIndex].correctAnswer) {
                    std::cout << BRIGHT_GREEN "\nCorrect!\n" RESET_COLOR;
                    score += 1.0;
                    questionsAsked++;
                    answeredCorrectly = true;
                } else {
                    std::cout << BRIGHT_RED "\nIncorrect! The correct answer was " << BRIGHT_GREEN << static_cast<char>('A' + questions[qIndex].correctAnswer) << ". " << questions[qIndex].options[questions[qIndex].correctAnswer].substr(3) << "\n" RESET_COLOR;
                    answeredCorrectly = false;
                }
                break;
            } else {
                int answerIndex = playerAnswer - 'A';
                if (answerIndex == questions[qIndex].correctAnswer) {
                    std::cout << BRIGHT_GREEN "\nCorrect!\n" RESET_COLOR;
                    score += 1.0;
                    questionsAsked++;
                    answeredCorrectly = true;
                } else {
                    std::cout << BRIGHT_RED "\nIncorrect! The correct answer was " << BRIGHT_GREEN << static_cast<char>('A' + questions[qIndex].correctAnswer) << ". " << questions[qIndex].options[questions[qIndex].correctAnswer].substr(3) << "\n" RESET_COLOR;
                    answeredCorrectly = false;
                }
                break;
            }
        }

        questions[qIndex].used = true;

        if (!answeredCorrectly) {
            std::cout << BRIGHT_YELLOW "Game Over!\n" RESET_COLOR;
            break;
        }

        std::cout << "Press Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    clearScreen();
    std::cout << BRIGHT_CYAN "Game Over! Your final score: " << BRIGHT_GREEN << std::fixed << std::setprecision(1) << score << "\n" RESET_COLOR;

    std::string nameBuffer;
    while (true) {
        std::cout << BRIGHT_BLACK "Enter your name " BRIGHT_BLUE "(max " << MAX_NAME - 1 << " characters)" BRIGHT_BLACK ": " RESET_COLOR;
        std::getline(std::cin, nameBuffer);

        if (!nameBuffer.empty() && nameBuffer.length() < MAX_NAME) {
            break;
        } else {
            std::cout << BRIGHT_RED "Invalid name! Name cannot be empty and must be less than " << MAX_NAME << " characters.\n" RESET_COLOR;
        }
    }

    if (leaderboardSize < 100) {
        leaderboard[leaderboardSize].name = nameBuffer;
        leaderboard[leaderboardSize].score = score;
        leaderboardSize++;
        std::cout << BRIGHT_GREEN "Score added to leaderboard!\n" RESET_COLOR;
    } else {
        std::cout << BRIGHT_YELLOW "Leaderboard is full. Score not saved.\n" RESET_COLOR;
    }

    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void showLeaderboard() {
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Leaderboard ===\n\n" RESET_COLOR;
    if (leaderboardSize == 0) {
        std::cout << BRIGHT_CYAN "No scores recorded yet. Play a game to add entries!\n" RESET_COLOR;
    } else {
        std::vector<Player> sortedLeaderboard(leaderboard, leaderboard + leaderboardSize);
        std::sort(sortedLeaderboard.begin(), sortedLeaderboard.end(), [](const Player& a, const Player& b) {
            return a.score > b.score;
        });

        std::cout << BRIGHT_BLACK "No. " BRIGHT_BLUE "Player Name" BRIGHT_BLACK " \t " BRIGHT_GREEN "Score\n" RESET_COLOR;
        std::cout << BRIGHT_BLACK "-----------------------------------\n" RESET_COLOR;
        for (int i = 0; i < leaderboardSize; i++) {
            std::string displayName = sortedLeaderboard[i].name;
            if (displayName.length() > 15) {
                displayName = displayName.substr(0, 12) + "...";
            }
            std::cout << BRIGHT_BLACK << std::left << std::setw(3) << i + 1 << ". "
                      << BRIGHT_BLUE << std::left << std::setw(15) << displayName
                      << BRIGHT_BLACK "\t " BRIGHT_GREEN << std::fixed << std::setprecision(1) << sortedLeaderboard[i].score << "\n" RESET_COLOR;
        }
    }
    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void showPlayerHistory() {
    clearScreen();
    std::cout << BRIGHT_YELLOW "=== Player History ===\n\n" RESET_COLOR;
    std::string nameToSearch;

    while (true) {
        std::cout << BRIGHT_BLACK "Enter player name to search " BRIGHT_BLUE "(max " << MAX_NAME - 1 << " characters)" BRIGHT_BLACK ": " RESET_COLOR;
        std::getline(std::cin, nameToSearch);

        if (!nameToSearch.empty() && nameToSearch.length() < MAX_NAME) {
            break;
        } else {
            std::cout << BRIGHT_RED "Invalid name! Name cannot be empty and must be less than " << MAX_NAME << " characters.\n" RESET_COLOR;
        }
    }

    clearScreen();
    std::cout << BRIGHT_CYAN "History for " << BRIGHT_BLUE << nameToSearch << BRIGHT_CYAN ":\n" RESET_COLOR;
    bool found = false;
    for (int i = 0; i < leaderboardSize; i++) {
        if (leaderboard[i].name == nameToSearch) {
            std::cout << BRIGHT_BLACK " - Score: " << BRIGHT_GREEN << std::fixed << std::setprecision(1) << leaderboard[i].score << "\n" RESET_COLOR;
            found = true;
        }
    }
    if (!found) std::cout << BRIGHT_CYAN "No history found for " << BRIGHT_BLUE << nameToSearch << BRIGHT_CYAN ".\n" RESET_COLOR;

    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void mainMenu() {
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
                playGame();
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
            default:
                std::cout << BRIGHT_RED "Invalid choice! Please try again.\n" RESET_COLOR;
                break;
        }
    } while (choice != 4);
}

int main() {
    std::random_device rd;
    globalRng.seed(rd()); // Initialize global random number generator

    const std::string filename = "questions.txt";
    std::ifstream fileCheck(filename);
    if (!fileCheck.is_open()) {
        saveDefaultQuestions(filename);
    } else {
        fileCheck.close();
    }

    loadQuestionsFromFile(filename);

    mainMenu();

    return 0;
}
