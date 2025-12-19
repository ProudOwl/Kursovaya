#include "solar_system.h"
#include <iostream>
#include <sstream>
#include <cmath>

SolarSystem::SolarSystem()
    : currentMode(DisplayMode::ORRERY)
    , scale(1.0f)
    , center(512.0f, 384.0f)
    , selectedBody(nullptr) {
}

void SolarSystem::init() {
    createSolarSystem();
}

void SolarSystem::createSolarSystem() {
    bodies.clear();
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Sun", CelestialBody::Type::STAR, 
        40.0f,
        0.0f,
        0.0f,
        sf::Color::Yellow,
        "resources/textures/sun.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Mercury", CelestialBody::Type::PLANET,
        8.0f,
        80.0f,
        4.15f,
        sf::Color(150, 150, 150),
        "resources/textures/mercury.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Venus", CelestialBody::Type::PLANET,
        12.0f,
        120.0f,
        1.62f,
        sf::Color(255, 200, 100),
        "resources/textures/venus.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Earth", CelestialBody::Type::PLANET,
        12.0f,
        160.0f,
        1.0f,
        sf::Color::Blue,
        "resources/textures/earth.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Mars", CelestialBody::Type::PLANET,
        10.0f,
        200.0f,
        0.53f,
        sf::Color::Red,
        "resources/textures/mars.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Jupiter", CelestialBody::Type::PLANET,
        25.0f,
        260.0f,
        0.08f,
        sf::Color(255, 165, 0),
        "resources/textures/jupiter.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Saturn", CelestialBody::Type::PLANET,
        22.0f,
        320.0f,
        0.03f,
        sf::Color::Yellow,
        "resources/textures/saturn.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Uranus", CelestialBody::Type::PLANET,
        18.0f,
        380.0f,
        0.01f,
        sf::Color::Cyan,
        "resources/textures/uranus.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Neptune", CelestialBody::Type::PLANET,
        18.0f,
        440.0f,
        0.006f,
        sf::Color(0, 0, 255),
        "resources/textures/neptune.png"
    ));
    
    bodies.push_back(std::make_unique<CelestialBody>(
        "Pluto", CelestialBody::Type::DWARF_PLANET,
        6.0f,
        480.0f,
        0.004f,
        sf::Color(169, 169, 169),
        "resources/textures/pluto.png"
    ));
    
    std::cout << "Created solar system with " << bodies.size() << " bodies" << std::endl;
}

void SolarSystem::update(float deltaTime) {
    for (auto& body : bodies) {
        body->update(deltaTime);
    }
}

void SolarSystem::draw(sf::RenderWindow& window) {
    for (auto& body : bodies) {
        body->draw(window, center.x, center.y);
    }
}

void SolarSystem::selectBodyAt(float x, float y) {
    clearSelection();
    
    for (auto it = bodies.rbegin(); it != bodies.rend(); ++it) {
        if ((*it)->contains(x, y, center.x, center.y)) {
            (*it)->setSelected(true);
            selectedBody = it->get();
            std::cout << "Selected: " << selectedBody->getName() << std::endl;
            return;
        }
    }
}

void SolarSystem::clearSelection() {
    if (selectedBody) {
        selectedBody->setSelected(false);
    }
    selectedBody = nullptr;
}

CelestialBody* SolarSystem::getSelectedBody() const {
    return selectedBody;
}

std::string SolarSystem::getInfo() const {
    std::stringstream ss;
    ss << "Solar System contains " << bodies.size() << " bodies:\n";
    
    for (const auto& body : bodies) {
        ss << "  - " << body->getName() << " (" << body->getTypeString() << ")\n";
    }
    
    ss << "\nDisplay mode: ";
    switch (currentMode) {
        case DisplayMode::REAL_SCALE: ss << "Real scale"; break;
        case DisplayMode::ORRERY: ss << "Orrery (educational)"; break;
        case DisplayMode::ZOOMED: ss << "Zoomed"; break;
    }
    
    return ss.str();
}

void SolarSystem::setDisplayMode(DisplayMode mode) {
    if (currentMode != mode) {
        currentMode = mode;
        
        switch (mode) {
            case DisplayMode::REAL_SCALE:
                scale = 0.0001f;
                break;
            case DisplayMode::ORRERY:
                scale = 1.0f;
                break;
            case DisplayMode::ZOOMED:
                scale = 2.0f;
                break;
        }
        
        std::cout << "Display mode set to: ";
        switch (mode) {
            case DisplayMode::REAL_SCALE: std::cout << "Real scale"; break;
            case DisplayMode::ORRERY: std::cout << "Orrery"; break;
            case DisplayMode::ZOOMED: std::cout << "Zoomed"; break;
        }
        std::cout << " (scale: " << scale << "x)" << std::endl;
    }
}

void SolarSystem::setScale(float newScale) {
    scale = newScale;
    if (scale < 0.1f) scale = 0.1f;
    if (scale > 5.0f) scale = 5.0f;
}

void SolarSystem::setCenter(float x, float y) {
    center.x = x;
    center.y = y;
}

void SolarSystem::addBody(std::unique_ptr<CelestialBody> body) {
    bodies.push_back(std::move(body));
}

float SolarSystem::calculateDisplayRadius(float realRadius) const {
    return realRadius * scale;
}

float SolarSystem::calculateDisplayOrbit(float realOrbit) const {
    return realOrbit * scale;
}
