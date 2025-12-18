#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "game.h"

class GameStates {
public:
    static void renderLogin(Game* game);
    static void renderMainMenu(Game* game);
    static void renderSolarSystem(Game* game);
    static void renderQuiz(Game* game);
    static void renderPlanetInfo(Game* game);
    static void renderAchievements(Game* game);
    static void renderStatistics(Game* game);
    static void renderHUD(Game* game);
};

#endif
