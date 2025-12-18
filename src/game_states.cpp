#include "game_states.h"
#include "game_ui.h"
#include "game_database.h"
#include "game_logic.h"

void GameStates::renderLogin(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    sf::Text title;
    title.setFont(font);
    title.setString("ASTROLEARN");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(1024/2.0f - titleBounds.width/2.0f, 100);
    window.draw(title);
    
    if (!game->playerNameConfirmed) {
        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("Enter Your Astronaut Name");
        subtitle.setCharacterSize(24);
        subtitle.setFillColor(sf::Color::White);
        sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
        subtitle.setPosition(1024/2.0f - subtitleBounds.width/2.0f, 180);
        window.draw(subtitle);
        
        sf::RectangleShape inputBg(sf::Vector2f(600, 60));
        inputBg.setPosition(1024/2.0f - 300, 250);
        inputBg.setFillColor(sf::Color(30, 30, 60, 200));
        inputBg.setOutlineThickness(2);
        inputBg.setOutlineColor(sf::Color::White);
        window.draw(inputBg);
        
        std::string displayText = game->playerNameInput;
        if (game->getShowCursor() && !game->passwordEnterMode) {
            displayText += "|";
        }
        
        sf::Text inputText;
        inputText.setFont(font);
        inputText.setString(displayText);
        inputText.setCharacterSize(28);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(1024/2.0f - 290, 260);
        window.draw(inputText);
        
        sf::Text instruction;
        instruction.setFont(font);
        instruction.setString("Type your name and press Enter or click a button\nUse letters, numbers, spaces, underscores and hyphens\nMax 20 characters");
        instruction.setCharacterSize(18);
        instruction.setFillColor(sf::Color(200, 200, 200));
        instruction.setLineSpacing(1.2f);
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(1024/2.0f - instructionBounds.width/2.0f, 320);
        window.draw(instruction);
        
    } else if (game->passwordEnterMode && !game->confirmPasswordMode) {
        sf::Text subtitle;
        subtitle.setFont(font);
        if (game->foundExistingPlayer) {
            subtitle.setString("Enter Password for " + game->existingPlayerData.name);
        } else {
            subtitle.setString("Create Password for " + game->playerNameInput);
        }
        subtitle.setCharacterSize(24);
        subtitle.setFillColor(sf::Color::White);
        sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
        subtitle.setPosition(1024/2.0f - subtitleBounds.width/2.0f, 180);
        window.draw(subtitle);
        
        sf::RectangleShape inputBg(sf::Vector2f(600, 60));
        inputBg.setPosition(1024/2.0f - 300, 250);
        inputBg.setFillColor(sf::Color(30, 30, 60, 200));
        inputBg.setOutlineThickness(2);
        inputBg.setOutlineColor(sf::Color::White);
        window.draw(inputBg);
        
        std::string displayText = std::string(game->playerPasswordInput.length(), '*');
        if (game->getShowCursor()) {
            displayText += "|";
        }
        
        sf::Text inputText;
        inputText.setFont(font);
        inputText.setString(displayText);
        inputText.setCharacterSize(28);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(1024/2.0f - 290, 260);
        window.draw(inputText);
        
        sf::Text instruction;
        instruction.setFont(font);
        if (game->foundExistingPlayer) {
            instruction.setString("Enter your password and press Enter");
        } else {
            instruction.setString("Create a secure password and press Enter");
        }
        instruction.setCharacterSize(18);
        instruction.setFillColor(sf::Color(200, 200, 200));
        instruction.setLineSpacing(1.2f);
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(1024/2.0f - instructionBounds.width/2.0f, 320);
        window.draw(instruction);
        
    } else if (game->confirmPasswordMode) {
        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("Confirm Password for " + game->playerNameInput);
        subtitle.setCharacterSize(24);
        subtitle.setFillColor(sf::Color::White);
        sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
        subtitle.setPosition(1024/2.0f - subtitleBounds.width/2.0f, 180);
        window.draw(subtitle);
        
        sf::RectangleShape inputBg(sf::Vector2f(600, 60));
        inputBg.setPosition(1024/2.0f - 300, 250);
        inputBg.setFillColor(sf::Color(30, 30, 60, 200));
        inputBg.setOutlineThickness(2);
        inputBg.setOutlineColor(sf::Color::White);
        window.draw(inputBg);
        
        std::string displayText = std::string(game->playerConfirmPasswordInput.length(), '*');
        if (game->getShowCursor()) {
            displayText += "|";
        }
        
        sf::Text inputText;
        inputText.setFont(font);
        inputText.setString(displayText);
        inputText.setCharacterSize(28);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(1024/2.0f - 290, 260);
        window.draw(inputText);
        
        sf::Text instruction;
        instruction.setFont(font);
        instruction.setString("Re-enter your password to confirm");
        instruction.setCharacterSize(18);
        instruction.setFillColor(sf::Color(200, 200, 200));
        instruction.setLineSpacing(1.2f);
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(1024/2.0f - instructionBounds.width/2.0f, 320);
        window.draw(instruction);
    }
    
    for (const auto& button : game->currentButtons) {
        GameUI::drawButton(button, window, font);
    }
    
    sf::Text footer;
    footer.setFont(font);
    std::string footerText = "Press ESC to ";
    if (game->playerNameConfirmed) {
        footerText += "go back";
    } else {
        footerText += "exit";
    }
    footerText += " | Press Enter to confirm";
    footer.setString(footerText);
    footer.setCharacterSize(18);
    footer.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect footerBounds = footer.getLocalBounds();
    footer.setPosition(1024/2.0f - footerBounds.width/2.0f, 680);
    window.draw(footer);
}

void GameStates::renderMainMenu(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    if (game->getPlayer()) {
        sf::Text playerInfo;
        playerInfo.setFont(font);
        playerInfo.setString("Astronaut: " + game->getPlayer()->getName() + 
                           " | Score: " + std::to_string(game->getPlayer()->getScore()));
        playerInfo.setCharacterSize(20);
        playerInfo.setFillColor(sf::Color(200, 200, 255));
        playerInfo.setStyle(sf::Text::Bold);
        sf::FloatRect infoBounds = playerInfo.getLocalBounds();
        playerInfo.setPosition(1024/2.0f - infoBounds.width/2.0f, 30);
        window.draw(playerInfo);
    }
    
    sf::Text title;
    title.setFont(font);
    title.setString("ASTROLEARN");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(1024/2.0f - titleBounds.width/2.0f, 120);
    window.draw(title);
    
    sf::Text subtitle;
    subtitle.setFont(font);
    subtitle.setString("Explore the Solar System");
    subtitle.setCharacterSize(24);
    subtitle.setFillColor(sf::Color::White);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition(1024/2.0f - subtitleBounds.width/2.0f, 200);
    window.draw(subtitle);
    
    for (const auto& button : game->currentButtons) {
        GameUI::drawButton(button, window, font);
    }
}

void GameStates::renderSolarSystem(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    if (game->getSolarSystem()) {
        game->getSolarSystem()->draw(window);
    }
    
    sf::Text title;
    title.setFont(font);
    title.setString("Solar System Explorer - " + (game->getPlayer() ? game->getPlayer()->getName() : "Unknown"));
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(20, 60);
    window.draw(title);
    
    int unlockedCount = 0;
    for (const auto& [name, unlocked] : game->planetUnlockStatus) {
        if (unlocked) unlockedCount++;
    }
    
    sf::Text instruction;
    instruction.setFont(font);
    instruction.setString("Click on celestial bodies to learn more and take quizzes\n"
                         "Unlocked bodies: " + std::to_string(unlockedCount) + "/" + 
                         std::to_string(game->planetUnlockStatus.size()));
    instruction.setCharacterSize(18);
    instruction.setFillColor(sf::Color(200, 200, 200));
    instruction.setPosition(20, 90);
    window.draw(instruction);
    
    float startY = 130.0f;
    sf::Text unlockInfo;
    unlockInfo.setFont(font);
    unlockInfo.setCharacterSize(16);
    unlockInfo.setFillColor(sf::Color(180, 180, 220));
    unlockInfo.setPosition(20, startY);
    
    std::string unlockText = "Celestial Body Status:\n";
    unlockText += (game->planetUnlockStatus["Sun"] ? "+ " : "- ") + std::string("Sun (Star)") + "\n";
    
    std::vector<std::string> planetOrder = {"Mercury", "Venus", "Earth", "Mars", 
                                           "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"};
    
    for (const auto& planet : planetOrder) {
        if (game->planetUnlockStatus.find(planet) != game->planetUnlockStatus.end()) {
            std::string type = (planet == "Pluto") ? " (Dwarf Planet)" : " (Planet)";
            unlockText += (game->planetUnlockStatus[planet] ? "+ " : "- ") + planet + type + "\n";
        }
    }
    
    unlockInfo.setString(unlockText);
    window.draw(unlockInfo);
    
    for (const auto& button : game->currentButtons) {
        GameUI::drawButton(button, window, font);
    }
    
    if (game->isPaused()) {
        sf::Text pause;
        pause.setFont(font);
        pause.setString("PAUSED");
        pause.setCharacterSize(48);
        pause.setFillColor(sf::Color::Red);
        pause.setStyle(sf::Text::Bold);
        sf::FloatRect pauseBounds = pause.getLocalBounds();
        pause.setPosition(1024/2.0f - pauseBounds.width/2.0f, 384);
        window.draw(pause);
    }
}

void GameStates::renderQuiz(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    if (!game->getQuiz()) {
        return;
    }
    
    if (game->getQuiz()->isFinished()) {
        // Проверяем режим повторения
        if (game->getQuiz()->needsRetry() && !game->getQuiz()->hasRetryMode()) {
            sf::Text title;
            title.setFont(font);
            title.setString("Quiz Results - Need More Practice!");
            title.setCharacterSize(36);
            title.setFillColor(sf::Color::Yellow);
            title.setStyle(sf::Text::Bold);
            sf::FloatRect titleBounds = title.getLocalBounds();
            title.setPosition(1024.0f / 2.0f - titleBounds.width / 2.0f, 100.0f);
            window.draw(title);
            
            int correctCount = 0;
            int totalQuestions = game->getQuiz()->getTotalQuestions();
            const auto& userAnswers = game->getQuiz()->getUserAnswers();
            const auto& questions = game->getQuiz()->getSelectedQuestions();
            
            for (size_t i = 0; i < questions.size() && i < userAnswers.size(); ++i) {
                if (userAnswers[i] == questions[i].correctAnswer) {
                    correctCount++;
                }
            }
            
            int requiredCorrect = game->getQuiz()->getRequiredCorrectAnswers();
            
            sf::Text resultText;
            resultText.setFont(font);
            resultText.setString("You got " + std::to_string(correctCount) + 
                               " out of " + std::to_string(totalQuestions) + " correct.\n" +
                               "You need at least " + std::to_string(requiredCorrect) + 
                               " correct answers to unlock the next planet.\n\n" +
                               "You will now retry the questions you answered incorrectly.");
            resultText.setCharacterSize(24);
            resultText.setFillColor(sf::Color::White);
            resultText.setLineSpacing(1.3f);
            sf::FloatRect resultBounds = resultText.getLocalBounds();
            resultText.setPosition(1024.0f / 2.0f - resultBounds.width / 2.0f, 180.0f);
            window.draw(resultText);
            
            float startY = 350.0f;
            sf::Text incorrectTitle;
            incorrectTitle.setFont(font);
            incorrectTitle.setString("Questions to retry:");
            incorrectTitle.setCharacterSize(22);
            incorrectTitle.setFillColor(sf::Color::Red);
            incorrectTitle.setPosition(100.0f, startY);
            window.draw(incorrectTitle);
            
            startY += 40.0f;
            int incorrectCount = 0;
            for (size_t i = 0; i < questions.size() && i < userAnswers.size(); ++i) {
                if (userAnswers[i] != questions[i].correctAnswer) {
                    incorrectCount++;
                    
                    sf::Text questionText;
                    questionText.setFont(font);
                    sf::String displayText = sf::String(std::to_string(incorrectCount)) + ". " + sf::String(questions[i].text);
                    questionText.setString(displayText);
                    questionText.setCharacterSize(18);
                    questionText.setFillColor(sf::Color(255, 200, 200));
                    questionText.setPosition(120.0f, startY);
                    
                    sf::FloatRect bounds = questionText.getLocalBounds();
                    if (bounds.width > 800.0f) {
                        questionText.setScale(800.0f / bounds.width, 1.0f);
                    }
                    
                    window.draw(questionText);
                    startY += 30.0f;
                }
            }
            
            game->currentButtons.clear();
            game->currentButtons.push_back(GameUI::createButton("Retry Incorrect Questions", 362.0f, 550.0f, 300.0f, 50.0f, Game::GameState::QUIZ));
            game->currentButtons.push_back(GameUI::createButton("Exit Quiz", 362.0f, 610.0f, 300.0f, 50.0f, Game::GameState::MAIN_MENU));
            game->currentButtons.push_back(GameUI::createButton("Return to Planet Info", 362.0f, 670.0f, 300.0f, 50.0f, Game::GameState::PLANET_INFO));
            
            for (const auto& button : game->currentButtons) {
                GameUI::drawButton(button, window, font);
            }
            
            return;
        }
        
        if (game->getQuiz()->hasRetryMode()) {
            auto result = game->getQuiz()->getCurrentResult();
            
            sf::Text title;
            title.setFont(font);
            title.setString("Retry Quiz Completed!");
            title.setCharacterSize(36);
            title.setFillColor(sf::Color::Yellow);
            title.setStyle(sf::Text::Bold);
            sf::FloatRect titleBounds = title.getLocalBounds();
            title.setPosition(1024.0f / 2.0f - titleBounds.width / 2.0f, 100.0f);
            window.draw(title);
            
            int requiredCorrect = game->getQuiz()->getRequiredCorrectAnswers();
            
            sf::Text resultText;
            resultText.setFont(font);
            if (result.correctAnswers >= requiredCorrect) {
                resultText.setString(std::string("Congratulations! You passed!\n") +
                    "Correct answers: " + std::to_string(result.correctAnswers) + 
                    "/" + std::to_string(result.totalQuestions) +
                    "\nScore: " + std::to_string(result.score));
                resultText.setFillColor(sf::Color::Green);
            } else {
                resultText.setString(std::string("You still need more practice.\n") +
                    "Correct answers: " + std::to_string(result.correctAnswers) + 
                    "/" + std::to_string(result.totalQuestions) +
                    "\nRequired: " + std::to_string(requiredCorrect));
                resultText.setFillColor(sf::Color::Red);
            }
            
            resultText.setCharacterSize(28);
            resultText.setLineSpacing(1.2f);
            sf::FloatRect resultBounds = resultText.getLocalBounds();
            resultText.setPosition(1024.0f / 2.0f - resultBounds.width / 2.0f, 180.0f);
            window.draw(resultText);
            
            // ОДНОКРАТНАЯ обработка результатов ретрая
            if (!game->getSelectedPlanet().empty() && game->getPlayer() && result.correctAnswers >= requiredCorrect) {
                // Проверяем, не обрабатывали ли мы уже этот ретрай
                if (!game->lastQuizCompletion.processed || 
                    game->lastQuizCompletion.planetName != game->getSelectedPlanet() ||
                    game->lastQuizCompletion.score != result.score) {
                    
                    game->lastQuizCompletion.processed = true;
                    game->lastQuizCompletion.planetName = game->getSelectedPlanet();
                    game->lastQuizCompletion.score = result.score;
                    game->lastQuizCompletion.correctAnswers = result.correctAnswers;
                    game->lastQuizCompletion.totalQuestions = result.totalQuestions;
                    
                    game->getPlayer()->completeQuiz(result);
                    
                    std::string nextPlanet = game->planetInfo[game->getSelectedPlanet()].nextPlanet;
                    if (!nextPlanet.empty() && !GameLogic::isPlanetUnlocked(game, nextPlanet)) {
                        GameLogic::unlockPlanet(game, nextPlanet);
                        
                        sf::Text unlockText;
                        unlockText.setFont(font);
                        unlockText.setString("Congratulations!\nYou unlocked " + nextPlanet + "!");
                        unlockText.setCharacterSize(24);
                        unlockText.setFillColor(sf::Color::Green);
                        unlockText.setLineSpacing(1.3f);
                        sf::FloatRect unlockBounds = unlockText.getLocalBounds();
                        unlockText.setPosition(1024.0f / 2.0f - unlockBounds.width / 2.0f, 300.0f);
                        window.draw(unlockText);
                    }
                }
            }
            
            game->currentButtons.clear();
            game->currentButtons.push_back(GameUI::createButton("Return to Solar System", 362.0f, 450.0f, 300.0f, 50.0f, Game::GameState::SOLAR_SYSTEM));
            GameUI::drawButton(game->currentButtons[0], window, font);
            return;
        }
        
        auto result = game->getQuiz()->getCurrentResult();
        
        sf::Text title;
        title.setFont(font);
        title.setString("Quiz Completed!");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::Yellow);
        title.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setPosition(1024.0f / 2.0f - titleBounds.width / 2.0f, 100.0f);
        window.draw(title);
    
        // ВЫЧИСЛЯЕМ ПРОЦЕНТ ПРАВИЛЬНОСТИ
        float accuracy = 0.0f;
        if (result.totalQuestions > 0) {
            accuracy = (static_cast<float>(result.correctAnswers) / 
                       static_cast<float>(result.totalQuestions)) * 100.0f;
        }
        
        sf::Text resultText;
        resultText.setFont(font);
        resultText.setString("Correct: " + std::to_string(result.correctAnswers) + 
                           "/" + std::to_string(result.totalQuestions) +
                           " | Score: " + std::to_string(result.score) +
                           "\nAccuracy: " + std::to_string(static_cast<int>(accuracy)) + "%");
        resultText.setCharacterSize(28);
        resultText.setFillColor(sf::Color::White);
        resultText.setLineSpacing(1.2f);
        sf::FloatRect resultBounds = resultText.getLocalBounds();
        resultText.setPosition(1024.0f / 2.0f - resultBounds.width / 2.0f, 180.0f);
        window.draw(resultText);
    
        // ОДНОКРАТНАЯ обработка результатов обычного квиза
        if (!game->getSelectedPlanet().empty() && game->getPlayer()) {
            // Проверяем, не обрабатывали ли мы уже этот квиз
            if (!game->lastQuizCompletion.processed || 
                game->lastQuizCompletion.planetName != game->getSelectedPlanet() ||
                game->lastQuizCompletion.score != result.score) {
                
                // Обрабатываем результат только один раз
                game->lastQuizCompletion.processed = true;
                game->lastQuizCompletion.planetName = game->getSelectedPlanet();
                game->lastQuizCompletion.score = result.score;
                game->lastQuizCompletion.correctAnswers = result.correctAnswers;
                game->lastQuizCompletion.totalQuestions = result.totalQuestions;
                
                game->getPlayer()->completeQuiz(result);
                
                int requiredCorrect = game->planetUnlockRequirement[game->getSelectedPlanet()];
                if (result.correctAnswers >= requiredCorrect) {
                    std::string nextPlanet = game->planetInfo[game->getSelectedPlanet()].nextPlanet;
                    if (!nextPlanet.empty() && !GameLogic::isPlanetUnlocked(game, nextPlanet)) {
                        GameLogic::unlockPlanet(game, nextPlanet);
                        
                        sf::Text unlockText;
                        unlockText.setFont(font);
                        unlockText.setString("Congratulations!\nYou unlocked " + nextPlanet + "!");
                        unlockText.setCharacterSize(24);
                        unlockText.setFillColor(sf::Color::Green);
                        unlockText.setLineSpacing(1.3f);
                        sf::FloatRect unlockBounds = unlockText.getLocalBounds();
                        unlockText.setPosition(1024.0f / 2.0f - unlockBounds.width / 2.0f, 280.0f);
                        window.draw(unlockText);
                    }
                }
            }
        }
    
        game->currentButtons.clear();
        game->currentButtons.push_back(GameUI::createButton("Return to Solar System", 362.0f, 350.0f, 300.0f, 50.0f, Game::GameState::SOLAR_SYSTEM));
        game->currentButtons.push_back(GameUI::createButton("Take Another Quiz", 362.0f, 410.0f, 300.0f, 50.0f, Game::GameState::QUIZ));
    
        for (const auto& button : game->currentButtons) {
            GameUI::drawButton(button, window, font);
        }
        return;
    }
    
    // ... остальной код для отображения текущего вопроса ...
    auto currentQuestion = game->getQuiz()->getCurrentQuestion();
    int questionIndex = game->getQuiz()->getCurrentQuestionIndex() + 1;
    int totalQuestions = game->getQuiz()->getTotalQuestions();
    
    sf::Text title;
    title.setFont(font);
    std::string quizTitle = "Planet Quiz";
    if (!game->getSelectedPlanet().empty()) {
        quizTitle = game->getSelectedPlanet() + " Quiz";
    }
    title.setString(quizTitle + " - Question " + std::to_string(questionIndex) + " of " + std::to_string(totalQuestions));
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(1024.0f / 2.0f - titleBounds.width / 2.0f, 50.0f);
    window.draw(title);
    
    sf::Text question;
    question.setFont(font);
    question.setString(currentQuestion.text);
    question.setCharacterSize(24);
    question.setFillColor(sf::Color::White);
    question.setLineSpacing(1.2f);
    
    sf::FloatRect questionBounds = question.getLocalBounds();
    float maxWidth = 900.0f;
    if (questionBounds.width > maxWidth) {
        question.setScale(maxWidth / questionBounds.width, 1.0f);
        questionBounds = question.getLocalBounds();
    }
    
    question.setPosition(1024.0f / 2.0f - questionBounds.width / 2.0f, 120.0f);
    window.draw(question);
    
    bool hasAnswered = game->getQuiz()->hasAnsweredCurrent();
    int userAnswer = game->getQuiz()->getLastAnswer();
    
    float startY = 200.0f;
    for (size_t i = 0; i < currentQuestion.options.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        sf::String displayText = sf::String(std::to_string(i + 1)) + ". " + sf::String(currentQuestion.options[i]);
        option.setString(displayText);
        option.setCharacterSize(22);
        
        if (hasAnswered) {
            if (static_cast<int>(i) == currentQuestion.correctAnswer) {
                option.setFillColor(sf::Color::Green);
            } else if (static_cast<int>(i) == userAnswer) {
                option.setFillColor(sf::Color::Red);
            } else {
                option.setFillColor(sf::Color(150, 150, 150));
            }
        } else {
            option.setFillColor(sf::Color::White);
        }
        
        option.setLineSpacing(1.1f);
        sf::FloatRect optionBounds = option.getLocalBounds();
        option.setPosition(1024.0f / 2.0f - optionBounds.width / 2.0f, startY + i * 45.0f);
        window.draw(option);
    }
    
    if (hasAnswered) {
        sf::Text explanation;
        explanation.setFont(font);
        
        sf::String displayText = sf::String("Explanation: " + sf::String(currentQuestion.explanation));
        explanation.setString(displayText);
        explanation.setCharacterSize(20);
        explanation.setFillColor(sf::Color(200, 200, 255));
        explanation.setLineSpacing(1.1f);
        
        sf::FloatRect explanationBounds = explanation.getLocalBounds();
        float explanationMaxWidth = 900.0f;
        if (explanationBounds.width > explanationMaxWidth) {
            explanation.setScale(explanationMaxWidth / explanationBounds.width, 1.0f);
            explanationBounds = explanation.getLocalBounds();
        }
        
        explanation.setPosition(1024.0f / 2.0f - explanationBounds.width / 2.0f, 400.0f);
        window.draw(explanation);
        
        sf::Text feedback;
        feedback.setFont(font);
        if (userAnswer == currentQuestion.correctAnswer) {
            feedback.setString("Correct!");
            feedback.setFillColor(sf::Color::Green);
        } else {
            feedback.setString("Incorrect! The correct answer was option " + 
                             std::to_string(currentQuestion.correctAnswer + 1));
            feedback.setFillColor(sf::Color::Red);
        }
        feedback.setCharacterSize(24);
        sf::FloatRect feedbackBounds = feedback.getLocalBounds();
        feedback.setPosition(1024.0f / 2.0f - feedbackBounds.width / 2.0f, 360.0f);
        window.draw(feedback);
        
        sf::Text instruction;
        instruction.setFont(font);
        instruction.setString("Press Enter to continue to next question\nPress ESC to exit quiz");
        instruction.setCharacterSize(20);
        instruction.setFillColor(sf::Color(200, 255, 200));
        instruction.setLineSpacing(1.2f);
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(1024.0f / 2.0f - instructionBounds.width / 2.0f, 560.0f);
        window.draw(instruction);
    } else {
        sf::Text instruction;
        instruction.setFont(font);
        instruction.setString("Press 1-4 or Q/W/E/R to answer\nPress Enter to go to next question after answering\nPress ESC to exit quiz");
        instruction.setCharacterSize(20);
        instruction.setFillColor(sf::Color(255, 255, 200));
        instruction.setLineSpacing(1.2f);
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(1024.0f / 2.0f - instructionBounds.width / 2.0f, 500.0f);
        window.draw(instruction);
    }
}

void GameStates::renderPlanetInfo(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    if (game->getSelectedPlanet().empty()) {
        game->setCurrentState(Game::GameState::SOLAR_SYSTEM);
        return;
    }
    
    bool isUnlocked = GameLogic::isPlanetUnlocked(game, game->getSelectedPlanet());
    
    sf::RectangleShape panel(sf::Vector2f(900, 600));
    panel.setPosition(62, 84);
    panel.setFillColor(sf::Color(20, 20, 40, 220));
    panel.setOutlineThickness(3);
    panel.setOutlineColor(isUnlocked ? sf::Color::Yellow : sf::Color::Red);
    window.draw(panel);
    
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setString(game->getSelectedPlanet() + (isUnlocked ? "" : " (Locked)"));
    nameText.setCharacterSize(36);
    nameText.setFillColor(isUnlocked ? sf::Color::Yellow : sf::Color::Red);
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setPosition(1024/2.0f - nameBounds.width/2.0f, 100);
    window.draw(nameText);
    
    if (game->planetInfo.find(game->getSelectedPlanet()) != game->planetInfo.end()) {
        const auto& info = game->planetInfo[game->getSelectedPlanet()];
        
        sf::Text descText;
        descText.setFont(font);
        descText.setString(info.description);
        descText.setCharacterSize(22);
        descText.setFillColor(sf::Color::White);
        descText.setLineSpacing(1.3f);
        
        sf::FloatRect descBounds = descText.getLocalBounds();
        float descMaxWidth = 850.0f;
        if (descBounds.width > descMaxWidth) {
            descText.setScale(descMaxWidth / descBounds.width, 1.0f);
            descBounds = descText.getLocalBounds();
        }
        
        descText.setPosition(1024/2.0f - descBounds.width/2.0f, 160);
        window.draw(descText);
        
        float factsY = 220.0f;
        sf::Text factsTitle;
        factsTitle.setFont(font);
        factsTitle.setString("Interesting Facts:");
        factsTitle.setCharacterSize(24);
        factsTitle.setFillColor(sf::Color(255, 200, 100));
        factsTitle.setPosition(100, factsY);
        window.draw(factsTitle);
        
        factsY += 40;
        for (const auto& fact : info.facts) {
            sf::Text factText;
            factText.setFont(font);
            factText.setString("• " + fact);
            factText.setCharacterSize(20);
            factText.setFillColor(sf::Color(200, 220, 255));
            factText.setPosition(120, factsY);
            window.draw(factText);
            factsY += 30;
        }
        
        factsY += 20;
        sf::Text reqText;
        reqText.setFont(font);
        
        if (isUnlocked) {
            int totalQuestions = info.quizQuestionsRequired;
            int requiredCorrect = 0;
            
            if (game->planetUnlockRequirement.find(game->getSelectedPlanet()) != game->planetUnlockRequirement.end()) {
                requiredCorrect = game->planetUnlockRequirement[game->getSelectedPlanet()];
            } else {
                requiredCorrect = totalQuestions / 2 + 1;
            }
            
            std::string nextBody = info.nextPlanet;
            if (!nextBody.empty() && !GameLogic::isPlanetUnlocked(game, nextBody)) {
                reqText.setString("Quiz: " + std::to_string(totalQuestions) + " questions\n"
                                  "Required to unlock " + nextBody + ": " + 
                                  std::to_string(requiredCorrect) + "/" + 
                                  std::to_string(totalQuestions) + " correct answers");
            } else if (!nextBody.empty()) {
                reqText.setString("Quiz: " + std::to_string(totalQuestions) + " questions\n" +
                                  nextBody + " is already unlocked!");
            } else {
                reqText.setString("Quiz: " + std::to_string(totalQuestions) + " questions\n"
                                  "Final celestial body - congratulations!");
            }
            reqText.setFillColor(sf::Color::Green);
        } else {
            std::string requiredPlanet = "";
            for (const auto& [name, planetInfo] : game->planetInfo) {
                if (planetInfo.nextPlanet == game->getSelectedPlanet()) {
                    requiredPlanet = name;
                    break;
                }
            }
            
            if (!requiredPlanet.empty() && GameLogic::isPlanetUnlocked(game, requiredPlanet)) {
                int requiredCorrect = game->planetUnlockRequirement[game->getSelectedPlanet()];
                int totalQuestions = 15;
                if (game->planetInfo.find(requiredPlanet) != game->planetInfo.end()) {
                    totalQuestions = game->planetInfo[requiredPlanet].quizQuestionsRequired;
                }
                
                reqText.setString("To unlock " + game->getSelectedPlanet() + ":\n"
                                  "Complete " + requiredPlanet + " quiz with at least " +
                                  std::to_string(requiredCorrect) + "/" + 
                                  std::to_string(totalQuestions) + " correct answers");
            } else if (!requiredPlanet.empty()) {
                reqText.setString("To unlock " + game->getSelectedPlanet() + ":\n"
                                  "First unlock and complete " + requiredPlanet);
            } else {
                reqText.setString("This celestial body is currently locked.");
            }
            reqText.setFillColor(sf::Color::Red);
        }
        
        reqText.setCharacterSize(22);
        reqText.setLineSpacing(1.2f);
        reqText.setPosition(100, factsY);
        window.draw(reqText);
    }
    
    if (isUnlocked) {
        game->currentButtons.clear();
        game->currentButtons.push_back(GameUI::createButton(
            "Take " + game->getSelectedPlanet() + " Quiz", 
            362, 
            490, 
            300, 
            50, 
            Game::GameState::QUIZ
        ));
        game->currentButtons.push_back(GameUI::createButton(
            "Study More", 
            362, 
            550, 
            300, 
            50, 
            Game::GameState::SOLAR_SYSTEM
        ));
        game->currentButtons.push_back(GameUI::createButton(
            "Back to Solar System", 
            362, 
            610, 
            300, 
            50, 
            Game::GameState::SOLAR_SYSTEM
        ));
        
        for (const auto& button : game->currentButtons) {
            GameUI::drawButton(button, window, font);
        }
    } else {
        game->currentButtons.clear();
        game->currentButtons.push_back(GameUI::createButton(
            "Back to Solar System", 362, 550, 300, 50, Game::GameState::SOLAR_SYSTEM
        ));
        GameUI::drawButton(game->currentButtons[0], window, font);
        
        sf::Text lockText;
        lockText.setFont(font);
        lockText.setString("This celestial body is locked.\nComplete previous bodies to unlock it.");
        lockText.setCharacterSize(24);
        lockText.setFillColor(sf::Color::Red);
        lockText.setLineSpacing(1.3f);
        sf::FloatRect lockBounds = lockText.getLocalBounds();
        lockText.setPosition(1024/2.0f - lockBounds.width/2.0f, 450);
        window.draw(lockText);
    }
}

void GameStates::renderAchievements(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    sf::Text title;
    title.setFont(font);
    title.setString("Achievements - " + (game->getPlayer() ? game->getPlayer()->getName() : "Unknown"));
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(1024/2.0f - titleBounds.width/2.0f, 50);
    window.draw(title);
    
    if (game->getPlayer()) {
        auto achievements = game->getPlayer()->getAchievements();
        
        if (achievements.empty()) {
            sf::Text noAch;
            noAch.setFont(font);
            noAch.setString("No achievements yet");
            noAch.setCharacterSize(24);
            noAch.setFillColor(sf::Color::White);
            sf::FloatRect noAchBounds = noAch.getLocalBounds();
            noAch.setPosition(1024/2.0f - noAchBounds.width/2.0f, 200);
            window.draw(noAch);
        } else {
            float startY = 150.0f;
            
            for (size_t i = 0; i < achievements.size(); ++i) {
                const auto& ach = achievements[i];
                
                sf::RectangleShape bg(sf::Vector2f(900, 50));
                bg.setPosition(62, startY + i * 60);
                bg.setFillColor(sf::Color(30, 30, 60, 200));
                bg.setOutlineThickness(2);
                bg.setOutlineColor(ach.unlocked ? sf::Color::Yellow : sf::Color(100, 100, 100));
                window.draw(bg);
                
                std::string displayText = ach.name + " - " + ach.description;
                sf::Text name;
                name.setFont(font);
                name.setString(displayText);
                name.setCharacterSize(20);
                name.setFillColor(ach.unlocked ? sf::Color::White : sf::Color(150, 150, 150));
                
                sf::FloatRect nameBounds = name.getLocalBounds();
                if (nameBounds.width > 700) {
                    float scale = 700.0f / nameBounds.width;
                    name.setScale(scale, 1.0f);
                }
                
                name.setPosition(100, startY + i * 60 + 15);
                window.draw(name);
                
                sf::Text status;
                status.setFont(font);
                status.setString(ach.unlocked ? "UNLOCKED" : "LOCKED");
                status.setCharacterSize(18);
                status.setFillColor(ach.unlocked ? sf::Color::Green : sf::Color::Red);
                status.setPosition(800, startY + i * 60 + 15);
                window.draw(status);
            }
        }
    }
    
    for (const auto& button : game->currentButtons) {
        GameUI::drawButton(button, window, font);
    }
}

void GameStates::renderStatistics(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    sf::Text title;
    title.setFont(font);
    title.setString("Player Statistics");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(1024/2.0f - titleBounds.width/2.0f, 50);
    window.draw(title);
    
    try {
        auto allPlayers = GameDatabase::getAllPlayersFromDB();
        
        if (allPlayers.empty()) {
            sf::Text noData;
            noData.setFont(font);
            noData.setString("No player statistics available.\nDatabase is not connected or no players exist.");
            noData.setCharacterSize(24);
            noData.setFillColor(sf::Color::White);
            noData.setLineSpacing(1.5f);
            sf::FloatRect noDataBounds = noData.getLocalBounds();
            noData.setPosition(1024/2.0f - noDataBounds.width/2.0f, 200);
            window.draw(noData);
        } else {
            GameUI::renderMultiplePlayersStats(game, window, font);
        }
        
    } catch (const std::exception& e) {
        sf::Text error;
        error.setFont(font);
        error.setString("Error loading statistics:\n" + std::string(e.what()));
        error.setCharacterSize(24);
        error.setFillColor(sf::Color::Red);
        error.setLineSpacing(1.5f);
        sf::FloatRect errorBounds = error.getLocalBounds();
        error.setPosition(1024/2.0f - errorBounds.width/2.0f, 200);
        window.draw(error);
    }
    
    for (const auto& button : game->currentButtons) {
        GameUI::drawButton(button, window, font);
    }
    
    sf::Text instruction;
    instruction.setFont(font);
    instruction.setString("Press ESC to return");
    instruction.setCharacterSize(20);
    instruction.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect instructionBounds = instruction.getLocalBounds();
    instruction.setPosition(1024/2.0f - instructionBounds.width/2.0f, 700);
    window.draw(instruction);
}

void GameStates::renderHUD(Game* game) {
    sf::RenderWindow& window = game->getWindow();
    sf::Font& font = game->getFont();
    
    sf::RectangleShape panel(sf::Vector2f(1024, 30));
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(panel);
    
    std::string stateName;
    switch (game->getCurrentState()) {
        case Game::GameState::LOGIN: stateName = "Login"; break;
        case Game::GameState::MAIN_MENU: stateName = "Main Menu"; break;
        case Game::GameState::SOLAR_SYSTEM: stateName = "Solar System"; break;
        case Game::GameState::QUIZ: stateName = "Quiz"; break;
        case Game::GameState::PLANET_INFO: stateName = "Planet Info"; break;
        case Game::GameState::ACHIEVEMENTS: stateName = "Achievements"; break;
        case Game::GameState::STATISTICS: stateName = "Statistics"; break;
    }
    
    if (game->isPaused()) {
        stateName += " [PAUSED]";
    }
    
    sf::Text stateText;
    stateText.setFont(font);
    stateText.setString(stateName + " | Player: " + (game->getPlayer() ? game->getPlayer()->getName() : "Unknown") + 
                       " | Score: " + (game->getPlayer() ? std::to_string(game->getPlayer()->getScore()) : "0"));
    stateText.setCharacterSize(16);
    stateText.setFillColor(sf::Color::White);
    stateText.setPosition(10, 8);
    window.draw(stateText);
    
    if (game->getCurrentState() != Game::GameState::LOGIN) {
        int unlockedCount = 0;
        for (const auto& [name, unlocked] : game->planetUnlockStatus) {
            if (unlocked) unlockedCount++;
        }
        
        sf::Text planetCount;
        planetCount.setFont(font);
        planetCount.setString("Planets: " + std::to_string(unlockedCount) + "/" + 
                             std::to_string(game->planetUnlockStatus.size()));
        planetCount.setCharacterSize(16);
        planetCount.setFillColor(unlockedCount == game->planetUnlockStatus.size() ? 
                                sf::Color::Green : sf::Color::Yellow);
        planetCount.setPosition(800, 8);
        window.draw(planetCount);
    }
    
    if (game->getDeltaTime() > 0) {
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(1.0f / game->getDeltaTime()));
        
        sf::Text fpsDisplay;
        fpsDisplay.setFont(font);
        fpsDisplay.setString(fpsText);
        fpsDisplay.setCharacterSize(16);
        fpsDisplay.setFillColor(sf::Color::White);
        sf::FloatRect fpsBounds = fpsDisplay.getLocalBounds();
        fpsDisplay.setPosition(1024 - fpsBounds.width - 10, 8);
        window.draw(fpsDisplay);
    }
}
