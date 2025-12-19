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
        int score = 0;
        int correctAnswers = 0;
        int totalQuestions = 0;
        float accuracy = 0.0f;
        std::vector<bool> answers;
        std::vector<Question> questions;
        int timeSpent = 0;
        std::string category;

        QuizResult() : score(0), correctAnswers(0), totalQuestions(0), accuracy(0.0f), timeSpent(0) {}
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
    
    void startRetryIncorrectQuestions(int requiredCorrect);
    bool hasRetryMode() const { return retryMode; }
    bool needsRetry() const { return retryRequired; }
    int getRequiredCorrectAnswers() const { return requiredCorrectAnswers; }
    int getCurrentRetryQuestionCount() const { return retryQuestions.size(); }
    void setRequiredCorrectAnswers(int required) { requiredCorrectAnswers = required; }

private:
    std::vector<Question> questions;
    std::vector<Question> selectedQuestions;
    std::vector<Question> retryQuestions;
    std::vector<int> userAnswers;
    std::vector<int> originalUserAnswers;
    
    int currentQuestion;
    bool finished;
    bool retryMode;
    bool retryRequired;
    int startTime;
    int requiredCorrectAnswers;
    QuizResult currentResult;
    
    std::mt19937 rng;
    
    void initializeQuestions();
    void selectRandomQuestions(const std::string& category, int count);
    
    void prepareRetryQuestions();
};

#endif
