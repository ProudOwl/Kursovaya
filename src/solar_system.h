#ifndef SOLAR_SYSTEM_H
#define SOLAR_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "celestial_body.h"

class SolarSystem {
public:
    SolarSystem();
    
    // Инициализация
    void init();
    
    // Обновление
    void update(float deltaTime);
    
    // Отрисовка
    void draw(sf::RenderWindow& window);
    
    // Управление
    void selectBodyAt(float x, float y);
    void clearSelection();
    CelestialBody* getSelectedBody() const;
    
    // Информация
    std::string getInfo() const;
    int getBodyCount() const { return bodies.size(); }
    
    // Режимы отображения
    enum class DisplayMode {
        REAL_SCALE,
        ORRERY,
        ZOOMED
    };
    
    void setDisplayMode(DisplayMode mode);
    DisplayMode getDisplayMode() const { return currentMode; }
    
    // Масштаб
    void setScale(float scale);
    float getScale() const { return scale; }
    
    // Центр системы
    void setCenter(float x, float y);
    sf::Vector2f getCenter() const { return center; }
    
private:
    std::vector<std::unique_ptr<CelestialBody>> bodies;
    DisplayMode currentMode;
    float scale;
    sf::Vector2f center;
    CelestialBody* selectedBody;
    
    // Создание тел
    void createSolarSystem();
    void addBody(std::unique_ptr<CelestialBody> body);
    
    // Вспомогательные методы
    float calculateDisplayRadius(float realRadius) const;
    float calculateDisplayOrbit(float realOrbit) const;
};

#endif // SOLAR_SYSTEM_H
