#ifndef GAME_UI_H
#define GAME_UI_H

#include "game.h"

class GameUI {
public:
    static void initLoginScreen(Game* game);
    static void initMainMenu(Game* game);
    static void initQuizUI(Game* game);
    static void initPlanetInfoUI(Game* game);
    static void initStatisticsUI(Game* game);
    
    static Game::Button createButton(const std::string& text, float x, float y, 
                                   float width, float height, Game::GameState targetState);
    static void drawButton(const Game::Button& button, sf::RenderWindow& window, sf::Font& font);
    
    static void renderMultiplePlayersStats(Game* game, sf::RenderWindow& window, sf::Font& font);
};

#endif
