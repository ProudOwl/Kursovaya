#include "player.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

Player::Player(const std::string& name)
    : name(name)
    , totalScore(0) {
    
    initializeAchievements();
}

bool Player::initialize() {
    Database& db = Database::getInstance();
    if (!db.isConnected()) {
        std::cout << "Database not connected, using local player" << std::endl;
        return true;
    }
    
    try {
        auto playerData = db.getPlayerByName(name);
        fromDatabaseStruct(playerData);
        
        auto dbAchievements = db.getPlayerAchievements(name);
        for (const auto& dbAch : dbAchievements) {
            auto it = achievements.find(dbAch.achievementId);
            if (it != achievements.end()) {
                it->second.unlocked = true;
                it->second.unlockDate = static_cast<int>(dbAch.unlockDate);
            }
        }
        
        std::cout << "Loaded existing player from database: " << name 
                  << " (Score: " << totalScore << ")" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Player not found in database, creating new: " << name << std::endl;
        return true;
    }
}

void Player::addScore(int points) {
    if (points <= 0) {
        return;
    }
    
    int oldScore = totalScore;
    totalScore += points;
    
    std::cout << "[SCORE] " << name << " получил " << points << " очков. "
              << "Общий счет: " << oldScore << " → " << totalScore << std::endl;
}

void Player::completeQuiz(const Quiz::QuizResult& result) {
    quizHistory.push_back(result);
    
    addScore(result.score);
    
    Database& db = Database::getInstance();
    if (db.isConnected()) {
        Database::QuizResultData dbResult;
        dbResult.playerName = name;
        dbResult.score = result.score;
        dbResult.correctAnswers = result.correctAnswers;
        dbResult.totalQuestions = result.totalQuestions;
        dbResult.category = result.category;
        
        db.saveQuizResult(dbResult);
        saveToDatabase();
    }
    
    checkAchievements();
    
    std::cout << "[QUIZ] " << name << " завершил квиз. "
              << "Правильных ответов: " << result.correctAnswers 
              << "/" << result.totalQuestions << std::endl;
}


void Player::recordStudy(const std::string& celestialBody, int studyTime) {
    if (studyTime <= 0) {
        std::cout << "[STUDY] Время изучения должно быть положительным" << std::endl;
        return;
    }
    
    int oldStudyTime = studyProgress[celestialBody];
    studyProgress[celestialBody] += studyTime;
    
    int points = studyTime / 10;
    
    if (points > 0) {
        addScore(points);
    }
    
    checkAchievements();
    
    std::cout << "[STUDY] " << name << " изучил " << celestialBody 
              << " в течение " << studyTime << " секунд. "
              << "Общее время изучения " << celestialBody << ": " 
              << oldStudyTime << " → " << studyProgress[celestialBody] << " секунд" << std::endl;
}

bool Player::saveToDatabase() {
    Database& db = Database::getInstance();
    if (!db.isConnected()) {
        std::cerr << "Database not connected" << std::endl;
        return false;
    }
    
    Database::PlayerData data = toDatabaseStruct();
    return db.updatePlayer(data);
}

bool Player::loadFromDatabase() {
    Database& db = Database::getInstance();
    if (!db.isConnected()) {
        std::cerr << "Database not connected" << std::endl;
        return false;
    }
    
    try {
        Database::PlayerData data = db.getPlayerByName(name);
        fromDatabaseStruct(data);
        
        auto dbAchievements = db.getPlayerAchievements(name);
        for (const auto& dbAch : dbAchievements) {
            auto it = achievements.find(dbAch.achievementId);
            if (it != achievements.end()) {
                it->second.unlocked = true;
                it->second.unlockDate = static_cast<int>(dbAch.unlockDate);
            }
        }
        
        std::cout << "Loaded player from database: " << name 
                  << " (Score: " << totalScore << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load player from database: " << e.what() << std::endl;
        return false;
    }
}

bool Player::syncWithDatabase() {
    if (!saveToDatabase()) {
        return false;
    }
    
    Database& db = Database::getInstance();
    for (const auto& [id, achievement] : achievements) {
        if (achievement.unlocked) {
            db.unlockAchievement(name, id);
        }
    }
    
    return true;
}

Database::PlayerData Player::toDatabaseStruct() const {
    Database::PlayerData data;
    
    data.name = name;
    data.totalScore = totalScore;
    data.quizzesCompleted = static_cast<int>(quizHistory.size());
    
    return data;
}

void Player::fromDatabaseStruct(const Database::PlayerData& data) {
    name = data.name;
    totalScore = data.totalScore;
}

void Player::initializeAchievements() {
    achievements["first_study"] = {
        "first_study",
        "First Step",
        "Study your first celestial body",
        false, 0, 100
    };
    
    achievements["planet_explorer"] = {
        "planet_explorer",
        "Planet Explorer",
        "Study all planets of the Solar System",
        false, 0, 500
    };
    
    achievements["star_gazer"] = {
        "star_gazer",
        "Star Gazer",
        "Study 5 different stars",
        false, 0, 300
    };
    
    achievements["quiz_beginner"] = {
        "quiz_beginner",
        "Beginner Astronomer",
        "Complete your first quiz",
        false, 0, 200
    };
    
    achievements["quiz_master"] = {
        "quiz_master",
        "Quiz Master",
        "Complete 10 quizzes",
        false, 0, 1000
    };
    
    achievements["perfect_score"] = {
        "perfect_score",
        "Perfection",
        "Get 100% in a difficult quiz",
        false, 0, 1500
    };
    
    achievements["space_explorer"] = {
        "space_explorer",
        "Space Explorer",
        "Study 20 different celestial bodies",
        false, 0, 800
    };
    
    std::cout << "Initialized " << achievements.size() << " achievements" << std::endl;
}

void Player::checkAchievements() {
    if (!achievements["first_study"].unlocked && !studyProgress.empty()) {
        unlockAchievement("first_study");
    }
    
    std::vector<std::string> planets = {"Mercury", "Venus", "Earth", "Mars", 
                                       "Jupiter", "Saturn", "Uranus", "Neptune"};
    bool allPlanetsStudied = true;
    for (const auto& planet : planets) {
        if (studyProgress.find(planet) == studyProgress.end()) {
            allPlanetsStudied = false;
            break;
        }
    }
    if (allPlanetsStudied && !achievements["planet_explorer"].unlocked) {
        unlockAchievement("planet_explorer");
    }
    
    int starsStudied = 0;
    for (const auto& [body, time] : studyProgress) {
        if (body == "Sun" || body.find("Star") != std::string::npos) {
            starsStudied++;
        }
    }
    if (starsStudied >= 5 && !achievements["star_gazer"].unlocked) {
        unlockAchievement("star_gazer");
    }
    
    if (!quizHistory.empty() && !achievements["quiz_beginner"].unlocked) {
        unlockAchievement("quiz_beginner");
    }
    
    if (quizHistory.size() >= 10 && !achievements["quiz_master"].unlocked) {
        unlockAchievement("quiz_master");
    }
    
    for (const auto& result : quizHistory) {
        if (result.accuracy == 100.0f && result.totalQuestions >= 5) {
            if (!achievements["perfect_score"].unlocked) {
                unlockAchievement("perfect_score");
            }
            break;
        }
    }
    
    if (studyProgress.size() >= 20 && !achievements["space_explorer"].unlocked) {
        unlockAchievement("space_explorer");
    }
}

bool Player::unlockAchievement(const std::string& achievementId) {
    auto it = achievements.find(achievementId);
    if (it != achievements.end() && !it->second.unlocked) {
        it->second.unlocked = true;
        it->second.unlockDate = static_cast<int>(time(nullptr));
        
        addScore(it->second.rewardPoints);
        
        std::cout << "Achievement unlocked: " << it->second.name 
                  << " - " << it->second.description 
                  << " (Reward: " << it->second.rewardPoints << " points)" << std::endl;
        
        Database& db = Database::getInstance();
        if (db.isConnected()) {
            db.unlockAchievement(name, achievementId);
        }
        
        return true;
    }
    return false;
}

std::vector<Player::Achievement> Player::getAchievements() const {
    std::vector<Achievement> result;
    for (const auto& [id, achievement] : achievements) {
        result.push_back(achievement);
    }
    
    std::sort(result.begin(), result.end(), 
              [](const Achievement& a, const Achievement& b) {
                  if (a.unlocked != b.unlocked) {
                      return a.unlocked > b.unlocked;
                  }
                  return a.name < b.name;
              });
    
    return result;
}

int Player::getTotalStudyTime() const {
    int total = 0;
    for (const auto& [body, time] : studyProgress) {
        total += time;
    }
    return total;
}

int Player::getBestScore() const {
    int best = 0;
    for (const auto& result : quizHistory) {
        if (result.score > best) {
            best = result.score;
        }
    }
    return best;
}

bool Player::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
        return false;
    }
    
    file << "[Player]\n";
    file << "name=" << name << "\n";
    file << "score=" << totalScore << "\n";
    
    file << "\n[StudyProgress]\n";
    for (const auto& [body, time] : studyProgress) {
        file << body << "=" << time << "\n";
    }
    
    file << "\n[Achievements]\n";
    for (const auto& [id, achievement] : achievements) {
        file << id << "=" << (achievement.unlocked ? "1" : "0") 
             << "," << achievement.unlockDate << "\n";
    }
    
    file << "\n[QuizHistory]\n";
    file << "count=" << quizHistory.size() << "\n";
    
    file.close();
    return true;
}

bool Player::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Savegame not found: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        if (currentSection == "Player") {
            if (key == "name") name = value;
            else if (key == "score") totalScore = std::stoi(value);
        }
        else if (currentSection == "StudyProgress") {
            studyProgress[key] = std::stoi(value);
        }
        else if (currentSection == "Achievements") {
            size_t commaPos = value.find(',');
            if (commaPos != std::string::npos) {
                bool unlocked = (value.substr(0, commaPos) == "1");
                int unlockDate = std::stoi(value.substr(commaPos + 1));
                
                auto it = achievements.find(key);
                if (it != achievements.end()) {
                    it->second.unlocked = unlocked;
                    it->second.unlockDate = unlockDate;
                }
            }
        }
    }
    
    file.close();
    
    std::cout << "Loaded savegame: " << name 
              << " (Score: " << totalScore << ")" << std::endl;
    return true;
}

int Player::getUnlockedPlanetsCount() const {
    int count = 0;
    for (const auto& [body, time] : studyProgress) {
        if (body == "Mercury" || body == "Venus" || body == "Earth" || 
            body == "Mars" || body == "Jupiter" || body == "Saturn" || 
            body == "Uranus" || body == "Neptune") {
            count++;
        }
    }
    return count;
}

bool Player::hasCompletedPlanetQuiz(const std::string& planetName) const {
    for (const auto& quizResult : quizHistory) {
        if (quizResult.category == "planets" && quizResult.correctAnswers >= 3) {
            return true;
        }
    }
    return false;
}
