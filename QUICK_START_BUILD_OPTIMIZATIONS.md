# Quick Start: Testing Build Optimizations

## ✅ All Optimizations Have Been Implemented!

The following build time optimizations are now active in SubzeroECS:
- **ccache** (compiler caching)
- **Precompiled Headers** (PCH)
- **Unity Builds** (for tests & benchmarks)
- **Job Pools** (optimized parallelization)
- **Modern Linker Support** (LLD/mold)

## Prerequisites

Before testing, ensure you have:

1. **ccache installed**:
   ```powershell
   # Windows (using Chocolatey)
   choco install ccache
   
   # Or download from: https://ccache.dev/download.html
   ```

2. **Clean environment**: No processes locking build files

3. **VS Code or VS Developer Command Prompt** (for Windows)

## Quick Test Instructions

### Method 1: Using VS Code CMake Extension

1. **Delete Build Directory**:
   - In VS Code, delete `out\Windows-build\windows-x64` folder
   - Or run: `Remove-Item -Recurse out\Windows-build\windows-x64 -Force`

2. **Reconfigure**:
   - Press `Ctrl+Shift+P` → "CMake: Delete Cache and Reconfigure"
   - Or: CMake: Configure
   - Select preset: `windows-x64`

3. **Build All**:
   - Press `Ctrl+Shift+P` → "CMake: Build"
   - Or press `F7`
   - **Note the build time!**

4. **Test Incremental Build**:
   - Touch a file: `(Get-Content source\SubzeroECS\World.hpp -Raw) | Set-Content source\SubzeroECS\World.hpp`
   - Build again (F7)
   - **Should be much faster!**

5. **Test ccache**:
   ```powershell
   # Clean and rebuild to populate cache
   cmake --build --preset windows-x64-debug --clean-first
   
   # Check ccache stats
   ccache -s
   
   # Clean and rebuild again (should use cache)
   cmake --build --preset windows-x64-debug --clean-first
   
   # Check stats again - should show cache hits
   ccache -s
   ```

### Method 2: Using Command Line

```powershell
# 1. Navigate to project
cd d:\Github\SubzeroECS

# 2. Clean build directory
Remove-Item -Recurse out\Windows-build\windows-x64 -Force -ErrorAction SilentlyContinue

# 3. Configure (time this)
Measure-Command { cmake --preset windows-x64 }

# 4. Clean build (time this - BASELINE)
Measure-Command { cmake --build --preset windows-x64-debug }

# 5. Touch a file
(Get-Content source\SubzeroECS\World.hpp -Raw) | Set-Content source\SubzeroECS\World.hpp

# 6. Incremental build (time this - should be MUCH faster with ccache)
Measure-Command { cmake --build --preset windows-x64-debug }
```

### Method 3: Using Profiling Scripts

```powershell
# Automated measurement (requires clean environment setup)
.\profile_build.ps1 -Label "with_optimizations" -Configuration Debug
```

## Verifying Optimizations Are Active

### 1. Check ccache is Working

```powershell
ccache -s
```

Look for:
- Cache hits (should increase on rebuilds)
- Cache misses (first build)
- Cache hit rate % (aim for >80% on second clean build)

### 2. Check Build Output

During configuration, you should see:
```
-- CPM: Adding package Ccache.cmake@1.2.4
```

During build, check for:
- PCH being built: `Precompiling SubzeroECS.pch`
- Unity build files: `CMakeFiles/...unity_*.cxx`

### 3. Check CMakeCache.txt

```powershell
Get-Content out\Windows-build\windows-x64\CMakeCache.txt | Select-String -Pattern "USE_CCACHE|UNITY_BUILD"
```

Should show:
```
USE_CCACHE:STRING=YES
```

## Expected Results

### First Build (Cold Cache)
- **Configuration**: 5-15 seconds
- **Compilation**: Baseline time
- ccache: All misses

### Second Clean Build (Warm Cache)
- **Configuration**: 5-15 seconds
- **Compilation**: **5-10x faster** 🚀
- ccache: >90% hits

### Incremental Build (Touch One File)
- **Compilation**: **1-5 seconds** ⚡
- Only affected files rebuild

## Troubleshooting

### ccache not found
```powershell
# Verify installation
where.exe ccache

# If not found, install:
choco install ccache

# Or add to PATH manually
```

### Wrong linker being used (ld.exe instead of link.exe)
This happens if Strawberry Perl or MinGW's ld.exe is in PATH before MSVC tools.

**Solution**: Use VS Developer Command Prompt or clean PATH:
```powershell
# Option 1: Use VS Developer PowerShell
# In VS Code: Terminal → New Terminal → Select "Developer PowerShell for VS 2022"

# Option 2: Remove conflicting tools from PATH temporarily
$env:PATH = ($env:PATH -split ';' | Where-Object { $_ -notmatch 'Strawberry|mingw' }) -join ';'
```

### Build fails with "cannot access file"
- Close any running executables from build folder
- Close VS Code's CMake output terminals
- Use `Stop-Process -Name "BallsSimulation" -Force`

### "UNITY_BUILD not found" or similar
- CMake version too old (need 3.16+)
- Update CMake: `choco upgrade cmake`

### PCH not being used
- Check build output for "Precompiling" messages
- Ensure target has sources to compile
- PCH only affects .cpp files, not headers

## Comparing Before/After

If you want to measure the actual improvement:

### Before (Baseline)
1. Checkout previous commit (before optimizations)
2. Clean build and measure time
3. Note the time

### After (With Optimizations)
1. Checkout current commit (with optimizations)
2. Clean build and measure time
3. Compare!

Expected improvement: **40-60% faster**

## What Each Optimization Does

### ccache
- **First build**: Caches compiled objects
- **Subsequent builds**: Reuses cached objects
- **Best for**: Switching branches, cleaning builds

### Precompiled Headers
- **Precompiles**: Common STL headers once
- **Reuses**: For every .cpp file
- **Best for**: Large projects with common headers

### Unity Builds
- **Combines**: Multiple .cpp files
- **Reduces**: Parsing overhead
- **Best for**: Test files with similar includes

### Job Pools
- **Limits**: Parallel compilation
- **Prevents**: Out-of-memory errors
- **Best for**: Large projects on limited RAM

## Files Changed

All changes are documented in:
- [BUILD_OPTIMIZATIONS_SUMMARY.md](BUILD_OPTIMIZATIONS_SUMMARY.md) - Complete details
- [BUILD_OPTIMIZATION_PLAN.md](BUILD_OPTIMIZATION_PLAN.md) - Methodology
- [documentation/pages/CPP20_MODULES_FUTURE.md](documentation/pages/CPP20_MODULES_FUTURE.md) - Future improvements

## Need Help?

1. Check the error output carefully
2. Verify ccache is installed and in PATH
3. Ensure using correct compiler toolchain
4. Try building with verbose output: `cmake --build --preset windows-x64-debug --verbose`
5. Check CMake version: `cmake --version` (need 3.20+)

---

**Ready to build faster? Follow the Quick Test Instructions above!** 🚀
