#Requires -Version 5.1
<#
.SYNOPSIS
    Builds the Mod Menu .geode for Geometry Dash on Windows.

.DESCRIPTION
    Configures and builds the mod with CMake + Visual Studio 2022, then copies
    the resulting .geode to the project root.

    Needs the Geode SDK (https://github.com/geode-sdk/sdk/releases) with the
    GEODE_SDK environment variable pointing at the extracted SDK folder, plus
    Visual Studio 2022 with the "Desktop development with C++" workload.

.PARAMETER Configuration
    Build configuration: Release (default), RelWithDebInfo or Debug.

.PARAMETER SdkPath
    Path to the Geode SDK. Defaults to $env:GEODE_SDK.

.PARAMETER Clean
    Delete the build folder before configuring.

.PARAMETER Install
    Also copy the built .geode into the local Geode mods folder.

.EXAMPLE
    .\build.ps1

.EXAMPLE
    .\build.ps1 -Install -Clean
#>
[CmdletBinding()]
param(
    [ValidateSet('Release', 'RelWithDebInfo', 'Debug')]
    [string] $Configuration = 'Release',

    [string] $SdkPath = $env:GEODE_SDK,

    [switch] $Clean,

    [switch] $Install
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $root

function Write-Step([string] $text) {
    Write-Host ""
    Write-Host "==> $text" -ForegroundColor Cyan
}

function Fail([string] $text) {
    Write-Host ""
    Write-Host "!!  $text" -ForegroundColor Red
    exit 1
}

# --- Geode SDK --------------------------------------------------------------

if (-not $SdkPath) {
    foreach ($candidate in @("$root\geode-sdk", "$env:USERPROFILE\geode-sdk", "C:\geode-sdk")) {
        if (Test-Path (Join-Path $candidate 'CMakeLists.txt')) {
            $SdkPath = $candidate
            break
        }
    }
}

if (-not $SdkPath) {
    Fail @"
Could not find the Geode SDK.

Download it from https://github.com/geode-sdk/sdk/releases (v5.x), extract it
somewhere permanent, then set the environment variable and reopen your terminal:

    setx GEODE_SDK "C:\path\to\geode-sdk"

Or pass it to this script directly:

    .\build.ps1 -SdkPath "C:\path\to\geode-sdk"
"@
}

if (-not (Test-Path $SdkPath)) {
    Fail "GEODE_SDK points at '$SdkPath', which does not exist."
}

$SdkPath = (Resolve-Path $SdkPath).Path
if (-not (Test-Path (Join-Path $SdkPath 'CMakeLists.txt'))) {
    Fail "'$SdkPath' is not a Geode SDK folder (no CMakeLists.txt in it)."
}

# The SDK is consumed from the environment by CMakeLists.txt.
$env:GEODE_SDK = $SdkPath
Write-Step "Using Geode SDK at $SdkPath"

# --- CMake ------------------------------------------------------------------

$cmake = $null
$systemCmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($systemCmake) {
    $cmake = $systemCmake.Source
}
else {
    # Fall back to the CMake that ships with Visual Studio.
    $vsRoot = Join-Path $env:ProgramFiles 'Microsoft Visual Studio\2022'
    if (Test-Path $vsRoot) {
        $bundled = Get-ChildItem -Path $vsRoot -Filter 'cmake.exe' -Recurse -ErrorAction SilentlyContinue |
            Select-Object -First 1
        if ($bundled) {
            $cmake = $bundled.FullName
        }
    }
}

if (-not $cmake) {
    Fail @"
Could not find CMake.

Install Visual Studio 2022 with the "Desktop development with C++" workload
(it includes CMake), or install CMake from https://cmake.org/download/ and put
it on your PATH.
"@
}

# --- Build ------------------------------------------------------------------

$buildDir = Join-Path $root 'build'

if ($Clean -and (Test-Path $buildDir)) {
    Write-Step "Removing $buildDir"
    Remove-Item -Path $buildDir -Recurse -Force
}

Write-Step "Configuring (Visual Studio 17 2022, x64)"
& $cmake -S $root -B $buildDir -G 'Visual Studio 17 2022' -A x64
if ($LASTEXITCODE -ne 0) {
    Fail @"
CMake configure failed (exit code $LASTEXITCODE).

The Visual Studio 2022 C++ toolchain is required for this generator. Install
"Desktop development with C++" in the Visual Studio Installer and try again.
"@
}

Write-Step "Building $Configuration"
& $cmake --build $buildDir --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) {
    Fail "Build failed (exit code $LASTEXITCODE). See the compiler errors above."
}

# --- Collect the .geode -----------------------------------------------------

$geode = Get-ChildItem -Path $buildDir -Filter '*.geode' -Recurse -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1

if (-not $geode) {
    Fail "The build finished but no .geode file was produced in '$buildDir'."
}

$output = Join-Path $root $geode.Name
Copy-Item -Path $geode.FullName -Destination $output -Force

Write-Host ""
Write-Host ("Built {0} ({1:n0} KB)" -f $output, ($geode.Length / 1KB)) -ForegroundColor Green

if ($Install) {
    $mods = Join-Path $env:LOCALAPPDATA 'GeometryDash\geode\mods'
    if (-not (Test-Path $mods)) {
        Write-Host "!!  No Geode mods folder at $mods - skipping install." -ForegroundColor Yellow
    }
    else {
        Copy-Item -Path $output -Destination $mods -Force
        Write-Host "Installed into $mods" -ForegroundColor Green
    }
}
else {
    Write-Host "Copy it into <Geometry Dash>\geode\mods, or run .\build.ps1 -Install." -ForegroundColor Gray
}
