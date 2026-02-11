# Build Time Profiling Script for SubzeroECS
# Measures clean build and incremental rebuild times

param(
    [string]$PresetName = "windows-x64",
    [string]$Configuration = "Debug",
    [string]$Label = "baseline",
    [switch]$SkipClean,
    [switch]$SkipConfigure,
    [switch]$VerboseBuild
)

$ErrorActionPreference = "Stop"
$SourceDir = $PSScriptRoot
$LogFile = Join-Path $SourceDir "build_profile_results.csv"

# Initialize results file if it doesn't exist
if (-not (Test-Path $LogFile)) {
    "Timestamp,Label,Preset,Configuration,CleanBuildTime(s),IncrementalBuildTime(s),TotalTargets,Notes" | Out-File $LogFile -Encoding UTF8
}

Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host " Build Time Profiling - $Label" -ForegroundColor Cyan
Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host "Preset: $PresetName"
Write-Host "Configuration: $Configuration"
Write-Host ""

# Check optimization settings from CMakeCache
$BuildDir = Join-Path $SourceDir "out\Windows-build\$PresetName"
$CacheFile = Join-Path $BuildDir "CMakeCache.txt"

if (Test-Path $CacheFile) {
    Write-Host "Build Optimization Status:" -ForegroundColor Yellow
    $CacheContent = Get-Content $CacheFile
    
    # Check ccache
    $ccacheEnabled = $CacheContent | Select-String "USE_CCACHE:BOOL=ON"
    if ($ccacheEnabled) {
        Write-Host "  ✓ ccache: ENABLED" -ForegroundColor Green
    } else {
        Write-Host "  ✗ ccache: disabled" -ForegroundColor Gray
    }
    
    # Check export compile commands
    $compileCommandsEnabled = $CacheContent | Select-String "CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON"
    if ($compileCommandsEnabled) {
        Write-Host "  ✓ Export Compile Commands: ENABLED" -ForegroundColor Green
    } else {
        Write-Host "  ✗ Export Compile Commands: disabled" -ForegroundColor Gray
    }
    
    # Check job pools
    $jobPools = $CacheContent | Select-String "CMAKE_JOB_POOLS:STRING="
    if ($jobPools) {
        $jobPoolValue = ($jobPools -split "=", 2)[1]
        Write-Host "  ✓ Job Pools: $jobPoolValue" -ForegroundColor Green
    } else {
        Write-Host "  ✗ Job Pools: not configured" -ForegroundColor Gray
    }
    
    # Check linker
    $useLLD = $CacheContent | Select-String "USE_LLD:BOOL=ON"
    $useMold = $CacheContent | Select-String "USE_MOLD:BOOL=ON"
    if ($useMold) {
        Write-Host "  ✓ Linker: mold (fastest)" -ForegroundColor Green
    } elseif ($useLLD) {
        Write-Host "  ✓ Linker: lld (fast)" -ForegroundColor Green
    } else {
        Write-Host "  ○ Linker: default" -ForegroundColor Gray
    }
    
    Write-Host ""
    Write-Host "Note: PCH and Unity Build status checked per-target during build" -ForegroundColor Gray
    Write-Host ""
}

Write-Host ""

# Clean build measurement
$CleanBuildTime = 0
$UseCleanFirst = $false
if (-not $SkipClean) {
    Write-Host "[1/3] Cleaning build directory..." -ForegroundColor Yellow
    $BuildDir = Join-Path $SourceDir "out\Windows-build\$PresetName"
    if (Test-Path $BuildDir) {
        try {
            Remove-Item -Path $BuildDir -Recurse -Force -ErrorAction Stop
        } catch {
            Write-Host "Warning: Could not delete some files (may be in use). Using 'cmake --build --clean-first' instead." -ForegroundColor Yellow
            $UseCleanFirst = $true
        }
    }
    
    Write-Host "[2/3] Configuring project..." -ForegroundColor Yellow
    if (-not $SkipConfigure) {
        $ConfigureStart = Get-Date
        cmake --preset $PresetName
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Configuration failed!" -ForegroundColor Red
            exit 1
        }
        $ConfigureEnd = Get-Date
        $ConfigureTime = ($ConfigureEnd - $ConfigureStart).TotalSeconds
        Write-Host "Configuration time: $([math]::Round($ConfigureTime, 2))s" -ForegroundColor Green
    } else {
        Write-Host "Skipping configuration (using existing)" -ForegroundColor Gray
        $ConfigureTime = 0
    }
    
    Write-Host "[3/3] Clean build (all targets)..." -ForegroundColor Yellow
    $BuildStart = Get-Date
    
    if ($VerboseBuild) {
        if ($UseCleanFirst) {
            cmake --build --preset "$PresetName-$($Configuration.ToLower())" --clean-first --verbose
        } else {
            cmake --build --preset "$PresetName-$($Configuration.ToLower())" --verbose
        }
    } else {
        if ($UseCleanFirst) {
            cmake --build --preset "$PresetName-$($Configuration.ToLower())" --clean-first
        } else {
            cmake --build --preset "$PresetName-$($Configuration.ToLower())"
        }
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        exit 1
    }
    $BuildEnd = Get-Date
    $CleanBuildTime = ($BuildEnd - $BuildStart).TotalSeconds
    
    Write-Host ""
    Write-Host "Clean build completed in: $([math]::Round($CleanBuildTime, 2))s" -ForegroundColor Green
    Write-Host "Total (configure + build): $([math]::Round($ConfigureTime + $CleanBuildTime, 2))s" -ForegroundColor Green
}

# Incremental rebuild measurement (touch a file and rebuild)
Write-Host ""
Write-Host "Measuring incremental rebuild..." -ForegroundColor Yellow

# Touch a source file to trigger incremental rebuild
$TestFile = Join-Path $SourceDir "source\SubzeroECS\World.hpp"
if (Test-Path $TestFile) {
    (Get-Content $TestFile -Raw) | Set-Content $TestFile -NoNewline
}

$IncrementalStart = Get-Date

if ($VerboseBuild) {
    cmake --build --preset "$PresetName-$($Configuration.ToLower())" --verbose
} else {
    cmake --build --preset "$PresetName-$($Configuration.ToLower())"
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "Incremental build failed!" -ForegroundColor Red
    exit 1
}

$IncrementalEnd = Get-Date
$IncrementalBuildTime = ($IncrementalEnd - $IncrementalStart).TotalSeconds

Write-Host "Incremental rebuild completed in: $([math]::Round($IncrementalBuildTime, 2))s" -ForegroundColor Green

# Count targets
$TargetCount = "N/A"

# Log results
$Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$ResultLine = "$Timestamp,$Label,$PresetName,$Configuration,$([math]::Round($CleanBuildTime, 2)),$([math]::Round($IncrementalBuildTime, 2)),$TargetCount,"
$ResultLine | Out-File $LogFile -Append -Encoding UTF8

Write-Host ""
Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host " RESULTS SUMMARY - $Label" -ForegroundColor Cyan
Write-Host "==============================================================" -ForegroundColor Cyan
Write-Host "Clean Build Time:        $([math]::Round($CleanBuildTime, 2))s" -ForegroundColor White
Write-Host "Incremental Build Time:  $([math]::Round($IncrementalBuildTime, 2))s" -ForegroundColor White
Write-Host ""

# Check if ccache is available and show stats
if (Get-Command ccache -ErrorAction SilentlyContinue) {
    Write-Host "ccache Statistics:" -ForegroundColor Yellow
    try {
        $ccacheStats = ccache -s 2>$null
        if ($ccacheStats) {
            $hits = ($ccacheStats | Select-String "cache hit" | Select-Object -First 1) -replace '.*:\s*', ''
            $misses = ($ccacheStats | Select-String "cache miss" | Select-Object -First 1) -replace '.*:\s*', ''
            Write-Host "  Cache hits:   $hits" -ForegroundColor Green
            Write-Host "  Cache misses: $misses" -ForegroundColor Cyan
        }
    } catch {
        Write-Host "  (ccache stats unavailable)" -ForegroundColor Gray
    }
    Write-Host ""
}

Write-Host "Results logged to: $LogFile" -ForegroundColor Gray
Write-Host ""

# Display comparison if previous results exist
$AllResults = Import-Csv $LogFile | Where-Object { $_.Preset -eq $PresetName -and $_.Configuration -eq $Configuration }
if ($AllResults.Count -gt 1) {
    Write-Host "Historical Results (same preset/config):" -ForegroundColor Cyan
    $AllResults | Select-Object Label, @{Name='Clean(s)';Expression={$_.'CleanBuildTime(s)'}}, @{Name='Incremental(s)';Expression={$_.'IncrementalBuildTime(s)'}} | Format-Table -AutoSize
}
