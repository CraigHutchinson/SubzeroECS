# Using Benchmarks in Visual Studio Code

## CMake Presets for Benchmarks

The project now includes dedicated CMake presets for building with benchmarks enabled. These presets are available for all platforms.

## Available Benchmark Presets

### Release (Recommended for Benchmarks)

- **`windows-x64-release-benchmark`** - Windows x64 Release + Benchmarks
- **`linux-x64-release-benchmark`** - Linux x64 Release + Benchmarks  
- **`macos-release-benchmark`** - macOS Release + Benchmarks

### Debug (For Development/Debugging)

- **`windows-x64-debug-benchmark`** - Windows x64 Debug + Benchmarks
- **`linux-x64-debug-benchmark`** - Linux x64 Debug + Benchmarks
- **`macos-debug-benchmark`** - macOS Debug + Benchmarks

**Important:** Always use **Release** presets for actual performance benchmarking. Debug builds are 10-20x slower and don't represent real performance.

All benchmark presets automatically set:
- `CMAKE_BUILD_TYPE` (Debug or Release)
- `SUBZEROECS_BUILD_BENCHMARKS=ON`

## Using in VS Code

### Method 1: Select Preset from Command Palette (Recommended)

1. **Open Command Palette**: `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (macOS)
2. **Type**: `CMake: Select Configure Preset`
3. **Choose**: `Windows x64 Release + Benchmarks` (or your platform's benchmark preset)
4. **Configure**: VS Code will automatically run CMake configure
5. **Build**: Use `CMake: Build` from command palette or click "Build" in the status bar

### Method 2: Status Bar (Quick Access)

1. Look at the bottom status bar in VS Code
2. Click on the **preset name** (e.g., "windows-x64-debug")
3. Select your platform's **benchmark preset** from the dropdown
4. VS Code will reconfigure automatically
5. Click the **Build** button in the status bar

### Method 3: settings.json (Default Preset)

Set a default preset in your workspace settings:

```json
{
  "cmake.configurePreset": "windows-x64-release-benchmark"
}
```

Add to `.vscode/settings.json` in your workspace.

## Building and Running

### Build Benchmarks

Once the benchmark preset is selected:

**Option A - Command Palette:**
```
Ctrl+Shift+P → CMake: Build
```

**Option B - Status Bar:**
- Click the "Build" button (hammer icon) in the status bar

**Option C - Keyboard:**
- Press `F7` (default build shortcut)

### Run Benchmarks

**Option A - Terminal in VS Code:**

Open integrated terminal (`Ctrl+`` `) and run:

```powershell
# Windows
.\out\Windows-build\windows-x64-release-benchmark\benchmarks\position_update\Release\position_update_benchmark.exe

# Linux
./out/Linux-build/linux-x64-release-benchmark/benchmarks/position_update/position_update_benchmark

# macOS
./out/Darwin-build/macos-release-benchmark/benchmarks/position_update/position_update_benchmark
```

**Option B - Add Launch Configuration:**

Add to `.vscode/launch.json`:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Run Position Update Benchmark",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "${workspaceFolder}/out/Windows-build/windows-x64-release-benchmark/benchmarks/position_update/Release/position_update_benchmark.exe",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "console": "integratedTerminal"
    }
  ]
}
```

Then press `F5` or use "Run → Start Debugging"

**Option C - Add Task:**

Add to `.vscode/tasks.json`:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Run Position Benchmark",
      "type": "shell",
      "command": "${workspaceFolder}/out/Windows-build/windows-x64-release-benchmark/benchmarks/position_update/Release/position_update_benchmark.exe",
      "problemMatcher": [],
      "group": {
        "kind": "test",
        "isDefault": true
      },
      "presentation": {
        "reveal": "always",
        "panel": "new"
      }
    }
  ]
}
```

Run with `Ctrl+Shift+P → Tasks: Run Task → Run Position Benchmark`

## Switching Between Regular and Benchmark Builds

You can easily switch between presets without losing your build artifacts:

1. **Regular Development**: Use `windows-x64-debug` or `windows-x64-release`
2. **Benchmark Testing**: Switch to `windows-x64-release-benchmark`
3. Each preset uses a separate build directory, so they won't conflict

## Verifying Benchmark Build

After configuring with a benchmark preset, check the CMake output panel:

```
-- SUBZEROECS_BUILD_BENCHMARKS: ON
-- Fetching Google Benchmark...
```

If you see this, benchmarks are enabled and will be built.

## Troubleshooting

### "Benchmark not found" after build

**Solution**: Make sure you selected a `-benchmark` preset, not a regular preset.

### Build is very slow

**Solution**: First build downloads Google Benchmark (~5-10 seconds). Subsequent builds are fast.

### Can't find preset in dropdown

**Solution**: 
1. Reload CMake cache: `Ctrl+Shift+P → CMake: Delete Cache and Reconfigure`
2. Restart VS Code
3. Check that CMakePresets.json is valid JSON

### Wrong build directory

Each preset has its own build directory:
- Regular: `out/Windows-build/windows-x64-release/`
- Benchmark: `out/Windows-build/windows-x64-release-benchmark/`

This is intentional to keep builds separate.

## Recommended Workflow

1. **Development**: Use `windows-x64-debug` for daily development
2. **Testing**: Build and run unit tests with regular presets
3. **Benchmarking**: Switch to `windows-x64-release-benchmark` when measuring performance
4. **Always use Release** for benchmarks - Debug builds are 10-20x slower!

## CMake Extension Settings

Recommended VS Code CMake extension settings:

```json
{
  "cmake.configureOnOpen": true,
  "cmake.buildBeforeRun": true,
  "cmake.configurePreset": "windows-x64-release-benchmark",
  "cmake.buildPreset": "windows-x64-release-benchmark"
}
```

Add to `.vscode/settings.json` for automatic benchmark preset selection.
