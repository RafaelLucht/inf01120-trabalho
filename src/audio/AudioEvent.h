// AudioEvent.h
#ifndef AUDIO_EVENT_H
#define AUDIO_EVENT_H

#include "AudioParameters.h"

class AudioEvent {
public:
    virtual ~AudioEvent() = default;
    virtual void runEvent(AudioParameters &params) = 0;
};

class InstrumentChangeEvent : public AudioEvent {
public:
    enum class ChangeMode { ABSOLUTE, RELATIVE };
private:
    short midi_val;
    ChangeMode mode;
public:
    explicit InstrumentChangeEvent(short midi_val, ChangeMode mode = ChangeMode::ABSOLUTE) :
        midi_val(midi_val), mode(mode) {}
    void runEvent(AudioParameters &params) override {
        switch (mode) {
            case ChangeMode::ABSOLUTE: params.inst.set(midi_val); break;
            case ChangeMode::RELATIVE: params.inst.setClamped(params.inst.getVal() + midi_val); break;
        }
    }
};

class OctaveChangeEvent : public AudioEvent {
private:
    short octShift;
public:
    explicit OctaveChangeEvent(short octShift) : octShift(octShift) {}
    void runEvent(AudioParameters &params) override {
        params.oct.setClamped(params.oct.getVal() + octShift);
    }
};

class BpmChangeEvent : public AudioEvent {
private:
    float bpmShift;
public:
    explicit BpmChangeEvent(float bpmShift) : bpmShift(bpmShift) {}
    void runEvent(AudioParameters &params) override {
        params.bpm.setClamped(params.bpm.get() + bpmShift);
    }
};

class VolumeChangeEvent : public AudioEvent {
public:
    void runEvent(AudioParameters &params) override {
        params.vol.setClamped(params.vol.get() * 2);
    }
};

class NoteEvent : public AudioEvent {
private:
    int baseNote;
public:
    explicit NoteEvent(int baseNote) : baseNote(baseNote) {}
    NoteEvent(const NoteEvent& other) : baseNote(other.baseNote) {}
    void runEvent(AudioParameters& params) override {}
    int getBaseNote() const { return baseNote; }
    int getNote(const AudioParameters& params) const {
        int nota = baseNote + (params.oct.getVal() - 5) * 12;
        return std::clamp(nota, 0, 127);
    }
};

#endif // AUDIO_EVENT_H