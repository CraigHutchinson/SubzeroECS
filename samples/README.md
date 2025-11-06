# SubzeroECS Samples

This directory contains sample applications demonstrating the usage of SubzeroECS.

## Available Samples

### Rocket Sample

A simple animated demonstration showing rockets moving across the screen.

**Location:** `samples/rocket/`

**Demonstrates:**
- Creating a World and registering Collections
- Defining custom components (Position, Velocity, Rocket)
- Creating entities with initial component data
- Implementing systems (movement and rendering)
- Using Views to query entities
- Frame-based game loop

**To build:**

```bash
cmake --build <build_directory> --target RocketSample
```

**To run:**

```bash
# Linux/Mac
./samples/RocketSample

# Windows
.\samples\Debug\RocketSample.exe
```

## Building All Samples

All samples are built automatically when you build the main project. To build only the samples:

```bash
cmake --build <build_directory> --target RocketSample
```

## Sample Output

The rocket sample shows three rockets moving at different speeds:
- Rocket 1 (>) - Slowest
- Rocket 2 (=) - Medium speed
- Rocket 3 (-) - Fastest

The simulation runs for 60 frames with ASCII animation showing the rockets moving from left to right across a 60x20 character screen.
