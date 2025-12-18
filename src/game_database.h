#ifndef GAME_DATABASE_H
#define GAME_DATABASE_H

#include "game.h"

class GameDatabase {
public:
    static void checkPlayerName(Game* game, const std::string& name);
    static void createNewPlayer(Game* game, const std::string& name, const std::string& password);
    static void loadExistingPlayer(Game* game);
    static void finishLogin(Game* game);
    
    static std::vector<Database::PlayerData> getAllPlayersFromDB();
    static std::vector<Database::QuizResultData> getPlayerQuizHistoryFromDB(const std::string& playerName);
};

#endif
