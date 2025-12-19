#include "game_database.h"
#include "game_ui.h"

void GameUI::initLoginScreen(Game* game) {
    game->loginButtons.clear();
    
    float centerX = 1024 / 2.0f;
    float startY = 400.0f;
    float buttonWidth = 300.0f;
    float buttonHeight = 50.0f;
    
    game->loginButtons.push_back(createButton(
        "Login", 
        centerX - buttonWidth/2, 
        startY, 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::LOGIN
    ));
    
    game->loginButtons.push_back(createButton(
        "Register New Player", 
        centerX - buttonWidth/2, 
        startY + 70,
        buttonWidth, 
        buttonHeight, 
        Game::GameState::LOGIN
    ));
    
    game->loginButtons.push_back(createButton(
        "Confirm Password", 
        centerX - buttonWidth/2, 
        startY + 140, 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::LOGIN
    ));
    
    game->loginButtons.push_back(createButton(
        "Back", 
        centerX - buttonWidth/2, 
        startY + 210, 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::LOGIN
    ));
    
    game->loginButtons.push_back(createButton(
        "Exit Game", 
        centerX - buttonWidth/2, 
        startY + 210, 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::LOGIN
    ));
    
    game->currentButtons.clear();
    game->currentButtons.push_back(game->loginButtons[0]);
    game->currentButtons.push_back(game->loginButtons[1]);
    game->currentButtons.push_back(game->loginButtons[4]);
}

void GameUI::initMainMenu(Game* game) {
    game->mainMenuButtons.clear();
    
    float centerX = 1024 / 2.0f;
    float startY = 300.0f;
    float buttonWidth = 300.0f;
    float buttonHeight = 60.0f;
    float spacing = 20.0f;
    
    game->mainMenuButtons.push_back(createButton(
        "Solar System", 
        centerX - buttonWidth/2, 
        startY, 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::SOLAR_SYSTEM
    ));
    
    game->mainMenuButtons.push_back(createButton(
        "Achievements", 
        centerX - buttonWidth/2, 
        startY + (buttonHeight + spacing), 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::ACHIEVEMENTS
    ));
    
    game->mainMenuButtons.push_back(createButton(
        "Statistics", 
        centerX - buttonWidth/2, 
        startY + 2*(buttonHeight + spacing), 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::STATISTICS
    ));
    
    game->mainMenuButtons.push_back(createButton(
        "Exit", 
        centerX - buttonWidth/2, 
        startY + 3*(buttonHeight + spacing), 
        buttonWidth, 
        buttonHeight, 
        Game::GameState::MAIN_MENU
    ));
}

void GameUI::initQuizUI(Game* game) {
    game->quizButtons.clear();
    
    game->quizButtons.push_back(createButton(
        "Next Question", 
        362, 
        500, 
        300, 
        50, 
        Game::GameState::QUIZ
    ));
    
    game->quizButtons.push_back(createButton(
        "Exit Quiz", 
        362, 
        560, 
        300, 
        50, 
        Game::GameState::MAIN_MENU
    ));
    
    game->quizButtons.push_back(createButton(
        "Return to Solar System", 
        362, 
        620, 
        300, 
        50, 
        Game::GameState::SOLAR_SYSTEM
    ));
}

void GameUI::initPlanetInfoUI(Game* game) {
    game->planetInfoButtons.clear();
    
    game->planetInfoButtons.push_back(createButton(
        "Take Quiz", 
        362, 
        550, 
        300, 
        50, 
        Game::GameState::QUIZ
    ));
    
    game->planetInfoButtons.push_back(createButton(
        "Back to Solar System", 
        362, 
        610, 
        300, 
        50, 
        Game::GameState::SOLAR_SYSTEM
    ));
    
    game->planetInfoButtons.push_back(createButton(
        "Study More", 
        362, 
        490, 
        300, 
        50, 
        Game::GameState::SOLAR_SYSTEM
    ));
}

void GameUI::initStatisticsUI(Game* game) {
    game->statisticsButtons.clear();
    
    game->statisticsButtons.push_back(createButton(
        "Back", 
        20, 
        20, 
        100, 
        40, 
        Game::GameState::MAIN_MENU
    ));
}

Game::Button GameUI::createButton(const std::string& text, float x, float y, 
                         float width, float height, Game::GameState targetState) {
    Game::Button button;
    button.text = text;
    button.x = x;
    button.y = y;
    button.width = width;
    button.height = height;
    button.targetState = targetState;
    button.isHovered = false;
    button.isPressed = false;
    return button;
}

void GameUI::drawButton(const Game::Button& button, sf::RenderWindow& window, sf::Font& font) {
    sf::RectangleShape buttonRect(sf::Vector2f(button.width, button.height));
    buttonRect.setPosition(button.x, button.y);
    
    if (button.isPressed) {
        buttonRect.setFillColor(sf::Color(100, 100, 200));
    } else if (button.isHovered) {
        buttonRect.setFillColor(sf::Color(80, 80, 180));
    } else {
        buttonRect.setFillColor(sf::Color(60, 60, 160));
    }
    
    buttonRect.setOutlineThickness(2);
    buttonRect.setOutlineColor(sf::Color::White);
    window.draw(buttonRect);
    
    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(button.text);
    buttonText.setCharacterSize(24);
    buttonText.setFillColor(sf::Color::White);
    
    sf::FloatRect bounds = buttonText.getLocalBounds();
    buttonText.setPosition(button.x + button.width/2.0f - bounds.width/2.0f, 
                          button.y + button.height/2.0f - bounds.height/2.0f - 5);
    
    window.draw(buttonText);
}

void GameUI::renderMultiplePlayersStats(Game* game, sf::RenderWindow& window, sf::Font& font) {
    try {
        auto allPlayers = GameDatabase::getAllPlayersFromDB();
        
        float startY = 120.0f;
        float columnWidth = 500.0f;
        int playersPerColumn = 5;
        int currentColumn = 0;
        
        sf::Text columnTitle;
        columnTitle.setFont(font);
        columnTitle.setString("Player Statistics:");
        columnTitle.setCharacterSize(24);
        columnTitle.setFillColor(sf::Color::Yellow);
        columnTitle.setPosition(50, startY);
        window.draw(columnTitle);
        
        startY += 40;
        
        for (size_t i = 0; i < allPlayers.size(); ++i) {
            const auto& playerData = allPlayers[i];
            
            if (i % playersPerColumn == 0 && i > 0) {
                currentColumn++;
                startY = 160.0f;
            }
            
            float xPos = 50 + (currentColumn * columnWidth);
            float yPos = startY + ((i % playersPerColumn) * 90.0f);
            
            sf::RectangleShape playerBg(sf::Vector2f(480, 80));
            playerBg.setPosition(xPos, yPos);
            playerBg.setFillColor(sf::Color(30, 30, 60, 200));
            playerBg.setOutlineThickness(2);
            playerBg.setOutlineColor(sf::Color(100, 100, 150));
            
            if (game->getPlayer() && playerData.name == game->getPlayer()->getName()) {
                playerBg.setOutlineColor(sf::Color::Yellow);
                playerBg.setOutlineThickness(3);
            }
            
            window.draw(playerBg);
            
            sf::Text playerName;
            playerName.setFont(font);
            playerName.setString(playerData.name);
            playerName.setCharacterSize(20);
            playerName.setFillColor(game->getPlayer() && playerData.name == game->getPlayer()->getName() ? 
                                   sf::Color::Yellow : sf::Color::White);
            playerName.setPosition(xPos + 10, yPos + 5);
            window.draw(playerName);
            
            sf::Text playerStats;
            playerStats.setFont(font);
            playerStats.setString("Score: " + std::to_string(playerData.totalScore) +
                                 "\nQuizzes: " + std::to_string(playerData.quizzesCompleted));
            playerStats.setCharacterSize(16);
            playerStats.setFillColor(sf::Color(200, 200, 255));
            playerStats.setLineSpacing(1.1f);
            playerStats.setPosition(xPos + 10, yPos + 30);
            window.draw(playerStats);
            
            if (playerData.lastPlayed > 0) {
                std::time_t now = std::time(nullptr);
                double daysSince = std::difftime(now, playerData.lastPlayed) / (60 * 60 * 24);
                
                sf::Text lastPlayed;
                lastPlayed.setFont(font);
                lastPlayed.setString("Last: " + std::to_string(static_cast<int>(daysSince)) + " days ago");
                lastPlayed.setCharacterSize(14);
                lastPlayed.setFillColor(sf::Color(150, 150, 150));
                lastPlayed.setPosition(xPos + 350, yPos + 5);
                window.draw(lastPlayed);
            }
        }
        
        if (allPlayers.size() > 15) {
            sf::Text warning;
            warning.setFont(font);
            warning.setString("Showing first " + std::to_string(allPlayers.size()) + " players");
            warning.setCharacterSize(18);
            warning.setFillColor(sf::Color(255, 150, 150));
            warning.setPosition(50, 650);
            window.draw(warning);
        }
        
    } catch (const std::exception& e) {
        sf::Text error;
        error.setFont(font);
        error.setString("Error loading player statistics:\n" + std::string(e.what()));
        error.setCharacterSize(20);
        error.setFillColor(sf::Color::Red);
        error.setLineSpacing(1.2f);
        error.setPosition(100, 150);
        window.draw(error);
    }
}
