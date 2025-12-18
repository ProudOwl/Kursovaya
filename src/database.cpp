#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <functional>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::~Database() {
    disconnect();
}

bool Database::connect(const std::string& connString) {
    try {
        std::cout << "Connecting to PostgreSQL database: " << connString << std::endl;
        connection = std::make_unique<pqxx::connection>(connString);
        
        if (connection->is_open()) {
            std::cout << "Successfully connected to database: " << connection->dbname() << std::endl;
            return initializeTables();
        } else {
            std::cerr << "Failed to connect to database" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Database connection error: " << e.what() << std::endl;
        return false;
    }
}

void Database::disconnect() {
    if (connection && connection->is_open()) {
        connection.reset();
        std::cout << "Database connection closed" << std::endl;
    }
    connection.reset();
}

bool Database::initializeTables() {
    try {
        pqxx::work txn(*connection);
        
        txn.exec(
            "CREATE TABLE IF NOT EXISTS players ("
            "name VARCHAR(50) PRIMARY KEY,"
            "password_hash VARCHAR(256) NOT NULL DEFAULT '',"
            "total_score INTEGER DEFAULT 0,"
            "quizzes_completed INTEGER DEFAULT 0,"
            "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "last_played TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ")"
        );
        
        txn.exec(
            "CREATE TABLE IF NOT EXISTS achievements ("
            "name VARCHAR(50) NOT NULL,"
            "achievement_id VARCHAR(50) NOT NULL,"
            "unlock_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "FOREIGN KEY (name) REFERENCES players(name) ON DELETE CASCADE,"
            "UNIQUE(name, achievement_id)"
            ")"
        );
        
        txn.exec(
            "CREATE TABLE IF NOT EXISTS quiz_results ("
            "name VARCHAR(50) NOT NULL,"
            "score INTEGER NOT NULL,"
            "correct_answers INTEGER NOT NULL,"
            "total_questions INTEGER NOT NULL,"
            "category VARCHAR(50),"
            "FOREIGN KEY (name) REFERENCES players(name) ON DELETE CASCADE"
            ")"
        );
                
        txn.exec("CREATE INDEX IF NOT EXISTS idx_players_score ON players(total_score DESC)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_quiz_results_player_name ON quiz_results(name)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_achievements_player_name ON achievements(name)");
        
        txn.commit();
        std::cout << "Database tables initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize tables: " << e.what() << std::endl;
        return false;
    }
}

std::string Database::hashPassword(const std::string& password) {
    std::string salt = "astrolearn_salt_2024";
    std::string to_hash = password + salt;
    
    std::size_t hash1 = std::hash<std::string>{}(to_hash);
    std::size_t hash2 = std::hash<std::string>{}(salt + password);
    
    std::stringstream ss;
    ss << std::hex 
       << std::setw(16) << std::setfill('0') << hash1
       << std::setw(16) << std::setfill('0') << hash2;
    
    return "simple$" + salt + "$" + ss.str();
}

bool Database::verifyPassword(const std::string& password, const std::string& hash) {
    size_t first_dollar = hash.find('$');
    size_t second_dollar = hash.find('$', first_dollar + 1);
    
    if (first_dollar == std::string::npos || second_dollar == std::string::npos) {
        return false;
    }
    
    std::string algorithm = hash.substr(0, first_dollar);
    std::string salt = hash.substr(first_dollar + 1, second_dollar - first_dollar - 1);
    std::string stored_hash = hash.substr(second_dollar + 1);
    
    if (algorithm == "simple") {
        std::string to_hash = password + salt;
        
        std::size_t hash1 = std::hash<std::string>{}(to_hash);
        std::size_t hash2 = std::hash<std::string>{}(salt + password);
        
        std::stringstream ss;
        ss << std::hex 
           << std::setw(16) << std::setfill('0') << hash1
           << std::setw(16) << std::setfill('0') << hash2;
        
        return ss.str() == stored_hash;
    }
    
    return false;
}

std::time_t Database::stringToTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    
    if (ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S")) {
        return std::mktime(&tm);
    }
    
    ss.clear();
    ss.str(timeStr);
    if (ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S.%f")) {
        return std::mktime(&tm);
    }
    
    return std::time(nullptr);
}

std::string Database::timeToString(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int Database::createPlayer(const std::string& name, const std::string& password) {
    try {
        pqxx::work txn(*connection);
        
        std::string password_hash = hashPassword(password);
        
        auto result = txn.exec_params(
            "SELECT name FROM players WHERE name = $1",
            name
        );
        
        if (!result.empty()) {
            std::cout << "Player '" << name << "' already exists" << std::endl;
            txn.abort();
            return -2;
        }
        
        txn.exec_params(
            "INSERT INTO players (name, password_hash) VALUES ($1, $2)",
            name, password_hash
        );
        
        txn.commit();
        
        std::cout << "Created new player: " << name << std::endl;
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to create player: " << e.what() << std::endl;
        return -1;
    }
}

Database::PlayerData Database::authenticatePlayer(const std::string& name, const std::string& password) {
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name, password_hash, total_score, "
            "quizzes_completed, created_at, last_played "
            "FROM players WHERE name = $1",
            name
        );
        
        if (result.empty()) {
            throw std::runtime_error("Player not found with name: " + name);
        }
        
        std::string stored_hash = result[0]["password_hash"].as<std::string>();
        
        if (stored_hash.empty()) {
            throw std::runtime_error("Player account has no password set");
        }
        
        if (!verifyPassword(password, stored_hash)) {
            throw std::runtime_error("Invalid password for player: " + name);
        }
        
        return playerFromRow(result[0]);
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to authenticate player: " << e.what() << std::endl;
        throw;
    }
}

bool Database::updatePassword(const std::string& playerName, const std::string& newPassword) {
    try {
        pqxx::work txn(*connection);
        
        std::string password_hash = hashPassword(newPassword);
        
        txn.exec_params(
            "UPDATE players SET password_hash = $1 WHERE name = $2",
            password_hash, playerName
        );
        
        txn.commit();
        std::cout << "Password updated for player: " << playerName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to update password: " << e.what() << std::endl;
        return false;
    }
}

bool Database::updatePlayer(const PlayerData& player) {
    try {
        pqxx::work txn(*connection);
        
        txn.exec_params(
            "UPDATE players SET "
            "total_score = $1, "
            "quizzes_completed = $2, "
            "last_played = CURRENT_TIMESTAMP "
            "WHERE name = $3",
            player.totalScore,
            player.quizzesCompleted,
            player.name
        );
        
        txn.commit();
        std::cout << "Updated player: " << player.name << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to update player: " << e.what() << std::endl;
        return false;
    }
}

Database::PlayerData Database::getPlayerByName(const std::string& name) {
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name, password_hash, total_score, "
            "quizzes_completed, created_at, last_played "
            "FROM players WHERE name = $1",
            name
        );
        
        if (result.empty()) {
            throw std::runtime_error("Player not found with name: " + name);
        }
        
        return playerFromRow(result[0]);
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get player by name: " << e.what() << std::endl;
        throw;
    }
}

Database::PlayerData Database::playerFromRow(const pqxx::row& row) {
    PlayerData player;
    
    player.name = row["name"].as<std::string>();
    player.password_hash = row["password_hash"].as<std::string>();
    player.totalScore = row["total_score"].as<int>();
    player.quizzesCompleted = row["quizzes_completed"].as<int>();
    
    if (!row["created_at"].is_null()) {
        std::string createdStr = row["created_at"].as<std::string>();
        player.createdAt = stringToTime(createdStr);
    } else {
        player.createdAt = std::time(nullptr);
    }
    
    if (!row["last_played"].is_null()) {
        std::string lastPlayedStr = row["last_played"].as<std::string>();
        player.lastPlayed = stringToTime(lastPlayedStr);
    } else {
        player.lastPlayed = std::time(nullptr);
    }
    
    return player;
}

std::vector<Database::PlayerData> Database::getAllPlayers() {
    std::vector<PlayerData> players;
    
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec(
            "SELECT name, password_hash, total_score, "
            "quizzes_completed, created_at, last_played "
            "FROM players ORDER BY total_score DESC"
        );
        
        for (const auto& row : result) {
            players.push_back(playerFromRow(row));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get all players: " << e.what() << std::endl;
    }
    
    return players;
}

std::vector<Database::PlayerData> Database::getTopPlayers(int limit) {
    std::vector<PlayerData> players;
    
    if (!connection || !connection->is_open()) {
        std::cout << "Database not connected, returning empty leaderboard" << std::endl;
        return players;
    }
    
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name, password_hash, total_score, "
            "quizzes_completed, created_at, last_played "
            "FROM players ORDER BY total_score DESC LIMIT $1",
            limit
        );
        
        for (const auto& row : result) {
            players.push_back(playerFromRow(row));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get top players: " << e.what() << std::endl;
    }
    
    return players;
}

bool Database::unlockAchievement(const std::string& playerName, const std::string& achievementId) {
    try {
        pqxx::work txn(*connection);
        
        auto checkResult = txn.exec_params(
            "SELECT name FROM achievements WHERE name = $1 AND achievement_id = $2",
            playerName, achievementId
        );
        
        if (!checkResult.empty()) {
            return true;
        }
        
        txn.exec_params(
            "INSERT INTO achievements (name, achievement_id) VALUES ($1, $2)",
            playerName, achievementId
        );
        
        txn.commit();
        std::cout << "Achievement '" << achievementId << "' unlocked for " << playerName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to unlock achievement: " << e.what() << std::endl;
        return false;
    }
}

bool Database::hasAchievement(const std::string& playerName, const std::string& achievementId) {
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name FROM achievements WHERE name = $1 AND achievement_id = $2",
            playerName, achievementId
        );
        
        return !result.empty();
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to check achievement: " << e.what() << std::endl;
        return false;
    }
}

Database::AchievementData Database::achievementFromRow(const pqxx::row& row) {
    AchievementData achievement;
    
    achievement.playerName = row["name"].as<std::string>();
    achievement.achievementId = row["achievement_id"].as<std::string>();
    
    if (!row["unlock_date"].is_null()) {
        std::string unlockStr = row["unlock_date"].as<std::string>();
        achievement.unlockDate = stringToTime(unlockStr);
    } else {
        achievement.unlockDate = std::time(nullptr);
    }
    
    return achievement;
}

std::vector<Database::AchievementData> Database::getPlayerAchievements(const std::string& playerName) {
    std::vector<AchievementData> achievements;
    
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name, achievement_id, unlock_date "
            "FROM achievements WHERE name = $1 ORDER BY unlock_date DESC",
            playerName
        );
        
        for (const auto& row : result) {
            achievements.push_back(achievementFromRow(row));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get player achievements: " << e.what() << std::endl;
    }
    
    return achievements;
}

bool Database::saveQuizResult(const QuizResultData& result) {
    try {
        pqxx::work txn(*connection);
        
        txn.exec_params(
            "INSERT INTO quiz_results (name, score, correct_answers, "
            "total_questions, category) "
            "VALUES ($1, $2, $3, $4, $5)",
            result.playerName, result.score, result.correctAnswers,
            result.totalQuestions, result.category
        );
        
        txn.exec_params(
            "UPDATE players SET "
            "quizzes_completed = quizzes_completed + 1, "
            "total_score = total_score + $1, "
            "last_played = CURRENT_TIMESTAMP "
            "WHERE name = $2",
            result.score, result.playerName
        );
        
        txn.commit();
        std::cout << "Quiz result saved for player " << result.playerName 
                  << " (Score: " << result.score << ")" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to save quiz result: " << e.what() << std::endl;
        return false;
    }
}

Database::QuizResultData Database::quizResultFromRow(const pqxx::row& row) {
    QuizResultData result;
    
    result.playerName = row["name"].as<std::string>();
    result.score = row["score"].as<int>();
    result.correctAnswers = row["correct_answers"].as<int>();
    result.totalQuestions = row["total_questions"].as<int>();
    result.category = row["category"].as<std::string>();
    
    return result;
}

std::vector<Database::QuizResultData> Database::getPlayerQuizHistory(const std::string& playerName, int limit) {
    std::vector<QuizResultData> results;
    
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec_params(
            "SELECT name, score, correct_answers, total_questions, "
            "accuracy, time_spent, category, completed_at "
            "FROM quiz_results WHERE name = $1 "
            "ORDER BY completed_at DESC LIMIT $2",
            playerName, limit
        );
        
        for (const auto& row : result) {
            results.push_back(quizResultFromRow(row));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get quiz history: " << e.what() << std::endl;
    }
    
    return results;
}

Database::GlobalStats Database::getGlobalStats() {
    GlobalStats stats{};
    
    try {
        pqxx::work txn(*connection);
        
        auto result = txn.exec(
            "SELECT "
            "COUNT(*) as total_players, "
            "COALESCE(SUM(quizzes_completed), 0) as total_quizzes "
            "FROM players"
        );
        
        if (!result.empty()) {
            const auto& row = result[0];
            stats.totalPlayers = row["total_players"].as<int>();
            stats.totalQuizzesCompleted = row["total_quizzes"].as<int>();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to get global stats: " << e.what() << std::endl;
    }
    
    return stats;
}
