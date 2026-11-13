#include "ModMenuPopup.hpp"

#include <Geode/utils/cocos.hpp>

#include <algorithm>
#include <string>

using namespace geode::prelude;

namespace {

constexpr float kPopupW = 460.f;
constexpr float kPopupH = 300.f;

constexpr float kSideX = 66.f;
constexpr float kSideW = 106.f;
constexpr float kSideTop = 240.f;
constexpr float kSideSpacing = 15.f;

constexpr float kListX = 124.f;
constexpr float kListW = 320.f;
constexpr float kRowsTop = 230.f;
constexpr float kRowH = 19.f;
constexpr int kRowsPerPage = 7;
constexpr float kPageY = 84.f;

constexpr float kVitalsX = 124.f;
constexpr float kVitalsY = 12.f;
constexpr float kVitalsW = 320.f;
constexpr float kVitalsH = 60.f;

ccColor3B const kBgIdle{ 84, 96, 116 };
ccColor3B const kBgActive{ 92, 206, 168 };
ccColor3B const kSideIdle{ 68, 78, 96 };
ccColor3B const kSideActive{ 82, 168, 148 };

NineSlice* makePanel(float width, float height) {
    auto panel = NineSlice::create("square02_001.png", {}, { 3.f, 3.f, 3.f, 3.f });
    panel->setContentSize({ width, height });
    return panel;
}

} // namespace

ModMenuPopup* ModMenuPopup::create() {
    auto popup = new ModMenuPopup();
    if (popup->init()) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void ModMenuPopup::openMenu() {
    auto scene = CCDirector::get()->getRunningScene();
    if (!scene) {
        return;
    }

    if (auto existing = scene->getChildByIDRecursive("mod-menu-popup"_spr)) {
        if (auto popup = typeinfo_cast<ModMenuPopup*>(existing)) {
            popup->closePopup();
        }
        else {
            existing->removeFromParentAndCleanup(true);
        }
        return;
    }

    if (auto popup = ModMenuPopup::create()) {
        popup->show();
    }
}

void ModMenuPopup::closePopup() {
    this->onClose(nullptr);
}

bool ModMenuPopup::init() {
    if (!Popup::init(kPopupW, kPopupH)) {
        return false;
    }

    this->setID("mod-menu-popup"_spr);
    this->setTitle("Mod Menu");

    m_category = allCategories().empty() ? std::string() : allCategories().front();

    this->buildSidebar();
    this->buildHeader();
    this->buildVitals();
    this->buildPager();
    this->rebuildList();

    return true;
}

void ModMenuPopup::buildSidebar() {
    m_sideMenu = CCMenu::create();
    m_sideMenu->setPosition({ 0.f, 0.f });
    m_sideMenu->setAnchorPoint({ 0.f, 0.f });
    m_mainLayer->addChild(m_sideMenu);

    m_tabButtons.clear();
    auto const& categories = allCategories();

    float y = kSideTop;
    for (size_t i = 0; i < categories.size(); ++i) {
        auto bg = makePanel(kSideW, 13.f);
        bg->setOpacity(165);

        auto item = CCMenuItemSpriteExtra::create(
            bg, this, menu_selector(ModMenuPopup::onCategory)
        );
        item->setTag(static_cast<int>(i));

        auto label = CCLabelBMFont::create(categories[i].c_str(), "bigFont.fnt");
        label->setScale(0.245f);
        item->addChild(label);

        item->setPosition({ kSideX, y });
        m_sideMenu->addChild(item);
        m_tabButtons.push_back(item);

        y -= kSideSpacing;
    }

    m_footerMenu = CCMenu::create();
    m_footerMenu->setPosition({ 0.f, 0.f });
    m_footerMenu->setAnchorPoint({ 0.f, 0.f });
    m_mainLayer->addChild(m_footerMenu);

    char const* footerLabels[] = { "ALL ON", "ALL OFF" };
    for (int i = 0; i < 2; ++i) {
        auto bg = makePanel(50.f, 13.f);
        bg->setColor({ 60, 70, 88 });
        bg->setOpacity(180);

        auto item = CCMenuItemSpriteExtra::create(
            bg, this, menu_selector(ModMenuPopup::onAll)
        );
        item->setTag(i + 1);

        auto label = CCLabelBMFont::create(footerLabels[i], "bigFont.fnt");
        label->setScale(0.2f);
        item->addChild(label);

        item->setPosition({ i == 0 ? 37.f : 87.f, 24.f });
        m_footerMenu->addChild(item);
    }

    this->refreshSidebar();
}

void ModMenuPopup::refreshSidebar() {
    auto const& categories = allCategories();
    for (size_t i = 0; i < m_tabButtons.size(); ++i) {
        auto item = m_tabButtons[i];
        if (auto image = typeinfo_cast<NineSlice*>(item->getNormalImage())) {
            image->setColor(
                i < categories.size() && categories[i] == m_category ? kSideActive : kSideIdle
            );
        }
    }
}

void ModMenuPopup::buildHeader() {
    m_searchInput = TextInput::create(316.f, "Search hacks...");
    m_searchInput->setPosition({ 286.f, 254.f });
    m_searchInput->setScale(0.8f);
    m_searchInput->setCallback([this](std::string const& text) {
        m_search = text;
        m_page = 0;
        this->rebuildList();
    });
    m_mainLayer->addChild(m_searchInput);

    m_listMenu = CCMenu::create();
    m_listMenu->setPosition({ 0.f, 0.f });
    m_listMenu->setAnchorPoint({ 0.f, 0.f });
    m_mainLayer->addChild(m_listMenu);
}

void ModMenuPopup::buildVitals() {
    m_vitalsBg = makePanel(kVitalsW, kVitalsH);
    m_vitalsBg->setPosition({ kVitalsX, kVitalsY });
    m_vitalsBg->setAnchorPoint({ 0.f, 0.f });
    m_vitalsBg->setColor({ 10, 32, 24 });
    m_vitalsBg->setOpacity(210);
    m_mainLayer->addChild(m_vitalsBg);

    m_monitor = HeartbeatMonitor::create(186.f, 42.f);
    m_monitor->setPosition({ kVitalsX + 8.f, kVitalsY + 8.f });
    m_mainLayer->addChild(m_monitor);

    m_bpmLabel = CCLabelBMFont::create("72 BPM", "bigFont.fnt");
    m_bpmLabel->setScale(0.45f);
    m_bpmLabel->setColor({ 120, 255, 170 });
    m_bpmLabel->setAnchorPoint({ 0.f, 0.5f });
    m_bpmLabel->setPosition({ kVitalsX + 206.f, kVitalsY + 38.f });
    m_mainLayer->addChild(m_bpmLabel);

    m_rhythmLabel = CCLabelBMFont::create("SINUS RHYTHM", "bigFont.fnt");
    m_rhythmLabel->setScale(0.22f);
    m_rhythmLabel->setColor({ 130, 170, 150 });
    m_rhythmLabel->setAnchorPoint({ 0.f, 0.5f });
    m_rhythmLabel->setPosition({ kVitalsX + 206.f, kVitalsY + 24.f });
    m_mainLayer->addChild(m_rhythmLabel);

    m_countLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_countLabel->setScale(0.22f);
    m_countLabel->setColor({ 150, 200, 175 });
    m_countLabel->setAnchorPoint({ 0.f, 0.5f });
    m_countLabel->setPosition({ kVitalsX + 206.f, kVitalsY + 11.f });
    m_mainLayer->addChild(m_countLabel);

    m_monitor->setOnBeat([this]() {
        if (m_bpmLabel) {
            m_bpmLabel->stopAllActions();
            m_bpmLabel->setScale(0.45f);
            m_bpmLabel->runAction(cocos2d::CCSequence::create(
                cocos2d::CCScaleTo::create(0.06f, 0.5f),
                cocos2d::CCScaleTo::create(0.2f, 0.45f),
                nullptr
            ));
        }
        if (m_vitalsBg) {
            m_vitalsBg->setColor({ 16, 54, 36 });
            m_vitalsBg->runAction(cocos2d::CCTintTo::create(0.28f, 10, 32, 24));
        }
    });
}

void ModMenuPopup::buildPager() {
    m_pagerMenu = CCMenu::create();
    m_pagerMenu->setPosition({ 0.f, 0.f });
    m_pagerMenu->setAnchorPoint({ 0.f, 0.f });
    m_mainLayer->addChild(m_pagerMenu);

    char const* arrows[] = { "<", ">" };
    for (int i = 0; i < 2; ++i) {
        auto bg = makePanel(26.f, 15.f);
        bg->setColor({ 60, 70, 88 });
        bg->setOpacity(180);

        auto item = CCMenuItemSpriteExtra::create(
            bg, this, menu_selector(ModMenuPopup::onPage)
        );
        item->setTag(i == 0 ? -1 : 1);

        auto label = CCLabelBMFont::create(arrows[i], "bigFont.fnt");
        label->setScale(0.4f);
        item->addChild(label);

        item->setPosition({ i == 0 ? kListX + 13.f : kListX + kListW - 13.f, kPageY });
        m_pagerMenu->addChild(item);
    }

    m_pageLabel = CCLabelBMFont::create("1/1", "bigFont.fnt");
    m_pageLabel->setScale(0.3f);
    m_pageLabel->setColor({ 180, 200, 220 });
    m_pageLabel->setPosition({ kListX + kListW / 2.f, kPageY });
    m_mainLayer->addChild(m_pageLabel);
}

void ModMenuPopup::rebuildList() {
    if (!m_listMenu) {
        return;
    }
    m_listMenu->removeAllChildren();

    m_visible = filterHacks(m_category, m_search);

    int const total = static_cast<int>(m_visible.size());
    int const pages = std::max(1, (total + kRowsPerPage - 1) / kRowsPerPage);
    m_page = std::clamp(m_page, 0, pages - 1);

    int const start = m_page * kRowsPerPage;
    int const end = std::min(total, start + kRowsPerPage);
    float const rowWidth = kListW - 6.f;

    for (int i = start; i < end; ++i) {
        auto const& hack = m_visible[static_cast<size_t>(i)];
        bool on = isHackEnabled(hack.id);

        auto bg = makePanel(rowWidth, kRowH - 3.f);
        bg->setColor(on ? kBgActive : kBgIdle);
        bg->setOpacity(185);

        auto item = CCMenuItemSpriteExtra::create(
            bg, this, menu_selector(ModMenuPopup::onToggle)
        );
        item->setTag(i);

        auto label = CCLabelBMFont::create(hack.name.c_str(), "bigFont.fnt");
        label->setScale(0.26f);
        label->setAnchorPoint({ 0.f, 0.5f });
        label->setPosition({ -rowWidth / 2.f + 8.f, 0.f });
        item->addChild(label);

        auto check = cocos2d::CCSprite::createWithSpriteFrameName(
            on ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png"
        );
        check->setScale(0.55f);
        check->setPosition({ rowWidth / 2.f - 13.f, 0.f });
        check->setTag(2);
        item->addChild(check);

        float const y = kRowsTop - kRowH * static_cast<float>(i - start) - kRowH / 2.f;
        item->setPosition({ kListX + kListW / 2.f, y });
        m_listMenu->addChild(item);
    }

    this->updateCounter();
    this->updatePager();
}

void ModMenuPopup::updatePager() {
    if (!m_pageLabel) {
        return;
    }
    int const total = static_cast<int>(m_visible.size());
    int const pages = std::max(1, (total + kRowsPerPage - 1) / kRowsPerPage);
    m_pageLabel->setString(
        (std::to_string(m_page + 1) + "/" + std::to_string(pages)).c_str()
    );
}

void ModMenuPopup::updateCounter() {
    if (!m_countLabel) {
        return;
    }
    m_countLabel->setString(
        (std::to_string(enabledCount()) + "/" + std::to_string(allHacks().size()) + " ENABLED").c_str()
    );
}

void ModMenuPopup::onCategory(cocos2d::CCObject* sender) {
    auto node = static_cast<cocos2d::CCNode*>(sender);
    auto index = node->getTag();
    auto const& categories = allCategories();
    if (index < 0 || static_cast<size_t>(index) >= categories.size()) {
        return;
    }
    m_category = categories[index];
    m_page = 0;
    this->refreshSidebar();
    this->rebuildList();
}

void ModMenuPopup::onPage(cocos2d::CCObject* sender) {
    auto node = static_cast<cocos2d::CCNode*>(sender);
    m_page += node->getTag();
    this->rebuildList();
}

void ModMenuPopup::onToggle(cocos2d::CCObject* sender) {
    auto item = static_cast<cocos2d::CCNode*>(sender);
    auto index = item->getTag();
    if (index < 0 || static_cast<size_t>(index) >= m_visible.size()) {
        return;
    }

    auto const& hack = m_visible[index];
    bool on = !isHackEnabled(hack.id);
    setHackEnabled(hack.id, on);

    if (auto check = typeinfo_cast<cocos2d::CCSprite*>(item->getChildByTag(2))) {
        check->setDisplayFrame(cocos2d::CCSpriteFrameCache::get()->spriteFrameByName(
            on ? "GJ_checkOn_001.png" : "GJ_checkOff_001.png"
        ));
    }
    if (auto itemSprite = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
        if (auto image = typeinfo_cast<NineSlice*>(itemSprite->getNormalImage())) {
            image->setColor(on ? kBgActive : kBgIdle);
        }
    }

    if (m_monitor) {
        m_monitor->excite(on ? 5.f : 2.f);
    }

    this->updateCounter();
}

void ModMenuPopup::onAll(cocos2d::CCObject* sender) {
    auto node = static_cast<cocos2d::CCNode*>(sender);
    bool on = node->getTag() == 1;
    for (auto const& hack : m_visible) {
        setHackEnabled(hack.id, on);
    }
    if (m_monitor) {
        m_monitor->excite(8.f);
    }
    this->rebuildList();
}
