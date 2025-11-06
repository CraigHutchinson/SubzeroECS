# Rocket Sample

A simple demonstration of SubzeroECS showing rockets moving across the screen.

## What it demonstrates

This sample shows how to:
- Create a World and register component Collections
- Define custom components (Position, Velocity, Rocket)
- Create entities with multiple components
- Implement systems that operate on entities with specific components
- Use Views to query and iterate over entities
- Update component data each frame

## Components

- **Position**: Stores x, y coordinates
- **Velocity**: Stores dx, dy velocity
- **Rocket**: Visual representation (symbol character)

## Systems

- **Movement System**: Updates position based on velocity
- **Render System**: Displays rockets on a ASCII screen buffer

## Building

From the build directory:
```bash
cmake --build . --target RocketSample
```

## Running

```bash
./samples/RocketSample
```

Or on Windows:
```bash
.\samples\Debug\RocketSample.exe
```

The simulation will run for 60 frames, showing three rockets moving at different speeds across the screen.
