// Voice.h
#ifndef VOICE_H
#define VOICE_H

#include "AudioEvent.h"
#include <vector>
#include <memory>
#include <array>

class Voice {
private:
    int voiceIndex;
    int delay;
    AudioParameters params;
    std::vector<std::unique_ptr<AudioEvent>> events;

    static constexpr std::array<short, 4> BASE_OCTAVES     = {6, 5, 4, 3};
    static constexpr std::array<short, 4> BASE_VOLUMES     = {100, 80, 60, 40};
    static constexpr std::array<short, 4> BASE_INSTRUMENTS = {6, 20, 0, 70};

public:
    explicit Voice(int voiceIndex) :
        voiceIndex(voiceIndex), delay(0), params()
    {
        int idx = voiceIndex % 4;
        params.oct.set(BASE_OCTAVES[idx]);
        params.vol.set(BASE_VOLUMES[idx]);
        params.inst.set(BASE_INSTRUMENTS[idx]);
    }

    void setDelay(int n) { delay = n; }

    void addEvent(std::unique_ptr<AudioEvent> event) {
        events.push_back(std::move(event));
    }

    int getDelay()                     const { return delay; }
    int getVoiceIndex()                const { return voiceIndex; }
    const AudioParameters& getParams() const { return params; }

    const std::vector<std::unique_ptr<AudioEvent>>& getEvents() const {
        return events;
    }
};

#endif // VOICE_H