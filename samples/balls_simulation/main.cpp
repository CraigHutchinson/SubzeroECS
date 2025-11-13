#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

#include "SubzeroECS/World.hpp"
#include "Components.hpp"
#include "ECS_Systems.hpp"
#include "SoA_Implementation.hpp"
#include "AoS_Implementation.hpp"
#include "OOP_Implementation.hpp"

namespace BallsSim {

enum class SimulationMode {
    ECS,
    SoA,
    AoS,
    OOP
};

const char* getModeString(SimulationMode mode) {
    switch (mode) {
        case SimulationMode::ECS: return "ECS (SubzeroECS)";
        case SimulationMode::SoA: return "DOD (Structure of Arrays)";
        case SimulationMode::AoS: return "Array of Structures";
        case SimulationMode::OOP: return "Object-Oriented";
        default: return "Unknown";
    }
}

class BallsSimulation {
public:
    BallsSimulation() 
        : window(sf::VideoMode(static_cast<unsigned>(config.boxWidth), 
                               static_cast<unsigned>(config.boxHeight)), 
                 "2D Balls Simulation - SubzeroECS Demo")
    {
        window.setFramerateLimit(60);
        
        initializeECS();
        
        // Initialize alternative implementations
        soaImpl.config = config;
        aosImpl.config = config;
        oopImpl.config = config;
        
        // Load font for UI
        if (!font.loadFromFile("C:/Windows/Fonts/consola.ttf")) {
            std::cerr << "Failed to load font\n";
        }
        
        setupUI();
        spawnInitialBalls();
    }
    
    ~BallsSimulation() {
        cleanupECS();
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            
            float frameTime = frameClock.restart().asSeconds();
            deltaTime = std::min(frameTime, 0.033f); // Cap at ~30 FPS to prevent huge timesteps
            
            update();
            render();
            
            updateFPS(frameTime);
        }
    }

private:
    // Simulation (must be declared before window since window init uses config)
    PhysicsConfig config;
    SimulationMode currentMode = SimulationMode::ECS;
    size_t entityCount = 0;
    
    // Window and rendering
    sf::RenderWindow window;
    sf::Font font;
    sf::Text fpsText;
    sf::Text modeText;
    sf::Text entityCountText;
    sf::Text sleepCountText;
    sf::Text updateTimeText;
    sf::Text helpText;
    
    // Timing
    sf::Clock frameClock;
    sf::Clock updateClock;
    float deltaTime = 0.0f;
    float fpsUpdateTimer = 0.0f;
    float currentFPS = 0.0f;
    float currentUpdateTimeMs = 0.0f;
    float currentItemsPerSecond = 0.0f;
    
    // ECS Implementation (using pointers to allow reinitialization)
    std::unique_ptr<SubzeroECS::World> world;
    std::unique_ptr<SubzeroECS::Collection<Position, Velocity, Radius, Mass, Color, SleepState>> ecsCollection;
    std::unique_ptr<GravitySystem> gravitySystem;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<BoundaryCollisionSystem> boundarySystem;
    std::unique_ptr<BallCollisionSystem> collisionSystem;
    
    // Alternative implementations
    SoA_Implementation soaImpl;
    AoS_Implementation aosImpl;
    OOP_Implementation oopImpl;
    
    void initializeECS() {
        world = std::make_unique<SubzeroECS::World>();
        ecsCollection = std::make_unique<SubzeroECS::Collection<Position, Velocity, Radius, Mass, Color, SleepState>>(*world);
        gravitySystem = std::make_unique<GravitySystem>(*world);
        movementSystem = std::make_unique<MovementSystem>(*world);
        boundarySystem = std::make_unique<BoundaryCollisionSystem>(*world);
        collisionSystem = std::make_unique<BallCollisionSystem>(*world);
        
        gravitySystem->gravity = config.gravity;
        boundarySystem->config = config;
        collisionSystem->config = config;
    }
    
    void cleanupECS() {
        collisionSystem.reset();
        boundarySystem.reset();
        movementSystem.reset();
        gravitySystem.reset();
        ecsCollection.reset();
        world.reset();
    }
    
    void setupUI() {
        fpsText.setFont(font);
        fpsText.setCharacterSize(18);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition(10, 10);
        
        modeText.setFont(font);
        modeText.setCharacterSize(18);
        modeText.setFillColor(sf::Color::Yellow);
        modeText.setPosition(10, 35);
        
        entityCountText.setFont(font);
        entityCountText.setCharacterSize(18);
        entityCountText.setFillColor(sf::Color::Cyan);
        entityCountText.setPosition(10, 60);
        
        sleepCountText.setFont(font);
        sleepCountText.setCharacterSize(18);
        sleepCountText.setFillColor(sf::Color::Magenta);
        sleepCountText.setPosition(10, 85);
        
        updateTimeText.setFont(font);
        updateTimeText.setCharacterSize(18);
        updateTimeText.setFillColor(sf::Color::Green);
        updateTimeText.setPosition(10, 110);
        
        helpText.setFont(font);
        helpText.setCharacterSize(16);
        helpText.setFillColor(sf::Color(200, 200, 200));
        helpText.setPosition(10, config.boxHeight - 140);
        helpText.setString(
            "Controls:\n"
            "  1-4: Switch implementation mode\n"
            "  SPACE: Add 10 balls\n"
            "  C: Clear all balls\n"
            "  R: Reset with 100 balls\n"
            "  ESC: Exit"
        );
    }
    
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Num1:
                        switchMode(SimulationMode::ECS);
                        break;
                    case sf::Keyboard::Num2:
                        switchMode(SimulationMode::SoA);
                        break;
                    case sf::Keyboard::Num3:
                        switchMode(SimulationMode::AoS);
                        break;
                    case sf::Keyboard::Num4:
                        switchMode(SimulationMode::OOP);
                        break;
                    case sf::Keyboard::Space:
                        spawnBalls(10);
                        break;
                    case sf::Keyboard::C:
                        clearAllBalls();
                        break;
                    case sf::Keyboard::R:
                        clearAllBalls();
                        spawnBalls(100);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    void switchMode(SimulationMode newMode) {
        if (newMode == currentMode) return;
        
        // Transfer state between modes
        std::vector<BallState> states = extractBallStates();
        currentMode = newMode;
        loadBallStates(states);
    }
    
    struct BallState {
        float x, y, dx, dy, radius, mass;
        uint32_t color;
    };
    
    std::vector<BallState> extractBallStates() {
        std::vector<BallState> states;
        
        switch (currentMode) {
            case SimulationMode::ECS: {
                auto view = SubzeroECS::View<Position, Velocity, Radius, Mass, Color, SleepState>(*world);
                for (auto it = view.begin(); it != view.end(); ++it) {
                    const auto& pos = it.get<Position>();
                    const auto& vel = it.get<Velocity>();
                    const auto& rad = it.get<Radius>();
                    const auto& m = it.get<Mass>();
                    const auto& col = it.get<Color>();
                    uint32_t color = (col.r << 24) | (col.g << 16) | (col.b << 8) | col.a;
                    states.push_back({pos.x, pos.y, vel.dx, vel.dy, rad.value, m.value, color});
                }
                break;
            }
            case SimulationMode::SoA:
                for (size_t i = 0; i < soaImpl.balls.count; ++i) {
                    states.push_back({
                        soaImpl.balls.positions_x[i],
                        soaImpl.balls.positions_y[i],
                        soaImpl.balls.velocities_dx[i],
                        soaImpl.balls.velocities_dy[i],
                        soaImpl.balls.radii[i],
                        soaImpl.balls.masses[i],
                        soaImpl.balls.colors[i]
                    });
                }
                break;
            case SimulationMode::AoS:
                for (const auto& ball : aosImpl.balls) {
                    states.push_back({ball.position.x, ball.position.y, ball.velocity.dx, ball.velocity.dy, ball.radius, ball.mass, ball.color});
                }
                break;
            case SimulationMode::OOP:
                for (const auto& ball : oopImpl.balls) {
                    states.push_back({ball.position.x, ball.position.y, ball.velocity.dx, ball.velocity.dy, ball.radius, ball.mass, ball.color});
                }
                break;
        }
        
        return states;
    }
    
    void loadBallStates(const std::vector<BallState>& states) {
        clearAllBalls();
        
        for (const auto& state : states) {
            addBall(state.x, state.y, state.dx, state.dy, state.radius, state.mass, state.color);
        }
    }
    
    void spawnInitialBalls() {
        spawnBalls(100);
    }
    
    void spawnBalls(size_t count) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> posXDist(config.minRadius * 2, config.boxWidth - config.minRadius * 2);
        std::uniform_real_distribution<float> posYDist(config.minRadius * 2, config.boxHeight - config.minRadius * 2);
        std::uniform_real_distribution<float> velDist(-200.0f, 200.0f);
        std::uniform_real_distribution<float> radiusDist(config.minRadius, config.maxRadius);
        std::uniform_int_distribution<int> colorDist(0, 255);
        
        for (size_t i = 0; i < count; ++i) {
            float x = posXDist(gen);
            float y = posYDist(gen);
            float dx = velDist(gen);
            float dy = velDist(gen);
            float radius = radiusDist(gen);
            float mass = radius * radius * 0.1f; // Mass proportional to area
            
            uint32_t color = (colorDist(gen) << 24) | (colorDist(gen) << 16) | (colorDist(gen) << 8) | 255;
            
            addBall(x, y, dx, dy, radius, mass, color);
        }
    }
    
    void addBall(float x, float y, float dx, float dy, float radius, float mass, uint32_t color) {
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;
        
        switch (currentMode) {
            case SimulationMode::ECS:
                world->create(
                    Position{x, y},
                    Velocity{dx, dy},
                    Radius{radius},
                    Mass{mass},
                    Color{r, g, b, a},
                    SleepState{}
                );
                break;
            case SimulationMode::SoA:
                soaImpl.addBall(x, y, dx, dy, radius, mass, color);
                break;
            case SimulationMode::AoS:
                aosImpl.addBall(x, y, dx, dy, radius, mass, color);
                break;
            case SimulationMode::OOP:
                oopImpl.addBall(x, y, dx, dy, radius, mass, color);
                break;
        }
        
        entityCount++;
    }
    
    void clearAllBalls() {
        switch (currentMode) {
            case SimulationMode::ECS:
                // Reinitialize ECS to clear all entities
                cleanupECS();
                initializeECS();
                break;
            case SimulationMode::SoA:
                soaImpl.clear();
                break;
            case SimulationMode::AoS:
                aosImpl.clear();
                break;
            case SimulationMode::OOP:
                oopImpl.clear();
                break;
        }
        
        entityCount = 0;
    }
    
    void update() {
        updateClock.restart();
        
        switch (currentMode) {
            case SimulationMode::ECS:
                gravitySystem->deltaTime = deltaTime;
                movementSystem->deltaTime = deltaTime;
                
                gravitySystem->update();
                movementSystem->update();
                boundarySystem->update();
                collisionSystem->update();
                break;
                
            case SimulationMode::SoA:
                soaImpl.update(deltaTime);
                break;
                
            case SimulationMode::AoS:
                aosImpl.update(deltaTime);
                break;
                
            case SimulationMode::OOP:
                oopImpl.update(deltaTime);
                break;
        }
        
        currentUpdateTimeMs = updateClock.getElapsedTime().asSeconds() * 1000.0f;
        if (currentUpdateTimeMs > 0.0f && entityCount > 0) {
            currentItemsPerSecond = (entityCount / currentUpdateTimeMs) * 1000.0f;
        }
    }
    
    void render() {
        window.clear(sf::Color(30, 30, 40));
        
        // Draw balls
        sf::CircleShape circle;
        
        switch (currentMode) {
            case SimulationMode::ECS: {
                auto view = SubzeroECS::View<Position, Radius, Color>(*world);
                for (auto it = view.begin(); it != view.end(); ++it) {
                    const auto& pos = it.get<Position>();
                    const auto& rad = it.get<Radius>();
                    const auto& col = it.get<Color>();
                    
                    circle.setRadius(rad.value);
                    circle.setPosition(pos.x - rad.value, pos.y - rad.value);
                    circle.setFillColor(sf::Color(col.r, col.g, col.b, col.a));
                    window.draw(circle);
                }
                break;
            }
            case SimulationMode::SoA:
                for (size_t i = 0; i < soaImpl.balls.count; ++i) {
                    float radius = soaImpl.balls.radii[i];
                    uint32_t color = soaImpl.balls.colors[i];
                    
                    circle.setRadius(radius);
                    circle.setPosition(soaImpl.balls.positions_x[i] - radius, 
                                      soaImpl.balls.positions_y[i] - radius);
                    circle.setFillColor(sf::Color(
                        (color >> 24) & 0xFF,
                        (color >> 16) & 0xFF,
                        (color >> 8) & 0xFF,
                        color & 0xFF
                    ));
                    window.draw(circle);
                }
                break;
            case SimulationMode::AoS:
                for (const auto& ball : aosImpl.balls) {
                    circle.setRadius(ball.radius);
                    circle.setPosition(ball.position.x - ball.radius, ball.position.y - ball.radius);
                    circle.setFillColor(sf::Color(
                        (ball.color >> 24) & 0xFF,
                        (ball.color >> 16) & 0xFF,
                        (ball.color >> 8) & 0xFF,
                        ball.color & 0xFF
                    ));
                    window.draw(circle);
                }
                break;
            case SimulationMode::OOP:
                for (const auto& ball : oopImpl.balls) {
                    circle.setRadius(ball.radius);
                    circle.setPosition(ball.position.x - ball.radius, ball.position.y - ball.radius);
                    circle.setFillColor(sf::Color(
                        (ball.color >> 24) & 0xFF,
                        (ball.color >> 16) & 0xFF,
                        (ball.color >> 8) & 0xFF,
                        ball.color & 0xFF
                    ));
                    window.draw(circle);
                }
                break;
        }
        
        // Draw UI
        window.draw(fpsText);
        window.draw(modeText);
        window.draw(entityCountText);
        window.draw(sleepCountText);
        window.draw(updateTimeText);
        window.draw(helpText);
        
        window.display();
    }
    
    void updateFPS(float frameTime) {
        fpsUpdateTimer += frameTime;
        
        if (fpsUpdateTimer >= 0.25f) { // Update UI 4 times per second
            currentFPS = 1.0f / frameTime;
            fpsUpdateTimer = 0.0f;
            
            std::ostringstream ss;
            ss << "FPS: " << std::fixed << std::setprecision(1) << currentFPS;
            fpsText.setString(ss.str());
            
            modeText.setString(std::string("Mode: ") + getModeString(currentMode));
            entityCountText.setString("Entities: " + std::to_string(entityCount));
            
            size_t sleepingCount = countSleepingBalls();
            sleepCountText.setString("Sleeping: " + std::to_string(sleepingCount) + 
                                     " (" + std::to_string((sleepingCount * 100) / std::max(entityCount, size_t(1))) + "%)");
            
            ss.str("");
            ss << "Update: " << std::fixed << std::setprecision(2) << currentUpdateTimeMs << " ms  ("
               << std::fixed << std::setprecision(1) << (currentItemsPerSecond / 1000000.0f) << " M items/s)";
            updateTimeText.setString(ss.str());
        }
    }
    
    size_t countSleepingBalls() {
        size_t count = 0;
        
        switch (currentMode) {
            case SimulationMode::ECS: {
                auto view = SubzeroECS::View<SleepState>(*world);
                for (auto it = view.begin(); it != view.end(); ++it) {
                    if (it.get<SleepState>().isAsleep) {
                        count++;
                    }
                }
                break;
            }
            case SimulationMode::SoA:
                for (size_t i = 0; i < soaImpl.balls.count; ++i) {
                    if (soaImpl.balls.isAsleep[i]) {
                        count++;
                    }
                }
                break;
            case SimulationMode::AoS:
                for (const auto& ball : aosImpl.balls) {
                    if (ball.isAsleep) {
                        count++;
                    }
                }
                break;
            case SimulationMode::OOP:
                for (const auto& ball : oopImpl.balls) {
                    if (ball.isAsleep) {
                        count++;
                    }
                }
                break;
        }
        
        return count;
    }
};

} // namespace BallsSim

int main() {
    try {
        BallsSim::BallsSimulation sim;
        sim.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
