#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
//#define NOMINMAX
//#include <windows.h>
#include <limits>
#include <conio.h>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

struct Trivia {
    std::string question;
    std::string correctAnswer;
};

struct Topic {
    std::string topic;
};

std::string capitaliseFirstLetter(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = std::toupper(result[0]);
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

bool loadTrivia(const std::string& filename, std::vector<Trivia>& triviaList) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string question, correctAnswer;

        if (std::getline(iss, question, '|') && std::getline(iss, correctAnswer)) {
            Trivia trivia;
            trivia.question = question;
            trivia.correctAnswer = correctAnswer;
            triviaList.push_back(trivia);
        }
    }
    file.close();
    return true;
}

bool loadTopics(const std::string& filename, std::vector<Topic>& topicList) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            Topic topic;
            topic.topic = capitaliseFirstLetter(line);
            topicList.push_back(topic);
        }
    }
    file.close();
    return true;
}

void waitForKey() {
    std::cout << "Press any key to continue...";
    _getch();
}

void clearScreen() {
    system("CLS");
}

void displayTriviaWithOptions(const Trivia& trivia, const std::vector<std::string>& allAnswers, int& lives, int& score) {
    std::vector<std::string> options;
    options.push_back(trivia.correctAnswer);

    std::unordered_set<std::string> usedAnswers;
    usedAnswers.insert(trivia.correctAnswer);

    while (options.size() < 4) {
        std::string randomAnswer = allAnswers[std::rand() % allAnswers.size()];

        if (usedAnswers.find(randomAnswer) == usedAnswers.end()) {
            options.push_back(randomAnswer);
            usedAnswers.insert(randomAnswer);
        }
    }

    std::random_shuffle(options.begin(), options.end());

    std::cout << CYAN << "QUESTION:\n" << trivia.question << "\n" << RESET;
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << char('A' + i) << ") " << options[i] << "\n";
    }

    char userChoice;
    std::cout << "What do you answer: ";
    std::cin >> userChoice;
    userChoice = toupper(userChoice);

    int choiceIndex = userChoice - 'A';

    clearScreen();

    if (choiceIndex >= 0 && choiceIndex < options.size() && options[choiceIndex] == trivia.correctAnswer) {
        std::cout << GREEN << "CORRECT!\n" << RESET;
        score++;
        std::cout << "Score: " << score << "\n";
    }
    else {
        std::cout << RED << "WRONG!" << RESET << "\nThe correct answer was " << trivia.correctAnswer << "\n";
        lives--;
        std::cout << "Lives remaining: " << lives << "\n";
        std::cout << "Current Score: " << score << "\n";
    }
}

void displayTriviaWithoutOptions(const Trivia& trivia, const std::vector<std::string>& allAnswers, int& lives, int& score) {
    std::cout << MAGENTA << "QUESTION:\n" << trivia.question << "\n" << RESET;

    std::string userAnswer;
    std::cout << "What do you answer: ";
    std::getline(std::cin, userAnswer);

    std::transform(userAnswer.begin(), userAnswer.end(), userAnswer.begin(), ::tolower);
    std::string correctAnswer = trivia.correctAnswer;
    std::transform(correctAnswer.begin(), correctAnswer.end(), correctAnswer.begin(), ::tolower);

    clearScreen();

    if (userAnswer == correctAnswer) {
        std::cout << GREEN << "CORRECT!\n" << RESET;
        score++;
        std::cout << "Score: " << score << "\n";
    }
    else {
        std::cout << RED << "WRONG!" << RESET << "\nThe correct answer was " << trivia.correctAnswer << "\n";
        lives--;
        std::cout << "Lives remaining: " << lives << "\n";
        std::cout << "Current Score: " << score << "\n";
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    int lives = 3;
    int score = 0;

    std::cout << YELLOW << "Welcome to the Trivia Dungeon!\nPick a topic and enter the dungeon!\n" << RESET;
    waitForKey();
    clearScreen();
    
    
    std::vector<Topic> topicList;
    if (!loadTopics("topics.txt", topicList)) {
        return 1;
    }

    std::cout << BLUE << "SELECT TOPIC:\n" << RESET;
    for (size_t i = 0; i < topicList.size(); ++i) {
        std::cout << i + 1 << ") " << topicList[i].topic << "\n";
    }

    int topicChoice;
    std::cout << "Enter the number of your chosen topic: ";
    std::cin >> topicChoice;

    if (topicChoice < 1 || topicChoice > topicList.size()) {
        std::cerr << "Invalid topic choice!" << std::endl;
        return 1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    clearScreen();

    int difficulty;
    std::cout << BLUE << "SELECT DIFFICULTY:" << RESET << "\n1) Easy\n2) Hard\n";
    std::cout << "Enter the number of your chosen difficulty: ";
    std::cin >> difficulty;
    if (difficulty != 2) {
        difficulty = 1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    clearScreen();

    std::string chosenTopic = topicList[topicChoice - 1].topic;
    std::vector<Trivia> triviaList;
    std::string triviaFilename = "questions/" + chosenTopic + ".txt";
    if (!loadTrivia(triviaFilename, triviaList)) {
        return 1;
    }

    std::vector<std::string> allAnswers;
    for (const auto& trivia : triviaList) {
        allAnswers.push_back(trivia.correctAnswer);
    }

    while (lives > 0 && score < 10 && !triviaList.empty()) {
        int randomIndex = std::rand() % triviaList.size();
        Trivia currentTrivia = triviaList[randomIndex];
        triviaList.erase(triviaList.begin() + randomIndex);

        if (difficulty == 1) {
            displayTriviaWithOptions(currentTrivia, allAnswers, lives, score);
        }
        else {
            displayTriviaWithoutOptions(currentTrivia, allAnswers, lives, score);
        }

        if (lives != 0 || score != 10) {
            waitForKey();
        }

        clearScreen();
    }

    if (lives == 0) {
        std::cout << "\nGame Over! You've run out of lives.\n";
    }
    else if (score >= 10) {
        std::cout << "\nCongratulations! You've won!\n";
    }
    else {
        std::cout << "\nYou've somehow answered every questions! Good Job!\n";
    }

    return 0;
}
