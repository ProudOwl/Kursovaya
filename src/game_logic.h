#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game.h"

class GameLogic {
public:
    static void initPlanetInfo(Game* game);
    static void initPlanetUnlockSystem(Game* game);
    
    static void selectPlanet(Game* game, const std::string& planetName);
    static void startPlanetQuiz(Game* game, const std::string& planetName);
    static bool isPlanetUnlocked(Game* game, const std::string& planetName);
    static bool canUnlockPlanet(Game* game, const std::string& planetName);
    static void unlockPlanet(Game* game, const std::string& planetName);
    static void updatePlanetUnlockStatus(Game* game);
};

#endif
