// AudioParameters.h
#ifndef AUDIO_PARAMETERS_H
#define AUDIO_PARAMETERS_H

#include <string>
#include <array>
#include <algorithm>
#include <stdexcept>

class AudioVolume {
private:
    static constexpr short MIN_VOLUME = 0;
    static constexpr short MAX_VOLUME = 127;
    short defaultVolume = 100;
    bool muted = false;
    short vol = defaultVolume;
public:
    AudioVolume() = default;
    explicit AudioVolume(short defaultVolume) :
        defaultVolume(defaultVolume) {}
    void set(short vol) {
        if (vol < MIN_VOLUME || vol > MAX_VOLUME)
            throw std::out_of_range("Attempted setting invalid volume value");
        this->vol = vol;
    }
    void setClamped(short vol) {
        short real_set_value = std::clamp(vol, MIN_VOLUME, MAX_VOLUME);
        this->vol = real_set_value;
    }
    short get() const { return vol; }
    short getMin() const { return MIN_VOLUME; }
    short getMax() const { return MAX_VOLUME; }
    void resetToDefault() { set(defaultVolume); }
    void mute() { muted = true; }
    void unmute() { muted = false; }
    bool isMuted() const { return muted; }
};

class AudioBPM {
private:
    static constexpr float MIN_BPM = 30;
    static constexpr float MAX_BPM = 240;
    static constexpr float DEFAULT_BPM = 120;
    float bpm = DEFAULT_BPM;
public:
    void set(float bpm) {
        if (bpm < MIN_BPM || bpm > MAX_BPM)
            throw std::out_of_range("Attempted setting invalid BPM value");
        this->bpm = bpm;
    }
    void setClamped(float bpm) {
        float real_set_value = std::clamp(bpm, MIN_BPM, MAX_BPM);
        this->bpm = real_set_value;
    }
    float get() const { return bpm; }
    float getMin() const { return MIN_BPM; }
    float getMax() const { return MAX_BPM; }
    void resetToDefault() { set(DEFAULT_BPM); }
};

class AudioInstrument {
private:
    static constexpr short MIN_MIDI_VAL = 0;
    static constexpr short MAX_MIDI_VAL = 127;
    short defaultMidiVal = 6;
    static constexpr std::array<const char*, 128> MIDI_NAMES = {
        "Acoustic Grand Piano", "Bright Acoustic Piano", "Electric Grand Piano",
        "Honky-tonk Piano", "Electric Piano 1", "Electric Piano 2", "Harpsichord", "Clavi",
        "Celesta", "Glockenspiel", "Music Box", "Vibraphone", "Marimba",
        "Xylophone", "Tubular Bells", "Dulcimer",
        "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",
        "Reed Organ", "Accordion", "Harmonica", "Tango Accordion",
        "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)", "Electric Guitar (jazz)",
        "Electric Guitar (clean)", "Electric Guitar (muted)", "Overdriven Guitar",
        "Distortion Guitar", "Guitar Harmonics",
        "Acoustic Bass", "Electric Bass (finger)", "Electric Bass (pick)", "Fretless Bass",
        "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2",
        "Violin", "Viola", "Cello", "Contrabass", "Tremolo Strings",
        "Pizzicato Strings", "Orchestral Harp", "Timpani",
        "String Ensemble 1", "String Ensemble 2", "SynthStrings 1", "SynthStrings 2",
        "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit",
        "Trumpet", "Trombone", "Tuba", "Muted Trumpet", "French Horn",
        "Brass Section", "SynthBrass 1", "SynthBrass 2",
        "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe",
        "English Horn", "Bassoon", "Clarinet",
        "Piccolo", "Flute", "Recorder", "Pan Flute", "Blown Bottle",
        "Shakuhachi", "Whistle", "Ocarina",
        "Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)", "Lead 4 (chiff)",
        "Lead 5 (charang)", "Lead 6 (voice)", "Lead 7 (fifths)", "Lead 8 (bass + lead)",
        "Pad 1 (new age)", "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)",
        "Pad 5 (bowed)", "Pad 6 (metallic)", "Pad 7 (halo)", "Pad 8 (sweep)",
        "FX 1 (rain)", "FX 2 (soundtrack)", "FX 3 (crystal)", "FX 4 (atmosphere)",
        "FX 5 (brightness)", "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)",
        "Sitar", "Banjo", "Shamisen", "Koto", "Kalimba", "Bag pipe", "Fiddle", "Shanai",
        "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drum",
        "Melodic Tom", "Synth Drum", "Reverse Cymbal",
        "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet",
        "Telephone Ring", "Helicopter", "Applause", "Gunshot"
    };
    static std::string nameMIDI(short midi_val) { return MIDI_NAMES[midi_val]; }
    struct MIDI_Instrument { short midi_val; std::string inst_name; };
    MIDI_Instrument inst = {defaultMidiVal, nameMIDI(defaultMidiVal)};
public:
    AudioInstrument() = default;
    explicit AudioInstrument(short defaultMidiVal) : defaultMidiVal(defaultMidiVal) {}
    void set(short midi_val) {
        if (midi_val < MIN_MIDI_VAL || midi_val > MAX_MIDI_VAL)
            throw std::out_of_range("Attempted setting invalid MIDI instrument value");
        inst.midi_val = midi_val;
        inst.inst_name = nameMIDI(midi_val);
    }
    void setClamped(short midi_val) {
        short real_set_val = std::clamp(midi_val, MIN_MIDI_VAL, MAX_MIDI_VAL);
        inst.midi_val = real_set_val;
        inst.inst_name = nameMIDI(real_set_val);
    }
    short getVal() const { return inst.midi_val; }
    short getMin() const { return MIN_MIDI_VAL; }
    short getMax() const { return MAX_MIDI_VAL; }
    std::string getName() const { return inst.inst_name; }
    void resetToDefault() { set(defaultMidiVal); }
};

class AudioOctave {
private:
    static constexpr short MIN_OCTAVE = 1;
    static constexpr short MAX_OCTAVE = 9;
    short defaultOctave = 6;
    static std::string nameOct(short oct) {
        std::string s;
        switch (oct) {
            case 1: s = "st"; break;
            case 2: s = "nd"; break;
            case 3: s = "rd"; break;
            default: s = "th";
        }
        return std::to_string(oct) + s;
    }
    struct Octave { short oct_val; std::string oct_name; };
    Octave oct = {defaultOctave, nameOct(defaultOctave)};
public:
    AudioOctave() = default;
    explicit AudioOctave(short defaultOctave) : defaultOctave(defaultOctave) {}
    void set(short oct_val) {
        if (oct_val < MIN_OCTAVE || oct_val > MAX_OCTAVE)
            throw std::out_of_range("Attempted setting invalid octave value");
        oct.oct_val = oct_val;
        oct.oct_name = nameOct(oct_val);
    }
    void setClamped(short oct_val) {
        short real_set_value = std::clamp(oct_val, MIN_OCTAVE, MAX_OCTAVE);
        oct.oct_val = real_set_value;
        oct.oct_name = nameOct(real_set_value);
    }
    short getVal() const { return oct.oct_val; }
    short getMin() const { return MIN_OCTAVE; }
    short getMax() const { return MAX_OCTAVE; }
    std::string getName() const { return oct.oct_name; }
    void resetToDefault() { set(defaultOctave); }
};

class AudioParameters {
public:
    AudioVolume vol;
    AudioBPM bpm;
    AudioInstrument inst;
    AudioOctave oct;
    AudioParameters() = default;
    void resetToDefaultAll() {
        vol.resetToDefault();
        bpm.resetToDefault();
        inst.resetToDefault();
        oct.resetToDefault();
    }
};

#endif // AUDIO_PARAMETERS_H