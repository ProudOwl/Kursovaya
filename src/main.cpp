#include "game.h"
#include <iostream>

int main() {
    std::cout << "AstroLearn Gamified - Starting..." << std::endl;
    
    Game game;
    
    if (!game.init()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    game.run();
    
    return 0;
}
