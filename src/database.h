#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <memory>
#include <ctime>

class Database {
public:
    struct PlayerData {
        std::string name;
        std::string password_hash;
        int totalScore;
        int quizzesCompleted;
        std::time_t createdAt;
        std::time_t lastPlayed;
    };
    
    struct AchievementData {
        std::string playerName;
        std::string achievementId;
        std::time_t unlockDate;
    };
    
    struct QuizResultData {
        std::string playerName;
        int score;
        int correctAnswers;
        int totalQuestions;
        std::string category;
    };
        
    static Database& getInstance();
    
    bool connect(const std::string& connString = "dbname=astrolearn user=postgres password=postgres host=localhost port=5432");
    void disconnect();
    bool isConnected() const { return connection != nullptr; }
    
    int createPlayer(const std::string& name, const std::string& password);
    bool updatePlayer(const PlayerData& player);
    PlayerData getPlayerByName(const std::string& name);
    PlayerData authenticatePlayer(const std::string& name, const std::string& password);
    bool updatePassword(const std::string& playerName, const std::string& newPassword);
    std::vector<PlayerData> getAllPlayers();
    std::vector<PlayerData> getTopPlayers(int limit = 10);
    
    bool unlockAchievement(const std::string& playerName, const std::string& achievementId);
    bool hasAchievement(const std::string& playerName, const std::string& achievementId);
    std::vector<AchievementData> getPlayerAchievements(const std::string& playerName);
    
    bool saveQuizResult(const QuizResultData& result);
    std::vector<QuizResultData> getPlayerQuizHistory(const std::string& playerName, int limit = 20);
        
    struct GlobalStats {
        int totalPlayers;
        int totalQuizzesCompleted;
    };
    
    GlobalStats getGlobalStats();
    
    bool initializeTables();
    
private:
    Database() = default;
    ~Database();
    
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    std::unique_ptr<pqxx::connection> connection;
    
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
    
    PlayerData playerFromRow(const pqxx::row& row);
    AchievementData achievementFromRow(const pqxx::row& row);
    QuizResultData quizResultFromRow(const pqxx::row& row);
    
    std::time_t stringToTime(const std::string& timeStr);
    std::string timeToString(std::time_t time);
};

#endif
