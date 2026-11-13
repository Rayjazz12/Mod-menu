#include "VitalsOverlay.hpp"

#include <Geode/binding/PlayLayer.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>

#include <algorithm>
#include <cmath>
#include <string>

using namespace geode::prelude;

namespace {

constexpr float kWidth = 132.f;
constexpr float kHeight = 44.f;
constexpr float kMargin = 8.f;

constexpr float kBaseBpm = 68.f;
constexpr float kProgressBpm = 0.85f; // extra bpm at 100% of the level

cocos2d::ccColor4F const kHeartColor{ 0.88f, 0.09f, 0.11f, 1.f };
cocos2d::ccColor4F const kHeartGlow{ 0.55f, 0.02f, 0.04f, 0.5f };
cocos2d::ccColor3B const kNumberColor{ 164, 222, 0 };

/** Two lobes and a pointed bottom, drawn twice for a soft red glow. */
cocos2d::CCDrawNode* makeHeart() {
    auto draw = cocos2d::CCDrawNode::create();

    cocos2d::CCPoint const leftLobe{ -4.2f, 3.0f };
    cocos2d::CCPoint const rightLobe{ 4.2f, 3.0f };
    cocos2d::CCPoint const apex{ 0.f, -6.2f };
    constexpr float radius = 4.4f;

    draw->drawDot(leftLobe, radius + 2.f, kHeartGlow);
    draw->drawDot(rightLobe, radius + 2.f, kHeartGlow);
    draw->drawSegment(leftLobe, apex, radius + 2.f, kHeartGlow);
    draw->drawSegment(rightLobe, apex, radius + 2.f, kHeartGlow);

    draw->drawDot(leftLobe, radius, kHeartColor);
    draw->drawDot(rightLobe, radius, kHeartColor);
    draw->drawSegment(leftLobe, apex, radius, kHeartColor);
    draw->drawSegment(rightLobe, apex, radius, kHeartColor);

    return draw;
}

} // namespace

VitalsOverlay* VitalsOverlay::create() {
    auto ret = new VitalsOverlay();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool VitalsOverlay::init() {
    if (!CCNode::init()) {
        return false;
    }

    this->setContentSize({ kWidth, kHeight });
    this->setAnchorPoint({ 0.f, 0.f });

    // dark panel
    auto bg = NineSlice::create("square02_001.png", {}, { 3.f, 3.f, 3.f, 3.f });
    bg->setContentSize({ kWidth, kHeight });
    bg->setAnchorPoint({ 0.f, 0.f });
    bg->setPosition({ 0.f, 0.f });
    bg->setColor({ 14, 6, 8 });
    bg->setOpacity(175);
    this->addChild(bg);

    // white outline
    auto border = cocos2d::CCDrawNode::create();
    cocos2d::ccColor4F const white{ 1.f, 1.f, 1.f, 0.95f };
    constexpr float thickness = 1.4f;
    border->drawSegment({ 1.f, 1.f }, { kWidth - 1.f, 1.f }, thickness, white);
    border->drawSegment({ 1.f, kHeight - 1.f }, { kWidth - 1.f, kHeight - 1.f }, thickness, white);
    border->drawSegment({ 1.f, 1.f }, { 1.f, kHeight - 1.f }, thickness, white);
    border->drawSegment({ kWidth - 1.f, 1.f }, { kWidth - 1.f, kHeight - 1.f }, thickness, white);
    this->addChild(border, 2);

    m_heart = makeHeart();
    m_heart->setPosition({ 21.f, 22.f });
    this->addChild(m_heart, 3);

    m_bpmLabel = CCLabelBMFont::create("72", "bigFont.fnt");
    m_bpmLabel->setScale(0.85f);
    m_bpmLabel->setAnchorPoint({ 0.5f, 0.5f });
    m_bpmLabel->setPosition({ 82.f, 22.f });
    m_bpmLabel->setColor(kNumberColor);
    this->addChild(m_bpmLabel, 3);

    this->scheduleUpdate();

    return true;
}

void VitalsOverlay::update(float dt) {
    float progress = 0.f;
    if (auto playLayer = typeinfo_cast<PlayLayer*>(this->getParent())) {
        if (playLayer->m_levelLength > 0.f) {
            progress = std::clamp(playLayer->getCurrentPercent(), 0.f, 100.f);
        }
    }

    // further into the level -> faster heartbeat
    float const target = kBaseBpm + progress * kProgressBpm + m_excite;
    m_bpm += (target - m_bpm) * std::min(1.f, dt * 2.5f);
    m_excite = std::max(0.f, m_excite - dt * 8.f);

    m_beatPhase += dt * (m_bpm / 60.f);
    if (m_beatPhase >= 1.f) {
        m_beatPhase -= 1.f;
        this->beat();
    }

    if (m_bpmLabel) {
        m_bpmLabel->setString(
            std::to_string(static_cast<int>(std::lround(m_bpm))).c_str()
        );
    }
}

void VitalsOverlay::beat() {
    if (m_heart) {
        m_heart->stopAllActions();
        m_heart->setScale(1.18f);
        m_heart->runAction(cocos2d::CCScaleTo::create(0.22f, 1.f));
    }
    if (m_bpmLabel) {
        m_bpmLabel->stopAllActions();
        m_bpmLabel->setScale(0.96f);
        m_bpmLabel->runAction(cocos2d::CCScaleTo::create(0.22f, 0.85f));
    }
}

void VitalsOverlay::excite(float amount) {
    m_excite = std::min(m_excite + amount, 60.f);
}

void VitalsOverlay::addTo(cocos2d::CCNode* parent) {
    if (!parent) {
        return;
    }
    auto overlay = VitalsOverlay::create();
    if (!overlay) {
        return;
    }

    auto winSize = CCDirector::get()->getWinSize();
    overlay->setPosition({ kMargin, winSize.height - kMargin - kHeight });
    parent->addChild(overlay, 1000);
}
