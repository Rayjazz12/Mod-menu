#pragma once

#include <Geode/Geode.hpp>

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>

/**
 * A fake ECG / vitals readout: scrolls a generated heartbeat waveform
 * across a dark strip and keeps a fluttering BPM number.
 * Nothing is measured - it is purely decorative.
 */
class HeartbeatMonitor : public cocos2d::CCNode {
public:
    static HeartbeatMonitor* create(float width, float height) {
        auto ret = new HeartbeatMonitor();
        if (ret->init(width, height)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init(float width, float height) {
        if (!cocos2d::CCNode::init()) {
            return false;
        }

        m_width = width;
        m_height = height;
        this->setContentSize({ width, height });
        this->setAnchorPoint({ 0.f, 0.f });

        m_draw = cocos2d::CCDrawNode::create();
        m_draw->setPosition({ 0.f, 0.f });
        this->addChild(m_draw);

        for (int i = 0; i < maxSamples(); ++i) {
            m_samples.push_back(restingValue());
        }

        this->scheduleUpdate();
        this->redraw();
        return true;
    }

    /** Slightly raises the displayed heart rate; it settles back down on its own. */
    void excite(float amount) {
        m_bpm = std::min(m_bpm + amount, 140.f);
    }

    float getBPM() const {
        return m_bpm;
    }

    void setOnBeat(std::function<void()> callback) {
        m_onBeat = std::move(callback);
    }

    void update(float dt) override {
        constexpr float step = 1.f / 60.f;

        m_accumulator += dt;
        while (m_accumulator >= step) {
            m_accumulator -= step;

            m_phase += step * (m_bpm / 60.f);
            if (m_phase >= 1.f) {
                m_phase -= 1.f;
                if (m_onBeat) {
                    m_onBeat();
                }
            }

            m_samples.push_back(waveform(m_phase));
            while (static_cast<int>(m_samples.size()) > maxSamples()) {
                m_samples.pop_front();
            }
        }

        // drift back towards a calm resting rate
        m_bpm += (m_baseBpm - m_bpm) * std::min(1.f, dt * 0.6f);

        this->redraw();
    }

private:
    cocos2d::CCDrawNode* m_draw = nullptr;
    std::deque<float> m_samples;
    std::function<void()> m_onBeat;
    float m_width = 0.f;
    float m_height = 0.f;
    float m_phase = 0.f;
    float m_accumulator = 0.f;
    float m_bpm = 72.f;
    float m_baseBpm = 72.f;

    int maxSamples() const {
        return std::max(16, static_cast<int>(m_width / 2.f));
    }

    static float restingValue() {
        return 0.02f;
    }

    /** One PQRST-ish cycle for a phase in [0, 1). */
    static float waveform(float p) {
        if (p < 0.06f) {
            return 0.08f;
        }
        if (p < 0.10f) {
            return -0.14f;
        }
        if (p < 0.135f) {
            return 0.95f;
        }
        if (p < 0.17f) {
            return -0.45f;
        }
        if (p < 0.23f) {
            return 0.06f;
        }
        if (p < 0.36f) {
            float t = (p - 0.23f) / 0.13f;
            return 0.24f * std::sin(t * 3.14159265f);
        }
        return restingValue();
    }

    void redraw() {
        if (!m_draw) {
            return;
        }
        m_draw->clear();

        const int total = maxSamples();
        const float dx = m_width / static_cast<float>(total - 1);
        const float mid = m_height * 0.5f;
        const float amplitude = m_height * 0.42f;

        cocos2d::ccColor4F const glow{ 0.2f, 1.f, 0.45f, 0.18f };
        cocos2d::ccColor4F const line{ 0.35f, 1.f, 0.5f, 1.f };

        // faint baseline
        m_draw->drawSegment(
            { 0.f, mid }, { m_width, mid }, 1.f,
            cocos2d::ccColor4F{ 0.25f, 0.6f, 0.35f, 0.18f }
        );

        auto pointAt = [&](size_t index) {
            return cocos2d::CCPoint{
                static_cast<float>(index) * dx,
                mid + m_samples[index] * amplitude
            };
        };

        if (m_samples.size() > 1) {
            for (size_t i = 1; i < m_samples.size(); ++i) {
                m_draw->drawSegment(pointAt(i - 1), pointAt(i), 2.6f, glow);
            }
            for (size_t i = 1; i < m_samples.size(); ++i) {
                m_draw->drawSegment(pointAt(i - 1), pointAt(i), 1.1f, line);
            }
        }

        // the leading edge of the trace
        float headX = static_cast<float>(m_samples.size() > 0 ? m_samples.size() - 1 : 0) * dx;
        m_draw->drawSegment(
            { headX, 0.f }, { headX, m_height }, 1.f,
            cocos2d::ccColor4F{ 0.35f, 1.f, 0.5f, 0.35f }
        );
    }
};
