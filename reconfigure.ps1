# Reconfigure CMake with optimizations enabled
# This script should be run from a VS Code terminal (which has proper environment)

param(
    [string]$PresetName = "windows-x64"
)

$ErrorActionPreference = "Stop"

Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host " Reconfiguring Build with Optimizations" -ForegroundColor Cyan
Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host ""

# Delete build directory to force clean reconfigure
$BuildDir = "out\Windows-build\$PresetName"
Write-Host "Removing existing build directory: $BuildDir" -ForegroundColor Yellow

if (Test-Path $BuildDir) {
    # Try to delete, but if files are locked, just delete CMakeCache
    try {
        Remove-Item -Recurse -Force $BuildDir -ErrorAction Stop
        Write-Host "  ✓ Build directory deleted" -ForegroundColor Green
    } catch {
        Write-Host "  ! Some files locked - deleting CMakeCache only" -ForegroundColor Yellow
        Remove-Item "$BuildDir\CMakeCache.txt" -Force -ErrorAction SilentlyContinue
        Remove-Item "$BuildDir\CMakeFiles" -Recurse -Force -ErrorAction SilentlyContinue
    }
}

Write-Host ""
Write-Host "Configuring with preset: $PresetName" -ForegroundColor Yellow
Write-Host "Optimizations that will be enabled:" -ForegroundColor Cyan
Write-Host "  ✓ ccache (compiler caching)" -ForegroundColor Green
Write-Host "  ✓ Precompiled headers (PCH)" -ForegroundColor Green  
Write-Host "  ✓ Unity builds (tests/benchmarks)" -ForegroundColor Green
Write-Host "  ✓ Export compile commands" -ForegroundColor Green
Write-Host "  ✓ Job pools (8 compile, 2 link)" -ForegroundColor Green
Write-Host "  ✓ Modern linker support (if available)" -ForegroundColor Green
Write-Host ""

# Configure
cmake --preset $PresetName

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "==============================================================" -ForegroundColor Green
    Write-Host " Configuration Successful!" -ForegroundColor Green
    Write-Host "==============================================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Verify optimizations are enabled:" -ForegroundColor Cyan
    
    $CacheFile = "$BuildDir\CMakeCache.txt"
    if (Test-Path $CacheFile) {
        $cache = Get-Content $CacheFile
        
        if ($cache | Select-String "USE_CCACHE:STRING=YES") {
            Write-Host "  ✓ ccache: ENABLED" -ForegroundColor Green
        } else {
            Write-Host "  ✗ ccache: NOT ENABLED" -ForegroundColor Red
        }
        
        if ($cache | Select-String "CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON") {
            Write-Host "  ✓ Export Compile Commands: ENABLED" -ForegroundColor Green
        }
        
        if ($cache | Select-String "CMAKE_JOB_POOLS:STRING=") {
            Write-Host "  ✓ Job Pools: CONFIGURED" -ForegroundColor Green
        }
    }
    
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Yellow
    Write-Host "  1. Build: cmake --build --preset $PresetName-debug" -ForegroundColor White
    Write-Host "  2. Profile: .\profile_build.ps1 -Label optimized -SkipConfigure" -ForegroundColor White
    Write-Host ""
    
} else {
    Write-Host ""
    Write-Host "==============================================================" -ForegroundColor Red
    Write-Host " Configuration Failed!" -ForegroundColor Red
    Write-Host "==============================================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "This usually happens because:" -ForegroundColor Yellow
    Write-Host "  1. Not running in VS Code terminal (needs VS environment)" -ForegroundColor White
    Write-Host "  2. CMake can't find the compiler (cl.exe not in PATH)" -ForegroundColor White
    Write-Host ""
    Write-Host "Solutions:" -ForegroundColor Yellow
    Write-Host "  • Use VS Code's terminal (has VS environment)" -ForegroundColor White
    Write-Host "  • Or use CMake extension: Ctrl+Shift+P → 'CMake: Configure'" -ForegroundColor White
    Write-Host ""
    exit 1
}
