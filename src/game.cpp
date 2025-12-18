#include "game.h"
#include "game_states.h"
#include "game_ui.h"
#include "game_logic.h"
#include "game_database.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

Game::Game()
    : window(sf::VideoMode(1024, 768), "AstroLearn", sf::Style::Close | sf::Style::Titlebar)
    , currentState(GameState::LOGIN)
    , isPausedFlag(false)
    , timeScale(1.0f)
    , showCursor(true)
    , cursorBlinkTime(0.5f)
    , cursorBlinkTimer(0.0f)
    , playerNameInput("")
    , playerPasswordInput("")
    , playerConfirmPasswordInput("")
    , playerNameConfirmed(false)
    , passwordEnterMode(false)
    , confirmPasswordMode(false)
    , foundExistingPlayer(false)
    , selectedPlanet("") {
    
    window.setFramerateLimit(60);
}

Game::~Game() {
    saveGame();
}

bool Game::init() {
    std::cout << "Initializing game..." << std::endl;
    
    Database& db = Database::getInstance();
    std::string connString = "dbname=astrolearn user=postgres password=postgres host=localhost port=5432";
    
    if (!db.connect(connString)) {
        std::cerr << "Warning: Could not connect to database. Game will run in offline mode." << std::endl;
    } else {
        std::cout << "Database connected successfully" << std::endl;
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(1024, 768), "AstroLearn", 
                  sf::Style::Close | sf::Style::Titlebar, settings);
    
    if (!window.isOpen()) {
        return false;
    }
    
    window.setFramerateLimit(60);
    
    std::vector<std::string> fontPaths = {
        "resources/fonts/Ubuntu-R.ttf",
        "resources/fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };
    
    bool fontLoaded = false;
    for (const auto& fontPath : fontPaths) {
        if (mainFont.loadFromFile(fontPath)) {
            std::cout << "Font loaded from: " << fontPath << std::endl;
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: No font file found. Using SFML default." << std::endl;
        sf::Text tempText;
        mainFont = *tempText.getFont();
    }
    
    sf::Image backgroundImage;
    backgroundImage.create(1024, 768, sf::Color(10, 10, 40));
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < 300; ++i) {
        int x = std::rand() % 1024;
        int y = std::rand() % 768;
        int brightness = 150 + std::rand() % 106;
        backgroundImage.setPixel(x, y, sf::Color(brightness, brightness, brightness));
    }
    
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);
    
    solarSystem = std::make_unique<SolarSystem>();
    solarSystem->init();
    
    quiz = std::make_unique<Quiz>();
    
    GameLogic::initPlanetInfo(this);
    GameLogic::initPlanetUnlockSystem(this);
    
    GameUI::initLoginScreen(this);
    GameUI::initMainMenu(this);
    GameUI::initQuizUI(this);
    GameUI::initPlanetInfoUI(this);
    GameUI::initStatisticsUI(this);
    
    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

void Game::run() {
    std::cout << "Starting game loop..." << std::endl;
    
    gameClock.restart();
    
    while (window.isOpen()) {
        handleEvents();
        
        if (currentState == GameState::LOGIN) {
            cursorBlinkTimer += deltaTime.asSeconds();
            if (cursorBlinkTimer >= cursorBlinkTime) {
                showCursor = !showCursor;
                cursorBlinkTimer = 0.0f;
            }
        }
        
        if (!isPausedFlag && currentState != GameState::LOGIN) {
            deltaTime = gameClock.restart();
            
            if (currentState == GameState::SOLAR_SYSTEM && solarSystem) {
                solarSystem->update(deltaTime.asSeconds() * timeScale);
            }
        } else {
            deltaTime = gameClock.restart();
        }
        
        render();
    }
    
    std::cout << "Game finished." << std::endl;
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
                
            case sf::Event::KeyPressed:
                handleKeyPress(event.key.code);
                break;
                
            case sf::Event::TextEntered:
                handleTextInput(event.text.unicode);
                break;
                
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                }
                break;
                
            case sf::Event::MouseMoved:
                handleMouseMove(event.mouseMove.x, event.mouseMove.y);
                break;
                
            default:
                break;
        }
    }
}

void Game::handleTextInput(sf::Uint32 unicode) {
    if (currentState == GameState::LOGIN) {
        bool isInNameInput = !playerNameConfirmed && !passwordEnterMode && !confirmPasswordMode;
        bool isInPasswordInput = passwordEnterMode && !confirmPasswordMode;
        bool isInConfirmPasswordInput = confirmPasswordMode;
        
        if (unicode == 8) {
            if (isInConfirmPasswordInput) {
                if (!playerConfirmPasswordInput.empty()) {
                    playerConfirmPasswordInput.pop_back();
                }
            }
            else if (isInPasswordInput) {
                if (!playerPasswordInput.empty()) {
                    playerPasswordInput.pop_back();
                }
            }
            else if (isInNameInput) {
                if (!playerNameInput.empty()) {
                    playerNameInput.pop_back();
                }
            }
            return;
        }
        else if (unicode == 13) {
            if (isInConfirmPasswordInput) {
                if (!playerConfirmPasswordInput.empty()) {
                    if (playerPasswordInput == playerConfirmPasswordInput) {
                        if (foundExistingPlayer) {
                            GameDatabase::loadExistingPlayer(this);
                        } else {
                            GameDatabase::createNewPlayer(this, playerNameInput, playerPasswordInput);
                        }
                    } else {
                        passwordEnterMode = true;
                        confirmPasswordMode = false;
                        playerPasswordInput = "";
                        playerConfirmPasswordInput = "";
                        std::cout << "Passwords do not match!" << std::endl;
                    }
                }
            }
            else if (isInPasswordInput) {
                if (!playerPasswordInput.empty()) {
                    confirmPasswordMode = true;
                }
            }
            else if (isInNameInput) {
                if (!playerNameInput.empty()) {
                    GameDatabase::checkPlayerName(this, playerNameInput);
                }
            }
        }
        else if (unicode >= 32 && unicode < 127) {
            char c = static_cast<char>(unicode);
            
            if (isInConfirmPasswordInput) {
                if (playerConfirmPasswordInput.length() < 50) {
                    playerConfirmPasswordInput += c;
                }
            }
            else if (isInPasswordInput) {
                if (playerPasswordInput.length() < 50) {
                    playerPasswordInput += c;
                }
            }
            else if (isInNameInput) {
                bool isValidChar = false;
                if (std::isalnum(c)) isValidChar = true;
                else if (c == ' ') isValidChar = true;
                else if (c == '_') isValidChar = true;
                else if (c == '-') isValidChar = true;
                
                if (isValidChar && playerNameInput.length() < 20) {
                    playerNameInput += c;
                }
            }
        }
    }
            else if (unicode == 13) {
                if (quiz->hasAnsweredCurrent()) {
                    quiz->nextQuestion();
                    std::cout << "Moving to next question" << std::endl;
                }
            }
    else if (currentState == GameState::MAIN_MENU) {
        if (unicode == '1' || unicode == 'q' || unicode == 'Q') {
            currentState = GameState::SOLAR_SYSTEM;
            currentButtons.clear();
            currentButtons.push_back(GameUI::createButton(
                "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
            ));
        }
        else if (unicode == '2' || unicode == 'w' || unicode == 'W') {
            quiz->startNewQuiz("planets", 5);
            currentState = GameState::QUIZ;
            currentButtons = quizButtons;
        }
        else if (unicode == 27) {
            currentState = GameState::LOGIN;
            resetLoginState();
            currentButtons = loginButtons;
        }
    }
}

void Game::handleMouseClick(int x, int y) {
    updateButtonState(x, y);
    
    if (currentState == GameState::LOGIN) {
        for (auto& button : currentButtons) {
            if (button.isPressed) {
                if (button.text == "Login" && !playerNameInput.empty()) {
                    GameDatabase::checkPlayerName(this, playerNameInput);
                }
                else if (button.text == "Register New Player" && !playerNameInput.empty()) {
                    foundExistingPlayer = false;
                    playerNameConfirmed = true;
                    passwordEnterMode = true;
                    confirmPasswordMode = false;
                    playerPasswordInput = "";
                    playerConfirmPasswordInput = "";
                    
                    currentButtons.clear();
                    currentButtons.push_back(loginButtons[2]);
                    currentButtons.push_back(loginButtons[3]);
                }
                else if (button.text == "Confirm Password") {
                    if (passwordEnterMode && !playerPasswordInput.empty() && !confirmPasswordMode) {
                        confirmPasswordMode = true;
                        passwordEnterMode = false;
                        playerConfirmPasswordInput = "";
                    }
                    else if (confirmPasswordMode && !playerConfirmPasswordInput.empty()) {
                        if (playerPasswordInput == playerConfirmPasswordInput) {
                            if (foundExistingPlayer) {
                                GameDatabase::loadExistingPlayer(this);
                            } else {
                                GameDatabase::createNewPlayer(this, playerNameInput, playerPasswordInput);
                            }
                        } else {
                            passwordEnterMode = true;
                            confirmPasswordMode = false;
                            playerPasswordInput = "";
                            playerConfirmPasswordInput = "";
                        }
                    }
                }
                else if (button.text == "Back") {
                    if (confirmPasswordMode) {
                        confirmPasswordMode = false;
                        passwordEnterMode = true;
                        playerConfirmPasswordInput = "";
                    }
                    else if (passwordEnterMode) {
                        passwordEnterMode = false;
                        playerPasswordInput = "";
                        playerNameConfirmed = false;
                        foundExistingPlayer = false;
                        
                        currentButtons.clear();
                        if (!playerNameInput.empty()) {
                            currentButtons.push_back(loginButtons[0]);
                            currentButtons.push_back(loginButtons[1]);
                        }
                        currentButtons.push_back(loginButtons[4]);
                    }
                    else if (playerNameConfirmed) {
                        playerNameConfirmed = false;
                        playerNameInput = "";
                        foundExistingPlayer = false;
                        
                        currentButtons.clear();
                        currentButtons.push_back(loginButtons[4]);
                    }
                }
                else if (button.text == "Exit Game") {
                    window.close();
                }
                break;
            }
        }
    }

    else if (currentState == GameState::SOLAR_SYSTEM) {
        if (solarSystem) {
            solarSystem->selectBodyAt(x, y);
            CelestialBody* selectedBody = solarSystem->getSelectedBody();
            
            if (selectedBody) {
                std::string planetName = selectedBody->getName();
                
                GameLogic::selectPlanet(this, planetName);
            }
        }
        
        for (auto& button : currentButtons) {
            if (button.isPressed) {
                executeButtonAction(button);
                break;
            }
        }
    }
    else {
        for (auto& button : currentButtons) {
            if (button.isPressed) {
                executeButtonAction(button);
                break;
            }
        }
    }
}

void Game::executeButtonAction(const Button& button) {
    std::cout << "Button clicked: " << button.text << std::endl;
    
    if (button.text == "Exit" || button.text == "Exit Game") {
        saveGame();
        window.close();
        return;
    }
    
    if (button.text == "Exit Quiz") {
        currentState = GameState::MAIN_MENU;
        currentButtons = mainMenuButtons;
        return;
    }
    
    if (button.text == "Retry Incorrect Questions" && quiz) {
        if (quiz->needsRetry() && !quiz->hasRetryMode()) {
            int requiredCorrect = planetUnlockRequirement[selectedPlanet];
            quiz->startRetryIncorrectQuestions(requiredCorrect);
            currentState = GameState::QUIZ;
            
            quizButtons.clear();
            quizButtons.push_back(GameUI::createButton(
                "Next Question", 
                362, 
                550, 
                300, 
                50, 
                GameState::QUIZ
            ));
            quizButtons.push_back(GameUI::createButton(
                "Exit Quiz", 
                362, 
                610, 
                300, 
                50, 
                GameState::MAIN_MENU
            ));
            quizButtons.push_back(GameUI::createButton(
                "Return to " + selectedPlanet + " Info", 
                362, 
                670, 
                300, 
                50, 
                GameState::PLANET_INFO
            ));
            
            currentButtons = quizButtons;
            
            std::cout << "Started retry quiz with " << quiz->getCurrentRetryQuestionCount() 
                      << " questions" << std::endl;
        }
        return;
    }
    
    if (button.text.find("Quiz") != std::string::npos && !selectedPlanet.empty()) {
        std::string quizPlanet = selectedPlanet;
        if (button.text.find("Take ") == 0) {
            size_t start = 5;
            size_t end = button.text.find(" Quiz");
            if (end != std::string::npos) {
                quizPlanet = button.text.substr(start, end - start);
            }
        }
        GameLogic::startPlanetQuiz(this, quizPlanet);
        return;
    }
    
    if (button.text == "Next Question" && quiz) {
        if (quiz->hasAnsweredCurrent()) {
            quiz->nextQuestion();
        }
        return;
    }
    
    if (button.text == "Take Another Quiz") {
        if (!selectedPlanet.empty() && GameLogic::isPlanetUnlocked(this, selectedPlanet)) {
            GameLogic::startPlanetQuiz(this, selectedPlanet);
        } else {
            quiz->startNewQuiz("planets", 5);
            currentState = GameState::QUIZ;
            currentButtons = quizButtons;
        }
        return;
    }
    
    if (button.text == "Back to Name Input") {
        playerNameInput = "";
        playerNameConfirmed = false;
        passwordEnterMode = false;
        confirmPasswordMode = false;
        foundExistingPlayer = false;
        
        GameUI::initLoginScreen(this);
        return;
    }
    
    if (button.text == "Return to Solar System" || button.text == "Back to Solar System") {
        currentState = GameState::SOLAR_SYSTEM;
        currentButtons.clear();
        currentButtons.push_back(GameUI::createButton(
            "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
        ));
        return;
    }
    
    if (button.text.find("Return to ") == 0 && button.text.find(" Info") != std::string::npos) {
        currentState = GameState::PLANET_INFO;
        return;
    }
    
    if (button.text == "Return to Menu") {
        currentState = GameState::MAIN_MENU;
        currentButtons = mainMenuButtons;
        return;
    }
    
    if (button.text == "Study More") {
        if (player && !selectedPlanet.empty()) {
            player->recordStudy(selectedPlanet, 60);
        }
        currentState = GameState::SOLAR_SYSTEM;
        currentButtons.clear();
        currentButtons.push_back(GameUI::createButton(
            "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
        ));
        return;
    }
    
    if (button.text == "Back" || button.text == "Back to Menu") {
        if (currentState == GameState::MAIN_MENU) {
            currentState = GameState::LOGIN;
            resetLoginState();
            currentButtons = loginButtons;
        } else {
            currentState = GameState::MAIN_MENU;
            currentButtons = mainMenuButtons;
        }
        return;
    }
    
    if (button.text == "Solar System") {
        currentState = GameState::SOLAR_SYSTEM;
        currentButtons.clear();
        currentButtons.push_back(GameUI::createButton(
            "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
        ));
        return;
    }
    
    if (button.text == "Achievements") {
        currentState = GameState::ACHIEVEMENTS;
        currentButtons.clear();
        currentButtons.push_back(GameUI::createButton(
            "Back", 20, 20, 100, 40, GameState::MAIN_MENU
        ));
        return;
    }
    
    if (button.text == "Statistics") {
        currentState = GameState::STATISTICS;
        currentButtons = statisticsButtons;
        return;
    }
    
    if (currentState == GameState::LOGIN) {
        if (button.text == "Continue" && !playerNameInput.empty()) {
            GameDatabase::checkPlayerName(this, playerNameInput);
            return;
        }
        else if (button.text == "Create New Player") {
            GameDatabase::createNewPlayer(this, playerNameInput, playerPasswordInput);
            return;
        }
        else if (button.text == "Load Existing Player") {
            GameDatabase::loadExistingPlayer(this);
            return;
        }
        else if (button.text == "Create New Instead") {
            GameDatabase::createNewPlayer(this, playerNameInput, playerPasswordInput);
            return;
        }
        else if (button.text == "Back") {
            playerNameConfirmed = false;
            playerNameInput = "";
            foundExistingPlayer = false;
            return;
        }
    }
    
    if (button.targetState != currentState) {
        currentState = button.targetState;
        
        switch (currentState) {
            case GameState::LOGIN:
                currentButtons = loginButtons;
                break;
            case GameState::MAIN_MENU:
                currentButtons = mainMenuButtons;
                break;
            case GameState::QUIZ:
                if (selectedPlanet.empty()) {
                    quiz->startNewQuiz("planets", 5);
                }
                currentButtons = quizButtons;
                break;
            case GameState::SOLAR_SYSTEM:
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
                ));
                break;
            case GameState::PLANET_INFO:
                currentButtons = planetInfoButtons;
                break;
            case GameState::ACHIEVEMENTS:
            case GameState::STATISTICS:
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back", 20, 20, 100, 40, GameState::MAIN_MENU
                ));
                break;
        }
    }
}

void Game::handleMouseMove(int x, int y) {
    updateButtonState(x, y);
}

void Game::updateButtonState(int x, int y) {
    for (auto& button : currentButtons) {
        button.isHovered = false;
        button.isPressed = false;
        
        if (x >= button.x && x <= button.x + button.width &&
            y >= button.y && y <= button.y + button.height) {
            button.isHovered = true;
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                button.isPressed = true;
            }
        }
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Escape:
            if (currentState == GameState::LOGIN) {
                if (confirmPasswordMode) {
                    confirmPasswordMode = false;
                    passwordEnterMode = true;
                    playerConfirmPasswordInput = "";
                    std::cout << "Back to password input" << std::endl;
                }
                else if (passwordEnterMode) {
                    passwordEnterMode = false;
                    playerPasswordInput = "";
                    playerNameConfirmed = false;
                    std::cout << "Back to name input" << std::endl;
                }
                else if (playerNameConfirmed) {
                    playerNameConfirmed = false;
                    playerNameInput = "";
                    foundExistingPlayer = false;
                    std::cout << "Cleared name input" << std::endl;
                }
                else {
                    window.close();
                }
            }
            else if (currentState == GameState::QUIZ) {
                if (quiz && !quiz->isFinished()) {
                    currentState = GameState::PLANET_INFO;
                    currentButtons = planetInfoButtons;
                    std::cout << "Exited quiz via ESC" << std::endl;
                } else {
                    currentState = GameState::MAIN_MENU;
                    currentButtons = mainMenuButtons;
                }
            }
            else if (currentState == GameState::SOLAR_SYSTEM) {
                currentState = GameState::MAIN_MENU;
                currentButtons = mainMenuButtons;
            }
            else if (currentState == GameState::PLANET_INFO) {
                currentState = GameState::SOLAR_SYSTEM;
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
                ));
            }
            else if (currentState == GameState::ACHIEVEMENTS || 
                     currentState == GameState::STATISTICS) {
                currentState = GameState::MAIN_MENU;
                currentButtons = mainMenuButtons;
            }
            else if (currentState == GameState::MAIN_MENU) {
                currentState = GameState::LOGIN;
                resetLoginState();
                currentButtons = loginButtons;
            }
            break;
            
        case sf::Keyboard::Enter:
            if (currentState == GameState::LOGIN) {
                if (playerNameConfirmed && passwordEnterMode && !confirmPasswordMode) {
                    if (!playerPasswordInput.empty()) {
                        confirmPasswordMode = true;
                    }
                }
                else if (playerNameConfirmed && confirmPasswordMode) {
                    if (!playerConfirmPasswordInput.empty()) {
                        if (playerPasswordInput == playerConfirmPasswordInput) {
                            if (foundExistingPlayer) {
                                GameDatabase::loadExistingPlayer(this);
                            } else {
                                GameDatabase::createNewPlayer(this, playerNameInput, playerPasswordInput);
                            }
                        }
                    }
                }
                else if (!playerNameInput.empty() && !playerNameConfirmed) {
                    GameDatabase::checkPlayerName(this, playerNameInput);
                }
            }
            else if (currentState == GameState::QUIZ && quiz) {
                if (quiz->hasAnsweredCurrent() && !quiz->isFinished()) {
                    quiz->nextQuestion();
                    std::cout << "Moving to next question" << std::endl;
                } else if (quiz->isFinished()) {
                    currentButtons.clear();
                }
            }
            break;
            
        case sf::Keyboard::BackSpace:
            break;
            
        case sf::Keyboard::Num1:
        case sf::Keyboard::Numpad1:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(0);
                std::cout << "Selected answer 1" << std::endl;
            }
            break;
            
        case sf::Keyboard::Num2:
        case sf::Keyboard::Numpad2:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(1);
                std::cout << "Selected answer 2" << std::endl;
            }
            break;
            
        case sf::Keyboard::Num3:
        case sf::Keyboard::Numpad3:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(2);
                std::cout << "Selected answer 3" << std::endl;
            }
            break;
            
        case sf::Keyboard::Num4:
        case sf::Keyboard::Numpad4:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(3);
                std::cout << "Selected answer 4" << std::endl;
            }
            break;
            
        case sf::Keyboard::Q:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(0);
                std::cout << "Selected answer 1 (Q)" << std::endl;
            }
            break;
            
        case sf::Keyboard::W:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(1);
                std::cout << "Selected answer 2 (W)" << std::endl;
            }
            break;
            
        case sf::Keyboard::E:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(2);
                std::cout << "Selected answer 3 (E)" << std::endl;
            }
            break;
            
        case sf::Keyboard::R:
            if (currentState == GameState::QUIZ && quiz && !quiz->hasAnsweredCurrent()) {
                quiz->submitAnswer(3);
                std::cout << "Selected answer 4 (R)" << std::endl;
            }
            break;
            
        case sf::Keyboard::F1:
            if (currentState != GameState::LOGIN) {
                currentState = GameState::SOLAR_SYSTEM;
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back to Menu", 20, 20, 150, 40, GameState::MAIN_MENU
                ));
            }
            break;
            
        case sf::Keyboard::F4:
            if (currentState != GameState::LOGIN) {
                currentState = GameState::ACHIEVEMENTS;
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back", 20, 20, 100, 40, GameState::MAIN_MENU
                ));
            }
            break;
            
        case sf::Keyboard::F5:
            if (currentState != GameState::LOGIN) {
                currentState = GameState::STATISTICS;
                currentButtons.clear();
                currentButtons.push_back(GameUI::createButton(
                    "Back", 20, 20, 100, 40, GameState::MAIN_MENU
                ));
            }
            break;
            
        case sf::Keyboard::Space:
            if (currentState != GameState::LOGIN) {
                isPausedFlag = !isPausedFlag;
                std::cout << (isPausedFlag ? "Game paused" : "Game resumed") << std::endl;
            }
            break;
            
        default:
            break;
    }
}

void Game::render() {
    window.clear(sf::Color(10, 10, 40));
    
    window.draw(backgroundSprite);
    
    switch (currentState) {
        case GameState::LOGIN:
            GameStates::renderLogin(this);
            break;
            
        case GameState::MAIN_MENU:
            GameStates::renderMainMenu(this);
            break;
            
        case GameState::SOLAR_SYSTEM:
            GameStates::renderSolarSystem(this);
            break;
            
        case GameState::QUIZ:
            GameStates::renderQuiz(this);
            break;
            
        case GameState::PLANET_INFO:
            GameStates::renderPlanetInfo(this);
            break;
            
        case GameState::ACHIEVEMENTS:
            GameStates::renderAchievements(this);
            break;
            
        case GameState::STATISTICS:
            GameStates::renderStatistics(this);
            break;
    }
    
    if (currentState != GameState::LOGIN) {
        GameStates::renderHUD(this);
    }
    
    window.display();
}

void Game::saveGame() {
    if (player) {
        std::string playerName = player->getName();
        
        std::string progressFilename = "planet_progress_" + playerName + ".dat";
        std::string saveFilename = "savegame_" + playerName + ".dat";
        
        std::ofstream progressFile(progressFilename);
        if (progressFile.is_open()) {
            progressFile << "[PlanetUnlockStatus]\n";
            for (const auto& [name, unlocked] : planetUnlockStatus) {
                progressFile << name << "=" << (unlocked ? "1" : "0") << "\n";
            }
            progressFile.close();
            std::cout << "Planet progress saved to: " << progressFilename << std::endl;
        }
        
        if (player->saveToFile(saveFilename)) {
            std::cout << "Player data saved to: " << saveFilename << std::endl;
        }
        
        if (player->saveToDatabase()) {
            std::cout << "Game saved to database!" << std::endl;
        }
    }
}

void Game::loadGame() {
    if (!player) {
        GameLogic::initPlanetUnlockSystem(this);
        return;
    }
    
    std::string playerName = player->getName();
    std::string progressFilename = "planet_progress_" + playerName + ".dat";
    std::string saveFilename = "savegame_" + playerName + ".dat";
    
    std::ifstream progressFile(progressFilename);
    bool hasExistingSave = progressFile.good();
    
    if (hasExistingSave && progressFile.is_open()) {
        std::string line;
        std::string currentSection;
        
        while (std::getline(progressFile, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string planetName = line.substr(0, equalsPos);
                std::string status = line.substr(equalsPos + 1);
                
                if (planetUnlockStatus.find(planetName) != planetUnlockStatus.end()) {
                    planetUnlockStatus[planetName] = (status == "1");
                }
            }
        }
        
        progressFile.close();
        std::cout << "Planet progress loaded from: " << progressFilename << std::endl;
    } else {
        GameLogic::initPlanetUnlockSystem(this);
    }
    
    planetUnlockStatus["Sun"] = true;
    
    if (player->loadFromFile(saveFilename)) {
        std::cout << "Player data loaded from: " << saveFilename << std::endl;
    }
    
    int unlockedCount = 0;
    for (const auto& [name, unlocked] : planetUnlockStatus) {
        if (unlocked) unlockedCount++;
    }
    std::cout << "Loaded unlocked planets for " << playerName 
              << ": " << unlockedCount << "/" << planetUnlockStatus.size() << std::endl;
}

void Game::cleanupOldSaves(const std::string& currentPlayerName) {
    std::string oldProgressFile = "planet_progress.dat";
    std::ifstream testOld(oldProgressFile);
    if (testOld.good()) {
        testOld.close();
        if (std::remove(oldProgressFile.c_str()) == 0) {
            std::cout << "Removed old planet progress file: " << oldProgressFile << std::endl;
        }
    }
    
    std::string oldSaveFile = "savegame.dat";
    std::ifstream testOldSave(oldSaveFile);
    if (testOldSave.good()) {
        testOldSave.close();
        if (std::remove(oldSaveFile.c_str()) == 0) {
            std::cout << "Removed old savegame file: " << oldSaveFile << std::endl;
        }
    }
    
    std::string currentProgressFile = "planet_progress_" + currentPlayerName + ".dat";
    std::string currentSaveFile = "savegame_" + currentPlayerName + ".dat";
    
    std::ifstream testCurrentProgress(currentProgressFile);
    if (testCurrentProgress.good()) {
        std::cout << "Found existing save for player: " << currentPlayerName << std::endl;
    }
    
    std::ifstream testCurrentSave(currentSaveFile);
    if (testCurrentSave.good()) {
        std::cout << "Found existing player data for: " << currentPlayerName << std::endl;
    }
}

void Game::resetLoginState() {
    playerNameInput = "";
    playerPasswordInput = "";
    playerConfirmPasswordInput = "";
    playerNameConfirmed = false;
    passwordEnterMode = false;
    confirmPasswordMode = false;
    foundExistingPlayer = false;
    showCursor = true;
    cursorBlinkTimer = 0.0f;
}

bool Game::isPlanetUnlocked(const std::string& planetName) const {
    auto it = planetUnlockStatus.find(planetName);
    if (it != planetUnlockStatus.end()) {
        return it->second;
    }
    return false;
}

void Game::unlockPlanet(const std::string& planetName) {
    auto it = planetUnlockStatus.find(planetName);
    if (it != planetUnlockStatus.end()) {
        it->second = true;
        std::cout << "Planet unlocked: " << planetName << std::endl;
        saveGame();
    }
}

std::string Game::getBodyType(const std::string& bodyName) {
    if (bodyName == "Sun") return "Star";
    if (bodyName == "Pluto") return "Dwarf Planet";
    return "Planet";
}
