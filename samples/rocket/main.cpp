#include "SubzeroECS/World.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/View.hpp"
#include "SubzeroECS/System.hpp"

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

// Components
struct Position
{
	float x = 0.0f;
	float y = 0.0f;
};

struct Velocity
{
	float dx = 0.0f;
	float dy = 0.0f;
};

struct Rocket
{
	char symbol = '>';
};

struct ScreenBounds
{
	int width = 60;
	int height = 20;
};

// Systems
class MovementSystem : public SubzeroECS::System<MovementSystem, Position, Velocity>
{
public:
	float deltaTime = 0.0f;
	
	MovementSystem(SubzeroECS::World& world) 
		: SubzeroECS::System<MovementSystem, Position, Velocity>(world)
	{}
	
	void processEntity(Iterator iEntity)
	{
		Position& pos = iEntity.get<Position>();
		const Velocity& vel = iEntity.get<Velocity>();

		pos.x += vel.dx * deltaTime;
		pos.y += vel.dy * deltaTime;
	}
};

class WrappingSystem : public SubzeroECS::System<WrappingSystem, Position, Velocity, Rocket>
{
public:
	ScreenBounds bounds{60, 20};
	
	WrappingSystem(SubzeroECS::World& world)
		: SubzeroECS::System<WrappingSystem, Position, Velocity, Rocket>(world)
	{}
	
	void processEntity(Iterator iEntity)
	{
		Position& pos = iEntity.get<Position>();
		Velocity& vel = iEntity.get<Velocity>();
		Rocket& rocket = iEntity.get<Rocket>();

		// Wrap horizontally
		if (pos.x >= bounds.width)
		{
			pos.x = 0.0f;
		}
		else if (pos.x < 0)
		{
			pos.x = static_cast<float>(bounds.width - 1);
		}
		
		// Bounce vertically and update symbol
		if (pos.y >= bounds.height)
		{
			pos.y = static_cast<float>(bounds.height - 1);
			vel.dy = -vel.dy;
			rocket.symbol = (vel.dx > 0) ? '/' : '\\';
		}
		else if (pos.y < 0)
		{
			pos.y = 0.0f;
			vel.dy = -vel.dy;
			rocket.symbol = (vel.dx > 0) ? '\\' : '/';
		}
		
		// Update symbol based on direction when moving horizontally
		if (vel.dy == 0.0f)
		{
			rocket.symbol = (vel.dx > 0) ? '>' : '<';
		}
	}
};

class RenderSystem : public SubzeroECS::System<RenderSystem, Position, Rocket>
{
public:
	int screenWidth = 60;
	int screenHeight = 20;
	std::vector<std::vector<char>> buffer;
	
	RenderSystem(SubzeroECS::World& world)
		: SubzeroECS::System<RenderSystem, Position, Rocket>(world)
	{
		buffer.resize(screenHeight, std::vector<char>(screenWidth, ' '));
	}
	
	void update() override
	{
		// Clear screen
		std::cout << "\033[2J\033[H"; // ANSI escape codes to clear screen
		
		// Clear buffer
		for (auto& row : buffer)
		{
			std::fill(row.begin(), row.end(), ' ');
		}
		
		// Update buffer with all rockets
		SubzeroECS::System<RenderSystem, Position, Rocket>::update();
		
		// Draw to screen
		drawBuffer();
	}
	
	void processEntity(Iterator iEntity)
	{
		const Position& pos = iEntity.get<Position>();
		const Rocket& rocket = iEntity.get<Rocket>();
		
		int x = static_cast<int>(pos.x);
		int y = static_cast<int>(pos.y);
		
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		{
			buffer[y][x] = rocket.symbol;
		}
	}
	
private:
	void drawBuffer()
	{
		// Draw top border
		std::cout << '+' << std::string(screenWidth, '-') << '+' << '\n';
		
		// Draw buffer
		for (const auto& row : buffer)
		{
			std::cout << '|';
			for (char c : row)
			{
				std::cout << c;
			}
			std::cout << '|' << '\n';
		}
		
		// Draw bottom border
		std::cout << '+' << std::string(screenWidth, '-') << '+' << '\n';
	}
};

int main()
{
	std::cout << "Rocket ECS Sample - SubzeroECS\n";
	std::cout << "Press Ctrl+C to exit\n\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	// Create world and register component collections
	SubzeroECS::World world;
	SubzeroECS::Collection<Position, Velocity, Rocket> collections(world);
	
	// Screen dimensions
	const int screenWidth = 60;
	const int screenHeight = 20;
	const ScreenBounds bounds{screenWidth, screenHeight};
	
	// Create rockets with varied trajectories
	auto rocket1 = world.create(
		Position{0.0f, 5.0f},
		Velocity{15.0f, 2.0f},  // Moving right and slightly down
		Rocket{'>'}
	);
	
	auto rocket2 = world.create(
		Position{30.0f, 2.0f},
		Velocity{12.0f, 3.0f},  // Moving right and down
		Rocket{'='}
	);
	
	auto rocket3 = world.create(
		Position{10.0f, 18.0f},
		Velocity{18.0f, -2.5f},  // Moving right and up
		Rocket{'-'}
	);
	
	std::cout << "Created 3 rockets with wrapping and bouncing!\n";
	std::cout << "Starting simulation...\n\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	// Create systems
	MovementSystem movementSystem(world);
	WrappingSystem wrappingSystem(world);
	wrappingSystem.bounds = bounds;
	
	RenderSystem renderSystem(world);
	renderSystem.screenWidth = screenWidth;
	renderSystem.screenHeight = screenHeight;

    std::array<SubzeroECS::ISystem*, 3> systems = { &movementSystem, &wrappingSystem, &renderSystem };

	// Game loop
	const float deltaTime = 0.30f; // 100ms per frame
	const int maxFrames = 100;  // Extended to see more wrapping
	int frame = 0;
	
	while (frame < maxFrames)
	{
		// Update systems
		movementSystem.deltaTime = deltaTime;
        for ( auto system : systems )
        {
            system->update();
        }

		std::cout << "Frame: " << frame << " / " << maxFrames << "\n";
		
		// Sleep to control frame rate
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(deltaTime * 1000)));
		
		frame++;
	}
	
	std::cout << "\nSimulation complete!\n";
	std::cout << "Final positions:\n";
	
	SubzeroECS::View<Position, Rocket> finalView(world);
	int rocketNum = 1;
	for (auto entity : finalView)
	{
		const Position& pos = entity.get<Position>();
		const Rocket& rocket = entity.get<Rocket>();
		std::cout << "  Rocket " << rocketNum++ << " [" << rocket.symbol << "]: x=" 
		          << std::fixed << std::setprecision(1) << pos.x 
		          << ", y=" << pos.y << "\n";
	}
	
	return 0;
}
