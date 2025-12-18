#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "quiz.h"
#include "database.h"

class Player {
public:
    struct Achievement {
        std::string id;
        std::string name;
        std::string description;
        bool unlocked;
        int unlockDate;
        int rewardPoints;
    };
    
    Player(const std::string& name);
    
    bool initialize();
    
    void addScore(int points);
    void completeQuiz(const Quiz::QuizResult& result);
    void recordStudy(const std::string& celestialBody, int studyTime);
    
    void checkAchievements();
    bool unlockAchievement(const std::string& achievementId);
    std::vector<Achievement> getAchievements() const;
    
    int getScore() const { return totalScore; }
    const std::string& getName() const { return name; }
    
    int getStudiedPlanetsCount() const { return studyProgress.size(); }
    int getTotalStudyTime() const;
    
    std::vector<Quiz::QuizResult> getQuizHistory() const { return quizHistory; }
    int getBestScore() const;
    
    bool saveToDatabase();
    bool loadFromDatabase();
    bool syncWithDatabase();
    
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    bool hasCompletedPlanetQuiz(const std::string& planetName) const;
    int getUnlockedPlanetsCount() const;

private:
    std::string name;
    int totalScore;
    
    std::map<std::string, int> studyProgress;
    std::map<std::string, Achievement> achievements;
    std::vector<Quiz::QuizResult> quizHistory;
    
    void initializeAchievements();
    
    Database::PlayerData toDatabaseStruct() const;
    void fromDatabaseStruct(const Database::PlayerData& data);
};

#endif
