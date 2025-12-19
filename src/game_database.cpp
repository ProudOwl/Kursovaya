#include "game_states.h"
#include "game_ui.h"
#include "game_database.h"
#include "game_logic.h"
#include <iostream>

void GameDatabase::checkPlayerName(Game* game, const std::string& name) {
    Database& db = Database::getInstance();
    
    if (db.isConnected()) {
        try {
            auto dbPlayerData = db.getPlayerByName(name);
            
            game->existingPlayerData.name = dbPlayerData.name;
            game->existingPlayerData.score = dbPlayerData.totalScore;
            
            game->foundExistingPlayer = true;
            game->playerNameConfirmed = true;
            game->passwordEnterMode = true;
            game->confirmPasswordMode = false;
            game->playerPasswordInput = "";
            game->playerConfirmPasswordInput = "";
            
            game->currentButtons.clear();
            game->currentButtons.push_back(game->loginButtons[2]);
            game->currentButtons.push_back(game->loginButtons[3]);
            
        } catch (const std::exception& e) {
            std::cout << "Player '" << name << "' not found. Please register first." << std::endl;
        }
    } else {
        createNewPlayer(game, name, "");
    }
}

void GameDatabase::createNewPlayer(Game* game, const std::string& name, const std::string& password) {
    Database& db = Database::getInstance();
    
    if (db.isConnected()) {
        try {
            int result = db.createPlayer(name, password);
            
            if (result > 0) {
                game->getPlayer() = std::make_unique<Player>(name);
                if (game->getPlayer()->initialize()) {
                    std::cout << "New player created: " << name << std::endl;
                    finishLogin(game);
                } else {
                    std::cerr << "Failed to initialize new player" << std::endl;
                    game->resetLoginState();
                }
            } else if (result == -2) {
                game->foundExistingPlayer = false;
                game->playerNameConfirmed = true;
                game->passwordEnterMode = false;
                game->confirmPasswordMode = false;
                
                game->loginButtons.clear();
                
                float centerX = 1024 / 2.0f;
                float startY = 350.0f;
                
                game->loginButtons.push_back(GameUI::createButton(
                    "Player '" + name + "' already exists!",
                    centerX - 200,
                    startY - 60,
                    400,
                    40,
                    Game::GameState::LOGIN
                ));
                game->loginButtons.back().isHovered = false;
                
                game->loginButtons.push_back(GameUI::createButton(
                    "Login Instead",
                    centerX - 150,
                    startY,
                    300,
                    50,
                    Game::GameState::LOGIN
                ));
                
                game->loginButtons.push_back(GameUI::createButton(
                    "Try Different Name",
                    centerX - 150,
                    startY + 70,
                    300,
                    50,
                    Game::GameState::LOGIN
                ));
                
                game->loginButtons.push_back(GameUI::createButton(
                    "Exit Game",
                    centerX - 150,
                    startY + 140,
                    300,
                    50,
                    Game::GameState::LOGIN
                ));
                
                game->currentButtons = game->loginButtons;
                
                std::cout << "Player name already exists: " << name << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to create player in database: " << e.what() << std::endl;
            game->getPlayer() = std::make_unique<Player>(name);
            game->getPlayer()->initialize();
            finishLogin(game);
        }
    } else {
        game->getPlayer() = std::make_unique<Player>(name);
        game->getPlayer()->initialize();
        finishLogin(game);
    }
    
    game->passwordEnterMode = false;
    game->confirmPasswordMode = false;
    game->playerPasswordInput = "";
    game->playerConfirmPasswordInput = "";
}

void GameDatabase::loadExistingPlayer(Game* game) {
    try {
        Database& db = Database::getInstance();
        
        auto authenticatedPlayer = db.authenticatePlayer(game->existingPlayerData.name, game->playerPasswordInput);
        
        game->getPlayer() = std::make_unique<Player>(authenticatedPlayer.name);
        if (game->getPlayer()->initialize()) {
            std::cout << "Existing player loaded: " << game->getPlayer()->getName() << std::endl;
            finishLogin(game);
        } else {
            std::cerr << "Failed to load existing player" << std::endl;
            game->resetLoginState();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading player: " << e.what() << std::endl;
        game->passwordEnterMode = true;
        game->playerPasswordInput = "";
        std::cout << "Invalid password, please try again" << std::endl;
    }
}

void GameDatabase::finishLogin(Game* game) {
    game->setCurrentState(Game::GameState::MAIN_MENU);
    game->currentButtons = game->mainMenuButtons;
    
    if (game->getPlayer()) {
        game->cleanupOldSaves(game->getPlayer()->getName());
    }
    
    game->loadGame();
    
    GameLogic::updatePlanetUnlockStatus(game);
    
    game->saveGame();
    
    game->resetLoginState();
}

std::vector<Database::PlayerData> GameDatabase::getAllPlayersFromDB() {
    Database& db = Database::getInstance();
    if (db.isConnected()) {
        return db.getAllPlayers();
    }
    return {};
}

std::vector<Database::QuizResultData> GameDatabase::getPlayerQuizHistoryFromDB(const std::string& playerName) {
    Database& db = Database::getInstance();
    if (db.isConnected()) {
        return db.getPlayerQuizHistory(playerName, 50);
    }
    return {};
}
