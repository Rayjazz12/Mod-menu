# Mod Menu (Geode)

A Geometry Dash mod that adds a big, tabbed mod menu in the style of the classic
Mega Hack layout.

- 14 categories in the left sidebar, 300+ hack rows
- live search across every category
- per-category `ALL ON` / `ALL OFF`
- every toggle is saved between sessions
- **fake heartbeat monitor**: an animated ECG trace, drifting BPM readout and a
  pulse on every beat, plus a live enabled-hacks counter
- opens with `Tab` (rebindable in the mod's settings) or the `Menu` button on the
  main menu

Rows are UI state only - this mod gives you the menu itself, not the underlying
cheats (it is a UI mod, not a cheat implementation).

## Project layout

```
mod.json                  mod metadata + the "Open Mod Menu" keybind setting
CMakeLists.txt            Geode mod build script
build.ps1                 one-command local Windows build
src/main.cpp              MenuLayer button + Tab keybind listener
src/Hacks.{hpp,cpp}       hack catalogue, search and saved toggle state
src/ModMenuPopup.*        the popup: sidebar, search, scrolling list
src/HeartbeatMonitor.hpp  the fake ECG widget (CCDrawNode)
.github/workflows/build.yml  CI: builds every platform and packages the .geode
```

## Building locally

You need:

- [Geode SDK](https://github.com/geode-sdk/sdk/releases) (v5.x) extracted
  somewhere permanent, with `GEODE_SDK` pointing at that folder
- Visual Studio 2022 with the "Desktop development with C++" workload (this
  also brings a usable CMake)

Then, from this folder, in PowerShell:

```powershell
.\build.ps1                            # Release build -> ModMenu.geode here
.\build.ps1 -Install                   # also copy it into the game's mods folder
.\build.ps1 -Clean                     # wipe build/ and rebuild from scratch
.\build.ps1 -SdkPath "C:\geode-sdk"    # if GEODE_SDK is not set
.\build.ps1 -Configuration RelWithDebInfo
```

The script looks for `GEODE_SDK` (and a few common install locations), falls
back to the CMake bundled with Visual Studio when `cmake` is not on `PATH`, and
stops with an actionable message if the SDK or C++ toolchain is missing. If
PowerShell blocks the script (`running scripts is disabled`), run it once with
`powershell -ExecutionPolicy Bypass -File .\build.ps1`.

Without the script, the same build by hand:

```sh
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

`build/ModMenu.geode` is the finished mod - drop it into
`<Geometry Dash>/geode/mods` (or `%LOCALAPPDATA%\GeometryDash\geode\mods`) and
restart the game.

If you have the [Geode CLI](https://github.com/geode-sdk/cli/releases) installed
and the SDK set up, `geode build` does the same thing.

## Building with GitHub Actions

Push the project to a GitHub repository - `.github/workflows/build.yml` builds
Windows, macOS, iOS, Android32 and Android64 on GitHub's runners, combines them
into one `.geode` and uploads it as the `Mod-Menu-geode` artifact.

- manual run: Actions -> "Build Geode Mod" -> Run workflow
- push a tag like `v1.0.0` and the packaged `.geode` is attached to that release
- no local compiler needed; the workflow sets up the Geode SDK itself

## Settings

| Setting | Default | What it does |
| --- | --- | --- |
| Open Mod Menu | `Tab` | Toggles the menu open/closed. |

Toggles are stored as Geode save data keys (`hack-<category>--<name>`), so they
survive restarts. Toggling a hack nudges the fake heart rate up for a moment;
pressing `ALL ON` / `ALL OFF` makes it jump.

## Status

The source has been reviewed against the Geode v5.11.2 headers, but it has not
been compiled on this machine (no MSVC toolchain or Geode SDK installed). Use
`build.ps1` locally or the CI workflow to produce the actual `.geode`, and
expect the first build to be the real compile check.
