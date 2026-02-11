# Build Time Optimization Plan for SubzeroECS

## Methodology

Each optimization will be introduced incrementally with before/after measurements:
1. **Baseline**: Current build configuration (no optimizations)
2. **ccache**: Add compiler caching
3. **Unity Build**: Combine compilation units for tests/benchmarks
4. **PCH**: Precompiled headers for common STL includes
5. **Modern Linker**: LLD/mold support
6. **Job Pools**: Optimize parallelization
7. **C++20 Modules**: Future consideration (requires CMake 3.28+, compiler support)

## Manual Measurement Instructions

### Prerequisites
- Clean git state
- Close all running executables from build
- Ensure VS Developer environment or CMake preset with proper compiler

### Measurement Process

For each optimization step:

```powershell
# 1. Clean build directory
Remove-Item -Recurse -Force out\Windows-build\windows-x64

# 2. Configure
Measure-Command { cmake --preset windows-x64 }

# 3. Full clean build (measure this)
Measure-Command { cmake --build --preset windows-x64-debug }

# 4. Touch a file and measure incremental
(Get-Content source\SubzeroECS\World.hpp -Raw) | Set-Content source\SubzeroECS\World.hpp
Measure-Command { cmake --build --preset windows-x64-debug }
```

## Optimization Implementations

### 1. Enable ccache (Easiest Win - 3-10x incremental builds)

**File**: `CMakePresets.json`
**Change**: Add to all configure presets under `cacheVariables`:
```json
"USE_CCACHE": "YES",
"CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
```

### 2. Unity Builds for Tests/Benchmarks (30-50% faster)

**File**: `test/CMakeLists.txt`
**Change**: After `set_target_properties`, add:
```cmake
set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
)
```

**File**: `benchmarks/update_patterns/CMakeLists.txt`
**Change**: Same as above for benchmark targets

### 3. Precompiled Headers (20-40% clean builds)

**File**: `CMakeLists.txt`
**Change**: After target creation for SubzeroECS:
```cmake
target_precompile_headers(${PROJECT_NAME} 
  PRIVATE
    <vector>
    <unordered_map>
    <memory>
    <algorithm>
    <cstdint>
    <type_traits>
)
```

### 4. Modern Linker Support (2-5x linking)

**File**: `cmake/tools.cmake`
**Change**: Add at end:
```cmake
# Enable faster linkers
option(USE_LLD "Use LLVM's lld linker" OFF)
option(USE_MOLD "Use mold linker (fastest)" OFF)

if(USE_MOLD AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
  add_link_options("-fuse-ld=mold")
  message(STATUS "Using mold linker")
elseif(USE_LLD AND CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  add_link_options("-fuse-ld=lld")
  message(STATUS "Using lld linker")
endif()
```

**Preset Change**: Add to `cacheVariables`:
```json
"USE_LLD": "ON"
```

### 5. Ninja Job Pools (Prevents OOM, better parallelization)

**File**: `CMakePresets.json`
**Change**: Add to base presets:
```json
"cacheVariables": {
  "CMAKE_JOB_POOLS": "compile=8;link=2",
  "CMAKE_JOB_POOL_COMPILE": "compile",
  "CMAKE_JOB_POOL_LINK": "link"
}
```

### 6. Optimized Debug Builds

**File**: `CMakePresets.json`
**Change**: Add to windows-base/linux-base/macos-base:
```json
"cacheVariables": {
  "CMAKE_CXX_FLAGS_DEBUG": "-O1 -g"
}
```

### 7. C++20 Modules (Future - Not Ready Yet)

**Status**: ⚠️ **Not Recommended for Production (Feb 2026)**

**Blockers**:
- Requires CMake 3.28+ with experimental features
- MSVC 2022 17.5+ has basic support but still unstable
- GCC 14+ needed (GCC 13 has partial support)
- Clang 16+ required
- Build system tooling (ccache, distcc) has limited support
- IDE integration still maturing

**When Ready** (estimate: late 2026-2027):
- 50-80% faster compilation for large projects
- Better encapsulation and dependency management
- Cleaner build graphs

**File**: `source/SubzeroECS/SubzeroECS.ixx` (future module interface)
```cpp
export module SubzeroECS;

export import :World;
export import :Entity;
export import :System;
// ...
```

**Documentation Note Added**: Keep monitoring:
- CMake 3.30+ releases
- Compiler vendor roadmaps
- CPM.cmake module support

## Expected Results Summary

| Optimization | Clean Build | Incremental | Linking | Complexity |
|--------------|-------------|-------------|---------|------------|
| Baseline     | 100%        | 100%        | 100%    | -          |
| + ccache     | 100%        | 10-30%      | 100%    | Low        |
| + Unity      | 60-70%      | 90%         | 90%     | Low        |
| + PCH        | 60-80%      | 95%         | 100%    | Medium     |
| + LLD/mold   | 100%        | 100%        | 20-50%  | Low        |
| + Job Pools  | 95-100%     | 95-100%     | 95-100% | Low        |
| **Combined** | ~40-50%     | ~10-20%     | ~20-30% | Medium     |

**Total Expected Improvement**:
- Clean builds: **40-60% faster**
- Incremental rebuilds: **5-10x faster**
- Link times: **2-5x faster**

## Tracking Results

Create a spreadsheet or table:
```
| Configuration | Configure (s) | Clean Build (s) | Incremental (s) | Notes |
|---------------|---------------|-----------------|-----------------|-------|
| Baseline      |               |                 |                 |       |
| + ccache      |               |                 |                 |       |
| + Unity       |               |                 |                 |       |
| + PCH         |               |                 |                 |       |
| + Linker      |               |                 |                 |       |
| + Job Pools   |               |                 |                 |       |
```

## Implementation Order

1. ✅ Document methodology (this file)
2. ⏭️ Measure baseline
3. ⏭️ Implement ccache (biggest win for development)
4. ⏭️ Implement unity builds (quick, significant impact)
5. ⏭️ Implement PCH (moderate effort, good payoff)
6. ⏭️ Add linker options (platform-dependent)
7. ⏭️ Fine-tune job pools
8. 📋 Document C++20 modules for future
