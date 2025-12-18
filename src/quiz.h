#ifndef QUIZ_H
#define QUIZ_H

#include <vector>
#include <string>
#include <random>

class Quiz {
public:
    struct Question {
        std::wstring text;
        std::vector<std::wstring> options;
        int correctAnswer;
        std::wstring explanation;
        std::string category;
        int difficulty;
        int points;
    };
    
    struct QuizResult {
        int score;
        int correctAnswers;
        int totalQuestions;
        float accuracy;
        std::vector<bool> answers;
        std::vector<Question> questions;
        int timeSpent;
        std::string category;
    };
    
    Quiz();
    
    void startNewQuiz(const std::string& category = "", int questionCount = 10);
    bool submitAnswer(int answerIndex);
    void nextQuestion();
    QuizResult finishQuiz();
    
    const Question& getCurrentQuestion() const;
    int getCurrentQuestionIndex() const { return currentQuestion; }
    int getTotalQuestions() const { return selectedQuestions.size(); }
    bool isFinished() const { return finished; }
    bool hasAnsweredCurrent() const { 
        return currentQuestion < static_cast<int>(userAnswers.size()); 
    }
    int getLastAnswer() const { 
        if (currentQuestion < static_cast<int>(userAnswers.size())) {
            return userAnswers[currentQuestion];
        }
        return -1;
    }
    
    QuizResult getCurrentResult() const { return currentResult; }
    
    std::vector<std::string> getAvailableCategories() const;
    
    bool isQuizActive() const { return !finished && !selectedQuestions.empty(); }
    bool isLastQuestion() const { return currentQuestion == static_cast<int>(selectedQuestions.size()) - 1; }
    
    int getCurrentCorrectAnswer() const {
        if (currentQuestion < static_cast<int>(selectedQuestions.size())) {
            return selectedQuestions[currentQuestion].correctAnswer;
        }
        return -1;
    }
    
    bool isCurrentAnswerCorrect() const {
        if (hasAnsweredCurrent()) {
            int userAnswer = getLastAnswer();
            int correctAnswer = getCurrentCorrectAnswer();
            return userAnswer == correctAnswer;
        }
        return false;
    }
    
    const std::vector<int>& getUserAnswers() const { return userAnswers; }
    const std::vector<Question>& getSelectedQuestions() const { return selectedQuestions; }
    
    // Новые методы для повторения неправильных ответов
    void startRetryIncorrectQuestions(int requiredCorrect);
    bool hasRetryMode() const { return retryMode; }
    bool needsRetry() const { return retryRequired; }
    int getRequiredCorrectAnswers() const { return requiredCorrectAnswers; }
    int getCurrentRetryQuestionCount() const { return retryQuestions.size(); }
    void setRequiredCorrectAnswers(int required) { requiredCorrectAnswers = required; }

private:
    std::vector<Question> questions;
    std::vector<Question> selectedQuestions;
    std::vector<Question> retryQuestions; // Вопросы для повторения
    std::vector<int> userAnswers;
    std::vector<int> originalUserAnswers; // Сохраняем оригинальные ответы
    
    int currentQuestion;
    bool finished;
    bool retryMode; // Режим повторения неправильных ответов
    bool retryRequired; // Нужно ли повторение
    int startTime;
    int requiredCorrectAnswers; // Требуемое количество правильных ответов
    QuizResult currentResult;
    
    std::mt19937 rng;
    
    void initializeQuestions();
    void selectRandomQuestions(const std::string& category, int count);
    
    // Новый метод для подготовки вопросов для повторения
    void prepareRetryQuestions();
};

#endif
