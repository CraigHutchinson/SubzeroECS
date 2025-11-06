#include "SubzeroECS/World.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/View.hpp"

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
void movementSystem(SubzeroECS::World& world, float deltaTime)
{
	SubzeroECS::View<Position, Velocity> view(world);
	
	for (auto entity : view)
	{
		Position& pos = entity.get<Position>();
		const Velocity& vel = entity.get<Velocity>();
		
		pos.x += vel.dx * deltaTime;
		pos.y += vel.dy * deltaTime;
	}
}

void wrappingSystem(SubzeroECS::World& world, const ScreenBounds& bounds)
{
	SubzeroECS::View<Position, Velocity, Rocket> view(world);
	
	for (auto entity : view)
	{
		Position& pos = entity.get<Position>();
		Velocity& vel = entity.get<Velocity>();
		Rocket& rocket = entity.get<Rocket>();
		
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
}

void renderSystem(SubzeroECS::World& world, int screenWidth, int screenHeight)
{
	// Clear screen
	std::cout << "\033[2J\033[H"; // ANSI escape codes to clear screen
	
	// Create a simple ASCII buffer
	std::vector<std::vector<char>> buffer(screenHeight, std::vector<char>(screenWidth, ' '));
	
	// Render rockets
	SubzeroECS::View<Position, Rocket> view(world);
	
	for (auto entity : view)
	{
		const Position& pos = entity.get<Position>();
		const Rocket& rocket = entity.get<Rocket>();
		
		int x = static_cast<int>(pos.x);
		int y = static_cast<int>(pos.y);
		
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		{
			buffer[y][x] = rocket.symbol;
		}
	}
	
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
	
	// Game loop
	const float deltaTime = 0.1f; // 100ms per frame
	const int maxFrames = 100;  // Extended to see more wrapping
	int frame = 0;
	
	while (frame < maxFrames)
	{
		// Update systems
		movementSystem(world, deltaTime);
		wrappingSystem(world, bounds);
		
		// Render
		renderSystem(world, screenWidth, screenHeight);
		
		std::cout << "Frame: " << frame << " / " << maxFrames << "\n";
		
		// Sleep to control frame rate
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		
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
