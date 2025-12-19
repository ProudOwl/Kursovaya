#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class CelestialBody {
public:
    enum class Type {
        STAR,
        PLANET,
        DWARF_PLANET,
        MOON,
        ASTEROID,
        COMET,
        GALAXY,
        BLACK_HOLE
    };
    
    CelestialBody(const std::string& name, Type type, float radius, float orbitRadius, 
                  float orbitSpeed, const sf::Color& color, const std::string& texturePath = "");
    
    CelestialBody(const CelestialBody&) = delete;
    CelestialBody& operator=(const CelestialBody&) = delete;
    
    CelestialBody(CelestialBody&&) = default;
    CelestialBody& operator=(CelestialBody&&) = default;
    
    void update(float deltaTime);
    
    void draw(sf::RenderWindow& window, float centerX, float centerY);
    
    std::string getName() const { return name; }
    Type getType() const { return type; }
    float getRadius() const { return radius; }
    float getOrbitRadius() const { return orbitRadius; }
    sf::Vector2f getPosition() const;
    std::string getTypeString() const;
    std::string getDescription() const;
    
    void setSelected(bool selected) { isSelected = selected; }
    bool getSelected() const { return isSelected; }
    
    bool contains(float x, float y, float centerX, float centerY) const;
    
    struct PhysicalInfo {
        float mass;
        float diameter;
        float density;
        float gravity;
        float temperature;
        std::string composition;
    };
    
    PhysicalInfo getPhysicalInfo() const;
    
private:
    std::string name;
    Type type;
    float radius;
    float orbitRadius;
    float orbitSpeed;
    float currentAngle;
    sf::Color color;
    bool isSelected = false;
    
    std::shared_ptr<sf::Texture> texture;
    sf::CircleShape shape;
    sf::CircleShape orbitCircle;
    
    void initGraphics();
    
    static sf::Color getDefaultColor(Type type);
};

#endif
