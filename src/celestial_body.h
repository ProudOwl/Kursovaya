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
    
    // Конструктор
    CelestialBody(const std::string& name, Type type, float radius, float orbitRadius, 
                  float orbitSpeed, const sf::Color& color, const std::string& texturePath = "");
    
    // Запрещаем копирование и присваивание из-за shared_ptr<sf::Texture>
    CelestialBody(const CelestialBody&) = delete;
    CelestialBody& operator=(const CelestialBody&) = delete;
    
    // Разрешаем перемещение
    CelestialBody(CelestialBody&&) = default;
    CelestialBody& operator=(CelestialBody&&) = default;
    
    // Обновление
    void update(float deltaTime);
    
    // Отрисовка
    void draw(sf::RenderWindow& window, float centerX, float centerY);
    
    // Геттеры
    std::string getName() const { return name; }
    Type getType() const { return type; }
    float getRadius() const { return radius; }
    float getOrbitRadius() const { return orbitRadius; }
    sf::Vector2f getPosition() const;
    std::string getTypeString() const;
    std::string getDescription() const;
    
    // Выбор/выделение
    void setSelected(bool selected) { isSelected = selected; }
    bool getSelected() const { return isSelected; }
    
    // Проверка клика
    bool contains(float x, float y, float centerX, float centerY) const;
    
    // Информация
    struct PhysicalInfo {
        float mass;           // в массах Земли
        float diameter;       // в км
        float density;        // в г/см³
        float gravity;        // в м/с²
        float temperature;    // в Кельвинах
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
    bool isSelected = false;  // Инициализация по умолчанию
    
    // Графические компоненты
    std::shared_ptr<sf::Texture> texture;
    sf::CircleShape shape;
    sf::CircleShape orbitCircle;
    
    // Инициализация
    void initGraphics();
    
    // Цвета по типам
    static sf::Color getDefaultColor(Type type);
};

#endif // CELESTIAL_BODY_H
