#pragma once

#include <string>
#include <string_view>
#include <array>

class AudioVolume {
private:
	static constexpr short MIN_VOLUME = 0;
	static constexpr short MAX_VOLUME = 127;

	short defaultVolume = 100;

	short vol = defaultVolume;
public:
	AudioVolume() = default;
	explicit AudioVolume(short defaultVolume);
	
	void set(short vol);
	void setClamped(short vol);
	
	short get() const;
	short getMin() const;
	short getMax() const;
	
	void resetToDefault();
};

class AudioBPM {
private:
	static constexpr float MIN_BPM = 30;
	static constexpr float MAX_BPM = 240;
	static constexpr float DEFAULT_BPM = 120;

	float bpm = DEFAULT_BPM;
public:
	AudioBPM() = default;
	
	void set(float bpm);
	void setClamped(float bpm);
	
	float get() const;
	float getMin() const;
	float getMax() const;
	
	void resetToDefault();
};

class AudioInstrument {
private:
	static constexpr short MIN_MIDI_VAL = 0;
	static constexpr short MAX_MIDI_VAL = 127;

	short defaultMidiVal = 6;

	short midi_val = defaultMidiVal;
	static const std::array<std::string_view, 128> MIDI_NAMES;

public:
	AudioInstrument() = default;
	explicit AudioInstrument(short defaultMidiVal);
	void set(short midi_val);
	void setClamped(short midi_val);
	
	short getVal() const;
	short getMin() const;
	short getMax() const;
	std::string_view getName() const;
	
	void resetToDefault();
};

class AudioOctave {
private:
	static constexpr short MIN_OCTAVE = 1;
	static constexpr short MAX_OCTAVE = 9;

	short defaultOctave = 6;

	short oct_val = defaultOctave;

public:
	AudioOctave() = default;
	explicit AudioOctave(short defaultOctave);
	
	void set(short oct_val);
	void setClamped(short oct_val);
	
	short getVal() const;
	short getMin() const;
	short getMax() const;
	std::string getName() const;
	
	void resetToDefault();
};

class AudioParameters {
public:
	AudioVolume vol;
	AudioBPM bpm;
	AudioInstrument inst;
	AudioOctave oct;

	AudioParameters() = default;

	void resetToDefaultAll();
};
