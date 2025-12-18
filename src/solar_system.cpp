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
    
    // Sun (in center)
    bodies.push_back(std::make_unique<CelestialBody>(
        "Sun", CelestialBody::Type::STAR, 
        40.0f,  // Radius
        0.0f,   // Orbit (0 - in center)
        0.0f,   // Rotation speed
        sf::Color::Yellow,
        "resources/textures/sun.png"
    ));
    
    // Planets
    // Mercury
    bodies.push_back(std::make_unique<CelestialBody>(
        "Mercury", CelestialBody::Type::PLANET,
        8.0f,    // Radius
        80.0f,   // Orbit
        4.15f,   // Speed (orbital period 88 days)
        sf::Color(150, 150, 150),  // Gray
        "resources/textures/mercury.png"
    ));
    
    // Venus
    bodies.push_back(std::make_unique<CelestialBody>(
        "Venus", CelestialBody::Type::PLANET,
        12.0f,   // Radius
        120.0f,  // Orbit
        1.62f,   // Speed (orbital period 225 days)
        sf::Color(255, 200, 100),  // Yellow-orange
        "resources/textures/venus.png"
    ));
    
    // Earth
    bodies.push_back(std::make_unique<CelestialBody>(
        "Earth", CelestialBody::Type::PLANET,
        12.0f,   // Radius
        160.0f,  // Orbit
        1.0f,    // Speed (orbital period 365 days)
        sf::Color::Blue,
        "resources/textures/earth.png"
    ));
    
    // Mars
    bodies.push_back(std::make_unique<CelestialBody>(
        "Mars", CelestialBody::Type::PLANET,
        10.0f,   // Radius
        200.0f,  // Orbit
        0.53f,   // Speed (orbital period 687 days)
        sf::Color::Red,
        "resources/textures/mars.png"
    ));
    
    // Jupiter
    bodies.push_back(std::make_unique<CelestialBody>(
        "Jupiter", CelestialBody::Type::PLANET,
        25.0f,   // Radius
        260.0f,  // Orbit
        0.08f,   // Speed (orbital period 12 years)
        sf::Color(255, 165, 0),  // Orange
        "resources/textures/jupiter.png"
    ));
    
    // Saturn
    bodies.push_back(std::make_unique<CelestialBody>(
        "Saturn", CelestialBody::Type::PLANET,
        22.0f,   // Radius
        320.0f,  // Orbit
        0.03f,   // Speed (orbital period 29 years)
        sf::Color::Yellow,
        "resources/textures/saturn.png"
    ));
    
    // Uranus
    bodies.push_back(std::make_unique<CelestialBody>(
        "Uranus", CelestialBody::Type::PLANET,
        18.0f,   // Radius
        380.0f,  // Orbit
        0.01f,   // Speed (orbital period 84 years)
        sf::Color::Cyan,
        "resources/textures/uranus.png"
    ));
    
    // Neptune
    bodies.push_back(std::make_unique<CelestialBody>(
        "Neptune", CelestialBody::Type::PLANET,
        18.0f,   // Radius
        440.0f,  // Orbit
        0.006f,  // Speed (orbital period 165 years)
        sf::Color(0, 0, 255),  // Blue
        "resources/textures/neptune.png"
    ));
    
    // Pluto (dwarf planet)
    bodies.push_back(std::make_unique<CelestialBody>(
        "Pluto", CelestialBody::Type::DWARF_PLANET,
        6.0f,    // Radius
        480.0f,  // Orbit
        0.004f,  // Speed
        sf::Color(169, 169, 169),  // Gray
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
    // Draw all bodies
    for (auto& body : bodies) {
        body->draw(window, center.x, center.y);
    }
    
    // Информационная панель УБРАНА - оставлен только выбор планет
}

void SolarSystem::selectBodyAt(float x, float y) {
    clearSelection();
    
    // Check all bodies starting from outermost (so inner ones don't overlap)
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
        ss << "  • " << body->getName() << " (" << body->getTypeString() << ")\n";
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
        
        // Change scale based on mode
        switch (mode) {
            case DisplayMode::REAL_SCALE:
                scale = 0.0001f; // Very small scale
                break;
            case DisplayMode::ORRERY:
                scale = 1.0f; // Normal scale
                break;
            case DisplayMode::ZOOMED:
                scale = 2.0f; // Zoomed
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
