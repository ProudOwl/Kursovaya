#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "solar_system.h"
#include "quiz.h"
#include "player.h"
#include "database.h"

class Game {
public:
    Game();
    ~Game();
    
    bool init();
    void run();
    
    enum class GameState {
        LOGIN,
        MAIN_MENU,
        SOLAR_SYSTEM,
        QUIZ,
        PLANET_INFO,
        ACHIEVEMENTS,
        STATISTICS
    };
    
    struct Button {
        std::string text;
        float x, y, width, height;
        GameState targetState;
        bool isHovered;
        bool isPressed;
    };
    
    struct PlanetInfo {
        std::string name;
        std::string description;
        std::vector<std::string> facts;
        int quizQuestionsRequired;
        std::string nextPlanet;
    };
    
    struct ExistingPlayerData {
        std::string name;
        int level;
        int score;
    };

    struct QuizCompletionData {
    bool processed = false;
    std::string planetName;
    int score = 0;
    int correctAnswers = 0;
    int totalQuestions = 0;
    };

    QuizCompletionData lastQuizCompletion;
    
    sf::RenderWindow& getWindow() { return window; }
    sf::Font& getFont() { return mainFont; }
    sf::Sprite& getBackgroundSprite() { return backgroundSprite; }
    GameState getCurrentState() const { return currentState; }
    void setCurrentState(GameState state) { currentState = state; }
    
    std::string getSelectedPlanet() const { return selectedPlanet; }
    void setSelectedPlanet(const std::string& planet) { selectedPlanet = planet; }
    
    bool isPaused() const { return isPausedFlag; }
    void setPaused(bool paused) { isPausedFlag = paused; }
    
    float getDeltaTime() const { return deltaTime.asSeconds(); }
    float getCursorBlinkTimer() const { return cursorBlinkTimer; }
    void setCursorBlinkTimer(float timer) { cursorBlinkTimer = timer; }
    bool getShowCursor() const { return showCursor; }
    void setShowCursor(bool show) { showCursor = show; }
    
    bool isPlanetUnlocked(const std::string& planetName) const;
    void unlockPlanet(const std::string& planetName);
    
    std::unique_ptr<Player>& getPlayer() { return player; }
    std::unique_ptr<Quiz>& getQuiz() { return quiz; }
    std::unique_ptr<SolarSystem>& getSolarSystem() { return solarSystem; }
    
    std::vector<Button> currentButtons;
    std::map<std::string, bool> planetUnlockStatus;
    std::map<std::string, PlanetInfo> planetInfo;
    std::map<std::string, std::string> planetQuizCategory;
    std::map<std::string, int> planetUnlockRequirement;
    
    std::string playerNameInput;
    std::string playerPasswordInput;
    std::string playerConfirmPasswordInput;
    bool playerNameConfirmed;
    bool passwordEnterMode;
    bool confirmPasswordMode;
    bool foundExistingPlayer;
    bool showCursor;
    ExistingPlayerData existingPlayerData;
    
    void handleEvents();
    void handleKeyPress(sf::Keyboard::Key key);
    void handleTextInput(sf::Uint32 unicode);
    void handleMouseClick(int x, int y);
    void handleMouseMove(int x, int y);
    void updateButtonState(int x, int y);
    void executeButtonAction(const Button& button);
    
    void saveGame();
    void loadGame();
    void cleanupOldSaves(const std::string& currentPlayerName);
    void resetLoginState();
    
    std::string getBodyType(const std::string& bodyName);

    std::vector<Button> loginButtons;
    std::vector<Button> mainMenuButtons;
    std::vector<Button> quizButtons;
    std::vector<Button> planetInfoButtons;
    std::vector<Button> statisticsButtons;

private:
    sf::RenderWindow window;
    sf::Font mainFont;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    
    std::unique_ptr<SolarSystem> solarSystem;
    std::unique_ptr<Quiz> quiz;
    std::unique_ptr<Player> player;
    
    GameState currentState;
    bool isPausedFlag;
    float timeScale;
    
    sf::Clock gameClock;
    sf::Time deltaTime;
    
    float cursorBlinkTime;
    float cursorBlinkTimer;
    
    std::string selectedPlanet;
    
    void render();
};

#endif
