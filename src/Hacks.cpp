#include "Hacks.hpp"

#include <Geode/loader/Mod.hpp>

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <utility>

namespace {

std::string slugify(std::string const& input) {
    std::string out;
    for (char c : input) {
        auto uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            out += static_cast<char>(std::tolower(uc));
        }
        else if (!out.empty() && out.back() != '-') {
            out += '-';
        }
    }
    while (!out.empty() && out.back() == '-') {
        out.pop_back();
    }
    return out;
}

std::string savedKey(std::string const& id) {
    return "hack-" + id;
}

struct Registry {
    std::vector<std::string> categories;
    std::vector<HackDef> hacks;

    Registry() {
        auto add = [this](char const* category, std::initializer_list<char const*> names) {
            categories.emplace_back(category);
            for (auto name : names) {
                HackDef def;
                def.category = category;
                def.name = name;
                def.id = slugify(category) + "--" + slugify(name);
                hacks.push_back(std::move(def));
            }
        };

        add("Mega Hack", {
            "Auto-Update", "Contribute Translations", "Language: en-GB", "Theme",
            "Rulesets", "All Hacks", "Icon Hacks", "Interface Scale", "Animations",
            "Soft Interface", "Miscellaneous", "Auto-Select", "Auto Save Config",
            "Config Profiles", "Discord Rich Presence", "Show Changelog",
            "Debug Logging", "Check for Updates", "Silent Updates", "Beta Builds",
        });

        add("Bypass", {
            "Anti Kick", "Challenge Level", "Keymaster", "Main Levels",
            "Music Customiser", "Slider Limit", "Text Length", "Treasure Room",
            "Unlock Icons", "Unlock Shops", "Unlock Vaults", "Unlock Chests",
            "Unlock Gauntlets", "Unlock All Levels", "Level Password Bypass",
            "Rate Limit Bypass", "Cooldown Bypass", "Store Bypass",
            "Daily Level Unlock", "Weekly Demon Unlock", "Comment Cooldown Bypass",
            "Upload Cooldown Bypass",
        });

        add("Creator", {
            "Accurate Save", "Copy Hack", "Custom Object Bypass", "Default Song Bypass",
            "Editor Extension", "Free Scroll", "Hide UI", "Level Edit",
            "Multiple Editor Tracks", "No C Mark", "Place Over", "Smooth Editor Trail",
            "Toolbox Button Bypass", "Trigger Value Bypass", "Verify Hack",
            "Object Limit Bypass", "Unlimited Undo", "Paste Limit Bypass",
            "Instant Zoom", "Grid Snap Disable", "Collision Block Preview",
            "Save Backup", "Group ID Limit", "Free Rotate",
        });

        add("Cosmetic", {
            "Accurate Percentage", "Ball Rotation Bug", "Classic Particles",
            "Classic Pulse", "Classic Wave Trail", "Coin Shower", "Frozen Animations",
            "Hide Pulse Button", "Hide Pulse Menu", "Hide Player", "Icon Randomiser",
            "No Camera", "No Camera Zoom", "No Circle Effect", "No Dash Fire",
            "No Death Effect", "No Do Not Flip", "No End Shake", "No Ghost Trail",
            "No Glow", "No Mirror", "No New Best Popup", "No Orb Ring", "No Particles",
            "No Particles Classic", "No Portal Circle", "No Portal Lightning",
            "No Pulse", "No Respawn Flash", "No Robot Fire", "No Shaders", "No Shake",
            "No Spider Dash", "No Swing Fire", "No Trail", "No Wave Pulse",
            "No Wave Trail", "No Trail Behind Wave", "Player 1 on Top",
            "Player or Top", "Show Total Attempts", "Solid Wave Trail",
            "StartPos Reset Camera", "Stop Triggers on Death", "Trail Always On",
            "Trail Cutting", "Wave Pulse Size", "Wave Trail on Death", "RGB Icons",
            "Rainbow Wave", "Hide Attempts", "Icon Glow",
        });

        add("Level", {
            "Allow Pause Buffering", "All Modes Platformer", "Auto Clicker",
            "Auto Deafen", "Auto Kill", "Auto Music Sync", "Auto Pickup Coins",
            "Auto Song Download", "Click Between Frames", "Click on Steps",
            "Checkpoint Limit Bypass", "Collect Coins in Practice", "Confirm Exit",
            "Confirm Full Reset", "Confirm Normal", "Confirm Practice", "Force Ice",
            "Force Platformer", "Frame Stepper", "Hitbox Multiplier", "Instant Complete",
            "Jumpscare", "Jump Hack", "Noclip", "Noclip Limits", "No Collision",
            "Pause During Complete", "Practice Bug Fix", "Practice Music", "Random Seed",
            "Replay Last Checkpoint", "Respawn Time", "Shipcopter", "Show Hitboxes Items",
            "Show Hitboxes on Death", "Show Hitboxes Trail", "Show Layout",
            "Show Trajectory", "Show Triggers", "Smart StartPos", "StartPos Switcher",
            "Fast Respawn", "Instant Restart", "Auto Retry", "Safe Mode",
            "Physics Bypass", "Speed Bypass", "Freeze Player",
        });

        add("Status", {
            "Field Formatting", "Font: Big Font", "Scale 1x", "Opacity 1x",
            "Hide Status", "Message", "Testmode", "Cheat Indicator", "FPS Counter",
            "CPS Counter", "Best Run", "Noclip Accuracy", "Noclip Deaths", "Attempts",
            "Jumps", "Percentage", "Level Time", "Session Time", "Clock",
            "Frame Counter", "Position", "Velocity", "Dead", "Replay State",
            "FPS Graph", "Memory Usage", "Ping", "Rotation",
        });

        add("Universal", {
            "Allow Low Volume", "Compact Lists", "Custom Background", "Fast Chests",
            "Load Audio to Memory", "Lock Cursor", "Main Menu Play",
            "No Music Fade Out", "No Transition", "Pitch Shifter", "Throat Priority",
            "Transition Customiser", "Transparent Lists", "Window Scale",
            "VSync Override", "Force Borderless",
        });

        add("Cheat Safety", {
            "Ruleset: Mega Hack", "Disable Cheats", "Auto Safe Mode", "Safe Mode",
            "Safe Mode Popup", "Choose Keybind", "Choose Hack to Set", "View Keybinds",
            "Remove Keybindings", "Disable in Editor", "Noclip Detection",
            "Ban Warning", "Safe Mode Icon", "Auto Disable on Death",
            "Warn on Level Start", "Hide Cheat Badge",
        });

        add("Display", {
            "240 FPS", "Unlock FPS", "240 Hz", "Frame Extrapolation", "Vertical Sync",
            "Lock Delta", "Real Time", "Borderless Classic", "Fullscreen",
            "FPS Limit Unlock", "Window Scale", "Custom Resolution", "No Vignette",
            "Remove Fog",
        });

        add("Utility", {
            "Resources", "AppData", "Toggle DevTools", "Crash Game",
            "Mode: Save & Copy", "Speedhack", "Speedhack Audio", "Classic Mode",
            "Process Priority", "Clear Cache", "Export Settings", "Import Settings",
            "Open Logs", "Restart Game",
        });

        add("Interface", {
            "Hide Endscreen Cheats", "Hide Endscreen Extras", "Hide Menu Show",
            "Hide Icon on Pause", "Hide RobsVault Shortcut", "Replay: Record",
            "Replay: Play", "Replay: Filename", "Auto-save Replays",
            "Clear & New Replay", "Delete Replay", "Gameplay Options",
            "Convert to JSON", "Convert to GDR", "Open Replay Folder",
            "Menu Scale", "Menu Opacity", "Compact Mode", "Show Tooltips",
            "Hide Watermark", "Panel Layout",
        });

        add("Audio", {
            "Music Volume", "SFX Volume", "Mute on Focus Loss", "Custom Song Bypass",
            "Song Offset", "Auto Download Songs", "Menu Music", "Practice Music Sync",
            "Audio Visualiser", "Bass Boost", "Mute Death Sound", "Mute UI Sounds",
            "Streamer Mode",
        });

        add("Bot", {
            "Autoclicker CPS", "Auto Jump", "Auto Straight Fly", "Bot Recorder",
            "Playback Speed", "Record Macro", "Replay Macro", "Clear Macro",
            "Perfect Frame", "Replay Accuracy", "Frame Perfect", "Auto Complete",
            "Path Finder", "Humanised Input",
        });

        add("Profile", {
            "Unlock Demons", "Stars", "Moons", "Diamonds", "Orbs", "Coins",
            "Custom Badge", "Leaderboard Freeze", "Hide Stats", "Show Real Stats",
            "Fake Rank", "Profile Banner", "Title Unlock", "Icon Gallery",
        });
    }
};

Registry const& registry() {
    static Registry instance;
    return instance;
}

} // namespace

std::vector<std::string> const& allCategories() {
    return registry().categories;
}

std::vector<HackDef> const& allHacks() {
    return registry().hacks;
}

std::vector<HackDef> filterHacks(std::string const& category, std::string const& search) {
    std::string needle = search;
    std::transform(needle.begin(), needle.end(), needle.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    std::vector<HackDef> out;
    for (auto const& hack : allHacks()) {
        if (needle.empty() && hack.category != category) {
            continue;
        }
        if (!needle.empty()) {
            std::string name = hack.name;
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            if (name.find(needle) == std::string::npos) {
                continue;
            }
        }
        out.push_back(hack);
    }
    return out;
}

bool isHackEnabled(std::string const& id) {
    return geode::Mod::get()->getSavedValue<bool>(savedKey(id), false);
}

void setHackEnabled(std::string const& id, bool on) {
    geode::Mod::get()->setSavedValue<bool>(savedKey(id), on);
}

int enabledCount() {
    int count = 0;
    for (auto const& hack : allHacks()) {
        if (isHackEnabled(hack.id)) {
            ++count;
        }
    }
    return count;
}
