#include "celestial_body.h"
#include <iostream>
#include <cmath>
#include <vector>

CelestialBody::CelestialBody(const std::string& name, Type type, float radius, 
                           float orbitRadius, float orbitSpeed, const sf::Color& color,
                           const std::string& texturePath)
    : name(name)
    , type(type)
    , radius(radius)
    , orbitRadius(orbitRadius)
    , orbitSpeed(orbitSpeed)
    , currentAngle(0.0f)
    , color(color)
    , isSelected(false) {
    
    // Initialize graphics
    initGraphics();
    
    // Load texture if specified
    if (!texturePath.empty()) {
        texture = std::make_shared<sf::Texture>();
        if (texture->loadFromFile(texturePath)) {
            shape.setTexture(texture.get());
        } else {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
            shape.setFillColor(color);
        }
    } else {
        shape.setFillColor(color);
    }
}

void CelestialBody::initGraphics() {
    // Main shape (planet/star)
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setOutlineThickness(2);
    
    // Orbit (circle)
    if (orbitRadius > 0) {
        orbitCircle.setRadius(orbitRadius);
        orbitCircle.setOrigin(orbitRadius, orbitRadius);
        orbitCircle.setFillColor(sf::Color::Transparent);
        orbitCircle.setOutlineColor(sf::Color(100, 100, 100, 100));
        orbitCircle.setOutlineThickness(1);
    }
}

void CelestialBody::update(float deltaTime) {
    // Update orbit angle
    if (orbitRadius > 0) {
        currentAngle += orbitSpeed * deltaTime;
        if (currentAngle >= 360.0f) {
            currentAngle -= 360.0f;
        }
    }
}

void CelestialBody::draw(sf::RenderWindow& window, float centerX, float centerY) {
    // Draw orbit
    if (orbitRadius > 0) {
        orbitCircle.setPosition(centerX, centerY);
        window.draw(orbitCircle);
    }
    
    // Calculate position on orbit
    float angleRad = currentAngle * 3.14159265f / 180.0f;
    float x = centerX + orbitRadius * std::cos(angleRad);
    float y = centerY + orbitRadius * std::sin(angleRad);
    
    // Draw body
    shape.setPosition(x, y);
    
    // Highlight if selected
    if (isSelected) {
        shape.setOutlineColor(sf::Color::Yellow);
        shape.setOutlineThickness(4);
        
        // Glow effect for stars
        if (type == Type::STAR) {
            sf::CircleShape glow(radius * 1.5f);
            glow.setOrigin(radius * 1.5f, radius * 1.5f);
            glow.setPosition(x, y);
            glow.setFillColor(sf::Color(color.r, color.g, color.b, 100));
            window.draw(glow);
        }
    } else {
        shape.setOutlineColor(sf::Color::Transparent);
    }
    
    window.draw(shape);
    
    // Label - с исправленной загрузкой шрифта
    if (!name.empty()) {
        static sf::Font font;
        static bool fontLoaded = false;
        
        // Пытаемся загрузить шрифт только один раз
        if (!fontLoaded) {
            std::vector<std::string> fontPaths = {
                "resources/fonts/Ubuntu-R.ttf",
                "resources/fonts/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "C:/Windows/Fonts/arial.ttf"  // для Windows
            };
            
            for (const auto& fontPath : fontPaths) {
                if (font.loadFromFile(fontPath)) {
                    fontLoaded = true;
                    std::cout << "Font loaded from: " << fontPath << std::endl;
                    break;
                }
            }
            
            // Если шрифт не загружен, используем стандартный шрифт SFML
            if (!fontLoaded) {
                std::cerr << "Warning: Could not load any font for celestial body labels." << std::endl;
                // Используем стандартный шрифт SFML
                sf::Text tempText;
                font = *tempText.getFont();
                fontLoaded = true;
            }
        }
        
        if (fontLoaded) {
            sf::Text label;
            label.setFont(font);
            label.setString(name);
            label.setCharacterSize(16);
            label.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            
            // Position above planet
            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.width / 2, bounds.height);
            label.setPosition(x, y - radius - 10);
            
            // Shadow for readability
            sf::Text shadow = label;
            shadow.setFillColor(sf::Color(0, 0, 0, 150));
            shadow.move(1, 1);
            window.draw(shadow);
            
            window.draw(label);
        }
    }
}

sf::Vector2f CelestialBody::getPosition() const {
    float angleRad = currentAngle * 3.14159265f / 180.0f;
    return sf::Vector2f(
        orbitRadius * std::cos(angleRad),
        orbitRadius * std::sin(angleRad)
    );
}

std::string CelestialBody::getTypeString() const {
    switch (type) {
        case Type::STAR: return "Star";
        case Type::PLANET: return "Planet";
        case Type::DWARF_PLANET: return "Dwarf Planet";
        case Type::MOON: return "Moon";
        case Type::ASTEROID: return "Asteroid";
        case Type::COMET: return "Comet";
        case Type::GALAXY: return "Galaxy";
        case Type::BLACK_HOLE: return "Black Hole";
        default: return "Unknown";
    }
}

std::string CelestialBody::getDescription() const {
    std::string desc;
    
    switch (type) {
        case Type::STAR:
            desc = "A star is a massive self-luminous celestial body.";
            break;
        case Type::PLANET:
            desc = "A planet is a celestial body orbiting a star.";
            if (name == "Earth") {
                desc = "Earth is the third planet from the Sun, the only known planet with life.";
            } else if (name == "Mars") {
                desc = "Mars is the fourth planet, known as the Red Planet.";
            }
            break;
        case Type::DWARF_PLANET:
            desc = "A dwarf planet is a celestial body similar to a planet.";
            break;
        default:
            desc = "A celestial body in the Solar System.";
    }
    
    return desc;
}

bool CelestialBody::contains(float x, float y, float centerX, float centerY) const {
    sf::Vector2f pos = getPosition();
    pos.x += centerX;
    pos.y += centerY;
    
    float dx = x - pos.x;
    float dy = y - pos.y;
    float distanceSquared = dx * dx + dy * dy;
    
    return distanceSquared <= (radius * radius);
}

CelestialBody::PhysicalInfo CelestialBody::getPhysicalInfo() const {
    PhysicalInfo info;
    
    // Example data (in real project would be accurate)
    switch (type) {
        case Type::STAR:
            info.mass = 333000.0f;
            info.diameter = 1392700.0f;
            info.density = 1.408f;
            info.gravity = 274.0f;
            info.temperature = 5778.0f;
            info.composition = "Hydrogen (73%), Helium (25%)";
            break;
            
        case Type::PLANET:
            if (name == "Earth") {
                info.mass = 1.0f;
                info.diameter = 12742.0f;
                info.density = 5.514f;
                info.gravity = 9.807f;
                info.temperature = 288.0f;
                info.composition = "Iron (32%), Oxygen (30%), Silicon (15%)";
            } else if (name == "Mars") {
                info.mass = 0.107f;
                info.diameter = 6779.0f;
                info.density = 3.933f;
                info.gravity = 3.711f;
                info.temperature = 210.0f;
                info.composition = "Silicon (45%), Iron (17%), Sulfur (14%)";
            } else {
                // Default values for planets
                info.mass = 0.5f;
                info.diameter = 10000.0f;
                info.density = 3.0f;
                info.gravity = 5.0f;
                info.temperature = 200.0f;
                info.composition = "Rocky material, metals";
            }
            break;
            
        default:
            info.mass = 0.01f;
            info.diameter = 1000.0f;
            info.density = 2.0f;
            info.gravity = 0.5f;
            info.temperature = 100.0f;
            info.composition = "Unknown";
    }
    
    return info;
}

sf::Color CelestialBody::getDefaultColor(Type type) {
    switch (type) {
        case Type::STAR: return sf::Color::Yellow;
        case Type::PLANET: return sf::Color::Blue;
        case Type::DWARF_PLANET: return sf::Color(169, 169, 169); // Gray
        case Type::MOON: return sf::Color::White;
        case Type::ASTEROID: return sf::Color(139, 69, 19); // Brown
        case Type::COMET: return sf::Color::Cyan;
        case Type::GALAXY: return sf::Color::Magenta;
        case Type::BLACK_HOLE: return sf::Color::Black;
        default: return sf::Color::White;
    }
}
