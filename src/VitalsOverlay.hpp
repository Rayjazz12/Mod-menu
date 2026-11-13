#pragma once

#include <Geode/Geode.hpp>

/**
 * The fake vitals readout shown while you play: a dark outlined box with a
 * pulsing red heart and a big BPM number. The number climbs as you get further
 * into the level - it is decorative, nothing is measured.
 */
class VitalsOverlay : public cocos2d::CCNode {
public:
    static VitalsOverlay* create();

    /** Adds the overlay to `parent`, pinned to the top-left of the screen. */
    static void addTo(cocos2d::CCNode* parent);

    /** Jolt the heart rate for a moment (used by menu toggles). */
    void excite(float amount);

protected:
    bool init() override;
    void update(float dt) override;

    void beat();

    cocos2d::CCLabelBMFont* m_bpmLabel = nullptr;
    cocos2d::CCNode* m_heart = nullptr;
    float m_bpm = 72.f;
    float m_beatPhase = 0.f;
    float m_excite = 0.f;
};
