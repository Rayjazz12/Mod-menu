#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>

#include "ModMenuPopup.hpp"
#include "VitalsOverlay.hpp"

using namespace geode::prelude;

// Tab (configurable in the mod's settings) opens and closes the menu.
$on_game(Loaded) {
    listenForKeybindSettingPresses("open-menu", [](Keybind const&, bool down, bool repeat, double) {
        if (down && !repeat) {
            ModMenuPopup::openMenu();
        }
    }, Mod::get());
}

class $modify(ModMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Menu", "bigFont.fnt", "GJ_button_01.png", 0.6f),
            this,
            menu_selector(ModMenuLayer::onMenuButton)
        );
        button->setID("mod-menu-button"_spr);

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(button);
            menu->updateLayout();
        }
        else {
            auto fallbackMenu = CCMenu::create();
            fallbackMenu->setPosition({ 0.f, 0.f });
            button->setPosition({ 36.f, 28.f });
            fallbackMenu->addChild(button);
            this->addChild(fallbackMenu);
        }

        return true;
    }

    void onMenuButton(CCObject*) {
        ModMenuPopup::openMenu();
    }
};

// The fake vitals monitor also runs as a HUD while you play.
class $modify(VitalsPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool dontCreateObjects) {
        if (!PlayLayer::init(level, dontCreateObjects)) {
            return false;
        }

        VitalsOverlay::addTo(this);
        return true;
    }
};
