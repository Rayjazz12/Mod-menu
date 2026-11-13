#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

#include "Hacks.hpp"
#include "HeartbeatMonitor.hpp"

#include <string>
#include <vector>

class ModMenuPopup : public geode::Popup {
public:
    static ModMenuPopup* create();

    /** Opens the menu, or closes it if it is already open. */
    static void openMenu();

    /** Closes the popup (fades out and removes it from its parent). */
    void closePopup();

protected:
    bool init();

    void buildSidebar();
    void refreshSidebar();
    void buildHeader();
    void buildVitals();
    void buildPager();
    void rebuildList();
    void updateCounter();
    void updatePager();

    void onCategory(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    void onAll(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);

private:
    cocos2d::CCMenu* m_listMenu = nullptr;
    cocos2d::CCMenu* m_sideMenu = nullptr;
    cocos2d::CCMenu* m_footerMenu = nullptr;
    cocos2d::CCMenu* m_pagerMenu = nullptr;
    geode::TextInput* m_searchInput = nullptr;
    cocos2d::CCLabelBMFont* m_bpmLabel = nullptr;
    cocos2d::CCLabelBMFont* m_rhythmLabel = nullptr;
    cocos2d::CCLabelBMFont* m_countLabel = nullptr;
    cocos2d::CCLabelBMFont* m_pageLabel = nullptr;
    HeartbeatMonitor* m_monitor = nullptr;
    geode::NineSlice* m_vitalsBg = nullptr;

    std::string m_category;
    std::string m_search;
    std::vector<HackDef> m_visible;
    std::vector<CCMenuItemSpriteExtra*> m_tabButtons;
    int m_page = 0;
};
