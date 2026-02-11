# C++20 Modules - Future Build Optimization

## Status: ⚠️ NOT RECOMMENDED FOR PRODUCTION (as of February 2026)

While C++20 modules promise significant build time improvements (50-80% faster compilation for large projects), the ecosystem is not yet ready for production use in cross-platform projects.

## Current Blockers (Feb 2026)

### Compiler Support
- **MSVC**: 17.5+ has basic support, but still encountering issues with complex templates and concepts
- **GCC**: 14+ required for full support (GCC 13 has partial/experimental support)
- **Clang**: 16+ required, improving but still has edge cases
- **Standard Library Modules**: `std` module not universally available

### Build System
- **CMake**: Requires 3.28+ with `CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API` 
- **Ninja**: 1.11+ required for proper dependency tracking
- **Other Build Systems**: Limited or no support

### Tooling Ecosystem
- **ccache**: Limited support for module files (.ifc, .pcm)
- **distcc**: No distributed compilation support for modules
- **IDE Integration**: Still maturing in VS Code, Visual Studio, CLion
- **Static Analyzers**: clang-tidy, cppcheck have partial support

### Dependency Management
- **CPM.cmake**: No special handling for module-enabled dependencies
- **vcpkg/Conan**: Limited packages with module support
- **Header-only libraries**: Cannot directly import as modules

## Expected Timeline

### Late 2026 - Early 2027
- Stable compiler implementations
- Better IDE integration
- CMake non-experimental module support
- Major libraries start offering module interfaces

### 2027-2028
- ccache/build cache solutions mature
- Standard library modules universally available
- Most popular libraries have module support
- Production-ready for new projects

## When Ready: Implementation Plan for SubzeroECS

### Phase 1: Module Interface Units

Create module interface files alongside existing headers:

```cpp
// source/SubzeroECS/SubzeroECS.ixx
export module SubzeroECS;

// Re-export sub-modules
export import :World;
export import :Entity;
export import :System;
export import :Collection;
export import :Query;
export import :View;

// Or export specific interfaces
export namespace SubzeroECS {
    // Forward declarations or key types
}
```

### Phase 2: Module Partitions

```cpp
// source/SubzeroECS/World.ixx
export module SubzeroECS:World;

import :Entity;
import :Collection;

export namespace SubzeroECS {
    class World {
        // Implementation
    };
}
```

### Phase 3: CMake Configuration

```cmake
# Future CMakeLists.txt (CMake 3.30+)
add_library(SubzeroECS)

target_sources(SubzeroECS
  PUBLIC
    FILE_SET CXX_MODULES 
    BASE_DIRS ${PROJECT_SOURCE_DIR}/source
    FILES
      source/SubzeroECS/SubzeroECS.ixx
      source/SubzeroECS/World.ixx
      source/SubzeroECS/Entity.ixx
      source/SubzeroECS/System.ixx
      source/SubzeroECS/Collection.ixx
      # ... other module interface files
)

# Keep headers for backward compatibility during transition
target_sources(SubzeroECS
  PUBLIC
    FILE_SET HEADERS
    FILES
      source/SubzeroECS/World.hpp
      # ... existing headers
)
```

### Phase 4: Preset Configuration

```json
{
  "name": "windows-modules",
  "inherits": "windows-base",
  "cacheVariables": {
    "CMAKE_CXX_STANDARD": "20",
    "CMAKE_CXX_SCAN_FOR_MODULES": "ON"
  }
}
```

## Expected Benefits (When Available)

### Build Time Improvements
- **Initial Build**: 50-80% faster compilation
- **Incremental Builds**: 2-5x faster (better than even ccache in some cases)
- **Parallel Builds**: Better parallelization due to cleaner dependency graphs

### Code Quality
- **Encapsulation**: True enforcement of public/private interfaces
- **Dependency Management**: Explicit, compile-time checked dependencies
- **ODR Violations**: Eliminated by design
- **Faster Error Detection**: Interface changes caught immediately

### Binary Size
- **Template Instantiation**: Reduced duplication
- **Debug Info**: Smaller debug symbols
- **Overall**: 10-30% smaller binaries in some cases

## Migration Strategy (Future)

### Approach 1: Dual-Mode (Recommended)
Keep both headers and modules for 2-3 years:
```cpp
// Users can choose:
#include "SubzeroECS/World.hpp"  // Traditional
// or
import SubzeroECS;                // Modern
```

### Approach 2: Header Units (Intermediate)
Convert existing headers to header units:
```cpp
import "SubzeroECS/World.hpp";  // Import header as module
```

### Approach 3: Full Migration
Remove headers entirely (only after ecosystem matures)

## Monitoring Progress

### Key Indicators to Watch
1. **CMake 3.30+ Release**: Non-experimental module support
2. **Compiler Versions**: When Ubuntu LTS, RHEL, etc. ship with module-capable compilers
3. **Third-Party Libraries**: fmt, GoogleTest, Benchmark with module support
4. **ccache**: Module caching support in stable release
5. **CPM.cmake**: MODULE_ENABLED packages

### Resources to Monitor
- [CMake Module Support Docs](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html)
- [C++ Modules Status](https://en.cppreference.com/w/cpp/language/modules)
- [LLVM Modules Documentation](https://clang.llvm.org/docs/StandardCPlusPlusModules.html)
- [GCC Modules](https://gcc.gnu.org/wiki/cxx-modules)

## Current Recommendation

**For SubzeroECS (Feb 2026):**
- ✅ Use all current optimizations: ccache, PCH, unity builds, fast linkers
- ⏸️ Wait for C++20 modules ecosystem to mature
- 📊 Re-evaluate in 12-18 months (Q3 2027)
- 🔬 Experiment in a separate branch if interested
- 📚 Keep this document updated with ecosystem progress

## References

- C++20 Standard: ISO/IEC 14882:2020, Section 10 (Modules)
- P1689R5: Format for describing dependencies of source files
- P2465R3: Standard Library Modules std and std.compat
- CMake Issue #18355: C++20 modules support tracking

---

**Last Updated**: February 11, 2026  
**Next Review**: Q3 2027  
**Status**: Monitoring, Not Implementing
