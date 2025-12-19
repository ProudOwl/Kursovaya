#include "game_ui.h"
#include "game_logic.h"
#include <iostream>

void GameLogic::initPlanetInfo(Game* game) {
    game->planetInfo["Sun"] = {
        "Sun",
        "The star at the center of our Solar System.",
        {
            "Contains 99.86% of Solar System mass",
            "Core temperature: 15 million K",
            "Age: 4.6 billion years",
            "Diameter: 1.4 million km",
            "Will become red giant in 5 billion years"
        },
        15,
        "Mercury"
    };
    
    game->planetInfo["Mercury"] = {
        "Mercury",
        "The smallest and innermost planet in the Solar System.",
        {
            "Orbital period: 88 Earth days",
            "Surface temperature: -173C to 427C",
            "No moons or rings",
            "Closest planet to the Sun"
        },
        15,
        "Venus"
    };
    
    game->planetInfo["Venus"] = {
        "Venus",
        "The second planet from the Sun and Earth's closest planetary neighbor.",
        {
            "Orbital period: 225 Earth days",
            "Surface temperature: 462C (hottest planet)",
            "Rotates backwards (retrograde rotation)",
            "Similar size to Earth"
        },
        15,
        "Earth"
    };
    
    game->planetInfo["Earth"] = {
        "Earth",
        "Our home planet, the only known planet with life.",
        {
            "Orbital period: 365.25 days",
            "71% of surface covered by water",
            "Has one natural satellite: the Moon",
            "Only planet with liquid water on surface"
        },
        15,
        "Mars"
    };
    
    game->planetInfo["Mars"] = {
        "Mars",
        "The fourth planet, known as the Red Planet.",
        {
            "Orbital period: 687 Earth days",
            "Has two moons: Phobos and Deimos",
            "Largest volcano in Solar System: Olympus Mons",
            "Evidence of ancient riverbeds"
        },
        15,
        "Jupiter"
    };
    
    game->planetInfo["Jupiter"] = {
        "Jupiter",
        "The largest planet in our Solar System.",
        {
            "Orbital period: 12 Earth years",
            "Has at least 79 moons",
            "Great Red Spot is a giant storm",
            "Strongest magnetic field"
        },
        15,
        "Saturn"
    };
    
    game->planetInfo["Saturn"] = {
        "Saturn",
        "Known for its spectacular ring system.",
        {
            "Orbital period: 29 Earth years",
            "Least dense planet (would float on water)",
            "Has 82 confirmed moons",
            "Rings made of ice and rock particles"
        },
        15,
        "Uranus"
    };
    
    game->planetInfo["Uranus"] = {
        "Uranus",
        "The ice giant that rotates on its side.",
        {
            "Orbital period: 84 Earth years",
            "Rotates at 98 angle (on its side)",
            "Has 13 known rings",
            "Coldest planetary atmosphere: -224C"
        },
        15,
        "Neptune"
    };
    
    game->planetInfo["Neptune"] = {
        "Neptune",
        "The windiest planet with supersonic storms.",
        {
            "Orbital period: 165 Earth years",
            "Fastest winds: up to 2,100 km/h",
            "Discovered through mathematical predictions",
            "Has 14 known moons"
        },
        15,
        "Pluto"
    };
    
    game->planetInfo["Pluto"] = {
        "Pluto",
        "A dwarf planet in the Kuiper belt.",
        {
            "Has 5 moons",
            "Surface temperature: -229C",
            "Orbital period: 248 years",
            "Has a heart-shaped glacier"
        },
        10,
        ""
    };
    
    std::cout << "Planet info initialized for " << game->planetInfo.size() << " celestial bodies" << std::endl;
}

void GameLogic::initPlanetUnlockSystem(Game* game) {
    game->planetQuizCategory["Sun"] = "sun";
    game->planetQuizCategory["Mercury"] = "mercury";
    game->planetQuizCategory["Venus"] = "venus";
    game->planetQuizCategory["Earth"] = "earth";
    game->planetQuizCategory["Mars"] = "mars";
    game->planetQuizCategory["Jupiter"] = "jupiter";
    game->planetQuizCategory["Saturn"] = "saturn";
    game->planetQuizCategory["Uranus"] = "uranus";
    game->planetQuizCategory["Neptune"] = "neptune";
    game->planetQuizCategory["Pluto"] = "pluto";
    
    game->planetUnlockRequirement["Sun"] = 0;
    game->planetUnlockRequirement["Mercury"] = 10;
    game->planetUnlockRequirement["Venus"] = 10;
    game->planetUnlockRequirement["Earth"] = 10;
    game->planetUnlockRequirement["Mars"] = 10;
    game->planetUnlockRequirement["Jupiter"] = 10;
    game->planetUnlockRequirement["Saturn"] = 10;
    game->planetUnlockRequirement["Uranus"] = 10;
    game->planetUnlockRequirement["Neptune"] = 10;
    game->planetUnlockRequirement["Pluto"] = 8;
    
    game->planetUnlockStatus.clear();
    
    game->planetUnlockStatus["Sun"] = true;
    game->planetUnlockStatus["Mercury"] = false;
    game->planetUnlockStatus["Venus"] = false;
    game->planetUnlockStatus["Earth"] = false;
    game->planetUnlockStatus["Mars"] = false;
    game->planetUnlockStatus["Jupiter"] = false;
    game->planetUnlockStatus["Saturn"] = false;
    game->planetUnlockStatus["Uranus"] = false;
    game->planetUnlockStatus["Neptune"] = false;
    game->planetUnlockStatus["Pluto"] = false;
    
    std::cout << "Planet unlock system initialized." << std::endl;
}

void GameLogic::selectPlanet(Game* game, const std::string& planetName) {
    game->setSelectedPlanet(planetName);
    
    if (isPlanetUnlocked(game, planetName)) {
        game->setCurrentState(Game::GameState::PLANET_INFO);
        game->currentButtons = game->planetInfoButtons;
    } else {
        game->setCurrentState(Game::GameState::PLANET_INFO);
        game->currentButtons = game->planetInfoButtons;
    }
}

void GameLogic::startPlanetQuiz(Game* game, const std::string& planetName) {
    if (!isPlanetUnlocked(game, planetName)) {
        std::cerr << "Cannot start quiz for locked planet: " << planetName << std::endl;
        return;
    }
    
    if (game->planetQuizCategory.find(planetName) != game->planetQuizCategory.end()) {
        std::string category = game->planetQuizCategory[planetName];
        int questionCount = 15;
        
        if (planetName == "Sun") {
            questionCount = 15;
        } else if (planetName == "Pluto") {
            questionCount = 10;
        }
        
        if (game->planetInfo.find(planetName) != game->planetInfo.end()) {
            questionCount = game->planetInfo[planetName].quizQuestionsRequired;
        }
        
        int requiredCorrect = game->planetUnlockRequirement[planetName];
        
        auto categories = game->getQuiz()->getAvailableCategories();
        bool categoryExists = std::find(categories.begin(), categories.end(), category) != categories.end();
        
        if (!categoryExists) {
            std::cerr << "Quiz category '" << category << "' not found for " << planetName << std::endl;
            category = "planets";
        }

        game->lastQuizCompletion.processed = false;
        
        game->getQuiz()->startNewQuiz(category, questionCount);
        game->getQuiz()->setRequiredCorrectAnswers(requiredCorrect);
        game->setCurrentState(Game::GameState::QUIZ);
        
        game->quizButtons.clear();
        game->quizButtons.push_back(GameUI::createButton(
            "Next Question", 
            362, 
            500, 
            300, 
            50, 
            Game::GameState::QUIZ
        ));
        game->quizButtons.push_back(GameUI::createButton(
            "Exit Quiz", 
            362, 
            560, 
            300, 
            50, 
            Game::GameState::MAIN_MENU
        ));
        game->quizButtons.push_back(GameUI::createButton(
            "Return to " + planetName + " Info", 
            362, 
            620, 
            300, 
            50, 
            Game::GameState::PLANET_INFO
        ));
        
        game->currentButtons = game->quizButtons;
        
        std::cout << "Starting " << planetName << " quiz: " 
                  << questionCount << " questions from category '" << category << "'" << std::endl;
        std::cout << "Required to unlock next: " << requiredCorrect 
                  << "/" << questionCount << " correct answers" << std::endl;
    } else {
        std::cerr << "No quiz category found for planet: " << planetName << std::endl;
    }
}

bool GameLogic::isPlanetUnlocked(Game* game, const std::string& planetName) {
    auto it = game->planetUnlockStatus.find(planetName);
    if (it != game->planetUnlockStatus.end()) {
        return it->second;
    }
    return false;
}

bool GameLogic::canUnlockPlanet(Game* game, const std::string& planetName) {
    if (game->planetUnlockRequirement.find(planetName) == game->planetUnlockRequirement.end()) {
        return false;
    }
    
    if (planetName == "Mercury") {
        return true;
    }
    
    std::string prevPlanet = "";
    for (const auto& [name, info] : game->planetInfo) {
        if (info.nextPlanet == planetName) {
            prevPlanet = name;
            break;
        }
    }
    
    if (prevPlanet.empty()) {
        return false;
    }
    
    return isPlanetUnlocked(game, prevPlanet);
}

void GameLogic::unlockPlanet(Game* game, const std::string& planetName) {
    auto it = game->planetUnlockStatus.find(planetName);
    if (it != game->planetUnlockStatus.end()) {
        it->second = true;
        std::cout << "Planet unlocked: " << planetName << std::endl;
        
        game->saveGame();
    }
}

void GameLogic::updatePlanetUnlockStatus(Game* game) {
    if (!game->getPlayer()) {
        return;
    }
    
    auto quizHistory = game->getPlayer()->getQuizHistory();
    
    auto checkPlanetQuizPassed = [&](const std::string& planetName) -> bool {
        if (game->planetQuizCategory.find(planetName) == game->planetQuizCategory.end()) {
            return false;
        }
        
        std::string category = game->planetQuizCategory[planetName];
        int requiredCorrect = game->planetUnlockRequirement[planetName];
        
        for (const auto& quizResult : quizHistory) {
            if (quizResult.category == category) {
                if (quizResult.correctAnswers >= requiredCorrect) {
                    return true;
                }
            }
        }
        return false;
    };
    
    game->planetUnlockStatus["Sun"] = true;
    
    if (!game->planetUnlockStatus["Mercury"] && checkPlanetQuizPassed("Sun")) {
        game->planetUnlockStatus["Mercury"] = true;
    }
    
    if (!game->planetUnlockStatus["Venus"] && game->planetUnlockStatus["Mercury"] && checkPlanetQuizPassed("Mercury")) {
        game->planetUnlockStatus["Venus"] = true;
    }
    
    if (!game->planetUnlockStatus["Earth"] && game->planetUnlockStatus["Venus"] && checkPlanetQuizPassed("Venus")) {
        game->planetUnlockStatus["Earth"] = true;
    }
    
    if (!game->planetUnlockStatus["Mars"] && game->planetUnlockStatus["Earth"] && checkPlanetQuizPassed("Earth")) {
        game->planetUnlockStatus["Mars"] = true;
    }
    
    if (!game->planetUnlockStatus["Jupiter"] && game->planetUnlockStatus["Mars"] && checkPlanetQuizPassed("Mars")) {
        game->planetUnlockStatus["Jupiter"] = true;
    }
    
    if (!game->planetUnlockStatus["Saturn"] && game->planetUnlockStatus["Jupiter"] && checkPlanetQuizPassed("Jupiter")) {
        game->planetUnlockStatus["Saturn"] = true;
    }
    
    if (!game->planetUnlockStatus["Uranus"] && game->planetUnlockStatus["Saturn"] && checkPlanetQuizPassed("Saturn")) {
        game->planetUnlockStatus["Uranus"] = true;
    }
    
    if (!game->planetUnlockStatus["Neptune"] && game->planetUnlockStatus["Uranus"] && checkPlanetQuizPassed("Uranus")) {
        game->planetUnlockStatus["Neptune"] = true;
    }
    
    if (!game->planetUnlockStatus["Pluto"] && game->planetUnlockStatus["Neptune"] && checkPlanetQuizPassed("Neptune")) {
        game->planetUnlockStatus["Pluto"] = true;
    }
    
    game->saveGame();
}
